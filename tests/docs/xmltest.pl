#!/usr/bin/env perl 
#
# Parses XML output from the dbgen program and then displays it.
#
#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#

use FindBin;
# This is used to find XML during normal usage.
use lib "$FindBin::RealBin/../../scripts";
# This uses Automake's $srcdir environment variable
# to find the XML libary.
use lib "$ENV{srcdir}/../../scripts";
use XML::Mini::Document;


if (!@ARGV) {
  print "usage:  $0 [make-db path] [adl file]\n\n";
  exit 1;
}

my $xmlDoc = XML::Mini::Document->new();
my $data = `$ARGV[0] --type=xml $ARGV[1]` or die "Error parsing Perl database.";
$xmlDoc->parse($data);
print "XML:",$xmlDoc->toString(),"\n";

