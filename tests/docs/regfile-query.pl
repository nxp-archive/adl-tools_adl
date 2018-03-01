#!/usr/bin/env perl 
#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#

my ($makedb,$input,$origpath,$regfile) = @ARGV;

eval `$makedb --type=perl $input` or die "Error parsing Perl database: $!";

my @path   = split /:/,$origpath;

# Get the target hash.
my $trg = $data;
for (@path) {
  $trg = $trg->{$_};
}
die "Bad path:  @path" if (!$trg);

@range = (0..$trg->{regfiles}->{$regfile}->{size}-1);
@used  = grep { exists $trg->{regfiles}->{$regfile}->{entries}->{$_}; } @range;
@avail = grep { !exists $trg->{regfiles}->{$regfile}->{entries}->{$_}; } @range;

for (@used) {  print "Used index:  $_\n";}
for (@avail) { print "Available index:  $_\n";}
