package Data::Compare::Plugins::Scalar::Numbers;

# The point of this package is to allow compare to work with numbers:
# We attempt to convert any number to a BigInt so that we can compare it
# irrespective of leading 0s, radix, etc.

use strict;
use vars qw($VERSION);
use Data::Compare;
use Math::BigInt;

$VERSION = 1.0;

sub register {
  return [
	  ['','', \&num_compare],
	  ['Math::BigInt','', \&bi_compare],
	  ['','Math::BigInt', \&bi_compare],
	 ];
}

sub bi_compare {
  my ($a,$b) = @_;
  # figure out which side is a scalar
  if ($a =~ /^(?:0x[a-fA-F0-9]|[0-9])/) {
    my $x = new Math::BigInt($a);
    my $y = $b;
  } else {
    my $x = $a;
    my $y = new Math::BigInt($b);
  }
  return 1 if($a eq $b);
  return 0;
}

# We try to stringify everything, otherwise we just do string comparison.
sub num_compare {
  my ($a,$b) = @_;

  #print "Number compare:  $a,$b\n";

  if ($a =~ /^0x(?:[a-fA-F0-9]|[0-9])+$/ && $b =~ /^0x(?:[a-fA-F0-9]|[0-9])+$/) {
	my $x = new Math::BigInt($a);
	my $y = new Math::BigInt($b);
	return $x == $y;
  }
  
  return 1 if($a eq $b);
  return 0;
}

register();

