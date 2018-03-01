#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#

=head1 NAME

 driver:  Regression driver.

=head1 SYNOPSIS

The regression driver is a framework for a regression suite.  Refer
to the full man page for more information.

=head1 OPTIONS

Command-line options:

=over 8

=item B<--list, --l>

List the tests in this regression.

=item B<--tests=range, --t=range>

Specify what tests to run.  Should be a number, range, or list of numbers or
ranges, where the numbers refer to indices of the test array (1 is the first
test).  See below fora description of the range format.  Multiple instances of
this parameter may be used.

=item B<--exclude=range, --x=range>

Exclude tests.  Should be a number, range, or list of numbers of ranges.  See
below for a description of the range format.  Multiple instances of this option
may be specified.

=item B<--exclude-string=str, --xs=str>

Exclude any tests which contain the string I<str>.  Multiple instances of this
string may be specified, in which case any match is excluded.

=item B<--string=str, --s=str>

Run any tests which contain string I<str>.  This is a simple substring search,
not a regular expression search. Multiple instances of this option may be
specified and are used as a disjunction.  In other words, one of the strings
must match in order for the test to be run.

=item B<--exclusive-string=str, --sx=str>

Run any tests which contain string I<str>.  This is a simple substring search,
not a regular expression search. Multiple instances of this option may be
specified and are used as a conjunction.  In other words, all strings must match
in order for the test to be run.

=item B<--regex=re, --r=re>

Run any tests which match regular expression I<re>. Multiple instances of this
option may be specified and are used as a disjunction.  In other words, one of
the regular expressions must match in order for the test to be run.

=item B<--keepoutput, --ko>

Do not delete temporary files. 

=item B<--keepdiff>

Write the output of any diff test to a per-test output file.  This writes diff
output using a form suitable for using with B<patch>.

=item B<--showcmd, --sc>

Print the command to be executed.  This is implied if I<--debug> is set.

=item B<--check-pkg=pkg>

If set, then if we're in the /pkg environment, make sure that the specified
package exists on the host machine.  If not, then attempt to update the machine
so that it exists.  This is just a workaround while /pkg is not 100% stable.
This option may be repeated.

=item B<--timeout=sec>

Specify the maximum time, in seconds, an individual test is allowed to run.  The
default is 5 minutes.  If a test exceeds this time, then it is aborted and
recorded as a fail.  If the timeout value is 0, then no timeout is created.

=item B<--debug[=level], --d[=level]>

Enable debug messages.  The default (no argument) is to display a minimal amount
of information.  Extra debug levels are available, however.

=item B<--seed>

Specify a seed value.


=item B<--lsf=str>

Use LSF with the given project.  This will run the individual regressions in
parallel, using LSF.  

Note that regressions must be written correctly in order be to able to run in
parallel; the regression driver simply runs these tests, but if conflicting
temporary files are used, then errors will occur.

Note also that, for LSF to work, the source and build environment must be
located on a network accessible file system.

=item B<--lsf-job-size=int, --j=int>

If LSF is being used to parallelize the regression run, then this parameter
specifies the number of tests to be run for each job.  The idea is to increase
the ratio between the amount of real work being done (the tests being executed)
vs. the overhead of submitting the job to LSF. The default is 10.

=item B<--lsf-group=str>

Set the LSF group to the specified string.  The default is "k2".

=item B<--lsf-queue=str>

Set the LSF queue to the specified string.  The default is "normal".

=item B<--lsf-res-str=str>

Specify an LSF resource string.  The default is an empty resource string.

= item B<--parallel-run=int,. --p=int>

Run jobs in parallel using separate processes, analogous to B<make -j=int>.
This may be used on a local file system and only parallelizes the jobs on the
host machine.

Note that regressions must be written correctly in order be able to run in
parallel; the regression driver simply runs these tests, but if conflicting
temporary files are used, then errors will occur.

=back

The valid range forms are:

=over 8

I<x-y>:  Run tests x through y.  Example:  --tests=3-5

I<-y>:  Run tests from start, up to and including y.

I<x->:  Run tests x through the last one.

=back

B<Note>:  Multiple parms are allowed and are concatenated together.

If arguments are found on the command-line (items not associated with a
particular option), then these are considered to be test-constraints.  They will
be associated with a test-constraint option that was specified, but was given no
arguments.  If no such option is found, then the arguments will be treated as
test numbers.  For example:

=over 8

regress -t 1 2 3 4 5

=back

will execute tests 1 thorugh 5.  This feature is provided for compatibility with
B<xargs>, so that various utilities such as B<find> and B<grep> may be used to
specify tests to be executed.

=head1 DESCRIPTION

Library usage:

use lib "..";
use rdriver;

doTest(@tests_list [,empty-list-ok]);

B<doTest>, the main entry function, takes an array reference as a parameter.
The optional second parameter, if 0 or omitted, means that the regression will
fail if no tests are selected to be executed.  If non-zero, then an empty
test-list is OK.

=head2 Hash Keys

Each entry in the supplied array should be a hash.  These are the allowed keys:

=over 8

=item B<cmd>

Command-line to execute.  If the token &seed is found, it will be replaced with
a numerical seed.  This value will either be the value specified by the seed
command-line parameter, or the current result of time().

=item B<pre>

Function to execute before running the command.

=item B<nocmts>

Remove comments from diff.  Default is 0 (preserve).  Set this to the comment
character or string that you want to remove, e.g. "#".

=item B<stdio>

Capture standard output.

=item B<stderr>

Direct standard error to standard output for capture.  If not specified, it is directed to /dev/null.

=item B<ignore_stderr>

If set, then no modifications are made to the command with respect to standard-error processing.

=item B<diff>

Diff the output with the file specified.

=item B<dpfx>

Only diff the output that starts with the given prefix.  This may be a regular
expression.

=item B<checker>

If present, this should be a function reference.  The function is called with
the test output as its argument and should die if an error occurs.

=item B<fail>

If this is non-zero then the cmd run is expected to have a non-zero return
value. The test will fail if 0 is returned.  The checking commands will still be
executed if they are present.  The value of fail should match the error code
returned by the program, or should be 999 to represent a don't-care condition.

=item B<temps>

Specify temporary files.  These will be unlinked after the test is complete,
unless the --keepoutput option is set on the command-line.

=over

Commands may be either passed on the command-line or passed in via the
environment variable RegressOpts.  The latter allows options to be applied
globally via, for example, a resursive make.

=head2 Utility Functions

The library also contains a number of utility functions.  These are listed
below.

=back 8

=item file_rdiff(<filename>,<array ref of regular expressions | string>)

This is a simple function for checking a file.  The second argument is a
reference to an array of strings.  Each string is considered to be a regular
expression.  The function must be able to successfully match lines in the file
against the array elements, proceeding to the end of the array, or else it fails
and calls die.

The regular expressions can be either a reference to an array of strings/regular
expressions or a single string.  If the latter, we'll split the string on
newlines.  This can be useful when you have a block of data that you want to
check: Rather than quoting each item, simply use here-document syntax.

=item str_rdiff(<input string>,<array ref of regular expressions | string>)

Same as file_rdiff, except that we take the input, split on newlines, then try
and match against the regular expressions.

=item dprint(...)

Same functionality as print if the --debug option is specified on the
command-line.  Otherwise, nothing is printed.

=item dprintf(...)

Same functionality as printf if the --debug option is specified on the
command-line.  Otherwise, nothing is printed.

=back

=cut
    
  package rdriver;

use FindBin;
use lib "$FindBin::RealBin";
use Cwd;
use File::Temp;

require Exporter;

our @ISA = ("Exporter");
our @EXPORT = qw( doTest doDiff setupForSeparateDirs add_child_args file_rdiff str_rdiff dprint dlprint 
                  dprintf $sepdirs $serialOverride $src $debug $timeout $keepoutput $keepdiff $showcmd @setuphooks run_with_timeout );

$diff = "diff -w";

$child_args;
$debug = -1;

# Use this to refer to source files.  Should be set up by a caller based upon
# where source files are located.
$src = ".";

# If set to 1, then run all tests serially, regardless of command-line options.
# Set this to 1 in a regression suite if you know that it cannot run in
# parallel.  This way, the -p option can be applied to other scripts which can
# run in parallel.
$serialOveride = 0;

# By default, do not run each test in separate directories.  This is mainly for
# compatibility with existing users, but might not be the best default.
$sepdirs = 0;

@setuphooks;

$showcmd = 0;
@checkpkg = ();
$timeout = (10 * 60); # 10 minute timeout for individual tests.

# These are global because Perl's closures are *broken*!!!!.
$seed = 0;
$cmd = "";
$testindex;
$fails = 0;
$lsfjobsize = 10;
@failedtests;
$failmsg;
$keepoutput = 0;
$keepdiff = 0;
$DontCare = 999;
$lsfgroup = "k2";
$lsfqueue = "normal";
$lsf_res_str = "";
$lsfproj  = "";

sub error_no_die {
	my ($msg,$noprnt) = @_;
  print $msg if (!$noprnt);
  print "  Executed command:  '$cmd'\n";
  ++$fails;
  push @failedtests,$testindex;
}

sub error {
	error_no_die(@_);
  die;
}

use strict;
use POSIX ":sys_wait_h";
use Data::Dumper;

use vars qw(@Tests $child_args $diff $sepdirs $serialOverride @setuphooks $src
$cmd $seed $parrun $parchild $lsf $lsfchild $lsfproj $lsfgroup $lsfqueue $lsf_res_str
$lsfjobsize $showcmd @checkpkg $fails $testindex @failedtests $failmsg
$keepoutput $keepdiff $debug $timeout $DontCare);

sub add_child_args {
	$child_args .= " @_";
}

# This returns parameters that should be propagated to child processes.
sub childParms {
	my ($args) = @_;
  # This first parameters is *VERY* *IMPORTANT*.  It prevents run-away LSF
  # recursion if the RegressOpts environment variable is being used to set
  # parameters.
  my $parms = " $args";
  $parms .= " --keepoutput" if ($keepoutput);
  $parms .= " --keepdiff" if ($keepdiff);	
  $parms .= " --showcmd" if ($showcmd);
  $parms .= " --debug" if ($debug);
	$parms .= " $child_args" if ($child_args);
  for (@checkpkg) {
    $parms .= " --check-pkg=$_";
  }
  $parms;
}

sub clean_job_files()
{
	`rm -f t*.job`;
}

sub clean_queue_files()
{
  `rm -f t*.queue`;
}


# Dispatch child jobs to LSF.
sub lsf_dispatch {
  my ($tests) = @_;

  # First, sanity check:  Do we have the LSF utilities?
  system ("which bsub >&1 > /dev/null") and die "The LSF utilities were not found in your PATH.";

  # We sort the keys of the hash here, numerically, so that
  # the user sees the tests execute in the expected order.
  my $parms = childParms("--lsf-child --p=$lsfjobsize");

  # Remove any existing lsf files
	clean_queue_files();
	clean_job_files();

  # Limit number of outstanding jobs
  my $os = `bjobs | wc -l 2>&1`;
  chomp $os;
  my $max = 500;
  while (eval($os) > $max) { 
    $os = `bjobs | wc -l 2>&1`;
    chomp $os;
    print "Waiting for existing LSF jobs to complete...\n";
    sleep 4;
  }

	#print "Job hash:\n",Dumper($tests),"\n\n";
  
  # Submit the jobs.  We issue in groups, taking slices of the overall
  # test-index array.
  print "Dispatching jobs via LSF (project=$lsfproj, group=$lsfgroup, queue=$lsfqueue, resource string='$lsf_res_str')...\n";
  my %jobs;
  my @test_list = sort { $a <=> $b} (keys %{$tests});
  for (my $i = 0; $i < @test_list; $i += $lsfjobsize) {
    # Create the slice of test numbers we're going to issue.
    my @s = @test_list[$i..$i+$lsfjobsize-1];
    # For each test, put it into our completion hash.
    @jobs{@s} = (1) x @s;
    # Now issue it via LSF.
    dprint ("Submitting jobs @s.\n");
    my $lsfcmd = "$lsf -oo t$i.queue $0 $parms -t @s";
    if ($showcmd) {
      print "LSF Executing: $lsfcmd\n";
    }
    my $rc = `$lsfcmd 2>&1`;
    dprint ("LSF dispatch output:  $rc\n");
    if ($?) {
      die "Could not submit jobs to LSF:\n$rc\n";
    }
  }

  # In case the slices weren't equal, remove a bogus undef key.
  delete $jobs{""};

  # Get the results
  dprint ("Polling for completion files...\n");
	my $done = 0;
	my $total = scalar(keys %jobs);
  while ( keys(%jobs) ) {
		
    # If any job files exist, process them.
    my @jf = <t*.queue>;
    if (! @jf ) {
      dprint ("Waiting for ",scalar(keys(%jobs))," jobs: ",(join ' ',keys(%jobs)),".\n");
      sleep 2;
    } else {
      for (@jf) {
				dprint ("Found completion file $_\n");
				my $jfile = $_;
				# First, sanity check: Is the file complete?  It appears that file
				# creation is not atomic, unfortunately.
				open IN,$_ or die "Could not open $_ for reading.";
				my $complete = 0;
				while (<IN>) {
					if ( /^Resource usage summary:/ ) {
						$complete = 1;
						last;
					}
				}
				if ($complete) {
					# We're OK, so go back to the beginning.
					seek IN,0,0;
					my $do_print = 1;
					# Sanity checker- if we get neither a SUCCEEDED nor a FAILED message,
					# then something is wrong, so we make sure that the error is
					# propagated.
					my $ok = 0;
					while (<IN>) {
						if (/^\s+Test (\d+)\.\.\./) {
							delete $jobs{$1};
							++$done;
						} elsif (/Regression SUCCEEDED/) {
							# Everything succeeded.
							$ok = 1;
							last;
						} elsif (/Regression FAILED:[^:]+:(.*)/) {
							my $fstr = $1;
							# We got a fail, so extract the failing tests.
							my @flist = ( $fstr =~ /(\d+)/g );
							$fails += @flist;
							push @failedtests,@flist;
							$ok = 1;
							last;
						} elsif (/^Sender:\s+LSF System/) {
							$do_print = 0;
						}
						print if ($do_print);
					}
					# All done- remove that output file.
					dprint ("Cleaning up $jfile.\n");
					close IN;
					unlink $jfile;
					if ($ok) {
						printf "     (%d/%d, %0.1f%%)\n",$done,$total,($done/$total)*100;
					} else {
						printf "     <bad lsf test- no fail or succeed found>\n";
						push @failedtests,-1;
					}
				} else {
					# Found an incomplete file, so wait a bit and then continue the
					# polling operation.
					dprint ("  ...not complete.  Try again later.\n");
					close IN;
					sleep 1;
				}
      }
    }
  }
}

# Fork a sub-process, with some error checking.
sub dofork {
  use Errno qw(EAGAIN);

  my $pid;
 FORK: {
	if ($pid = fork) {
	  # Parent here.
	  return $pid;
	}
	elsif (defined $pid) {
		# Redirect standard out and standard error.
		open STDOUT,">",$_[0]  or die "Could not redirect STDOUT to $_[0]:  $!";
		open STDERR,">&STDOUT" or die "Could dup STDOUT:  $!";
	  # Child here.
		runTest($_[1],$_[2]);
		exit ($fails) ? 1 : 0;
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

# Dispatch tests using fork/join.
sub par_dispatch {
  my ($tests) = @_;

  # We sort the keys of the hash here, numerically, so that
  # the user sees the tests execute in the expected order.
  my $parms = childParms("--par-child");
 
  # Run the jobs using fork/join.
  print "Dispatching jobs...\n" if (!$lsfchild);
  my %jobs;
  my @test_list = sort { $a <=> $b} (keys %{$tests});

	my $njobs = 0;

  # Remove any existing job output files
  clean_job_files() if (!$lsfchild);
	
	# Tracks the pids of outstanding processes.  Maps this to test numbers.
	my %pids;

	my $total = (scalar @test_list);
	my $done = 0;
	
	# Fails are stored locally so that a child process doesn't get confused and
	# think that fails occurred.
	my (@pfailedtests,$pfails);
	
	while ( @test_list || $njobs ) {
		# Dispatch as many jobs as we can while we have jobs to dispatch and we
		# haven't exceeded our job count.  All output is directed to a temporary
		# file.
		while ( ($njobs < $parrun) && @test_list ) {
			my $tnum = shift @test_list;
			my $kid = dofork("t$tnum.job",$tnum,$$tests{$tnum});
			$pids{$kid} = $tnum;
			++$njobs;
			dprint("  Dispatching test $tnum ($kid)\n");
		}

		# Iterate over our known pids, looking for any that have finished.  If
		# there's an error, record it.  Copy the temporary file's output to standard
		# out.
		my $processed = 0;
		for my $pid (keys %pids) {
			my $kid = waitpid $pid,WNOHANG;
			next unless $kid > 0;
			$processed = 1;
			my $tnum = $pids{$kid};
			if ($?) {
				# We had an error, so bump the fail count and add the test number to the
				# list of failed tests.
				++$pfails;
				push @pfailedtests,$tnum;
			}
			# Display any output.
			my $ofn = "t$tnum.job";
			open my $tout,$ofn or die "Could not open job file $ofn:  $!";
			while (<$tout>) {
				print;
			}
			close $tout;
			unlink $ofn;
			++$done;
			printf "     (%d/%d, %0.1f%%)\n",$done,$total,($done/$total)*100 if (!$lsfchild);
			delete $pids{$kid};
			--$njobs;
		}

		# Sleep for a quarter second if no finished jobs were found.
		select(undef,undef,undef,0.25) if (!$processed);
	}

	@failedtests = @pfailedtests;
	$fails = $pfails;
}

# Kill a tree of processes, starting with the argument pid.
# arg1:  Pid to kill.  All children of this pid, recursively evaluated, will
#        also be killed.
sub kill_all {
	my ($pid) = @_;

	my %procs = ();
	# Get all processes, listing the pid and parent pid.
	# This constructs a hash of ppid -> pids.
	my @pdata = split '\n',`ps -eo pid,ppid`;
	shift @pdata;
	#print "ps output:  @pdata\n";
	for (@pdata) {
		my ($ps_pid,$ps_ppid) = (/\s*(\d+)\s*(\d+)/);
		next if (!$ps_pid || !$ps_ppid);
		if (exists $procs{$ps_ppid}) {
			#print "Adding to existing ppid item:  $ps_ppid -> $ps_pid\n";
			push @{$procs{$ps_ppid}},$ps_pid;
		} else {
			#print "Creating new ppid item:  $ps_ppid -> $ps_pid\n";
			$procs{$ps_ppid} = [ $ps_pid ];
		}
	}

	#print "  Pid hash:  ",Dumper(\%procs),"\n";

	sub find_pids {
		my ($pid,$procs) = @_;

		#print "  Pid hash:  ",Dumper($procs),"\n";

		my @pids = ( $pid );
		#print "  Find pid start:  @pids, children:  ",Dumper($procs->{$pid}),"\n";
		if (my $children = $procs->{$pid}) {
			for (@$children) {
				push @pids,find_pids($_,$procs);
				#print "  Find pid next:  @pids\n";
			}
		}

		return @pids;
	}

	# Now, using the process tree, kill all pids eminating from the argument pid.
	my @pids = find_pids ($pid,\%procs);

	#print "All pids:  @pids\n";

	# Now kill the process and all children.
	for (@pids) {
		kill 'KILL',$_;
		waitpid $_,0;
	}
}

# Run a command with a timeout, returning the standard out, if specified, the return code,
# and whether a timeout occurred.
# arg1:  Command to run.
# arg2:  Timeout value.
# return:  (output,return-code,timeout-occurred).
sub run_with_timeout {
	my ($cmd,$timeout) = @_;

	# Run the command, along with an alarm.  If the alarm sounds first, then we've
  # exceeded our timeout and the test is recorded as a failure.
	my ($pid,$output,$rc,$timeout_error);
	eval {
		local $SIG{ALRM} = sub { die "timeout\n" };
		alarm ($timeout);
		$pid = open (PIPE,"$cmd |");
		setpgrp $pid,$pid;
		local $/ = undef;
		$output = <PIPE>;
		close PIPE;
		$rc = $?;
		alarm (0);
	};
	if ($@) {
		if ($@ =~ /timeout/) {
			# We got a timeout, so kill the process and all sub-processes.
			print ("  Timeout!\n");
			kill_all($pid);
			$timeout_error = 1;
			close PIPE;
		} else {
			# Clear still-pending alarm.
			alarm (0);
		}
	}

	return ($output,$rc,$timeout_error);
}

# Execute a single test.
#   arg1:  Test index.
#   arg2:  Hash reference to test item.
sub runTest {
	my ($testindex,$t) = @_;

	print " Test $testindex...\n";

	my ($c,$testdir);
	if ($sepdirs) {
		$testdir = "t$testindex";
		$c = getcwd();
		if ( ! -d $testdir) {
			mkdir $testdir or die "Could not create test directory $testdir:  $!";
		}
		chdir $testdir;
	}

	dprint ("  Current directory:  ",getcwd(),"\n");
	
	if ($t->{pre}) {
		&{$t->{pre}}();
	}

	my $retries = ($t->{retries}) ? $t->{retries} : 1;

	my $outfile =  $t->{outfile};      


	$cmd = $t->{cmd};
	$cmd =~ s/&seed/$seed/g;
	if ($sepdirs) {
		# Here, we replace ./ if it's at the start of the string or ./ if it's not
		# part of ../.  We then have to replicate over the non-dot first character
		# so that it's not deleted, hence the use of \1.
		$cmd =~ s#(?:^\./)|(?:([^./])\./)#\1../#g;
	} 
	if (!$t->{ignore_stderr}) {
		if ($t->{stderr}) {
			$cmd .= " 2>&1";
		} else {
			$cmd .= " 2>/dev/null";
		}
	}

	print "  Cmd: $cmd\n" if ($showcmd);
	#print "  Retry count:  $retries\n";

	my ($ofails);

	sub got_error {
		$failmsg = shift;
		print $failmsg;
		die;
	};

	TestLoop: for my $retry_count (1..$retries) {
		$failmsg = undef;
		if ($retry_count > 1) {
			print "  Retrying...\n";
			# Just in case there is some kind of transient problem.
			sleep 1;
		}
		my ($output,$rc,$timeout_error) = run_with_timeout($cmd,$timeout);

		if ($t->{stdio} || $t->{stderr} || !$outfile) {
			$outfile = $output;
		}
		
		my $failexecution = ($t->{fail});
		my $failchecker =  ($t->{failchecker});
		$ofails = $fails;
		eval {
			got_error("  Test timed out.\n") if ($timeout_error);
			# No timeout, so proceed with checking.
			my $fc = ($rc >> 8);
			if ($rc) {
				# Check return code from running the command
				if (!($failexecution)) {
					got_error(" Test failed and was not expected to.  Return code was $?. Output is in:\n\n$outfile\n");
				} else {
					if ($failexecution != $DontCare) {
						if ($fc != $failexecution) {
							got_error(" Test failed with a value of $? (return code of $fc), but $failexecution was expected.");
						}
					}
					print "  ...expected fail found.\n";
				}
			} elsif ($failexecution) {
				got_error(" Test did not fail but was expected to.  Return code was $?. Output is in:\n\n$outfile\n");
			}
			
			if ( $t->{diff} ) {
				got_error(" Failed diff test.\n") if (!doDiff($testindex,$outfile,$t->{diff},$t->{dpfx},$t->{nocmts}));
			}
			if ($t->{checker}) {
				# Call the check function and check its return code.
				my $checker = $t->{checker};
				eval { &$checker($outfile) };
				if ($@) {
					if ($failchecker) {
						print "  ...expected fail found.\n";
					} else {
						got_error("  Failed checker:  $@\n");
					}
				} else {
					if ($failchecker) {
						got_error("  Checker test did not fail but was expected to.\n");
					} else {
						print "  ...checker test passed.\n";
					}
				}
			}
			# If we get here, we've succeeded, so we exit the loop.
			last TestLoop;
		};
	}

	# If we did eventually get an error, then record it.
	error_no_die($failmsg,1) if ($failmsg);

	# Remove listed temporary files unless overridden by the user.
	if ($sepdirs) {
		# We're in a separate test directory, so we can just blow the whole
		# directory away.
		chdir $c;
		# For separate directories, keep the directory contents if we fail, since it
		# won't interfere with anything else.
		if (!$keepoutput && ($fails == $ofails)) {			
			# This might fail due to weird NFS issues, so don't check if it succeeds.
			system "rm -rf $testdir"
		}
	} else {
		# Not in a separate directory, so remove listed temporaries.
		if (!$keepoutput) {
			for (@{ $t->{temps} }) {
				dprint("Unlinking $_\n");
				unlink glob($_);
			}
		}
	}
}

# Main test code:  For each test, execute it, then scan the output for
# the tags we look for.  After that, check everything.
sub doTest {

  # get_run_list actually returns a hash, where the keys are
  # indices and the values are the items specified in the 
  # original test array.
  my $tests = get_run_list(shift);
	my $empty_list_ok = shift;
  print "\n";

	if (!keys(%$tests) && !$empty_list_ok) {
		print "\nRegression FAILED:  No tests were selected to be run.\n";
		exit 1;
	}

  # /pkg is not reliably updated on all machines in the farm.  Force the update
  # if adl is not installed on the machine we're on.  Do this only if we're in
  # the /pkg environment.
  if ( @checkpkg && -d "/pkg" ) {
    for (@checkpkg) {
      if (! -d "/pkg/$_") {
				print "Updating package $_...\n";
				`/run/pkg/foundation-/frame/sbin/update_pkg_entry $_`;
      }
    }
  }

  if (!$serialOverride && $lsf && !$lsfchild) {
		# Use LSF to dispatch tests in parallel.
    lsf_dispatch($tests);
	} elsif (!$serialOverride && $parrun && !$parchild) {
		# Use fork/join to dispatch tests in parallel.
		par_dispatch($tests);
  } else {
		# Run tests serially.
		my $total = scalar(keys %$tests);
		my $done = 0;
  TEST: for $testindex (sort { $a <=> $b} (keys %{$tests})) {
		my $t = $$tests{$testindex};
		runTest($testindex,$t);
		++$done;
	}
  }

	if (!$fails) {
		print "\nRegression SUCCEEDED.\n";
	} else {
		@failedtests = sort { $a <=> $b } @failedtests;
		print "\nRegression FAILED:  $fails fails found:  @failedtests.\n";
		exit 1;
	}
}

# Perform the diff between the output from the test and the
# regression file.
#
# arg0:  Test output file.
# arg1:  Name of regression file.
# arg2:  (optional).  Diff prefix.  If specified, we extract
#        only the output starting with the given prefix, then 
#        do the diff.
sub doDiff {
	my ($testindex,$outfile,$rfile,$dpfx,$nocmts) = @_;

  dprint("Performing diff against $rfile.\n");
	
	my $tmpfile = "cmp-t$testindex.out";
  unlink $tmpfile;
  open OUT,">$tmpfile" or die "Could not open $tmpfile for writing.";
  if ($dpfx) {
    # Prefix specified
    my @lines = split '\n',$outfile;
    for my $l (@lines) {
      if ($l =~ $dpfx) {
		print OUT $l,"\n";
      }
    }
  } elsif ($nocmts) {
	my $cmt = $nocmts;
    # We do not want to preserve comments, so strip all blank lines and
    # comment lines.
    my @lines = split '\n',$outfile;
    for my $l (@lines) {
      next if ($l =~ /^$cmt/ || $l =~ /^\s*$/);
	  if ( $l =~ /^([^#]+)$cmt/ ) {
		my $noncmt = $1;
		next if ( length($noncmt) == 0 || $noncmt =~ /^\s*$/);
		# Comment in line- print only non-comment portion.
		print OUT "$noncmt\n";
	  } else {
		# No comment.
		print OUT $l,"\n";
	  }
    }
  } else {
    # No prefix specified, so just print output to the file.
    print OUT $outfile;
  }
  close OUT;

  my $dout = `$diff $tmpfile $rfile`;
  if ($? > 8) {
    print " Error!  Output differs from regression file '$rfile'.\n";
    print "Diff output was:\n";
    print " <----- Test output file $tmpfile | Regression file $rfile ----->\n";
    print $dout;
		if ($keepdiff) {
			# Recreate the diff in a format suitable for patch.  Note that the order
			# is swapped so that patch will apply the changes to the expects file, not
			# the temporary output file.
			my $dfn = "t$testindex.diff";
			print "Saving diff output to $dfn\n";
			system ("$diff -u $rfile $tmpfile > $dfn");
		}
    return 0;
  } else {
    unlink $tmpfile unless $keepoutput;
    print "  ...diff check passed.\n";
    return 1;
  }
}

use Getopt::Long 2.33 qw(:config permute);
use Pod::Usage;

# This takes the original test array and returns a hash reference
# where the keys are test indices and the values are the elements of
# the original test.  The default is to return all tests; we modify that
# by the include and exclude specifiers.
sub get_run_list {
  my $tests = shift;
  my $size = scalar( @$tests );
  my (%includes, %excludes);
  my (@ilist,@xlist,@slist,@eslist,@xslist,@rlist,@extras);
  my ($help,$man,$list,$do_nothing);

  # Do we have a RegressOpts environment variable?  If so, add this to ARGV.
	# Don't do this for child items, because it might screw up the test list.
	# Unfortunately, we need to know this before we process the options...
	my $is_child = 0;
	foreach (@ARGV) {
		if ( /(lsf|par)-child/) {
			$is_child = 1;
			last;
		}
	}
	my $env_args = $ENV{RegressOpts};
	if ($env_args && !$is_child) {
		push @ARGV,(split / /,$env_args);
	}

	#print "Options:  @ARGV\n";

  # Process the command-line options, generating up a 
  # list of ranges to include and a list of ranges to exclude.
  if (!&GetOptions
      (
       "l|list"                => \$list,
       "h|help"                => \$help,
       "m|man"                 => \$man,
       "d|debug:i"             => \$debug,
       "t|tests:s"             => \@ilist,
       "s|string:s"            => \@slist,
       "xs|exclude-string:s"   => \@eslist,
       "sx|exclusive-string:s" => \@xslist,
       "r|regex:s"             => \@rlist,
       "x|excludes:s"          => \@xlist,
       "n|do-nothing"          => \$do_nothing,
       "ko|keepoutput"         => \$keepoutput,
			 "keepdiff"              => \$keepdiff,
       "seed=i"                => \$seed,
       "lsf:s"                 => \$lsf,
       "lsf-group=s"           => \$lsfgroup,
       "lsf-queue=s"           => \$lsfqueue,
       "lsf-res-str=s"         => \$lsf_res_str,
       "lsf-job-size:i"        => \$lsfjobsize,
			 "p|parallel-run=i"      => \$parrun,
       "sc|showcmd"            => \$showcmd,
       "lsf-child"             => \$lsfchild,
       "par-child"             => \$parchild,
       "check-pkg=s"           => \@checkpkg,
			 "timeout=i"             => \$timeout,
       "<>"                    => \&plusargs,
      )) {
    printhelp(1,1);
  }

  sub plusargs {
    push @extras,shift;
  }

  # A value on debug is optional.  If not specified, but the option is,
  # we want it to default to a value of 1.
  if ($debug == 0) {
    $debug = 1;
  } elsif ($debug < 0) {
    $debug = 0;
  }

  # Debugging implies showcmd.
  $showcmd = 1 if ($debug);

  # Print help if requested to do so.
  printhelp(0,1) if $help;

  printhelp(0,2) if $man;

  if (!$seed) {
    $seed = time();
  }

	if (defined($lsf) && !$lsf) {
		print "Error:  No LSF project string was specified.\n";
		exit 1;
	}

  print "Processed options from RegressOpts:  $ENV{RegressOpts}\n" if ($env_args && !$is_child);

  if ($lsf && !$lsfchild) {
		$lsfproj = $lsf;
		my $rstr = (length($lsf_res_str) > 0) ? "-R \"$lsf_res_str\"" : "";
    $lsf = "LSB_STDOUT_DIRECT=N RDRIVER_LSF_JOB=1 bsub -P $lsf  -C 0 -G $lsfgroup -q $lsfqueue $rstr";
    dprint ("Using lsf submission string: $lsf\n");
  }

  # This converts a list of range expressions (arg1)
  # into a hash (stored into hash reference arg0), where
  # the keys are indices of tests.
  sub get_indices {
    my $h = shift;
    my $pl = shift;

    for my $i (@$pl) {
      if ($i =~ /^([0-9]+)$/) {
				$$h{$1-1} = 1;
      } elsif ($i =~ /^([0-9]+)-$/) {
				for my $c ($1..$size) {
					$$h{$c-1} = 1;
				}
      } elsif ($i =~ /^-([0-9]+)$/) {
				for my $c (1..$1) {
					$$h{$c-1} = 1;
				}
      } elsif ($i =~ /^([0-9]+)-([0-9]+)$/) {
				for my $c ($1..$2) {
					$$h{$c-1} = 1;
				}
      }
    }
  }

  # If extra arguments are present, then we we try to find a test-constraint
  # option.  This will be either a list with an empty string (which will be
  # ignored) or a list with one argument due to the option-processor associating
  # the first item with that argument.  In either case, we append the extra
  # arguments to it.
  if (@extras) {
    if (@ilist) {
      push @ilist,@extras;
    } elsif (@slist) {
      push @slist,@extras;
    } elsif (@eslist) {
      push @eslist,@extras;
    } elsif (@rlist) {
      push @rlist,@extras;
    } elsif (@xlist) {
      push @xlist,@extras;
    } else {
      # Default- treat as test numbers.
      push @ilist,@extras;
    }
  }
  

  # First, break white-space delimited lists into individual elements.
  my (@p_ilist,@p_xlist);

  sub breakup {
    my $list = shift;
    for (@_) {
      push @$list,(split / /,$_);
    }
  }

  breakup (\@p_ilist, @ilist);
  breakup (\@p_xlist, @xlist);

  # Now convert the range specifications into hashes, where the
  # keys of each hash is an index of a test to include (or exclude).
  get_indices(\%includes,\@p_ilist);
  get_indices(\%excludes,\@p_xlist);

  # If the user didn't specify any includes, use all of them.
  if (!scalar(keys %includes)) {
    for my $i (0..$size-1) {
      $includes{$i} = 1;
    }
  }

  # Now remove all excluded indices from the include list.
  for my $i (keys %excludes) {
    delete $includes{$i};
  }

  # Now create a new test list with only the included indices.
  # If substrings were specified, include each item only if
  # the command contains all substrings.
  my %newtests;
 L1: for my $i (0..$size-1) {
	 if ($includes{$i}) {
		 my $test = $$tests[$i];
		 if (@xslist) {
			 # Have substrings, so do search.  If we have an exclusive-string list,
			 # then all must match in order to be used.
			 for my $s (@xslist) {
				 if ( index($test->{cmd},$s) < 0) {
					 next L1;
				 }
			 }
		 }
		 if (@slist) {
			 # If we have a simple substring list, then any must match in order to
			 # use the test.
			 my $pass = 0;
			 for my $s (@slist) {
				 if ( index($test->{cmd},$s) >= 0) {
					 $pass = 1;
				 }
			 }
			 if (!$pass) {
				 next L1;
			 }
		 }
		 if (@eslist) {
			 # If we have a list of exclude strings, then skip the test if it matches.
			 my $pass = 1;
			 for my $s (@eslist) {
				 if ( index($test->{cmd},$s) >= 0) {
					 $pass = 0;
				 }
			 }
			 if (!$pass) {
				 next L1;
			 }		
		 }
		 if (@rlist) {
			 # Have regular expressions, so do search and allow if a match occurs.
			 my $pass = 0;
			 for my $r (@rlist) {
				 if ( $test->{cmd} =~ /$r/ ) {
					 $pass = 1;
				 }
			 }
			 if (!$pass) {
				 next L1;
			 }
		 }
		 # Passed all tests, so include.
		 $newtests{$i+1} = $test;
	 }
 }

  print "Tests to be run:  ",join(' ',sort({ $a <=> $b} keys %newtests)),"\n\n" if ($debug);

  exit 0 if ($do_nothing);

  # List tests, if requested to do so.
  if ($list) {
    listtests(\%newtests);
  }

  return \%newtests;
}

# Called by users' regress scripts to set up for using separate regression
# directories to avoid conflicts during parallel runs.
sub setupForSeparateDirs {
	$sepdirs = 1;

	# If the src var is relative, then it needs to be modified. Don't do this if
	# we're an LSF job; the dispatcher will already have done this.
	if (!exists $ENV{RDRIVER_LSF_JOB}) {
		if ($src eq ".") {
			$src = "..";
		} elsif ($src !~ /^\//) {
			$src = "../$src";
		}
	}

	#print "Redhat release:\n";
	#system ("cat /etc/redhat-release");

	# Now, change to a temporary directory along the lines of what we'll use
	# for testing, so that we can get relative paths corrrect.
	my $td = mkdtemp("t.setupXXXXX");
	chdir $td;

	# Call any setup hook functions.
	for my $hook (@setuphooks) {
		&$hook();
	}

	chdir "..";
	rmdir $td;
}

sub listtests() {
  my $tests = shift;

  print "\nTests in this regression (commands to be executed):\n\n";
  for $testindex (sort { $a <=> $b} (keys %{$tests})) {
    print "  $testindex.  $tests->{$testindex}->{cmd}\n";
  }
  print "\n";
  exit 0;
}

# To be safe, we use the path to the module from %INC, since we know that that's
# from where the module was loaded.
sub printhelp() {
  my ($eval,$verbose) = (shift,shift);
  my $path = $INC{__PACKAGE__.".pm"};
  pod2usage(-exitval => $eval,-input=>$path,-verbose=>$verbose);
  exit 0;
}

sub file_rdiff {
  my $f = shift;
  my $r = shift;

  open IN,$f or die "Could not open $f for comparing.";

  if (!ref($r)) {
    # Not a reference, so assume that it's a string that we'll divide into
    # an array of strings.
    my @rl = split /\n/,$r;
    $r = \@rl;
  }
  dprint ("Match value:  ",Dumper($r),"\n");

  my $i = 0;
  my $max = scalar(@$r);
  while (<IN>) {
    ++$i if ( $i < $max && $_ =~ $r->[$i] )
  }
  if ($i != $max) {
    print "  Failed to match '$r->[$i]' in file '$f'.\n";
    die;
  }
}

sub str_rdiff {
  my @lines = split /\n/,shift;
  my $r = shift;

  if (!ref($r)) {
    # Not a reference, so assume that it's a string that we'll divide into
    # an array of strings.
    my @rl = split /\n/,$r;
    $r = \@rl;
  }
  dprint ("Match value:  ",Dumper($r),"\n");

  my $i = 0;
  my $max = scalar(@$r);
  for (@lines) {
    ++$i if ( $i < $max && $_ =~ $r->[$i] )
  }
  if ($i != $max) {
    print "  Failed to match '$r->[$i]'.\n";
    die;
  }
}

# This is a debug print function:  If debug is enabled, the output will show up.
sub dprint {
  if ($debug) {
	print @_;
  }
}

# Same as above, except that the first parameter is a debug level value.
sub dlprint {
  my ($level,@data) = @_;
  if ($level <= $debug) {
	print @data;
  }
}

# Equivalent of above for printf.
sub dprintf {
  if ($debug) {
	printf @_;
  }
}

1;
