// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various declarations used for the DevTech integration.
//

#ifndef _ASMDRIVER_H_
#define _ASMDRIVER_H_

#include <string>
#include <vector>

//******************************************
//******** types ***************************
//******************************************
//'e_message_priority' is used in the assembler internally. Using the enum that the assembler uses internally allows:
// - Using assembler flags as '-w' (disable warnings).
// - Integration in format output, example: when MS-style will be implemented, ADL-message will be ready automatically.
// - Stop the assembler in extreme cases.
enum e_message_priority
{
	// 'e_info' allows ADL-encoder to informs about legal cases .
	e_info,

	// 'e_note' allows ADL-encoder to informs about cases that are not documented (implantation-dependent).
	e_note,

	e_warning,
	e_error,

	// 'e_severe_error' allows ADL-encoder to stop execution of the assembler.
	e_severe_error,

	// used for 'msg' directive
	e_user_msg,

	e_num_of_message_priorities,
	e_no_such_message_priority
};


struct CADL_message
{
  CADL_message(std::string msg, e_message_priority priority) 
    : m_str(msg), m_priority(priority) {};

  CADL_message(const char *msg, e_message_priority priority) 
    : m_str(msg), m_priority(priority) {};

  std::string		m_str;
  e_message_priority	m_priority;
};

typedef std::vector<CADL_message> CADL_messages;

struct CADL_relocation_info
{
  CADL_relocation_info(int relocType, int offset);

  int  m_ABI_relocation_type;
  int  m_offset;
};

typedef std::vector<CADL_relocation_info> CADL_relocations;

struct CADL_ext_relocation_info : public CADL_relocation_info
{
  CADL_ext_relocation_info(int relocType, int offset, int order) :
    CADL_relocation_info(relocType,offset),
    m_order(order)
  {}

  int  m_order;
};

typedef std::vector<CADL_ext_relocation_info> CADL_ext_relocations;

#endif

