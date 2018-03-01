#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#

#
# This file contains a list of all logging events.  It's used by
# various macro expanders to automatically create logging broadcast code.
#
# The main data structure is a hash, where the keys are the names of
# the 'C' logging event names.  Each value is an array, where names/types alternate.
#
# If a type is an array reference, then its elements are:
#   0:  type
#   1:  type if the data must be stored.
#   2:  constructor statement to copy the data.
#   3:  destructor statement, if necessary.
#   4:  argument expression, when calling the client loggers.
# 
# The corresponding LoggingIface method is generated from the event name by
# converting to "camel" case, i.e. removing underscores and capitalizing
# characters which follow an underscore.

# Note:  Each element is an array, but we use the => syntax for readability.

our @ISA = ("Exporter"); 
our @EXPORT = qw( %Events );

%Events = (
		   log_instr_prefetch => [
								  ea => addr_t,
								 ],

		   log_basic_block_end => [
								  ea => addr_t,
								 ],

		   log_instr_from_buffer => [
								 ],

 		   log_instr_read => [
 							  id => unsigned,
							  name => "const char *",
							  ea => addr_t,
							  ra => addr_t,
							  value => uint32_t,
							 ],

		   log_instr => [
						 opc => ["const uint32_t*","uint32_t*",
								 " _opc = new uint32_t[2]; _opc[0] = opc[0]; if ((num_half_bytes/8) > 1) { _opc[1] = opc[1];} ",
								 " delete [] _opc;"],
						 num_half_bytes => 'int',
						 name => "const char *",
						 dis => Disassembler,
						 flags => uint32_t,
						],
		   log_instr_args => [
							  opc => ["const uint32_t*","uint32_t*",
									  " _opc = new uint32_t[2]; _opc[0] = opc[0]; if ((num_half_bytes/8) > 1) { _opc[1] = opc[1];} ",
									  " delete [] _opc;"],
							  num_half_bytes => 'int',
							  name => "const char *",
							  dis => Disassembler,
							 ],
		   
		   log_instr_issue_time => [
									issue_time => ttime_t,
								   ],

		   log_instr_completion_time => [
										 completion_time => ttime_t,
										],

		   log_mmu_translation => [
								   tt    => TransType,
								   seq   => 'int',
								   set   => 'int',
								   way   => 'int',
								   ea    => addr_t,
								   ra    => addr_t,
								   trans => ["const MmuBase *","const MmuBase*",
														 "_trans = (trans->user_entry()) ? trans->clone() : trans;",
														 "if (_trans->user_entry()) delete _trans;"],
								  ],

		   log_cache_action => [
								name => "const char *",
								ca => CacheAction,
								cac => CacheAccess,
								level => unsigned,
								set => 'int',
								way => 'int',
								linemask => unsigned,
								ra => addr_t,
							   ],

	     log_reg_ext_write => [
				 id => unsigned,
				 name => "const char *",
				 value => ["const rnumber::RNumber &","RNumber"],				 
	            ],

	     log_regfile_ext_write => [
				 id => unsigned,
				 name => "const char *",
				 index => uint32_t,
				 value => ["const rnumber::RNumber &","RNumber"],				 
	            ],

		   log_reg_write => [
							 id => unsigned,
							 name => "const char *",
							 value => ["const rnumber::RNumber &","RNumber"],
							],

		   log_reg_read => [
							id => unsigned,
							name => "const char *",
							value => ["const rnumber::RNumber &","RNumber"],
						   ],

		   log_regfile_write => [
								 id => unsigned,
								 name => "const char *",
								 index => uint32_t,
								 value => ["const rnumber::RNumber &","RNumber"],
								],

		   log_regfile_read => [
								id => unsigned,
								name => "const char *",
								index => uint32_t,
								value => ["const rnumber::RNumber &","RNumber"],
							   ],

		   log_reg_write_mask => [
								  id => unsigned,
								  name => "const char *",
								  value => ["const rnumber::RNumber &","RNumber"],
								  mask => ["const rnumber::RNumber &","RNumber"],
								 ],

		   log_reg_read_mask => [
								 id => unsigned,
								 name => "const char *",
								 value => ["const rnumber::RNumber &","RNumber"],
								 mask => ["const rnumber::RNumber &","RNumber"],
								],

		   log_regfile_write_mask => [
									  id => unsigned,
									  name => "const char *",
									  index => uint32_t,
									  value => ["const rnumber::RNumber &","RNumber"],
									  mask => ["const rnumber::RNumber &","RNumber"],
									 ],

		   log_regfile_read_mask => [
									 id => unsigned,
									 name => "const char *",
									 index => uint32_t,
									 value => ["const rnumber::RNumber &","RNumber"],
									 mask => ["const rnumber::RNumber &","RNumber"],
									],
		   
		   log_core_mem_write => [
								  id => unsigned,
								  name => "const char *",
								  ea => addr_t,
								  nb => 'int',
								 ],

		   log_core_mem_read => [
								 id => unsigned,
								 name => "const char *",
								 ea => addr_t,
								 nb => 'int',
								],

		   log_core_mem_write_typed => [
								  id => unsigned,
								  name => "const char *",
								  ea => addr_t,
								  nb => 'int',
				          type => "CacheAccess",
								 ],

		   log_core_mem_read_typed => [
								 id => unsigned,
								 name => "const char *",
								 ea => addr_t,
								 nb => 'int',
				         type => "CacheAccess",
								],
		   
		   log_mem_write => [
							 id => unsigned,
							 name => "const char *",
							 pre => bool,
							 seq => 'int',
							 ea => addr_t,
							 ra => addr_t,
							 value => uint32_t,
							],

		   log_mem_read => [
							id => unsigned,
							name => "const char *",
							pre => bool,
							seq => 'int',
							ea => addr_t,
							ra => addr_t,
							value => uint32_t,
						   ],

		   log_breakpoint => [
							  ea => addr_t,
							  h => unsigned,
							  payload => "PtBase *",
							 ],

		   log_watchpoint => [
							  ea => addr_t,
							  pre => bool,
							  type => WatchType,
				        num_bytes => "int",
				        value => uint64_t,
							  h => unsigned,
							  payload => "PtBase *",
							 ],

		   log_exception => [
							 handle => unsigned,
							 pre    => bool,
							 name   => "const char *",
							],

		   log_annotation => [
							  type => "MsgType",
							  level => unsigned,
							  msg => ["const string &","string"],
				        ndata => unsigned,
        				data => ["LogBase::Data *","std::vector<LogBase::Data>",
												 "_data.insert(_data.begin(),&data[0],&data[ndata]);",
												 0,
				                 "((_data.empty()) ? 0 : &_data[0])"],
							 ],


		   log_branch_taken => [
				        ea => addr_t,
							   ],

		   log_branch_end => [
				        ea => addr_t,
							   ],

		   log_core_change => [
							  ],

		   log_ctx_change => [
							  id => unsigned,
							  name => "const char *",
							  context_num => unsigned,
							  update => CtxType,
							 ],
		  );
