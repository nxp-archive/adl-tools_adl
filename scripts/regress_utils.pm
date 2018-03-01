##
## Place regression utility functions specific to ADL in this file.
##
#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#

package regress_utils;

require Exporter;

our @ISA = ("Exporter"); 
our @EXPORT = qw( 

                 $root $binutils $scripts $designs $ttfw2 $is_external_release
                 $make_iss $adldump $make_uadl $datdiff $makedat $adldb @JitArgs
                 model_test jit_test jit_model_test jit_ext_model_test
                 tc_model_test ext_model_test asm_test objdump_test
                 asm_and_run_test asm_elf_run_test asm_ld_run_test
                 rerun_model_test latency_test round_trip_test gdb_test
                 check_results uvp2dat uvp2ame ame2dat check_ir_results
                 tracediff_inc file_diff ext_file_diff objdump_diff
                 check_latencies parseint printhex is_elf64

                );

use FindBin;
use lib $FindBin::RealBin;
use lib "$FindBin::RealBin/../share";
use rdriver;
use Data::Dumper;
use File::Basename;
use File::Temp "tempfile";
use Math::BigInt lib => 'GMP';
use Cwd;

# This imports variables that Automake defines in the makefile.  For
# consistancy, it defines them in the regression even if they're not set
# externally. If the variable is not found in the environment, then we put it
# there so that any child-processes will also see a consistent view between
# running the regression manually and being invoked by make.
my $srcvar = $ENV{srcdir};

if (!$srcvar) {
	# No srcdir variable set, so use the current directory (for now).
	$ENV{srcdir} = $src = ".";
} else {
	# It was set, so our src is probably in a different location, e.g. separate
	# src/build directories.
	$src = $srcvar;
}

# Setup of various constants used in the test harness.
sub utilsSetup {

  # This attempts to figure out the root of the build tree by looking for
  # "config.status", which should always exist at the build-root.
	$root = getRoot("regress_utils.pm");

  #print "Root:  $root\n";

  # These are various variables which are exported, so that regression scripts
  # can point to things in a reliable manner.
	$binutils  = "$root/external/binutils-adl";
	$scripts   = "$src/../../scripts";
	$designs   = "$root/designs";
	$make_iss  = "$root/bin/make-iss";
	$adldump   = "$root/bin/adl-dump";
	$make_uadl = "$root/bin/make-uadl";
	$datdiff   = "$root/bin/tracediff";
	$adldb     = "$root/scripts/adl-db";
	$makedat   = "$scripts/makedat";

	$is_external_release = ( -e "$src/../../designs/external-release-distribution" );

	# Just in case, set the srcdir environment variable, so that any child
	# processes will see the correct value.
	$ENV{srcdir} = $src;
}

# Setup it up for the default case.  This will be evaluated again if we switch
# to separate-directory execution.
utilsSetup();

# If we switch to separate-dir usage, we need to call the setup routine again.
push @setuphooks,\&utilsSetup;

use strict;

use vars qw( $scripts $make_iss $datdiff @JitArgs );

##
## Place generic helper functions here.
## <HELPERS>
##

# To find the root, we look at %INC to find from where this directory was
# loaded, then go up a level and consider that our root.  If there's no bin
# directory there, then we start at our current directory and crawl up it,
# looking for a bin directory.
sub getRoot {
  my ($base,$path,$type) = fileparse($INC{$_[0]},".pm");
  if ( -d "$path../bin" ) {
	return "$path..";
  }
  
  # If not found, then we crawl up from our current directory, looking for the
  # bin directory.
  my @dirs = split /\/+/,getcwd();
  while (@dirs) {
	my $d = join '/',@dirs;
	if ( -d "$d/bin") {
	  return $d;
	}
	pop @dirs;
  }

  # Couldn't find it, so just default to the usual case.
  return "../..";
}

# Given a string representing an integer (hex must have 0x prefix), returns
# either an integer if the string is small enough to fit within a 32-bit
# integer, or else a BigInt.
sub parseint {
  my $x = shift;

  # Handling decimal is tough, and this won't handle leading 0s, so it's overly
  # conservative.
  if (($x =~ "^0x" && length($x) <= 10) || (length($x) <= 9)) {
    return eval $x;
  } else {
    return new Math::BigInt($x);
  }
}

# Prints a hex integer.  The value may be either an integer or a BigInt.
sub printhex {
  my $x = shift;

  if (!ref($x)) {
    return sprintf "0x%x",$x;
  } elsif ($x->isa("Math::BigInt")) {
    return $x->as_hex();
  } else {
    print "printhex:  Don't know how to handle $x.\n";
    die;
  }
}

# Converts a UVP file to a .dat file. Handles mt but not mp currently.
# arg0:    UVP file name.
# arg1:    .dat file name.
sub uvp2dat {
  my ($uvp,$dat) = @_;
  
  dprint "Converting $uvp -> $dat\n";
  
  open DAT,">$dat" or die "Could not open $dat for writing.";
  open UVP,$uvp or die "Could not open $uvp for reading.";
  print DAT "#\n# Source file:  $dat\n#\n";
 
  my $read = 1;
  my @uvp_ir;
  my $curr_core;
  my $reg;
  my $data;
  my $ra;
  my $sys = "";
  my $proc;
  # catch a change from init cards to result cards because there will be no RES card if therre
  # are no instructions, but we still need to write a RESULTS tag in the dat.
  my $pfx;
  my $curr_pfx = "I";
  my @aliases = ("TEN","TPRI");
  my $aliased;
  my $altair = 0;
  my $zen = 0;
  my $mt = 0;
  my @core_stack;
  # write the init tag for the global section 
  print DAT "INIT\n";
  
  while (1) {
    $_ = (<UVP>) if ($read);
    $read = 1;
    last if (!defined $_);
    # We keep track of the design so that we know
    # the tlb4ksize so we can calculate the set for the tcards and 
    # also so that we can fix Raptors EPN/RPN fields as necessary.
		next if ( m#^\s*//# );
    if (/altair/) {
      $altair = 1;
    }
    if (/zen/) {
      $zen = 1;
    }
    if (/sys => ([a-zA-Z]+)/) {
      # MP
      $sys = "$1:";
    }
    if (/thread/) {
      # MT
      $mt = 1;
    }
    if (/ => (powerpc[0-9]*)/) {
      $proc=$1;
      $curr_core =  "$sys$proc";
      $curr_pfx = "I";
      print DAT "CORE n=:$curr_core\n";
      print DAT "INIT\n";
      push @core_stack,$curr_core;
    } elsif (/thread => \w+([0-9]+) /) {
      # thread change
      push @aliases,"NIA";
      $curr_pfx = "I";
      $curr_core = "$sys$proc:thread$1";
      print DAT "CTX n=thread cn=$1\n";
      print DAT "INIT\n";
      push @core_stack,"CTX";
    } elsif (/: T \{n="([\w]*)"; t="[ID]*"; way=([0-9]+); ep=(0x[0-9a-fA-F]+)/) {
      my $set = 0;
      my $way = $2;
      my $name = $1;
      if ($name eq "TLB4K") {
				# calculate the set (this is altair specific)
				if ($altair) {
					$set = (parseint($3)>>12)&0xff;
				} else {
					die "ERROR: $.: don't know how to calculate the tlb4k set for this processor.\n";
				}
      } 
      print DAT "TD n=$name set=$set way=$way ";
      my $done = 0;
      # handle the subsequent lines of key/value pairs
      while (<UVP>) {
				next if ( m#^\s*//# );
				if (/\};\};/) {
					$done = 1;
				}
				s/",/=/g;				#replace ", with =
				s/\},\{"/:/g;				#replace },{" with  : 
				s/\},//g;				#delete },
				s/\};//g;				#delete };
				s/d=\{//g;				#delete d={
				s/\{"//g;				#delete {"
				s/\s//g;				#remove all whitespace
				
				my @fields = split ':';
				
				#write each out to the dat file
				foreach (@fields) {
					chomp $_;
					if (($altair or $zen) && ($_ =~ /([ER]PN)=(0x[0-9a-fA-F]*)/ )) {
						# the uvp format shifts the EPN & RPN to the left by two bits
						# no booke model supports size0 pages but unfortunately Raptor
						# models it this way so all parsers have to work around this.
						my $pn = parseint($2);
						my $shift = 2;
						$pn = $pn >> $shift;
						print DAT "$1=" . printhex($pn) . " " ;
					} else {
						print DAT "$_ ";
					}
				}
				last if $done;
      }
      print DAT "\n";
    } elsif (/ : T/) {
      die "ERROR: $.: unable to parse this tcard: $_\n";
    } elsif (/([IR]) : R \{nm="([\w]+)"; d=0x([0-9a-fA-F]+);};/) {
      $pfx = $1;
      if (!($curr_pfx eq $pfx)) {
				$curr_pfx = $pfx; print DAT "\nRESULTS\n\n";
      }
      # register card
      my $reg = $2;
      my $value = $3;
      
      # We have to do conversions on some of the register names.
      if ($reg =~ /^G([0-9]+)/ ) {
				print DAT "RD n=GPR i=$1 d=0x$value\n";
      } elsif ($reg =~ /^F([0-9]+)/ ) {
				print DAT "RD n=FPR i=$1 d=0x$value\n";
      } elsif ($reg =~ /^V([0-9]+)/ ) {
				print DAT "RD n=VPR i=$1 d=0x$value\n";
      } elsif ($reg =~ /NIA/) {
				# we only print the initial IARs, see below comment
				if ($pfx eq "I") {
					print DAT "RD n=$reg d=0x$value\n";
				}
      } elsif ($reg eq "IAR") {
				# Because of the halt instruction:
				# we only print the initial IAR, we currently do not do
				# results checking on the final IAR.  We need to move
				# to comparing uvps to uvps and not uvps to dats.  Also
				# for mt, Raptor prints an IAR within each thread, this
				# is redundant since we have the NIA0,1 and 2. ABE does
				# not need the IAR cards and ADL does not print them.   
				if (($pfx eq "I") && !$mt) {
					print DAT "RD n=NIA d=0x$value\n";
				}
      } elsif ($reg eq "VPIA0") {
				#ignore pseudo register VPIA0
      } elsif ($reg eq "AVPID0") {
				# adl models some thread-switching so AVPID0 will not
				# match between raptor and adliss
      } else {
				$aliased = 0;
				#make sure this isn't an alias
				foreach (@aliases) {
					if ($_ eq $reg) {
						$aliased = 1;
					}
				}
				if (!$aliased) {
					print DAT "RD n=$reg d=0x$value\n";
				}
      }
    } elsif (/([IR]) : D \{ea=0x([0-9a-fA-F]+); ra=0x([0-9a-fA-F]+); d=0x([0-9a-fA-F]+);(?: ta=(0x[0-9a-fA-F]+))?/) {
      # data card (ea,ra,data,ta form)
      $pfx = $1;
      if (!($curr_pfx eq $pfx)) {
				$curr_pfx = $pfx; print DAT "\nRESULTS\n\n";
      }
      print DAT "MD n=Mem ra=0x$3 d=0x$4\n";
      
    } elsif (/([IR]) : D \{ra=0x([0-9a-fA-F]+); d=0x([0-9a-fA-F]+);(?: ta=(0x[0-9a-fA-F]+))?/) {
      # data card (ra,data,ta form)
      $pfx = $1;
      if (!($curr_pfx eq $pfx)) {
				$curr_pfx = $pfx; print DAT "\nRESULTS\n\n";
      }
      print DAT "MD n=Mem ra=0x$2 d=0x$3\n";
    } elsif (/RES/) {
      # res card
      $curr_pfx = "R"; 
      # We've reached the result section, so we write out our trace data to the DAT
      # file before listing final results.
      print DAT "\nTRACE\n\n";
      @uvp_ir = reverse @uvp_ir;
      my $irs;
      while (@uvp_ir) {
				$irs = pop @uvp_ir;
				for ( @{$irs} ) {
					print DAT;
				}
      }
      
      print DAT "\nRESULTS\n\n";
    } elsif (/E : [R]*I \{asm=\"([\s\S]+)\";\s+c=([0-9]+)/) {
      my $instr_num = $2;
      my $instr = $1;
      my $type = ( /t=\"(\w+)\"/ )[0];
      $_ = (<UVP>);
      if (/ea=0x([0-9a-fA-F]+); ra=0x([0-9a-fA-F]+); d=0x([0-9a-fA-F]+); op=0x([0-9a-fA-F]+);(?: ta=(0x[0-9a-fA-F]+))?/) {
				print DAT "MD n=Mem ra=0x$2 d=0x$3  # $instr\n";
				if ($type !~ /^[NP]/) {
					my $frame = [];
					push @uvp_ir,$frame;
					push @$frame,"\nI # $instr_num  $instr\n";
					push @$frame,"M n=Mem t=ifetch ea=0x$1 ra=0x$2 d=0x$3\n";
					push @$frame,"INSTR op=0x$4 asm=\"$instr\"\n";
					while (<UVP>) {
						next if (/dep/);	#ignore dependency tracking
						next if ( m#^\s*(ir=\{)?//# );
						#print "IR line:  $_\n";
						if (/tea/) {
						} elsif ( /reg\", rn=>\"([A-Z0-9]+)\", d=>0x([0-9a-fA-F]*)/) {
							my ($reg,$index,$value) = ($1,"",$2);
							# We have to do conversions on some of the register names.
							if ($reg =~ /^G([0-9]+)/ ) {
								$reg = "GPR";
								$index = "i=$1";
							} elsif ($reg =~ /^F([0-9]+)/ ) {
								$reg = "FPR";
								$index = "i=$1";
							} elsif ($reg =~ /^V([0-9]+)/ ) {
								$reg = "VPR";
								$index = "i=$1";
							}
							push @$frame,"R n=$reg $index d=0x$value\n";
						} elsif ( /store\",\s+ra=>0x([0-9a-fA-F]+),\s+d=>0x([0-9a-fA-F]+)/ ) {
							push @$frame,"M n=Mem t=write ra=0x$1 d=0x$2\n";
						} elsif ( /load\",\s+ra=>0x([0-9a-fA-F]+),\s+d=>0x([0-9a-fA-F]+)/ ) {
							push @$frame,"M n=Mem t=read ra=0x$1 d=0x$2\n";
						} elsif ( /tlb\",\s+rn=>\"(\w+)\",\s+way=>(\d+),\s+ep=>0x[0-9a-fA-F]+/) {
							my $f = "T n=$1 t=write way=$2 ";
							my $d;
							while ( <UVP> ) {
								$d .= $_;
								last if ( /],};/ );
							}
							my @pairs = ($d =~ /{\"\w+\",0x[a-fA-F0-9]+}/g );
							for (@pairs) {
								m/\"(\w+)\",(0x[a-fA-F0-9]+)/;
								$f .= "$1=$2 ";
							}
							push @$frame,"$f\n";
						} else {
							# Break out of ir-record loop.
							$read = 0;
							last;
						}
					}
				}
      } else {
				print "ERROR: unrecognized instruction card\n $_";
      }
    } elsif (/\} \/\//) {
      #we've exited a scope, remove ourself and get the next core tag
      my $tmp_core = pop @core_stack;
			if ($tmp_core eq "CTX") {
				print DAT "NOCTX\n";
			} else {
				$curr_core = $tmp_core;
				if ($curr_core) {
					print DAT "CORE n=:$curr_core\n";
					print DAT "RESULTS\n";
				}
			}
    } 
  }
  close UVP;
  close DAT;
}


# Converts a UVP file to an AME command file. Handles mt but not mp currently.
# arg0:    UVP file name.
# arg1:    AME command file name.
# assumption/limitation:
#   init cards precede result cards
#   one test per UVP file
sub uvp2ame {
  my ($uvp, $ame) = @_;
  
  open AME, ">$ame" or die "Could not open $ame for writing.";
  open UVP, $uvp or die "Could not open $uvp for reading.";
  print AME "#\n# Source file:  $uvp\n#\n";
 
  my $read = 1;
  my $curr_core;
  my $reg;
  my $data;
  my $ra;
  my $sys = "";
  my $proc;
  # catch a change from init cards to result cards because there will be no RES
  # card if there are no instructions
  my $curr_pfx = "I";
  my @aliases = ("TEN","TPRI");
  my $aliased;
  my $altair = 0;
  my $zen = 0;
  my $mt = 0;
  my @core_stack;
  
  # command sequence to set up log, run program and display register contents
  my $AME_go_cmd = "\nlog ipipe\n\ng\n\nrd\n\n";

  while (1) {
    $_ = (<UVP>) if ($read);
    $read = 1;
    last if (!defined $_);
    # We keep track of the design so that we know
    # the tlb4ksize so we can calculate the set for the tcards and 
    # also so that we can fix Raptors EPN/RPN fields as necessary.
    next if (m#^\s*//#);

    if (/altair/) { $altair = 1; }
    if (/zen/) { $zen = 1; }
    # MP
    if (/sys => ([a-zA-Z]+)/) { $sys = "$1:"; }
    # MT
    if (/thread/) { $mt = 1; }

    if (/ => (powerpc[0-9]*)/) {
      die("error: init cards after result cards\n") if ($curr_pfx eq "R");

      $proc = $1;
      $curr_core =  "$sys$proc";
      push @core_stack, $curr_core;
    }
    elsif (/thread => \w+([0-9]+) /) {
      # thread change
      die("error: init cards after result cards\n") if ($curr_pfx eq "R");

      push @aliases,"NIA";
      $curr_core = "$sys$proc:thread$1";
      push @core_stack, "CTX";
    }
    elsif (/: T \{n="([\w]*)"; t="[ID]+"; way=([0-9]+); ep=(0x[0-9a-fA-F]+)/) {}
    elsif (/ : T/) { die "error: unable to parse this tcard: $_\n"; }
    elsif (/I : R \{nm="([\w]+)"; d=0x([0-9a-fA-F]+);};/) {
      # register card
      die("error: init cards after result cards\n") if ($curr_pfx eq "R");

      my ($reg, $value) = ($1, $2);
      my $GPR = 0;
      
      # We have to do conversions on some of the register names.
      if ($reg =~ /^G([0-9]+)/ ) {
        $reg = $1;
        $GPR = 1;
      }
      elsif ($reg =~ /^[FV]([0-9]+)/ ) {
        die("error: AME does not support FPR or VPR.\n");
      }
      elsif ($reg =~ /NIA/) {
        $reg = "pc";
      }
      elsif ($reg eq "IAR") {
	if (!$mt) { $reg = "pc"; }
        else { $reg = ""; }
      }
      elsif ($reg eq "VPIA0" or $reg eq "AVPID0") {
        # (*) ignore pseudo register VPIA0
	# (*) adl models some thread-switching so AVPID0 will not match between
        #     raptor and adliss
        $reg = "";
      }
      else {
	# make sure this isn't an alias
	foreach (@aliases) {
	  if ($_ eq $reg) {
            $reg = "";
            last;
          }
	}
      }

      if ($GPR) { # $GPR implies $reg
        my @data = split(//, $value);

        if (@data > 8) {
          my $upper_data = join('', splice(@data, 0, @data - 8));
          print AME "rm rh$reg $upper_data\n";
        }

        my $lower_data = join('', @data);
        print AME "rm r$reg  $lower_data\n";
      }
      elsif ($reg) {
        print AME "rm $reg $value\n";
      }
    }
    elsif (/([IR]) : D \{ea=0x([0-9a-fA-F]+); ra=0x([0-9a-fA-F]+); d=0x([0-9a-fA-F]+);(?: ta=(0x[0-9a-fA-F]+))?/) {
      # data card (ea, ra, data, ta form)
      my ($pfx, $ea, $ra, $data, $ta) = ($1, $2, $3, $4, $5);

      if ($curr_pfx ne $pfx) {
        if ($curr_pfx eq "I") {
          $curr_pfx = "R";
          print AME $AME_go_cmd;
        }
        else {
          die("error: init cards after result cards\n");
        }
      }

      if ($curr_pfx eq "I") { print AME "mm -w $ea $data\n"; }
      else { print AME "md -w $ea\n"; }
    }
    elsif (/([IR]) : D \{ra=0x([0-9a-fA-F]+); d=0x([0-9a-fA-F]+);(?: ta=(0x[0-9a-fA-F]+))?/) {
      # data card (ra, data, ta form)
      my ($pfx, $ra, $data, $ta) = ($1, $2, $3, $4);

      if ($curr_pfx ne $pfx) {
        if ($curr_pfx eq "I") {
          $curr_pfx = "R";
          print AME $AME_go_cmd;
        }
        else {
          die("error: init cards after result cards\n");
        }
      }

      if ($curr_pfx eq "I") { print AME "mm -w $ra $data\n"; }
      else { print AME "md -w $ra\n"; }
    }
    elsif (/RES/ or /R : R \{nm="([\w]+)"; d=0x([0-9a-fA-F]+);};/) {
      # result card
      if ($curr_pfx eq "I") {
        $curr_pfx = "R";
        print AME $AME_go_cmd;
      }
    }
    elsif (/E : I \{asm=\"([\s\S]+)\";\s+c=([0-9]+)/) {
      # instruction card
      die("error: init cards after result cards\n") if ($curr_pfx eq "R");

      my $type = ( /t=\"(\w+)\"/ )[0];
      $_ = (<UVP>);

      if (/ea=0x([0-9a-fA-F]+); ra=[^;]+; d=[^;]+; op=0x([0-9a-fA-F]+);/) {
        my ($ea, $opc) = ($1, $2);
        my $size;

        # determine instruction size
        if (/op_size=(\d+);/) { $size = $1; }
        else { $size = 32; }

        if ($size == 32) { $size = "-w"; }
        elsif ($size == 16) { $size = "-h"; }
        else { die("error: invalid instruction size: $size\n$_"); }

        print AME "mm $size $ea $opc\n";

	if ($type !~ /^[NP]/) {
	  while (<UVP>) {
	    next if (/dep/); # ignore dependency tracking
	    next if (m#^\s*//#);

	    if (/tea/ or /reg\", rn=>\"[A-Z0-9]+\", d=>0x[0-9a-fA-F]*/ or
	        /store\", ra=>0x[0-9a-fA-F]+, d=>0x[0-9a-fA-F]+/ or
                /load\", ra=>0x[0-9a-fA-F]+, d=>0x[0-9a-fA-F]+/) {}
	    elsif (/tlb\", rn=>\"\w+\", way=>\d+, ep=>0x[0-9a-fA-F]+/) {
	      while (<UVP>) { last if (/],};/); }
            }
	    else { # Break out of ir-record loop.
	      $read = 0;
	      last;
	    }
	  }
	}
      }
      else {
        print "error: unrecognized instruction card\n$_";
      }
    }
    elsif (/E : [R]+I \{asm=\"([\s\S]+)\";\s+c=([0-9]+)/) {}
    elsif (/\} \/\//) {}
    elsif (/^\s*\}\s*$/) {
      # we've exited a scope
      last;
    } 
  }

  # AME quit command
  # NOTE: There SHOULD NOT be any blank line between the last 'md' command and
  #       'q', otherwise unnecessary lines of memory data will be displayed.
  print AME "q\n";

  close UVP;
  close AME;
}


# convert an AME ipipe report (with register data) to DAT format
sub ame2dat($$$) { # args: $core, $AMEiPipeFile, $AMEDatFile
  my ($core, $AMEiPipeFile, $AMEDatFile) = @_;

  open(AMEFile, "<$AMEiPipeFile")
    or die("error: cannot open $AMEiPipeFile for read\n");
  open(DatFile, ">$AMEDatFile")
    or die("error: cannot open $AMEDatFile for write\n");

  # output prologue
  print DatFile "CORE n=:$core\n\n";
  print DatFile "TRACE\n";

  # parse AME output file
  my $hx = "[0-9a-f]+"; # heximal number pattern
                        # 'hx' is not a good name, but can squeeze 8 in one line
  my $phase = 1;  # phase 1: instruction trace (commit time)
                  # phase 2: register results
                  # phase 3: memory results
  my $instId = 1; # instruction id

  # ignore initial six lines (initial register file data)
  <AMEFile>; <AMEFile>; <AMEFile>; <AMEFile>; <AMEFile>; <AMEFile>; 

  while (my $line = <AMEFile>) {
    if ($phase == 1) {
      if ($line =~ /^( +(\d+).+)WrBack/) {
        # extract prefix size and time
        # increment time because commit time is defined as when an instruction
        # LEAVES WrBack
        my ($prefixSize, $time) = (length($1), $2);

        <AMEFile>; # banner
        $line = <AMEFile>;

        if ($line =~ /^.{$prefixSize}($hx)/) {
          # has instruction at WB, record it
          my $addr = $1;
          print DatFile "I ea=0x$addr id=$instId\n";
          print DatFile "CTIME t=$time\n\n";
          # increment instruction id
          $instId++;
        }

        # ignore a few lines
        <AMEFile>; # opcode
        <AMEFile>; # assembly mnemonic
        <AMEFile>; # assembly code
        <AMEFile>; # ipid
        <AMEFile>; # blank line
      }
      elsif ($line =~ /PC/) {
        # switch to phase 2: register results
        $phase = 2;
        print DatFile "\nRESULTS\n";
        # PC (NIA) is not compared, so discard current line
        $line = <AMEFile>;
      }
    }

    if ($phase == 2) {
      # SPRs
      $line =~ /^\s+CR ($hx)\s+XER ($hx)\s+CTR ($hx)\s+LR ($hx)/o
        or die("error: register results not found\n");
      my ($CR, $XER, $CTR, $LR) = ($1, $2, $3, $4);

      # GPRs
      my @GPR = ();

      $line = <AMEFile>;
      $line =~ /^  R0-R7 ($hx) ($hx) ($hx) ($hx) ($hx) ($hx) ($hx) ($hx)/o
        or die("error: register results not found\n");
      push(@GPR, $1, $2, $3, $4, $5, $6, $7, $8);

      $line = <AMEFile>;
      $line =~ /^ R8-R15 ($hx) ($hx) ($hx) ($hx) ($hx) ($hx) ($hx) ($hx)/o
        or die("error: register results not found\n");
      push(@GPR, $1, $2, $3, $4, $5, $6, $7, $8);

      $line = <AMEFile>;
      $line =~ /^R16-R23 ($hx) ($hx) ($hx) ($hx) ($hx) ($hx) ($hx) ($hx)/o
        or die("error: register results not found\n");
      push(@GPR, $1, $2, $3, $4, $5, $6, $7, $8);

      $line = <AMEFile>;
      $line =~ /^R24-R31 ($hx) ($hx) ($hx) ($hx) ($hx) ($hx) ($hx) ($hx)/o
        or die("error: register results not found\n");
      push(@GPR, $1, $2, $3, $4, $5, $6, $7, $8);

      # write to dat file
      print DatFile "RD n=CR d=0x$CR\n";
      print DatFile "RD n=LR d=0x$LR\n";
      print DatFile "RD n=XER d=0x$XER\n";
      print DatFile "RD n=CTR d=0x$CTR\n";

      for (my $i = 0; $i < 32; $i++) {
        print DatFile "RD n=GPR i=$i d=0x$GPR[$i]\n";
      }

      # switch to phase 3: memory results
      $phase = 3;
      print DatFile "\n";
      $line = <AMEFile>;
    }

    if ($phase == 3) {
      # This pattern has trailing useless information added to mismatch the
      # first memory data line, which is for PC (NIA)
      if ($line =~ /($hx): ($hx) $hx $hx $hx/o) {
        my ($addr, $data) = ($1, $2);
        print DatFile "MD n=Mem ra=0x$addr d=0x$data\n";
      }
    }
  }

  close(AMEFile);
  close(DatFile);
}


# This is the old PERL DAT reader.
# Parses simulation data, returning a ref to a hash of hash of hashes.  The
# outer-most hash stores cores by name and memory in a hash named "memory".  If
# no core is specified, then there will be a single entry with an empty name.
#
# The data hashes are:
#  "RD"        :  Register data:  { <reg name> => <value> }  (register name has its index appended)
#  "MD"        :  Memory data:    { <address> => <value> }
#  "CD"        :  Cache data.     { < type_level_addr > => { valid => <value>, dirty => <value>, data => <list of words> } }
#  "TD"        :  TLB data.       { < name_set_way > => { <data> } }
#  "__trace"   :  List of frames.  Each frame is a list of intermediate results.
#  "__excludes":  List of keys which should not exist for this core.  This data can bet set by listing keys in a
#                 comment block of name "excludes".
#
# arg0 :  File handle reference.
# arg1 :  File name, for error message purposes.
sub parse_sim_data {
  my ($input,$file,$first) = @_;

  # Initial entry stores non-MP/MT testcase data.
  my $cur = newcore();
  my $mem = newcore();
  my $frame;
  my $cores = { "" => $cur, memory => $mem };
  my ($record_trace, $record_results);
  my @cmts;

  my $lc = 0;

  my $done = 1;

  dlprint 3,"Parsing $file\n";

  while (1) {
	$_ = <$input>;
	last if (!defined $_);
	++$lc;
	if (/^TEST/) {
	  if ($first) {
		$first = 0;
		next;
	  } else {
		$done = 0;
		last;
	  }
	}
    if (/^TRACE/) {
	  dlprint 3,"  $lc:  Trace section\n";
      $record_trace = 1;
      next;
    } elsif (/^RESULTS/) {
	  dlprint 3,"  $lc:  Results section\n";
      $record_results = 1;
      $record_trace =0;
      next;
    } elsif (/^INIT/) {
	  dlprint 3,"  $lc:  Init section\n";
      $record_results = 0;
      $record_trace = 0;
      next;
    }
    next if (!/^\s*(\S+)(.*)/);
    my $type = $1;
    my $line = $2;

    if ($type =~ /^#/ ) {
      next;
	} elsif ($type eq "=") {
	  $line =~ /\s+(\S+)/;
	  my $id = $1;
	  if ($id =~ /^\/(\S+)/) {
		if ($1 ne (pop @cmts)) {
		  die "$file:$lc:  Incorrectly nested multi-line comments.\n";
		}
	  } else {
		push @cmts,$id;
	  }
	  next;
	}

	# Skip if we're in the middle of a multi-line comment.
	if (@cmts) {
	  # Any special processing of comment blocks.
	  if ($cmts[$#cmts] eq "excludes") {
		my @e = split ' ',$_;
		push @{$cur->{__excludes}},@e;
	  }
	  next;
	}

	# Gather up all pair data.  The key is just a simple identifier while the
	# value can be either a simple identifier (which includes numbers) or a
	# double-quote delimited string.  In the later case, we handle escaped
	# quotes.
	my %data = $line =~ /(\S+)=((?:\"(?:\\.|[^\\\"])*\")|(?:\S+))/g;

	dlprint 4,"Type:  $type, Data pairs:  ",Dumper(\%data),"\n";

    if ($type eq "CORE") {
	  my $current_core = $data{n};
	  $current_core = ($current_core =~ /^:/) ? $current_core : ":$current_core";
	  if (!exists $cores->{$current_core}) {
		$cores->{$current_core} = newcore();
	  }
	  $cur = $cores->{$current_core};
	  next;
	}

    next if (!($record_trace || $record_results));

	if ($type eq "I") {
      my $instr = $cur->{__trace};
      push @$instr,($frame = []);	  
    } elsif ($type eq "INSTR") {
	  # Skip for now.
	} elsif ($type eq "RD") {
	  $cur->{$type}->{regkey(\%data)} = parseint $data{d};
    } elsif ($type eq "MD") {
	  $mem->{$type}->{parseint $data{ra}} = parseint $data{d};	
	} elsif ($type eq "CD") {
	  $cur->{$type}->{cachekey(\%data)} = \%data;
	} elsif ($type eq "B") {
	  # We treat this in a special manner, since we want to add the type field.
	  push @$frame,[$type,{ ea => $data{ea}, t => "brk" }];
	} elsif ($type eq "TD") {
	  $cur->{$type}->{tlbkey(\%data)} = \%data;	  
    } else {
	  # Anything else is simply pushed onto the current frame if in
	  # trace mode, otherwise ignored w/a warning.
	  if ($record_trace) {
		push @$frame,[$type,\%data];
	  } else {
		print "$file:$lc:  Warning:  Unknown data type $type\n";
	  }
    }
  }

  return ($cores,$done);
}

# Compares simulation output with expected values in a .dat file.
# Handles intermediate and final result checking.
# arg0:  .dat (expects) file name.
# arg1:  Simulation results file name.
# arg2:  Quiet:  Do not print error messages.
# arg3:  Do not do length checking on IR traces.
# arg4:  Maximum number of tests to check.
# arg5:  List reference, listing registers for tracediff to ignore.
#        May also contain "extra-mem", which means to ignore extra memory
#        not in the found file.
#        May also be of the form "field:<name>", in which case the field <name> will
#        be ignored or "card:<name>", in which case the card <name> will be ignored, or
#        "cache:<name", in which case the cache <name> will be ignored.
# arg6:  Any extra tracediff flags.
sub check_results {

  my ($infile,$outfile,$quiet,$no_ir_lc,$max_tests,$ignoreregs,$flags) = @_;

  my $cmd = "$datdiff $infile $outfile";

  $cmd .= " --quiet" if ($quiet);

  $cmd .= " --verbose=$debug" if ($debug);

  $cmd .= " --no-ir-length-check" if ($no_ir_lc);

  $cmd .= " --max-tests=$max_tests" if ($max_tests);

  $cmd .= " $flags" if ($flags);

  if ($ignoreregs) {
		for (@$ignoreregs) {
			#print "Ignore item:  $_\n";
			if ( $_ eq "extra-mem" ) {
				$cmd .= " --ignore-extra-mem";
			} elsif ( /field:(\S+)/ ) {
				$cmd .= " --ignore-field=$1";
			} elsif ( /cache:(\S+)/ ) {
				$cmd .= " --ignore-cache=$1";
			} elsif ( /card:(\S+)/ ) {
				$cmd .= " --ignore-card=$1";
			} else {
				$cmd .= " --ignore-reg=$_";
			}
		}
  }

  dprint "Executing $cmd\n";

  system ($cmd) and die "Comparison failed ($infile (expected) vs. $outfile (found)).\n";
}

# Runs diff on two files.
# arg0:  Generated file- will be erased if diff finds no differences.
# arg1:  Reference file.
# arg2:  If set to 1, do not delete the generated file.
sub file_diff {
  my ($gen,$exp,$nodel) = @_;
  dprint ("Comparing $gen (found) vs. $exp (expected)\n");
  if (system "diff -w $gen $exp") {
	print "Differences were found.\n",
	  " <---- Generated $gen | Reference $exp ---->\n";
	die;
  }
	if (!$keepoutput) {
		dprint("Unlinking $gen\n");
		unlink $gen;
	}
}

#to check tracediff in incremetal mode
sub tracediff_inc {
  my ($gen,$exp) = @_;
  if (system "$datdiff --inc-scan --max-cmd=1 $gen $exp") {
	print "Differences were found for tracediff in incremental mode.\n",
	die;
  }
}

# An extended diff routine.
# Takes named parameters:
# in:      Input object file.
# exp:     Reference file.
# nodel:   Do not delete file when done.
# start:   If defined, regular expression to match to start checking.
# stop:    If defined, regular expression to match to stop checking.
# stripcr: Remove all newlines from the input file.
# trans:   Apply a substitution regular expression to each line of the file.
#          The format is [ <from> , <to> ].
sub ext_file_diff {
  my %args = @_;
  
  my ($in,$exp) = ($args{in},$args{exp});

  my $out;

  my $b = 0;
  my $stripcr = $args{stripcr};
  if (exists $args{start} || exists $args{stop} || exists $args{trans} || $stripcr) {
    #print "Opening $in, creating $in.out\n";
    $out = "$in.out";
    open IN,$in or die "Could not open $in\n";
    open OUT,">$out" or die "Could not open output file $out\n";
    my $b = (exists $args{start}) ? 0 : 1;
    my ($trans_cmp,$trans_sub);
    if ( exists $args{trans} ) {
      $trans_cmp = qr/$args{trans}->[0]/;
      $trans_sub = qr/$args{trans}->[1]/;
    }
    while (<IN>) {
      #print "IN:  $_\n";
      $b = 1 if ( !$b && /^$args{start}/ );
      s/\n//g if $stripcr;
      if ( $trans_cmp ) {
	s/$trans_cmp/$trans_sub/;
      }
      print OUT if ($b);
      last if ($args{stop} && /^$args{stop}/ );
    }
    close IN;
    close OUT;
	
    if ($stripcr) {
      my ($ebase,$epath) = fileparse($exp);
      my $exp_orig = $exp;
      $exp = "$ebase.out";
      open IN,$exp_orig or die "Could not open $exp_orig\n";
      open OUT,">$exp" or die "Could not open output file $exp\n";
      while (<IN>) {
	s/\n//g if $stripcr;
	print OUT if ($b);
      }
      close IN;
      close OUT;
    }
	
    file_diff($out,$exp);

    unlink $out unless $keepoutput;
    if ($stripcr) {
      unlink $exp unless $keepoutput;
    }
  } else {
    file_diff($in,$exp);
  }
}

# Looks at the DAT file to see if a TRACE section exists.
# arg0:  DAT file to check.
sub check_for_trace_section {
  my ($file) = @_;

  my $rc;
  
  open IN,$file or die "Could not open $file for checking for a TRACE section.";
  while (<IN>) {
    if (/^TRACE/) {
      $rc = 1;
      last;
    }
  }
  close IN;

  return $rc;
}

# Constructs a checker function which calls check_results on the output
# using the same datfile for checking.
sub make_checker {
  my ($file,$fail,$resfn,$no_ir_lc,$ignoreregs,$max_tests,$td_flags) = @_;
  my ($irfn,$vp_ir,$checker);

  my ($base,$path,$type) = fileparse($file,qr/\.(uvp|dat|elf|dbz2)/);

  my $vpfn = "$path/$base$type";
  
  # If the input file is a UVP then we want the output to also be a UVP so that
  # we can easily compare the two.  An output DAT cannot be easily compared to a
  # UVP because of issues such as cache state vs. memory, etc.
  # If no result file was explicitly set then construct an output file of the same type
  # as the input, unless the input is an elf, in which case the output is a dat.
  if (!$resfn) {
		if ($type eq ".elf" || $type eq ".dat") {
			$resfn = "$base.out.dat";
		} elsif ($type eq ".uvp") {
			$resfn = "$base.out.uvp";
		} else {
			die "Unknown input file type '$type'";
		}
  }
  
  # Parse the output file to get its type.
  my ($rbase,$rpath,$rtype) = fileparse($resfn,qr/\.(uvp|dat|elf)/);

  my ($pre,$checker);

  my $input  = $file;

  my @temps;

  if  ($type eq ".uvp") {
		my ($expects,$found) = ($input,$resfn);
		# If the input and output files are of the same type, then tracediff can
		# compare them directly.  Otherwise, we have to convert the output UVP to a
		# DAT.
		if ($rtype ne ".uvp") {
			$expects = "$base.gen.dat";
			push @temps,$expects;
			$pre = sub {
				dprint "Converting input UVP to DAT.\n";
				uvp2dat($vpfn,$expects);
				dprint "Done with conversion.\n";
			};
		}

    $checker = sub {
			check_results($expects,$found,$fail,$no_ir_lc,$max_tests,$ignoreregs,$td_flags);
    };

  } elsif ($type eq ".elf") {
    # elf file format:  checking comes from a third file
    my $datfn = "$path/$base.dat";
    
    $checker = sub {
      check_results($datfn,$resfn,$fail,$no_ir_lc,$max_tests,$ignoreregs,$td_flags);
    };
  } else {
    # dat file format:  just check results
    $checker = sub {
      check_results($input,$resfn,$fail,$no_ir_lc,$max_tests,$ignoreregs,$td_flags);
    };
  }

  push @temps,$resfn;

  return ($pre,$checker,$input,$resfn,\@temps);
}

# This is similar to model_test, except that it takes named parameters and is more parameterized.
#
# Args:
#
# model:      The model to execute.
# flags:      Any additional flags to supply to the model.
# results:    Testcase file containing expected results.
# input:      Input file to use.  Defaults to 'results' file if not specified.
# output:     Output file produced by the model.  Defaults to <result>.out.<ext>.
# omitout:    If specified, do not specify the output on the command line (omits -o=<file>).
# fail:       1 => Test is expected to fail, 0 => Test is not expected to fail.
# no_ir_lc:   1 => Intermediate-results length checking should not be performed.
#             0 => Length checking should be done.  If the number of instructions in the expects file
#                  differs from the found file, an error will be produced.
# ignores:    Array ref. to a list of register names to be ignored during the trace comparison.
#             If this contains the string "extra-mem", then extra memory is ignored.
#             If it contains a string of the form "field:<name>", then that field is ignored.
#             If it contains a string of the form "card:<name>", then that card is ignored.
# max_tests:  Specify a maximum number of tests to be checked.
# td_flags:   Any other tracediff flags to be used during the trace comparison process.
# retries:    Max retry attempts
# temps:      Array ref. to a list of files to be deleted once the test has completed.
# checker:    Subroutine ref. to use as a checker.  This user-supplied checker is called first, followed by
#             the default checker.
# stdio:      1 => Capture standard output.  The checker subroutine receives this data as its first argument.
#             0 => Do not capture standard output.
# stderr      1 => Capture standard error.  This is handled the same way as standard output.  If both are captured,
#                  then both data streams are supplied via the first argument.
#             0 => Do not capture standard error.
sub ext_model_test {
  my (%options) = @_;

  my ($pre,$checker,$input,$output,$temps,$td_flags) = make_checker($options{results},$options{fail},$options{output},
																	$options{no_ir_lc},$options{ignores},$options{max_tests},
																    $options{td_flags});

  # Override only if user-specified.
  $input  = $options{input} if (exists $options{input});

  push @$temps,$options{temps};

  # Also call a user checker, if one is specified.
  if ( $options{checker} ) {
		$checker = sub { &{ $options{checker} }; $checker; };
  }

	my $outcmd = (!$options{omitout}) ? "-o=$output" : "";

  return {
		  cmd         => "$options{model} $options{flags} $input $outcmd",
		  checker     => $checker,
		  pre         => $pre,
		  temps       => $temps,
		  failchecker => $options{fail},
		  stdio       => $options{stdio},
		  stderr      => $options{stderr},
			retries     => $options{retries},
	 };
}

# Constructs a list of entries for a given model.
# arg0:  Model to run.
# arg1:  Source file.
# arg2:  Whether the test should fail or not (0 => no fail, 1 => fail).
# arg3:  Results file name.  If undefined or 0, the default name is used.
# arg4:  Reference to a list of registers to ignore.
#        If this contains the string "extra-mem", then extra memory is ignored.
#        If it contains a string of the form "field:<name>", then that field is ignored.
#        If it contains a string of the form "cache:<name>", thne that cache is ignored.
# arg5:  Any tracediff flags (string).
# arg6:  Retry count, if desired.
sub model_test {

  my ($model,$file,$fail,$output,$ignoreregs,$tdflags,$retries) = @_;

  my ($base,$path,$type) = fileparse($file,qr/\.(uvp|dat|elf|dbz2)/);

  my ($pre,$checker,$input,$output,$temps) = make_checker($file,$fail,$output,0,$ignoreregs,0,$tdflags);

  if (($type eq ".uvp")) {
    return {
			cmd => "$model -trace $input -o=$output",
			checker => $checker,
			pre => $pre,
			temps => $temps,
			failchecker => $fail,
			retries => $retries,
		};
  }
  elsif ($type eq ".dat" || $type eq ".dbz2") {
		# If the target file doesn't exist, then assume no trace.  The target file
		# may not yet exist because it has to be generated or extracted at the time
		# that the test is run.
		if ($sepdirs) {
			# If we're in separate-directory mode, then change relative paths back to
			# the current directory so that we can find the file.
			$file =~ s#^\.\.#.#;
		}
    my $trace = (-e $file) ? ((check_for_trace_section($file)) ? "--trace" : "") : ""; 
    return {
			cmd => "$model $trace $input -o=$output",
			checker => $checker,
			pre => $pre,
			temps => $temps,
			failchecker => $fail,
			retries => $retries,
		};
  }
  elsif ($type eq ".elf") {
    return {
			cmd => "$model -if=elf $input -o=$output",
			checker => $checker,
			pre => $pre,
			temps => $temps,
			failchecker => $fail,
			retries => $retries,
		};
  } else {
    die "$file: Unrecognized file type '$type':  We only recognize .dat, .elf, and .uvp files.";
  }
}

# List various argument combinations for JIT and decode-cache tests.  The JIT
# tests must precede all decode-cache tests.
@JitArgs = (
	# Normal cold-block interpreting enabled with decode cache.
	" --jit --dc-hot-count=4 --jit-hot-count=10",
	# No cold-block interpreting, just decode-cache and JIT.  No MT for the JIT.
	" --jit --dc-hot-count=0 --jit-hot-count=10 --no-jit-mt",
	# Cold-block interpreting disabled- everything is JIT'd.
	" --jit --dc-hot-count=0 --jit-hot-count=0",
	# Cold-block interpreting disabled, no MT, everything is JIT'd immediately.
	" --jit --dc-hot-count=0 --jit-hot-count=0 --no-jit-mt",
	# Standalone decode-cache, cold-block interpreting.
	" --decode-cache",
	# Decode-cache, no cold-block interpreting.
	" --decode-cache --dc-hot-count=0",
		);

# Replicates a plain test, generating tests for various combinations of JIT and
# decode-cache run-time parameters.  the first argument should be the jit-ok
# flag, followed by the test parameters as pairs.
sub jit_test {
	my ($jit_ok,%origtest) = @_;

	my @tests;
	$a = 0;
	while ( $JitArgs[$a] =~ /--jit/ ) {
		if ($jit_ok) {
			my %newtest = %origtest;
			$newtest{cmd} .= $JitArgs[$a];
			$newtest{retries} = 2;
			push @tests,\%newtest;
		}
		++$a;
	}
	for my $d ($a..$#JitArgs) {
		my %newtest = %origtest;
		$newtest{cmd} .= $JitArgs[$d];
		push @tests,\%newtest;
	}
	
  return @tests;
}

# Similar to model_test, except that this uses the JIT and disables IR checking
# on the output.  This actually adds four tests: One with cold-block
# interpreting turned on, one with it off, and one which uses the standalone
# decode-cache with cold-block interpreting and one with it off.
sub jit_model_test {
	my $jit_ok = shift;

	my @args;
	for (1..@JitArgs) {
		push @args,[ @_ ];
	}

	my $a = 0;
	for my $arg ( @args ) {
		$arg->[0] .= $JitArgs[$a++];
		$arg->[5] .= " --ignore-ir";
		$arg->[6] = 2 if ($arg->[0] =~ /--jit/);
	}

	my @tests;
	$a = 0;
	while ( $JitArgs[$a] =~ /--jit/ ) {
		if ($jit_ok) {
			push @tests,model_test(@{ $args[$a] });
		}
		++$a;
	}
	for my $d ($a..$#JitArgs) {
		push @tests,model_test(@{ $args[$d] });
	}
	
  return @tests;
}

# Similar to ext_model_test, except that this uses the JIT and disables IR checking
# on the output.  This actually adds four tests: One with cold-block
# interpreting turned on, one with it off, and one which uses the standalone
# decode-cache with cold-block interpreting and one with it off.
#
# Arguments are:
#   arg0:  JIT-active flag.
#   argn, argn+1:  Key/value pairs for ext_model_test
sub jit_ext_model_test {
	my ($jit_ok,%origtest) = @_;

	my @tests;
	$a = 0;
	while ( $JitArgs[$a] =~ /--jit/ ) {
		if ($jit_ok) {
			my %newtest = %origtest;
			$newtest{flags} .= $JitArgs[$a];
			push @tests,ext_model_test(%newtest);
		}
		++$a;
	}
	for my $d ($a..$#JitArgs) {
		my %newtest = %origtest;
		$newtest{flags} .= $JitArgs[$d];
		push @tests,ext_model_test(%newtest);
	}
	
  return @tests;
}

# Similar to model_test, except that this uses the trace-cache and disables IR checking on the output.
sub tc_model_test {
  my @args = @_;
  $args[0] .= " --trace-cache";
  $args[5] .= " --ignore-ir";
  return model_test(@args);
}

# Constructs a list of entries for a given model.
# arg0:  Model to run.
# arg1:  Source file.
sub rerun_model_test {
    
  my ($model,$file,$fail) = @_;

  my ($base,$path,$type) = fileparse($file,qr/\.(dat|uvp)/);

  my ($pre,$checker,$input2,$output2,$temps) = make_checker("$base.out.dat");

  if (! ($type eq ".uvp")) {
    return { 
	    cmd => "$model $file -o=$input2 2>/dev/null && $model $input2 -o=$output2", 
	    pre => $pre,
            checker => $checker,
	    temps => [ $input2, $output2, $temps ],
           };
  } else {
    die "$file: Unrecognized file type '$type':  We only recognize .dat files.";
  }
}

# Constructs a test that converts a UVP to DAT, runs that, then uses that output UVP
# as input (via conversion to DAT), then diffs the output of that.
sub round_trip_test {
    
  my ($model,$file) = @_;

  my ($base,$path,$type) = fileparse($file,qr/\.(uvp|dat|elf)/);

  my ($irfn,$vp_ir,$checker);

  my $vpfn = "$path$base$type";
  my $uvp1 = "$base.out1.uvp";
  my $uvp2 = "$base.out2.uvp";

  # uvp file format, create dat and check both intermediate and final results
  my $indat = "$base.gen.dat";
  my $dat1 = "$base.out1.dat";
  my $dat2 = "$base.out2.dat";

  my @temps;

  my $pre;

  # Create the .dat file from the input .uvp or do nothing (if dat).
  if ($type eq ".uvp") {
    $pre = sub {
	  uvp2dat($vpfn,$indat); 
	};
    push @temps,$indat;
  } elsif ($type eq ".dat") {
    # Adjust the name of the dat file back to the original location, since
    # we won't be creating it in the current directory.
    $indat = "$path$base.dat";
  } else {
    die "$vpfn:  Unknown file type.";
  }

  push @temps,$uvp1,$uvp2,$dat1,$dat2;

  my $checker = sub {
    # Check that the simulation passed.
    uvp2dat($uvp1,$dat1);
    check_results($indat,$dat1);
    my $cmd = "$model -trace $dat1 --of=uvp --output=$uvp2 > /dev/null 2>&1 ";
	print "  Cmd:  $cmd\n" if ($showcmd);
    system($cmd) and die "Error executing:  $cmd\n";
    uvp2dat($uvp2,$dat2);
    check_results($indat,$dat2);
  };

  return {
		  pre     => $pre,
		  cmd     => "$model -trace $indat --of=uvp --output=$uvp1",
		  checker => $checker,
		  temps   => \@temps,
	 };
}

# Constructs a test that assembles a .s into a .o and then compares the
# object-dump against a .regress.
#
# Arguments (named):
# as:     Name of assembler
# of:     Name of disassembler
# ld:     Name of linker, if applicable
# sf:     Name of assembly file.
# rf:     Name of regress file.
# fail:   Whether the test should fail or not.
# flags:  Any additional assembler flags.
# dflags: Any additional disassembler flags.  Used in place of the default -dx.
sub asm_test {
  my %args = @_;
  my ($rbase,$rpath,$rtype) = fileparse($args{sf},qr/\.(s)/);
  my $regress = "$rpath$rbase.regress";
  my ($sbase,$spath) = fileparse($args{sf});
	my $pfx = ($sepdirs) ? ".." : ".";
	my ($as,$of) = ("$pfx/$args{as}","$pfx/$args{of}");

	my $ld; $ld = "$pfx/$args{ld}" if ($args{ld});
 	
  if ( !$args{fail} ) {
  	return {
			cmd     => (($ld) ? 
									"$as $args{flags} $args{sf} -o $rbase.o && $ld $rbase.o -o $rbase.elf"
									: "$as $args{flags} $args{sf} -o $rbase.o"),
			checker => (($ld) ?
									sub { objdump_diff($of,"$rbase.elf",$args{rf},$args{dflags}); }
									: sub { objdump_diff($of,"$rbase.o",$args{rf},$args{dflags}); }),
			fail    => 0,
			temps   => (($ld) ? [ "$rbase.o", "$rbase.elf" ] : [ "$rbase.o" ]),
		},
	}
  else {		
		return {
			pre    => sub {
			  # In order for the diff to pass, we have to move the source file into the
			  # current directory, so that the file name in any error messages matches.
			  `cp $args{sf} $sbase` if ( ! -e $sbase );
			},
			cmd    => "$as $args{flags} $sbase",
			fail   => 1,		
			stderr => 1,
			diff   => $regress,
		},
	}
}

# Similar to asm_test, except it just runs objdump on an input file and compares
# the results to a regression file.
#
# Arguments (named):
# of:    Name of the disassembler
# in:    Name of the input file.
# rf:    Name of the regress file.
# flags: Any additional disassembler flags.
sub objdump_test {
  my %args = @_;
  my ($ibase,$ipath) = fileparse($args{in});
	my $pfx = ($sepdirs) ? ".." : ".";
	my $of = "$pfx/$args{of}";
 	
	return {
		pre    => sub {
			# In order for the diff to pass, we have to move the source file into the
			# current directory, so that the file name in any error messages matches.
			`cp $args{in} $ibase` if ( ! -e $ibase );
		},
		cmd     => "$of $args{flags} -dx $ibase",
		diff    => $args{rf},
	};
}

# Constructs a test that assembles a .s into a .dat and then runs a model.
# arg0:  Assembler
# arg0:  Model to run.
# arg2:  Space delimited list of files.  First one should be a .s file.  The 
#        rest are .dat include files.
sub asm_and_run_test {
    
  my ($as,$model,@files) = (shift,shift,(split / /,shift));

  my ($base,$path,$type) = fileparse($files[0],qr/\.(s)/);

  my @flist = (shift @files,map { "--include=$_"; } @files);

  my ($pre,$checker) = make_checker("$base.dat");

  return {
	  cmd     => "perl $scripts/makedat --aopts=\" \" --as=$as @flist --o=$base.dat && $model $base.dat", 
	  pre     => $pre,
	  checker => $checker,
	  temps   => [ "$base.dat", "$base.out.dat" ],
	 };
}

# Similar to asm_and_run_test, except that an elf file is used as the input to the model.
# arg0:  Assembler.
# arg1:  Model to run.
# arg2:  Input assembly file.
# arg3:  Expects dat file.
sub asm_elf_run_test {
    
  my ($as,$model,$asm,$exp) = @_;

  my ($base,$path,$type) = fileparse($asm,qr/\.(s)/);

	my $input = "$base.elf";
	my $output = "$base.out.dat";

  return {
	  cmd     => "./$as $asm -o $input && ./$model $input -o $output -trace", 
	  checker => sub {
			check_results($exp,$output);
		},
	  temps   => [ "$base.elf", "$base.out.dat" ],
	 };
}

# Similar to asm_elf_run_test, except that this uses a linker to generate the ELF file.
# arg0:  Assembler.
# arg1:  Linker
# arg1:  Model to run.
# arg2:  Input assembly file.
# arg3:  Expects dat file.
sub asm_ld_run_test {
    
  my ($as,$ld,$model,$asm,$exp) = @_;

  my ($base,$path,$type) = fileparse($asm,qr/\.(s)/);

	my $obj    = "$base.o";
	my $input  = "$base.elf";
	my $output = "$base.out.dat";

  return {
	  cmd     => "./$as $asm -o $obj && ./$ld $obj -o $input && ./$model $input -o $output -trace", 
	  checker => sub {
			check_results($exp,$output);
		},
	  temps   => [ $obj, $input, $output ],
	 };
}

# Runs objdump and then diffs the output with an expected file.
# arg0:  Disassembler program.
# arg1:  Object file.
# arg2:  Reference file.
# arg3:  Any dissasembly flags to use.  Default is -dx.
# arg4:  If undefined/0, generated file will be deleted, otherwise it will not be.
sub objdump_diff {
  my ($od,$gen,$exp,$dflags,$nodel) = @_;

	$dflags = "-d -x" if (!$dflags);
  system ("$od $dflags $gen > $gen.out") and die "Could not run objdump.\n";
  if (system "diff -w $gen.out $exp") {
        print "Differences were found.\n",
          " <---- Generated | Reference ---->\n";
        die;
  }
  unlink ($gen,"$gen.out") unless ($nodel || $keepoutput);
}

# Called by latency_test- do not call directly.
sub process_latencies {
  my ($output,$checker) = @_;
  
  my ($cur,%data);

  my @lines = split /\n/,$output;
  for (@lines) {
	if ( /^<CORE name=\"(.*)\">/ ) {
	  $data{$1} = $cur = {};
	} elsif ( /^Latency:\s+(.+)\s+=\s+(\d+)/ ) {
	  $cur->{$1} = eval $2;
	}
  }

  &$checker(\%data);
}

# Constructs a test that runs the ISS generator and runs a checker, supplying it
# with information about instruction latencies.  The format of the supplied data
# is a hash, with keys of core names.  Each value is then a hash of
# instruction/latency pairs.
sub latency_test {
  my ($model,$cfg,$checker) = @_;

  my ($base,$path,$type) = fileparse($model,qr/\.(adl)/);

  return {
		  cmd     => "$make_iss --time-tagged --config=$cfg $model --verbose --output=$base.cc.temp",
		  checker => sub { process_latencies($_[0],$checker); },
		  temps   => [ "$base.cc.temp" ],
		  stdio   => 1,
		 };
}

# This compares a hash of latencies against a hash of expects.
# The expects data structure is a hash with keys naming cores and
# values of array references.  Each array is a reference of two elements-
# a regular expression and a latency.  We match in sequence and quit after
# the first match.
sub check_latencies {
  my ($expect_hash,$found_hash) = @_;

  for (keys %$expect_hash) {
	my $expects = $expect_hash->{$_};
	my $found   = $found_hash->{$_};
	
	Found : for my $f (keys %$found) {
	  for my $e (@$expects) {
		if ( $f =~ $e->[0]) {
		  if ( $found->{$f} != $e->[1] ) {
			die "Expected instruction $f to have a latency of $e->[1] but found $found->{$f} instead.\n";
		  }
		  next Found;
		}
	  }
	}
  }
}

# Sets up a test of the GDB interface using gdbclient.
# arguments are name based:
#   m:      Name of model.
#   i:       Name of model input file.
#   t:       Name of gdbclient input test file.
#   o:       Name of temporary output file.  If not specified, it's baseame(<test>).out
#   r:       Name of golden regress file.  If not specified, it's basename(<test>).regress
#   cfg:     Name of register mapping config file.  If not specified, it's <model>-config.tcl.  This is ommitted
#            if the name is set to "none".
#   prot:    Protocol to use.  Defaults to empty, which means to use the gdbclient's default.
#   gopts:   Any gdbclient options to add.
#   mopts:   Any model options to add.
#   checker: Specify a checker function.  Default is to do a file-diff against the regress file. 
#   retries: An optional retry count for the test.
sub gdb_test {
  my (%options) = @_;

  my ($model,$input,$test) = ($options{m},$options{i},$options{t});

  my ($mbase,$mpath) = fileparse($model);
  my ($tbase,$tpath) = fileparse($test,".gdb");
	
  my $regress = ($options{r}) ? $options{r} : "$tpath$tbase.regress";
  my $output  = ($options{o}) ? $options{o} : "$tbase.out";

  my $config;
  if ($options{cfg}) {
		$config  = $options{cfg};
	} else { 
		$config = "$tpath$mbase-config.tcl";
	}

	if ($config eq "none") {
		$config = "";
	} else {
		$config = "--config=$config";
	}

  my $prot  = ($options{prot}) ? "--$options{prot}" : "";
  my $gopts = ($options{gopts}) ? $options{gopts} : "";
  my $mopts = ($options{mopts}) ? "--mo='$options{mopts}'" : "";

  my $checker = ($options{checker}) ? $options{checker} : sub { file_diff($output,$regress); };

  $gopts .= " -v" if ($debug);

  return {
		  cmd     => "$scripts/gdbclient -m=$model -i='$input' -t=$test -o=$output -mo=\"$config --no-output\" $prot $gopts $mopts",
		  checker => $checker,
		  temps   => [ $output ],
		  stderr  => ($debug) ? 1 : 0,
			retries => $options{retries},
		 };
}

# Returns true (1) if the specified program is a 64-bit ELF executable, false
# otherwise.  This is smart enough to look in a .libs directory if the supplied
# path points to a Bourne shell program- we assume that we've got a libtool
# wrapper script.
sub is_elf64 {
  my ($prog) = @_;

  my $type = `file $prog`;
  if ( $type =~ /Bourne shell script/) {
	my ($base,$path) = fileparse($prog);
	$type = `file $path/.libs/$base`;
  }
  #print "type:  $type\n";
  my $is_64bit = ($type =~ /ELF 64-bit/);
  return $is_64bit;
}

##
## </HELPERS>
##

1;
