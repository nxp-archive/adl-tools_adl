#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#

#
# Helper functions for generating logger events.
#

use strict;
use Data::Dumper;

our @ISA = ("Exporter"); 
our @EXPORT = qw( 
				 getHandlerName printHeader parmList argList initList memberList
				 );

# Return the method name in LoggingIface, given an event name.
sub getHandlerName {

  my ($h) = @_;

  $h =~ s/_(\p{IsAlpha})/\U\1/g;
  
  return $h;
}

sub printHeader {
  my ($out) = @_;

  print $out <<HEADER;
//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This file is generated automatically by the script $0.
// DO NOT EDIT MANUALLY.
//

HEADER
}

sub updateEventParm
{
	my ($pname,$pnew,$events) = (shift,shift,shift);

	for my $event (@_) {
		my $e = $events->{$event};

		my $index = 0;
		for my $p (@$e) {
			if ($p eq $pname) {
				$e->[$index+1] = $pnew;
				last;
			}
			++$index;
		}
	}

}

sub printStdCode {
  my ($out) = @_;

  print $out <<CODE1;

#include <assert.h>
#include <vector>

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"

#include "iss/LoggingMgr.h"
#include "iss/ModelInterface.h"

#ifndef _MSC_VER
# include "iss/ThreadedLogger.h"
#endif

using namespace std;
using namespace rnumber;

namespace adl {

CODE1
}

sub printClassHeader {
	my ($out,$cname) = @_;

	print $out <<CODE2;

  struct $cname : public LoggingObjBase {

  LoggerClients _clients;

  void registerLogger(LogParentIface *parent,LoggingIface* iface)
  {
    _clients.push_back(make_pair(parent,iface));
  }
 
  void unregisterLogger(LogParentIface *parent)
  {
    auto iter = _clients.begin();
    while (iter != _clients.end()) {
      if ((*iter).first == parent) {
        _clients.erase(iter);
        break;
      }
      iter++;
    }
  }

CODE2
}

# Print an event's parameters.
sub parmList {
  my ($parms) = @_;

  my $pl;

  for (my $i = 0; $i < @$parms; $i += 2) {
	my $arg  = $parms->[$i];
	my $type = $parms->[$i+1];
	if (ref($type) eq "ARRAY") {
	  $type = $type->[0];
	}
	$pl .= ", " if ($i);
	$pl .= "$type $arg";
  }
  return $pl;
}

# Print's a list of just an event's parameter names.
# An optional prefix may be added to each element.
# $copy indicates whether data has been copied to member variables or not.
# If so, then the optional element 4 of the type value will be used as
# the argument.
sub argList {
  my ($parms,$pfx,$copy) = @_;

  my $pl;

  for (my $i = 0; $i < @$parms; $i += 2) {
		my $arg  = $parms->[$i];
		my $type = $parms->[$i+1];
		#print "arg:  $arg, type:  ",Dumper($type),"\n";
		$pl .= ", " if ($i);
		if ($copy && ref($type) eq "ARRAY") {
			if ($type->[4]) {
				$pl .= "$type->[4]";
				next;
			}
		}
		$pl .= "$pfx$arg";
  }
  return $pl;
}

# Print's a constructor initialization list using
# the event's parameters.
sub initList {
  my ($parms) = @_;

  my $pl;
  
  my $first = 1;
  for (my $i = 0; $i < @$parms; $i += 2) {
		my $arg  = $parms->[$i];
		my $type = $parms->[$i+1];
		my $constr;
		
		my $to_print = 0;
		if (ref($type) eq "ARRAY") {
			$constr = $type->[2];
			if (!$constr) {
				$to_print = 1;
			} 
		} else {
			$to_print = 1;
		}
		
		if ($to_print) {
			$pl .= ", " if ($first == 0);
			$pl .= "_$arg($arg)";
			$first = 0;
		}
		
		
  }
  return $pl;
}

# Print's a list of member variables using the
# event's parameter information.
sub memberList {
  my ($parms) = @_;

  my $pl = "\n";

  for (my $i = 0; $i < @$parms; $i += 2) {
	my $arg  = $parms->[$i];
	my $type = $parms->[$i+1];
	if (ref($type) eq "ARRAY") {
	  $type = $type->[1];
	}
	$pl .= "    $type _$arg;\n";
  }
  return $pl;
}

# Print's a constructor body using
# the event's parameters.
sub constr {
  my ($parms) = @_;
  my $pl;
  for (my $i = 0; $i < @$parms; $i += 2) {
	my $second = $parms->[$i+1];
	if (ref($second) eq "ARRAY") {
	  $pl .= $second->[2];	  
	} 
  }  
  return $pl;
}
# Print's a destructor body using
# the event's parameters.
sub destr {
  my ($parms) = @_;
  my $pl = "";
  for (my $i = 0; $i < @$parms; $i += 2) {
	my $second = $parms->[$i+1];
	if (ref($second) eq "ARRAY") {
	  $pl .= $second->[3];	  
	} 
  }  
  return $pl;
}
