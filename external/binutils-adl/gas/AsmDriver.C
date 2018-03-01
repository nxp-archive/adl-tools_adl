// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The DevTech assembler interface functions.
//

extern "C" {
#define  COMMON
#include "as.h"
#include "xregex.h"
#include "safe-ctype.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <inttypes.h>

  bfd_boolean  dt_debug(void);
}

#include <iostream>
#include <algorithm>

#include "adl-asm-impl.h"
#include "AsmDriver.h"

#include <iostream>

void init_line_assemble(unsigned &error_count,CADL_messages &messages,CADL_ext_relocations &ext_relocs);
void adl_assembler_init(void);
void adl_get_vles_encoding(const char **encoding, int *bits);
extern std::string adl_asm_version;

CADL_relocation_info::CADL_relocation_info(int relocType, int offset) : 
  m_ABI_relocation_type(relocType), 
  m_offset(offset)
{
  if (dt_debug()) {
    std::cerr << "\n relocation: type " << std::dec << m_ABI_relocation_type << " offset " << std::dec << m_offset;
  }
}

using namespace adl;

extern "C" {

  void ADL_init()
  {
    // Workaround for the 'unused argument' warning
    adl_assembler_init();
  }

  void ADL_set_b0_7_instead_r8_15 (bool b ATTRIBUTE_UNUSED) { }

  void ADL_set_refer_c_bit (bool b ATTRIBUTE_UNUSED) {};

  void ADL_set_big_memory_model (bool b ATTRIBUTE_UNUSED) {}

  const std::vector<std::string> &ADL_get_instructions_names()
  {
    static std::vector<std::string> instrs;
    const char** iNames;
    int   nInstr = 0;
  
    if (instrs.empty()) {
      adl_get_instr_names(&iNames, &nInstr);
      for(int i=0; i < nInstr; ++i) {
        instrs.push_back(iNames[i]);
      }
    }

    return instrs;
  }

  const std::vector<std::string> &ADL_get_operands_names()
  {
    static std::vector<std::string>  opNames;
    const adl_name_pair *ops;
    static int nOps = 0;

    if (opNames.empty()) {
      adl_get_instr_ops(&ops, &nOps);
      for(int i=0; i < nOps; ++i) {
        opNames.push_back(ops[i].name);
      }
    }
    return opNames;
  }

  const std::string &ADL_get_version()
  {
    return adl_asm_version;
  }

  void ADL_terminate()
  {
    input_scrub_end();
  }

  bool ADL_set_config(const std::vector<std::string>  *flags,std::vector<CADL_message> **pMessages)
  {
    static std::vector<CADL_message>  messages;
    *pMessages = &messages;
    for (int i=0; i < (int) flags->size(); ++i) {
      if ((*flags)[i] == "-no-rules") {
        adl_set_check_asm_rules(0);
      } else {
        messages.push_back(CADL_message((std::string("Unknown assembler flag ") + (*flags)[i]),e_severe_error));
      }
    }
    return messages.empty();
  }

  bool sort_relocs(const CADL_ext_relocation_info &x,const CADL_ext_relocation_info &y)
  {
    return x.m_order < y.m_order;
  }

  bool ADL_encode_vles(const std::string &vles, int &numOfBits, std::vector<unsigned short> **pCode,
                       CADL_relocations **pRelocs, CADL_messages **pMessages)
  {
    static unsigned                    error_count;
    static std::vector<unsigned short> code;
    static CADL_relocations            relocs;
    static CADL_ext_relocations        ext_relocs;
    static CADL_messages               messages;
    static char buffer[32768]; // 32K buffer for input vles, since md_assembler modifies input line

    try {
      error_count = 0;
      code.clear();
      relocs.clear();
      ext_relocs.clear();
      messages.clear();

      init_line_assemble(error_count,messages,ext_relocs);

      *pCode = &code;
      *pRelocs = &relocs;
      *pMessages = &messages;
      numOfBits = 0;

      if (dt_debug()) {
        std::cerr << "\n vles " << vles;
      }

      if (vles.size() > 32768) {
        throw CADL_message("Input line too long",e_severe_error);
      }
      int bitsNum = 0;

      // md_assembler modifies input line
      strcpy(&buffer[0],vles.c_str());
      md_assemble(&buffer[0]);

      const char *vCode = 0;

      adl_get_vles_encoding(&vCode,&bitsNum);
      const char *vCodeEnd = &vCode[bitsNum];
      const char *ptr = vCode;
      while(ptr != vCodeEnd) {
        unsigned short part = 0;
        for(unsigned i=0; i < sizeof(unsigned short) && ptr != vCodeEnd; ++i, ++ptr) {
          unsigned char c = *ptr;
          part = (part << 8) | c;
        }
        code.push_back(part);
      }
  
      numOfBits = bitsNum*8;
    }
    catch (CADL_message &err) {
      messages.push_back(err);
    }

    // Sort the relocations accordering to their initial orderig.
    sort(ext_relocs.begin(),ext_relocs.end(),sort_relocs);
    copy(ext_relocs.begin(),ext_relocs.end(),back_inserter(relocs));

    // Both the ADL error count and the GAS error count must be 0 for it to be
    // OK.  The gas error-count is incremented by such things as the expression
    // parser.
    return (error_count == 0 && !had_errors());
  }


};
