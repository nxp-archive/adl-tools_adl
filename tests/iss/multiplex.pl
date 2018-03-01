#!/usr/bin/env perl 
# -- -*-perl-*-a
#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#

#
# This is used to test the ability of the UDP RSP implementation to receive and
# correctly reply to traffic from multiple ports.  The gdbclient has opened 2
# sockets.  It then sends data from these two sockets simultaneously, so that if
# the model can't handle that, it will see a new packet rather than the ack it
# was expecting.
#

do {
	  $msg1 = get_next_line();
	  $msg2 = get_next_line();

	  write_data($socket1,$socket1,$msg1) if ($msg1);
	  write_data($socket2,$socket2,$msg2) if ($msg2);

	  read_response($socket1,$socket1,$outfile) if ($msg1);
	  read_response($socket2,$socket2,$outfile) if ($msg2);

} while ($msg1 && $msg2);

return 1;
