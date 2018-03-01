#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#
#
# Basic database handler.
# 
#

package BasicHandler;
require Exporter;

our @ISA = qw(Exporter);
our @EXPORT = qw(new preprocess_objects postprocess_object process_query);

use Data::Dumper;
use strict;

# Creates handler.
# Arg0 - classname
# Arg1 - core
# Arg2 - Name of objects to take
# Ret  - handler.
sub new {
  my ($class,$core,$objname) = @_;
  my $this  = {
    core => $core,
    objects => $core->{$objname},
  };
  return bless ($this, $class);
};

# Preprocess objects: converts attributes into additional fields.
# No args
sub preprocess_objects {
  my $this = shift;
  foreach (keys %{$this->{objects}}) {
    my $attrs = []; 
    my $entry = $this->{objects}->{$_};
    $entry->{name} = $_;
    if (exists $entry->{attributes}) {
      foreach my $attrname (keys %{$entry->{attributes}}) {
				push(@$attrs,$attrname);  
				my $attr = $entry->{attributes}->{$attrname};
				# Add the attribute as a key, but don't clobber anything if it's already
				# a key.
				if (!exists $entry->{$attrname}) {
					$entry->{$attrname} = $attr;
				}
      }
    }
    if (exists $entry->{blocks}) {
      $entry->{blk} = $entry->{blocks};
    } elsif (exists $entry->{block})  {
      $entry->{blk} = $entry->{block};
    }
		@$attrs = sort @$attrs;
    $entry->{attrs} = $attrs;
  }
};

# Process query in a special way, if needed.
sub process_query {
  my $this = shift;
  my $cond = shift;
  return $cond;
}

# Postrocess objects in a special way, if needed.
sub postprocess_object {
  my ($this,$entry,$le) = (shift,shift,shift);
  return $entry;
}

1;
