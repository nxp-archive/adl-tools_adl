#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#
#
#  Perl database interface.
#


package ADLperlDB;

require Exporter,
our @ISA = ("Exporter");
our @EXPORT = qw(new valid_sort valid_format query $debug dprint dlprint dprintf set_unroll_arrays);
use Scalar::Util "looks_like_number";
use Data::Dumper;

$debug = -1;

use strict;

use vars qw ($debug $unroll_arrays);

sub set_unroll_arrays
{
	$unroll_arrays = $_[0];
}

# Arg0 - classname
# Arg1 - ref to data
# Arg2 - ref to the whole core
sub new {
  my ($classname,$data,$core) = @_;
  my $this  = {};
  $this->{data} = $data;
  $this->{config} = {};
  $this->{core} = $core;
  bless ($this, $classname);
  $this->create_config();
	#print "Data:  ",Dumper($this->{data}),"\n";
	#print "Config:  ",Dumper($this->{config}),"\n";
  return $this;
}

# Creates config - representation of database fields.
sub create_config {
  my $this = shift;
  foreach (keys %{$this->{data}}) {
    my $tmp = create_config_internal($this->{data}->{$_});
    $this->add_config($tmp);
  }
}

# Merges new information from the new entry
# So essenitally we go thru all object in order to get 
# a "general" object.
sub add_config {
  my ($this,$cfg) = (shift,shift);
  foreach my $key (keys %$cfg) {
    if (not exists $this->{config}->{$key}) {
      $this->{config}->{$key} = $cfg->{$key};
    } else {
      if (ref($cfg->{$key}) eq "HASH") {
				foreach my $subkey (keys %{$cfg->{$key}}) {
					if (not exists $this->{config}->{$key}->{$subkey}) {
						$this->{config}->{$key}->{$subkey} = $cfg->{$key}->{$subkey};
					}
				}
      }
    }
  }
}

# Creates config for the single entry
sub create_config_internal {
  my $entry = shift;
  my $conf = {};
  foreach (keys %$entry) {
    next if ($_ eq "attributes");
    if (ref($entry->{$_}) eq "HASH") {
      $conf->{$_} = create_hash_config($entry->{$_});
    } elsif (ref($entry->{$_}) eq "ARRAY") {
      $conf->{$_} = [];
    } else {
      $conf->{$_} = "";
    }
  }
  return $conf;
}

# Config for hash
sub create_hash_config {
  my $h = shift;
  my $val;
  foreach (keys %$h) {
    if (ref($h->{$_}) eq "HASH") {
      foreach (keys %{$h->{$_}}) {
	$val->{$_} = ref($h->{$_});
      }
      next;
    } else {
      $val = {};
    }  
  }
  return $val;
}

# for debugging.
sub dump_config() {
  my $this = shift;
  print Dumper($this->{config});
}

# Checks if the sort is OK,
sub valid_sort {
  my ($this,$sort) = (shift,shift);
  my @fields = split(/,/,$sort);
  foreach (@fields) {
    if ($this->{config}->{$_} =~ /ARRAY/ ||  $this->{config}->{$_} =~ /HASH/) {
      return "Bad field is sort: $_";
    }
  }
  return "";
}

# Checks if the format of output is OK
# Checks number is requered
# We go through format and check if the same fields are present in config structure
sub valid_format {
  my ($this,$format,$check_number) = (shift,shift,shift);
  my @fields = split(/,/,$format);
  foreach (@fields) {
    my ($name,$width) = split(/:/,$_);
    if ($check_number) {
      return "Please enter number in format '$format' instead of '$width'" 
          unless (looks_like_number($width));
    }
    my @path = split /->/,$name;
    if (scalar @path == 1) {
      return "Unknown field '$path[0]' in format '$format'" unless exists $this->{config}->{$path[0]};
    } elsif (scalar @path == 2) {
      return "Unknown field '$path[0]' in format '$format'" 
         unless exists $this->{config}->{$path[0]};
      return "Unknown field '$name' in format '$format'"
         unless exists $this->{config}->{$path[0]}->{$path[1]};
    } else {
      return "Not supported format $format - too much nesting\n.";
    }
  }
  return "";
}

# Goes through all the objects and saves names of that passed the condition.
sub query {
  my ($this,$query,$exclude,$sort) = @_;
  my @names = ();
  my $cond  = process_query($query,$this->{config});
  my $xcond = process_query($exclude,$this->{config});
	#print "Conditional:  $cond\n";
	#print "Exclude    :  $xcond\n";
  foreach my $objname (keys %{$this->{data}}) {
    my $object = $this->{data}->{$objname};
		#print "Object:  ",Dumper($object),"\n";
    if ((eval $cond) && !(eval $xcond)) {
			#print "  Add\n";
      push(@names,$objname);
    }  
    die "Error in condition: $@" if ($@);
  }
  my @sorted = $this->my_sort(\@names,$sort);
  return @sorted;
}

# Sorts fields
sub my_sort {
  my ($this,$names,$sort) = @_;
  my @fields = split(/,/,$sort);
  my $sortstr = "";
  foreach (@fields) {
    if ($sortstr) {
      $sortstr .= " or ";
    }
    # The sort for numbers adn strigns is not the same
    if (looks_like_number($this->{config}->{$_})) {
      $sortstr .= "(\$this->{data}->{\$a}->{$_} <=> \$this->{data}->{\$b}->{$_})"
    } else {
      $sortstr .= "(\$this->{data}->{\$a}->{$_} cmp \$this->{data}->{\$b}->{$_})"
    }
  }
  return (sort {eval $sortstr} @$names);
}

# Substitues simple query with the real perl variables.
sub process_query {
  my ($query,$config) = (shift,shift);

  my @props;
  my %subst_loop;
  my %subst;
	my %unrolls;
	my %unroll_subst;
  my %exists_loop;
  my %exists;
	#print "Config hash:  ",Dumper($config),"\n\n";
  while (my($key,$val)=each(%$config)) {
    if (ref($val) eq "HASH") {
      foreach my $subkey (keys %$val) {
				my $tmp = $key."->".$subkey;
				push (@props,$tmp); 
				$subst_loop{$tmp} = "foreach my \$_$key\_ (\"\",keys %{\$object->{\"$key\"}})";
				$subst{$tmp} = "\$object->{$key}->{\$\_$key\_}->{\"$subkey\"}";
				$exists_loop{$tmp} = "foreach my \$_$key\_ (keys %{\$object->{\"$key\"}})";
				$exists{$tmp} = "exists \$object->{\$_$key\_}->{\"$subkey\"}";
      } 
      push (@props,$key);
      $subst_loop{$key} = "foreach my \$_$key\_ (\"\",keys %{\$object->{\"$key\"}})";
      $subst{$key} = "\$_$key\_";
      $exists_loop{$key} = "";
      $exists{$key} = " exists \$object->{\"$key\"}";
    } elsif (ref($val) eq "ARRAY") {
      push (@props,$key);
      $subst_loop{$key}  = "foreach my \$_$key\_ (\"\",\@{\$object->{\"$key\"}})";
      $subst{$key} = "\$_$key\_";
      $unrolls{$key} = "my \$_unrolled_$key\_ = join(\"\t\",(map { \$_ } \@{\$object->{\"$key\"}}));";
      $unroll_subst{$key} = "\$_unrolled_$key\_";
      $exists_loop{$key} = "";
      $exists{$key} = " exists \$object->{\"$key\"}";
    } else {
      push (@props,$key);
      $subst_loop{$key} = "";
      $subst{$key} = "\$object->{\"$key\"}";
      $exists_loop{$key} = "";
      $exists{$key} = " exists \$object->{\"$key\"}";
    }
  }
  my @loops = ();
  $query = " ".$query." ";

	#print "Config              :  ",Dumper($config),"\n";
	#print "Properties          :  ",Dumper(\@props),"\n";
	#print "Substitution loops  :  ",Dumper(\%subst_loop),"\n\n";
	#print "Substitution body   :  ",Dumper(\%subst),"\n\n";
	#print "Unroll operations   :  ",Dumper(\%unrolls),"\n\n";
	#print "Unroll substititions:  ",Dumper(\%unroll_subst),"\n\n";
	#print "Exists hash         :  ",Dumper(\%exists),"\n\n";

  # If the field in the query is a list - we'll need to go through all elements of the list
  foreach my $prop (@props) {
    if ($query =~ /exists\s+$prop/) {
      $query =~ s/exists $prop/exists#$prop/g;
      push (@loops,$exists_loop{$prop});
    }
  }

	#print "Query 1:  ",$query,"\n\n";

	sub add_loop {
		my ($query,$prop,$loops) = @_;

		$$query =~ s/([^\w\/"'#]+)$prop([^\w\/"']+)/${1}$subst{$prop}${2}/g;
		push (@$loops,$subst_loop{$prop});
	}

	sub add_unrolled_loop {
		my ($query,$prop,$loops,$pfx) = @_;

		$$query =~ s/([^\w\/"'#]+)($pfx)$prop([^\w\/"']+)/${1}$unroll_subst{$prop}${3}/g;
		push (@$loops,$unrolls{$prop});
	}

	#print "Unroll arrays:  $unroll_arrays\n";

  # Wrap the query with a loop cycling on lists
  foreach my $prop (@props) {
		# First- do we have any unroll operators, e.g. @foo, where foo is a
		# property?  We also unroll implicitly if we have string operators on array
		# properties, based upon the global flag set by the command-line flag.
    if ($query =~ /([^\w\/"'#]+)\@$prop([^\w\/"']+)/) {
      next if (grep (/$prop/,@loops));
			add_unrolled_loop(\$query,$prop,\@loops,"\@");
		}
		# Next, see if we just have a property and unroll it into a loop if
		# relevant.
    elsif ($query =~ /([^\w\/"'#]+)$prop\s*[=!]~/) {
			# Found a string operator on a property, so unroll if it's an array
			# operation, unless overridden by the global flag.
      next if (grep (/$prop/,@loops));
			if (exists $unrolls{$prop} && $unroll_arrays ne "off") {
				add_unrolled_loop(\$query,$prop,\@loops);
			} else {
				add_loop(\$query,$prop,\@loops);
			}
		}
    elsif ($query =~ /([^\w\/"'#]+)$prop([^\w\/"']+)/) {
      next if (grep (/$prop/,@loops));
			# Unroll the array only if the global flag says to always do it.
			if (exists $unrolls{$prop} && $unroll_arrays eq "on") {
				add_unrolled_loop(\$query,$prop,\@loops);
			} else {			
				add_loop(\$query,$prop,\@loops);
			}
    }
  }

	#print "Query 2:  ",$query,"\n\n";
	#print "All loops:  ",Dumper(\@loops),"\n\n";

  $query = "\$success = \$success || ($query);";

	# Transform any exists operators so that they reference the appropriate key.	
	$query =~ s/exists#(\w+)/$exists{$1}/eg;

	#print "Query 3:  ",$query,"\n\n";

	# Any remaining exists are removed and replaced with a 'false' value, since we
	# know they don't exist.  This way, bad attributes don't cause problems.
	$query =~ s/exists\s+\w+/0/eg;

	#print "Query 4:  ",$query,"\n\n";

  foreach (@loops) {
    my $pre  = "$_ {";
    my $post = "};";
    $query = $pre.$query.$post;
  }

  $query = "my \$success = 0;".$query.";\$success;";
  return $query;
}

1;
