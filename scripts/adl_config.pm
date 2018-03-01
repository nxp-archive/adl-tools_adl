#!/pkg/perl-/5.8.9-FSL/x86_64-linux2.6-glibc2.5/bin/perl 
# -- -*-perl-*-a
#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#
#
# Autoconf generated configuration package for configuring Perl scripts used within ADL.
#

package adl_config;

require Exporter;

our @ISA = ("Exporter"); 
our @EXPORT = qw( 
                 $jit_ok
                 $jit_flags
                 $gdb_ok
                 $lz4
                 $lz4_ok
        				 $adl_version
				         $gcc_version
         				 $runsim_path
				         $dtasm_path
				         $lang_frontend
                 $prest
                 $llvm_bindir
                 $graphite_pfx
                );

$jit_ok        = 0;
$jit_flags     = " -I/pkg/fs-rnumber-/2.1.31-gcc_5.4.0/x86_64-linux2.6-glibc2.12/include -I/run/pkg/OSS-boost-/1.58.0-OSS-gcc_5.4.0/include";
$gdb_ok        = 1;
$lz4           = "/pkg/fs-foundation-/dynamic/bin/lz4";
$lz4_ok        = ("true" eq "true") ? 1 : 0;
$adl_version   = "3.17.2";
$gcc_version   = "5.4.0";
$runsim_path   = "";
$dtasm_path    = "";
$lang_frontend = "1";
$prest         = "/pkg/prest-/0.003040/bin/prest";
$llvm_bindir   = "@LLVM_BINDIR@";
$graphite_pfx  = "@GRAPHITE_PREFIX@";

1;
