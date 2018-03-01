#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#
#
# Helper package that build encoding string of instruction.
#
# Typical output is: 100201AAAGGGFFFFFFFFFFFF
# For immediate letters "i" and "I" are used, for others
# The first letter of the field name

package EncHelper;

use Data::Dumper;

require Exporter;
our @ISA = ("Exporter"); 
our @EXPORT = ("get_enc");

# globals
$UndefChar = '*';
$ReservedChar = '*';
$EncSep = " ";
$EncPrefixBits = 1;

# Encoding types.
# abrev         = single-char string.  
# fields        = list field names with bit ranges.
# bin-fields    = list of fields with bit ranges, binary bits for opcodes.
# spread-fields = list all fields (each bit).  
$EncType = "abrev";

use strict;

use vars qw ( $UndefChar $ReservedChar $EncSep $EncType $EncPrefixBits );

# Main routine.
# Returns encoding in binary format
# Arg0    : Reference to the instruction.
# Arg1    : Reference to instruction fields db
# Arg2    : Endianness (little => true)/
# Returns : String, representing encoding in binary format.
sub gen_enc {
	if ($EncType eq "abrev") {
		return gen_abrev_enc(@_);
	} else {
		return gen_field_enc(@_);
	}
}

# Generate a pair representing the instruction's width- either including prefix
# bits or not.
sub get_ifw {
	my ($instr,$little_endian) = @_;

	if ($little_endian) {
		return [ $instr->{fetch_width}-1,0 ];
	} else {
		return [ 0,$instr->{fetch_width}-1 ];
	}
}

# Return true if arg0 is a subset of arg1.  arg2 => true = little-endian.
sub subset {
	my ($a,$b,$le) = @_;

	if (!$le) {
		return ($a->[0] >= $b->[0] && $a->[1] <= $b->[1]);
	} else {
		return ($a->[0] <= $b->[0] && $a->[1] >= $b->[1]);
	}
}

# Bits are valid either if we're including prefix bits or else if it's part of
# the actual fetch width.
# arg0:  bit range to check
# arg1:  bit range for fetch width.
# arg2:  little-endian => true
sub valid_bits {
	return 1 if ($EncPrefixBits);

	return subset(@_);
}

# Routine for generating encodings with field names, either all bits spread out
# or just each field with a bit range.
#
# Arg0    : Reference to the instruction.
# Arg1    : Reference to instruction fields db
# Arg2    : Endianness (little => true)
# Returns : String, representing encoding with all fields listed.
sub gen_field_enc {
	my ($instr,$instrfields,$little_endian) = (shift,shift,shift);  
  my $fields = $instr->{fields};
	my $ifw = get_ifw($instr,$little_endian);
	my $iwidth = ($EncPrefixBits) ? $instr->{width} : $instr->{fetch_width};

	# First, create a list of all instruction fields, then sort.  If we have an
	# opcode, add it to the instruction field data structure.
	my @ifields;

	# This will store the current count of what bits we've numbered for each
	# field.
	my $incr = ($little_endian) ? -1 : 1;
	my $counter;
	# Start the counter at the appropriate bit location, which means to not
	# include prefix bits if we're not supposed to.
	$counter = ($little_endian) ? $iwidth-1 : 0;

	# We now populate the @ifields array, which stores all elements of the
	# instruction, broken down into individual, contiguous bits.
  foreach my $fname (keys %$fields) {
		my $instrfield = $instrfields->{$fname};
		my $opc = $fields->{$fname};
		my $width = $instrfield->{width};
		my $display_name = (exists $instrfield->{implements}) ? $instrfield->{implements} : $fname;
		# A list of 0s and 1s for the entire field.
		my @bin = split //,dec2bin($opc,$instrfield->{width});
		my $start = ($little_endian) ? $instrfield->{width}-1 : 0;
		# Add on an element for each contiguous bits range of the field.
		foreach (@{$instrfield->{bits}}) {
			my $bwidth = (($little_endian) ? ($_->[0] - $_->[1]) : 
										($_->[1] - $_->[0]) );
			my $end = $start + ($bwidth * $incr);
			my $li = ($little_endian) ? ($width-$start-1) : $start;
			my $ri = ($little_endian) ? ($width-$end-1) : $end;

			push @ifields,{ 
				name   => $display_name, 
				# If not empty, then this is an opcode.
				opc    => $opc, 
				# Binary encoding: A list of 0s and 1s for the portion we care about.
				# Populated regardless of whether we're an opcode, but only relevant if
				# we are an opcode.
				bin    => [ @bin[$li..$ri] ],
				# Bit indexes within the instruction.
				bits   => $_,
				# The bit indexes of the field itself.
				fbits  => [ $start , $end ],
				# Width of the whole field.
				width  => $width,
				# Width of this portion of the field.
				bwidth => $bwidth,
			};
			$start = $end + $incr;
		}
	}	

	sub sort_ifields {
		my $a_bit = $a->{bits}->[0];
		my $b_bit = $b->{bits}->[0];
		# For little-endian, sort high to low, for big-endian, low to high.  In
		# either case, it's the left value of the pair that we use.
		if ($little_endian) {
			return $b_bit <=> $a_bit;
		} else {
			return $a_bit <=> $b_bit;
		}
	}

	# This sorts the individual contiguous bit elements into the proper order.
	@ifields = sort sort_ifields @ifields;

	my $enc = "";

  #print "Fields:  ",Dumper(\@ifields),"\n\n";

	# Pad with reserved markers, if necessary.
	sub pad_reserved {
		my ($enc,$counter,$l_bit,$r_bit) = @_;
		#print "Counter:  $counter,  [ $l_bit , $r_bit ]\n";
		if ($EncType =~ /bin-fields|long-fields/) {
			my $diff = 0;
			if (!$little_endian) {
				if ($l_bit > $counter) {
					$diff = $l_bit - $counter;
				}
			} else {
				if ($l_bit < $counter) {
					$diff = $counter - $l_bit;
				}
			}
			if ($diff > 0) {
				#print "Pad by $diff\n";
				$enc .= join($EncSep,(($ReservedChar) x $diff)).$EncSep
			}
		}
		return ($enc,$r_bit + $incr);
	}

	#print "Start enc $instr->{syntax} , counter $counter\n";

	# Now we iterate over the individual contiguous bit sequences, printing them
	# to the encoding string.
  foreach my $ifield (@ifields) {
		#print "Field:  ",Dumper($ifield),"\n\n";
		my $name = $ifield->{name};
		my $width = $ifield->{width};
		my $bwidth = $ifield->{bwidth},
		my $l = $ifield->{fbits}->[0];
		my $r = $ifield->{fbits}->[1];
		#print "Bits:  $l,$r , $bwidth\n";
		
		# Skip if not part of the instruction, i.e. we're skipping prefetch bits.
		next if (!valid_bits($ifield->{bits},$ifw,$little_endian));

		($enc,$counter) = pad_reserved($enc,$counter,$ifield->{bits}->[0],$ifield->{bits}->[1]);

			my $has_value = ($ifield->{opc} ne "");

		if ($ifield->{opc} eq "reserved") {
			if ($EncType =~ /bin-fields|long-fields/) {
				$enc .= (join $EncSep,(($ReservedChar) x ($bwidth+1))).$EncSep;
			}			
		} elsif ($has_value && ($ifield->{name} =~ /^Var(Instr|Prefix)Opcode/ || 
																			$EncType eq "fields") ) {
			# Opcode w/o name or fields encoding: Display as binary or decimal,
			# depending upon encoding type.
			if ($EncType =~ /bin-fields|long-fields/) {
				# Use just the portion of the opcode string that we need.
				$enc .= join($EncSep,@{$ifield->{bin}}) . $EncSep;
			} else {
				$enc .= sprintf("%s(%llu)%s",${name},$ifield->{opc},$EncSep);
			}
		} elsif ( $ifield->{width} == 1) {
			# Single bit field situation- just print the name.
			$enc .= "$name";
			# If we have a value, print it.
			$enc .= ":$ifield->{opc}" if ($ifield->{opc} ne "");
			$enc .= $EncSep;
		} elsif ($EncType =~ /^(fields|bin-fields)/) {
			# Field:  List field's bit range, unless single-bit, in which case just list that bit.
			if ($l eq $r) {
				$enc .= "${name}[$l]";
			} else {
				$enc .= "${name}[$l:$r]";
			}
			$enc .= ":$ifield->{opc}" if ($has_value);
			$enc .= "$EncSep";
		} elsif ($EncType eq "long-fields") {
			# Field:  For this encoding, list all field bits separately.
			my $b = $l;
			for (0 .. $bwidth ) {
				$enc .= "${name}[$b]";
				$enc .= ":$ifield->{bin}->[$_]" if ($has_value);
				$enc .= $EncSep;
				$b += $incr;
			}
		}
	}

	($enc,$counter) = pad_reserved($enc,$counter,$iwidth,$iwidth) if (length($enc) > 0);

	# The end of the string will always be the encoding separator, so remove that.
	$enc = substr($enc,0,length($enc)-length($EncSep));

	return $enc;
};

# Routine for generating an abrevated encoding, in which each binary bit is displayed, else first character
# of field, or I or i for immediate.
#
# Returns encoding in binary format
# Arg0    : Reference to the instruction.
# Arg1    : Reference to instruction fields db
# Arg2    : Endianness (little => true)/
# Returns : String, representing encoding in binary format.
sub gen_abrev_enc {  
  my ($instr,$instrfields,$little_endian) = (shift,shift,shift);  
  my $fields = $instr->{fields};  
  my $enc = $UndefChar x $instr->{width};
  my $num_imm = 0;
	my $ifw = get_ifw($instr,$little_endian);

	#print "Instr:  $instr->{syntax}\n";
  foreach my $fname (sort (keys %$fields)) {
		my $instrfield = $instrfields->{$fname};
		my $opc  = $fields->{$fname};
		my $width = $instr->{width};
		my $char;
		my $impl = exists $instrfields->{implements} ?
				$instrfields->{$instrfield->{implements}} :$instrfield;
		# Immidiates are represented as "I" or "i"
		if ((($instrfield->{type} eq "imm") or ($impl->{type} eq "imm")) 
				and ($fields->{$fname} eq "" ) ) {
			if ($num_imm == 0) {
				$char = "I";
			} elsif ($num_imm == 1) {
				$char = "i";  
			} else {
				$char = substr($fname,0,1);
			}
			++$num_imm;
		} else {
			$char = substr($fname,0,1);
		}
		# Add field to encoding string
		$enc = addinstrfield($enc,$instrfield,$opc,$fname,$width,$char,$ifw,$little_endian);
		#print "  Encoding:  $enc\n";
  }
  return $enc;  
}

# Internal function, adds stirng for every field to encoding string
# Arg0    : Input String.
# Arg1    : Reference to instrfield.
# Arg2    : Integer value of the opcode.
# Arg3    : Field name
# Arg4    : Instruction width
# Returns : Modified string;
sub addinstrfield {
  
  my ($enc,$instrfield,$opc,$fname,$width,$char,$ifw,$little_endian) = @_;
  my $bits = $instrfield->{bits};
 
	
  my $opcstr;
  if ($opc eq "") {
    # Usual field = > just use the given char
    $opcstr = $char x $instrfield->{width};
  } elsif ($opc eq "reserved") {
    $opcstr =  $ReservedChar x $instrfield->{width};
  } else {
    # Opcode field => will fill with zerose and ones
    $opcstr  = dec2bin($opc,$instrfield->{width});
  }
  my $opcstart = 0;

  foreach my $range (@$bits) {
    if (scalar @$range == 1) {
      # need to check if it can happen.
      die "unexpected range.";
		} elsif (! valid_bits($range,$ifw,$little_endian)) {
			# Skip if not in our range of interest.
			next;
    } else {
      my $rwidth = $little_endian ? ($range->[0]-$range->[1]+1) : ($range->[1]-$range->[0]+1) ;
      my $start  = $little_endian ? ($width-$range->[0]-1)      : ($range->[0]);
      # Put it on the appropriate place
      substr($enc, $start,$rwidth) = substr($opcstr,$opcstart,$rwidth);
      $opcstart += $rwidth; 
    }
  }
  return $enc;
}

# Internal function, convert decimal to a binary string.  Not as efficient as using
# pack/unpack, but that technique doesn't seem to work reliably on 64-bit integers.
# Arg0: Decimal string;
# Arg1: width of desired output binary vector
# Returns: binary string.
sub dec2bin {
  my ($input,$width) = @_;
	my $str;
	while ($input) {
		$str = ( ($input & 1) ? "1" : "0" ) . $str;
		$input >>= 1;
	}
  if ($width && length($str) != $width) {
		# Add leading zeros. 
    $str = ('0' x ($width - length($str))).$str;
  }
  return $str;
}
1;
