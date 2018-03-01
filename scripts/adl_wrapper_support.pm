#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#
# This file contains utilities used by the various Perl wrapper scripts for
# invoking ADL commands.
#

package adl_wrapper_support;

require Exporter;

use File::Basename;
use File::Temp;
use Pod::Usage;

our @ISA = ("Exporter"); 
our @EXPORT = qw( 
                 addOption addFlag printhelp runcmd runcmd_stdio runbuild modify_path_for_os
                 get_compiler_invocation $verbose $show_output $jobs $lsf $parallel_build $distcc
                );

use strict;

use vars qw( $verbose $show_output $jobs $parallel_build $lsf $lsf_default $lsf_is_setup $distcc $distcc_enabled);

$parallel_build = 0;

$lsf = "dummy";
$lsf_default = "bsub -Ip -P adl";
$lsf_is_setup = 0;

$distcc = 1;
$distcc_enabled = 0;

sub addOption {
  my ($str,@opt) = @_;

  my $s;
  for my $i (@opt) {
	if (defined $i) {
	  $s .= " --$str='$i'";
	  #print "  Option:  $i\n" if ($verbose)
	}
  }
  return $s;
}

sub addFlag {
  my ($str,$flag) = @_;

  if (defined $flag) {
	if ($flag) {
	  return " --$str";
	} else {
	  return " --no-$str";
	}
  } else {
	return "";
  }
}

sub printhelp($$$) {
  my ($eval,$printversion,$verbose) = @_;
  &$printversion();
  print "\n";
  pod2usage(-exitval => $eval,-verbose=>$verbose);
  exit 0;
}

sub lsf_setup
{
	if (!$lsf_is_setup) {
		if ($lsf eq "") {
			$lsf = $lsf_default;
			print "lsf is now set to $lsf\n";
		} elsif ($lsf eq "dummy") {
			$lsf = "";
    }
		$lsf_is_setup = 1;
	}
}


# Runs a command.  If the return code is non-zero, the function prints an error
# and exits.
sub runcmd(@) {
	lsf_setup();
  if (defined $verbose) {
    print "[@_]\n";

		if ((my $err = (system "$lsf @_")) != 0) {
			print "Error executing command",(($err & 127) ? " (coredump)" : ""),":  @_\n";
			exit 1;
		}
  } else {

		my $out = `$lsf @_ 2>&1`;

		if ($show_output) {
			print "$out";
		}
		if ($?) {
			print "Error executing command",(($? & 127) ? " (coredump)" : ""),":  @_.\n";
      if ($out && !$show_output) {
        print "  Output was:\n\n$out";
      }
      else {
        print "\n\n";
      }
			
			exit 1;
		}
  }
}

# Runs a command and returns standard out.
sub runcmd_stdio(@) {
	lsf_setup();
	print "[@_]\n" if (defined $verbose);

	my $out = `$lsf @_`;

	print $out if (defined $verbose);
	if ($?) {
		print "Error executing command",(($? & 127) ? " (coredump)" : ""),":  @_.\n";
		if ($out) {
			print "  Output was:\n\n$out";
		}
		else {
			print "\n\n";
		}
		
		exit 1;
	}
	return $out;
}


sub dofork {
  use Errno qw(EAGAIN);

  my $pid;
 FORK: {
	if ($pid = fork) {
	  # Parent here.
	  return $pid;
	}
	elsif (defined $pid) {
	  # Child here.
	  runcmd(@_);
	  exit 0;
	}
	elsif ($! == EAGAIN) {
	  sleep 5;
	  redo FORK;
	}
	else {
	  die "Can't fork:  $!\n";
	}
  }
}

# Enable the include server (pump) and turn on the use of distcc if we can.
#
# return:  distcc compiler string if enabled, empty string if not.
sub distcc_setup {
	if ($distcc) {
		# Do we have pump and distcc in our PATH, plus the hosts environment
		# variable?  If so, then we can use distcc.
		if ( !system("which pump > /dev/null 2<&1") && 
				 !system("which distcc > /dev/null 2<&1") &&
				 ($ENV{DISTCC_POTENTIAL_HOSTS} || $ENV{DISTCC_HOSTS})) {
			# Yes, they're available, so we can enable distcc.
			print "Enabling the use of distcc.\n";

			# Do we need to enable the pump server (pump-mode only).
			if ($ENV{DISTCC_POTENTIAL_HOSTS}) {
				if ( !$ENV{INCLUDE_SERVER_PORT}) {
					# Need to turn it on.  In a shell, we'd just eval this, but here we have
					# to do that manually.
					my $out = `pump --startup`;
					if ($?) {
						die "Could not start pump server:  $!\n";
					}
					print "  Turning on the pump server.\n";
					my @vars = split /\n/,$out;
					for (@vars) {
						my ($k,$v) = (/export ([^=]*)='([^']*)'/);
						#print "Adding pump variable $k = $v\n";
						$ENV{$k} = $v;;
					}
					
					$distcc_enabled = 1;
				} else {
					# Sanity check- the socket file must exist.
					die "Pump's socket is defined, but the file does not exist:  $ENV{INCLUDE_SERVER_PORT}"
							if (! -e $ENV{INCLUDE_SERVER_PORT});
				}
			}

			return "distcc";
		}
	}
	return "";
}

# Shutdown the distcc server, if relevant.
sub distcc_shutdown {
	if ($distcc_enabled) {
		print "  Shutting down the pump server.\n";
		`pump --shutdown 2>&1 >/dev/null`;
		if ($?) {
			die "Could not shut down pump serveR:  $!\n";
		}
	}
}

# Runs the build command based upon the number of jobs and based upon whether we
# can do it in parallel.
# arg1:  List of { cmd => cmd, src => source-file , obj => object-file , flags => compile-flags }
sub runbuild {
	my ($jobs) = @_;

	# We're doing multiple jobs, so we need to loop.
	if ($parallel_build >= 0) {
		my $dcc = distcc_setup();

		my @tjobs = @$jobs;
		# If a max number of jobs was specified, then use it.  Otherwise, run all in
		# parallel.
		my $maxjobs = ($parallel_build == 0) ? scalar(@tjobs) : $parallel_build;
		my $njobs = 0;
		# Doing it in parallel, so use fork/join.
		my @pids;
		my $error = 0;
		while ( @tjobs || @pids ) {
			while ( ($njobs < $maxjobs) && @tjobs ) {
				my $cmd = "$dcc $tjobs[0]->{cmd}";
				push @pids,dofork $cmd," $tjobs[0]->{flags} $tjobs[0]->{src} -o $tjobs[0]->{obj}";
				shift @tjobs;
				++$njobs;
			}

			waitpid $pids[0],0;
			$error = 1 if $?;
			shift @pids;
			--$njobs;
		}

		distcc_shutdown();

		if ($error) {
			print "Exiting due to errors...\n";
			exit 1;
		} 
	} else {
		# Doing it sequentially, so just use a loop.
		for (@$jobs) {
			runcmd $_->{cmd},"$_->{flags} $_->{src} -o $_->{obj}";
		}
	}
}

# This modifies the compiler command for OS differences, then prepends a PATH
# update, so that gcc will invoke the proper version of ld (or other binutils
# programs).
sub get_compiler_invocation {
	my ($cc) = @_;

	$cc = modify_path_for_os($cc);
	my $compiler_path = dirname($cc);
	$cc = "PATH=${compiler_path}:".'${PATH} '.$cc;

	return $cc;
}

# If a path looks to contain an OS version, then make sure it matches against
# our current OS.  This is fairly limited in functionality- we're basically just
# handling the case where we recognize a RHEL5 OS path in the TR and them, if
# we're actually on RHEL6, modify it to be a RHEL6 path.  Otherwise, we leave it
# alone.
#
# arg1:  Full path to compiler
#
# return:  Updated compiler path.
sub modify_path_for_os {

	my ($cc) = @_;

	# Are we using a compiler with our expected glibc version in the path (must be in the TR)?
	if ($cc =~ m#/pkg/.*-glibc(\d+\.\d+)/#) {

		my $glibc_version = $1;

		#print "  This is a TR compiler version.  glibc = ${glibc_version}\n";

		# Then find our current version of glibc.  If they don't match, then try to
		# substitute in the newer version and then see if the compiler exists.  If
		# so, then we use the updated path.
		if ( `/usr/bin/ldd --version` =~ (/ldd \(.*\) (\d+\.\d+)/) ) {
			my $curr_glibc_ver = $1;

			#print "  Current glibc = $curr_glibc_ver\n";

			if ($glibc_version != $curr_glibc_ver) {

				my $new_cc = $cc;
				$new_cc =~ s#-glibc(${glibc_version})/#-glibc${curr_glibc_ver}/#;

				#print "New path:  $new_cc\n";

				if ( -e $new_cc ) {
					print "Updating compiler path to '$new_cc'\n" if (defined $verbose);
					return $new_cc;
				}
			}
		}	
	}

	return $cc;
}


1;
