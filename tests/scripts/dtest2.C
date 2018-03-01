//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A test of the shared assembler, used as StarCore uses it.
//
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>

#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#include <vector>
#include <set>
#include <stack>
#include <map>
#include <list>
#include <string>
#include <algorithm>

using namespace std;

#define ADL_ENCODER
#include "e_message_priority.h"
using namespace _asm_;
#include "./ADL_encoder.h"


void	*g_adl_asm_handle;
bool	(*g_ADL_encode_vles)(const std::string& vles_text,
                           int& num_of_bits,
                           std::vector<unsigned short>** code,
                           std::vector<CADL_relocation_info>** relocation_info,
                           std::vector<CADL_message>** messages
                           );


void* dlsym_err(char* func_name)
{
	void	(*func_ptr)(...);
	char	*error;

	dlerror();
	*(void **)(&func_ptr) = dlsym(g_adl_asm_handle, func_name);
	error = dlerror();
	if ((error != NULL) || (func_ptr == NULL)) {
    printf("\nError while loading function\n");
  }
	return (void*)func_ptr;
}

void dlsym_void(char* func_name)
{
	void	(*func_ptr)(...);

	*(void **)(&func_ptr) = dlsym_err(func_name);
	(*func_ptr)();
}

int main(int argc,const char *argv[])
{
	char								                *error;
	std::string						              adl_version;
	const std::string&		              (*ver_func_ptr)();
	std::string						              ADL_vles_text;
	int									                ADL_num_of_bits;
	std::vector<unsigned short>*        ADL_code;
	std::vector<CADL_relocation_info>*	ADL_relocation_info;
	std::vector<CADL_message>*			    ADL_messages;
	std::vector<int>					          instr_num_vec, op_num_vec;
	CADL_message						            ADL_message;
	bool								                b;


  if (argc < 2) {
    printf ("usage:  %s <so-lib-name> [asm string]\n\n",argv[0]);
    return 1;
  }

  std::string asmstr = " add r1,r2,r3 ";
  if (argc > 2) {
    asmstr = argv[2];
  }

	// load 'adl_asm.so'
	dlerror();	//clear former 'dlerror()'
	g_adl_asm_handle = dlopen(argv[1], RTLD_LAZY);
	error = dlerror();
	if ((error != NULL) || (g_adl_asm_handle == NULL)) {
    fprintf(stderr, "%s\n", error);
    exit(1);
  }
  
	// init
	dlsym_void((char*)"ADL_init");
	
	// version
	*(void **)(&ver_func_ptr) = dlsym_err((char*)"ADL_get_version");
	adl_version = (*ver_func_ptr)();
	printf("ADL-library version: %s\n", adl_version.c_str());

	// encoding
	ADL_vles_text = std::string(asmstr.c_str());
	*(void **)(&g_ADL_encode_vles) = dlsym_err((char*)"ADL_encode_vles");
	b = (*g_ADL_encode_vles)(ADL_vles_text,
                           ADL_num_of_bits,
                           &ADL_code,
                           &ADL_relocation_info,
                           &ADL_messages);
	if (b == true) {
    printf("\n Encoding Succeeded \n");
		for (int i=0; i<ADL_num_of_bits/16; i++) {
		  printf("   ADL_code[%d] = 0x%X  \n", i, (*ADL_code)[i]);
    }
  } else {
    printf("\n Encoding Failed \n");
  }
	printf("\n ADL_num_of_bits = %d\n", ADL_num_of_bits);
	printf("\n ADL_code->size() = %d\n", (int)ADL_code->size());
	printf("\n ADL_relocation_info->size() = %d\n", (int)ADL_relocation_info->size());
	printf("\n ADL_messages->size() = %d\n", (int)ADL_messages->size());
	for (unsigned i=0; i<ADL_messages->size(); i++) {
    ADL_message = (*ADL_messages)[i];
    switch (ADL_message.m_priority) {
    case e_info:
      printf("\n ADL_message[%d] is e_info \n", i);
      break;
    case e_note:
      printf("\n ADL_message[%d] is e_note\n", i);
      break;
    case e_warning:
      printf("\n ADL_message[%d] is e_warning \n", i);
      break;
    case e_error:
      printf("\n ADL_message[%d] is e_error \n", i);
      break;
    case e_severe_error:
      printf("\n ADL_message[%d] is e_severe_error \n", i);
      break;
    default:
      printf("\n ADL_message[%d] of type %d\n", i, ADL_message.m_priority);
      break;
#ifdef _DEBUG
    default:	DEBUG_MESS;
#endif
    }
    printf("\n ADL_message[%d] = %s\n", i, ADL_message.m_str.c_str());
  }

  for (unsigned i = 0; i != ADL_relocation_info->size(); ++i) {
    printf("\n ADL Relocation[%d]:  type %d offset %d.",i,(*ADL_relocation_info)[i].m_ABI_relocation_type,(*ADL_relocation_info)[i].m_offset);
  }

	
	printf("\nDONE\n");
}
