#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#
#
# Helper package expands given instruction field:
#   For enumerated we return array of options (index is the value)
#   For referencing to a sparse regfile - the same
#   For nested with syntax - all possible combinations.
#

package ExpandHelper;

require Exporter;

use List::Util "max";
use Data::Dumper;

our @ISA = ("Exporter"); 
our @EXPORT = ("expand_field");


# Main routine.
# Arg0: name of instruction field.
# Arg1: ref to core
# Arg2: true if little endian
# Returm: array of options
sub expand_field {
  my ($fname,$core,$little_endian) = (shift,shift,shift);
  my $ifield = $core->{instrfields}->{$fname};
  my @ret;
  if (exists $ifield->{implements}) {
    # Take the pseudo if needed
    $ifield = $core->{instrfields}->{"$ifield->{implements}"};
  }
	
  if (exists $ifield->{enumerated}) {
    # For the enumerated expandes to its emums
    foreach (sort {$a <=> $b} keys %{$ifield->{enumerated}}) {
      push(@ret,$ifield->{enumerated}->{$_});
    }
  } elsif (exists $ifield->{syntax}) {

    # Syntax only can be for nested fields.
    my $syntax = $ifield->{syntax};
    if ($ifield->{alias}) {
      # Use alias's syntax but real field definitions.
      $ifield = $core->{instrfields}->{$ifield->{alias}};
    }
    my $nested = $ifield->{instrfields};
    my $width = $ifield->{width};
    my $size  = $ifield->{size};

    my @bits = (); #bit vector for the current value of the field.
    for (my $i = 0 ; $i != $width; ++$i) {
      $bits[$i] = 0;
    }

    for (my $i = 0 ; $i != 2**$width; ++$i) {
      # First, based on "fields" definition compose new value of the field.

      my @fields_bits = field_bits(\@bits,$ifield,$little_endian);

      my @syntax_fields = split /\W+/,$syntax;
      my $cur_syntax = $syntax;
      # Will loop through all syntax fields and substitue appropriate field with 
      # string

      foreach my $f (@syntax_fields) {
        # We are interested only in fields
				next if not exists $ifield->{instrfields}->{$f};
        # Real value if the field in the syntax.
				my $val = get_val($ifield->{instrfields}->{$f}->{fields},\@fields_bits,$little_endian);
        # Assign appropriated string.
				if (exists $ifield->{"ref"}) {
					my $regfile = $core->{regfiles}->{$ifield->{"ref"}};
					if (exists $regfile->{entries}) {
						my $entry = $regfile->{entries}->{$val};
						if (exists $entry->{syntax}) {
							$val = $entry->{syntax}."$val";
						} else {
							$val = $ifield->{"ref"}."$val";
						}
					} elsif (exists $regfile->{prefix}) {
						$val = $regfile->{prefix}."$val";
					}
				}
        # Substitute; 
				$cur_syntax =~ s/$f/$val/;
      };
      # Save this option.
      push(@ret,$cur_syntax);
      # get next value of the ifields.
      update_bits(\@bits);
    }

  } elsif (exists $ifield->{ref}) {
    # For refernsed to a register - just prefix with value or sparse syntax
    my $regfile = $core->{regfiles}->{$ifield->{"ref"}};
    if (exists $regfile->{entries}) {
      for (my $i = 0 ; $i != 2**$ifield->{width}; ++$i) {
				if (exists $regfile->{entries}->{"$i"}) {
					my $entry = $regfile->{entries}->{"$i"};
					if (exists $entry->{syntax}) {
						push(@ret,$entry->{syntax});
					} elsif (exists $regfile->{prefix}) {
						push(@ret,"$regfile->{prefix}$i");
					} else {
						push(@ret,"$i");
					}
				} else {
					push(@ret,"reserved");
				}
      }
    } elsif (exists $regfile->{prefix}) {
      for (my $i = 0 ; $i != 2**$ifield->{width}; ++$i) {
				push(@ret,"$regfile->{prefix}$i"); 
      }
    }
  } else {
    # Default : field name.
    push(@ret,$fname); 
  }

	@ret = sort(@ret);

	return @ret;
}

# Build virtual field from real.
# Arg0: Value of the ifield as array of binary
# Arg1: Reference to the field
# Arg2: True if little endian
# Return all bits set with the given value.
sub field_bits {
  my ($bits_val,$ifield,$le) = (shift,shift,shift);
  my @ret = ();

  for (my $i = 0; $i != $ifield->{size}; ++$i) {
    $ret[$i] = 0;
  }
  my $count = $ifield->{size} - 1;

  foreach (@{$ifield->{fields}}) {
    if (ref($_) eq "ARRAY") {
      # Just reference to a field value
      my ($l,$r) = @{$_};
      $ret[$count--] = $bits_val->[$l];
    } elsif (ref($_) eq "HASH") {
      # Reference to a nested field table.
      my $sname = (keys %{$_})[0];
      my $sind = $_->{$sname};
      my $subf = $ifield->{instrfields}->{$sname};
      my $val = get_val($subf->{bits},$bits_val,$le);
      my $max = 0;

      foreach (sort  { $a <=> $b } (keys %{$subf->{table}})) {
        # Infer the width of the field
				$max = max ($max,$subf->{table}->{$_}->[$sind]); 
				if ($_ == $val) {
					$val = $subf->{table}->{$_}->[$sind];
				}
      }

      my $w =  int(log($max)/log(2));
      my $start = $count;
      my $end   = $count - $w;

      foreach (0..$w) {
				$ret[$end + $_] = $val&1;
				$val >>= 1;
				$count--;
      }
    }
  }
  ;
  return @ret;
}

# Return value based on bits
# Arg0 - array of bit ranges
# Arg1 - Value to be examined
# Arg2 - Little endian
# Return - decimal value
sub get_val {
  my ($bits,$val,$le) = (shift,shift,shift);
  my @tmp;
  my $ind = 0;
  foreach (@{$bits}) {
    my ($l,$r) = @{$_};
    if (!$le) {
      my $tmp = $l;
      $l = $r;
      $r = $tmp;
    }
    foreach ($r..$l) {
      $tmp[$ind++] = $val->[$_];
    }
  }

  return bin2dec((join("",reverse(@tmp))));
}

# Updates array of bits
# Arg0 <=> Ret - bitvector.
sub update_bits {
  my $val = shift;
  my $length = scalar @{$val};
  my $ind = 0;
  while ($val->[$ind] == 1 && $ind != $length) {
    $val->[$ind] = 0; 
    ++$ind;
  }
  if ($ind != $length) {
    $val->[$ind] = 1;
  }
}

sub bin2dec {
  return unpack("N", pack("B32", substr("0" x 32 . shift, -32)));
}

1;



