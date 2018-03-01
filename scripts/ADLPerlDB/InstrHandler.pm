#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#
#
#  Handler for instructions.
#
package InstrHandler;
require EncHelper;
require ExpandHelper;
require ADLPerlDB::BasicHandler;
use Data::Dumper;
use lib "./";

use strict;
require Exporter,
our @ISA = ("BasicHandler");
our @EXPORT= qw(new preprocess_objects postprocess_object process_query);

# Arg0 - classname
# Arg1 - core
# Arg2 - Name of objects to take
# Arg3 - If need to expand syntax.
# Ret  - handler.
sub new {
  my ($class,$core,$expand) = @_;
  my $this  = $class->SUPER::new($core,"instrs");
  $this->{expand} = $expand;
  return bless ($this, $class);
};

# Preprocess: 
# Transfer usual fields to abstract fields.
# Compute "enc" and "bits".
# Generate alias into action if needed.
# Copy parent's data if this is a nested instruction.
sub preprocess_objects {
  my $this = shift;
  my $instrfields = $this->{core}->{instrfields};
  my $instrs = $this->{objects};
  my $little_endian = $this->{core}->{bit_endianness} eq "little";


  while (my($iname, $instr) = each(%$instrs)) {
    $instr->{enc} = EncHelper::gen_enc($instr,$instrfields,$little_endian);
    $instr->{bits} = gen_bits($instr->{fields},$instrfields);
    $instr->{fields} = gen_fields($instr->{fields});  
		#print "Instruction $iname:  ",Dumper($instr),"\n";
		my ($atag,$orig_format);
    if (exists $instr->{aliases}) {
			# Newer alias format which supports multiple alias targets.
			$atag = "aliases";			
      $instr->{is_alias} = &gen_aliases($instr->{$atag});
      $instr->{is_alias} =~ s/^(\w+)(.+)$/$1/;
			$instr->{parent_action} = &get_from_alias($instr->{$atag},"parent_action");
			$instr->{"extracted-doc-op"} = &get_from_alias($instr->{$atag},"extracted-doc-op");
    } elsif (exists $instr->{alias}) {
			# Original format, provided for compatibility.
			$atag = "alias";
			$orig_format = 1;
			$instr->{is_alias} = &gen_alias($instr->{$atag});
      $instr->{is_alias} =~ s/^(\w+)(.+)$/$1/;
		}
    if (!exists $instr->{action} && exists $instr->{$atag}) {
			my $alias_str = ($orig_format) ? &gen_alias($instr->{$atag}) : &gen_aliases($instr->{$atag});
      $instr->{action} = "alias = \"".$alias_str.'"';
      $instr->{action} =~ s/_imp_bits_[0123456789_x]*//g;
			if ($orig_format) {
				$instr->{sources}      = gen_src_dst_orig($instr->{alias}->{sources},"source");
				$instr->{destinations} = gen_src_dst_orig($instr->{alias}->{destinations},"destination");
			} else {
				$instr->{sources}      = gen_src_dst($instr->{$atag},"sources","source");
				$instr->{destinations} = gen_src_dst($instr->{$atag},"destinations","destination");
			}
      $instr->{alias} = $alias_str;
    }
    $instr->{iname} = $instr->{syntax};
    $instr->{iname} =~ s/\s+.*//;
    $instr->{desc} = $instr->{doc};
		populate_child($instr,$instrs);
		#print "Instruction $iname:  ",Dumper($instr),"\n\n";
  } 
  $this->SUPER::preprocess_objects();
};

# We catch references to fields as questions to instructionfields.
sub process_query {
  my $this = shift;
  my $cond = shift;
  $cond =~ s/fields->(\w+)/\$this->{core}->{instrfields}->{fields}->{$1}/g;
  $cond = $this->SUPER::process_query($cond);
  return $cond;
}

# Postprocess
# Remove variable and reserved opcode fields
# Expand syntax if asked
# process hashes to arrays of keys.
sub postprocess_object {
  my ($this,$instr,$le) = (shift,shift,shift);
  my $ret = {};

  my %ret = %$instr;
  delete $ret{fields}->{VarInstrOpcode};
  delete $ret{fields}->{VarPrefixOpcode};
  delete $ret{fields}->{reserved};
  $ret{syntax} = [];
  if ($this->{expand}) {
    my $expand = $this->{expand};
    my $orig_syntax = $instr->{syntax};
    my @expanded = ();
    my @exp = split /,/,$expand;
    foreach my $f (@exp) {
      if (exists $instr->{fields}->{$f}) {
				my @replacements = ExpandHelper::expand_field($f,$this->{core},$le);
				if (scalar @expanded) {
					my @new_exp = ();
					foreach my $prev (@expanded) {
						foreach (@replacements) {
							my $new_syntax = " ".$prev." ";
							$new_syntax =~ s/$f/$1$_$2/g; 
							push(@new_exp,$new_syntax);
						}
					}
					@expanded = @new_exp;
				} else {
					foreach (@replacements) {
						my $new_syntax = " ".$orig_syntax." ";
						$new_syntax =~ s/$f/$1$_$2/g; 
						push(@expanded,$new_syntax);
					}
				}
      }
    }
    if (scalar @expanded) {
			@expanded = sort @expanded;
			$ret{syntax} = \@expanded;
    } else {
      push(@{$ret{syntax}},$instr->{syntax});
    }
  } else {
    push(@{$ret{syntax}},$instr->{syntax});
  }

  foreach (keys %ret) {
    if ($_ ne "syntax" && $_ ne "bits") {
      if (ref($ret{$_}) eq "ARRAY") {
        $ret{$_} = join(", ",@{$ret{$_}});
      } elsif (ref($ret{$_}) eq "HASH") {
				my @keys = sort(keys %{$ret{$_}});
        $ret{$_} = join(",",@keys);
      }
    }
  }
	
  return \%ret;
}

# If this is a child instruction (parent key exists), then copy in all
# keys from the parent which don't exist in the child.
sub populate_child {
	my ($instr,$instrs) = @_;

	if (exists $instr->{parent}) {
		my $parent = $instrs->{ $instr->{parent} };

		for my $key ( keys %$parent ) {
			if ($key eq "attributes") {
				my $pattrs = $parent->{$key};
				# For attributes, we merge them.
				my $attrs = (exists $instr->{attributes}) ? $instr->{attributes} : {};
				for my $pattr ( keys %$pattrs ) {
					if (!exists $attrs->{$pattr}) {
						$attrs->{$pattr} = $pattrs->{$pattr};
					}
				}
			} elsif (!exists $instr->{$key} ) {
				$instr->{$key} = $parent->{$key};
			}
		}
	}
}

# Removes unecessary information from the sources/destinations list.
sub gen_src_dst {
	my ($input,$tag,$hdr) = @_;

	my @ret;

	#print "src/dst data;  ",Dumper($input),"\n";

	for my $akey (keys %$input) {

		my $alias = $input->{$akey};
		# First, remove the header items.
		my @tmpr = grep { $_ ne $hdr } @{ $alias->{$tag} };
		
		# Then convert to a friendlier description.
		my @tmpr = map { "$_->{field}($_->{value})"; } @tmpr;

		push @ret,@tmpr;
	}

	return \@ret;
}

# Same as above, for the original alias format.
sub gen_src_dst_orig {
	my ($input,$hdr) = @_;
	
	# First, remove the header items.
	my @ret = grep { $_ ne $hdr } @$input;

	# Then convert to a friendlier description.
	my @ret = map { "$_->{field}($_->{value})"; } @ret;

	return \@ret;
}

# For the newer, multi-alias format.
sub gen_aliases {
  my $aliases = shift;
	#print "My alias data:  ",Dumper($aliases),"\n";
  my %fmap;
  my $ret;
	my $first = 1;
	foreach my $akey (keys %$aliases) {
		$ret .= " | " if (!$first);
		$ret .= $akey;
		my $alias = $aliases->{$akey};
		foreach  my $key (keys %$alias) {
			my $vals = $alias->{$key};
			if (ref($vals) eq "ARRAY") {
				gen_alias_internal(\%fmap,$vals);
			}
		}
		$first = 0;
		foreach my $src (sort(keys %fmap)) {
			$ret .= " $src($fmap{$src})";
		}
	}

	#print "  return:  $ret\n";

  return $ret;
}

# For the older, single-alias format.
sub gen_alias {
  my $input = shift;
  my %alias = %$input;
  my %fmap;
  my $ret = $alias{"target"};
  foreach  my $key (keys %alias) {
		my $vals = $alias{$key};
    if ($key ne "target") {
			gen_alias_internal(\%fmap,$vals);
		}
  }

  foreach my $src (keys %fmap) {
    $ret .= " $src($fmap{$src})";
  }

  return $ret;
}

sub gen_alias_internal {
	my ($fmap,$vals) = @_;
	my @field_list = @$vals;
	while(@field_list) {
		my $elem = shift @field_list;
		$elem = shift @field_list;
		my $src = $elem->{field};
		$src  =~ s/_imp_bits_[0123456789_x]*//g;
		$fmap->{$src} = $elem->{value};
	}	
}

# Concatenate together all alias parent actions.
sub get_from_alias {
  my ($aliases,$tag) = @_;
	#print "My alias data:  ",Dumper($aliases),"\n";
  my $ret;
	my $first = 1;
	foreach my $akey (keys %$aliases) {
		$ret .= "\n" if (!$first);
		$ret .= $aliases->{$akey}->{$tag};
		$first = 0;
	}

  return $ret;
}

# Generates "bits" array.
sub gen_bits {
  my ($fields,$instrfields) = (shift,shift);
  my @ret = ();
  while (my ($fname,$opc) = each(%$fields)) {
# First name.
    my $ifield = $instrfields->{$fname};
    $fname =~ s/_imp_bits.*//;
    $fname .= "(".$opc.")" if ($opc && $opc ne "reserved");
    $fname .= ":";
    foreach (@{$ifield->{bits}}) {
      my ($l,$r) = @{$_};
      $fname .= "[$l,$r] "
    }
    push (@ret,$fname);
  }
	@ret = sort @ret;
  return \@ret;
}

# generates abstract fields from real
sub gen_fields {
  my $fields = shift;
  my $abstract_fields = {};
  while (my($fname, $field) = each(%$fields)) {
    my $name = $fname;
    $name =~ s/_imp_bits.*//;
    $abstract_fields->{$name} = $field;
  }
  return $abstract_fields;
}

1;
