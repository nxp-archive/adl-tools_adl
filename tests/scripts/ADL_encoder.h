#ifndef __ADL_ENCODER_H__
#define __ADL_ENCODER_H__
#ifdef ADL_ENCODER


#ifdef WIN32  // Windows Platforms
	#define DLL_IMPORT	__declspec( dllimport ) __stdcall
	#define DLL_IMPORT_	__declspec( dllimport )
	#define STDCALL_	__stdcall
#else         // Unix/Linux Platforms
	#define DLL_IMPORT
	#define DLL_IMPORT_
	#define STDCALL_
#endif


//=========================
class CADL_relocation_info
{
public:
	int			m_ABI_relocation_type;

	// offset in the VLES in bytes
	int			m_offset;
};
//=========================
class CADL_message
{
public:
	std::string				m_str;
	e_message_priority		m_priority;
};
//=========================
void DLL_IMPORT ADL_init();

// effects: r8-15 --> b0-b7
void DLL_IMPORT ADL_set_b0_7_instead_r8_15(bool b);

// effects: avoid considering ordering according C-flag
void DLL_IMPORT ADL_set_refer_c_bit(bool b);

// effects: small-sign to absolute-address operands of move-like instructions
void DLL_IMPORT ADL_set_big_memory_model(bool b);

//=========================
DLL_IMPORT_ const std::string& STDCALL_ ADL_get_version();
//=========================
DLL_IMPORT_ const std::vector<std::string>& STDCALL_ ADL_get_instructions_names();
//=========================
DLL_IMPORT_ const std::vector<std::string>& STDCALL_ ADL_get_operands_names();
///delete void DLL_IMPORT ADL_get_instructions_names(std::vector<std::string>** ADL_instructions_names);;
///delete //=========================
///delete void DLL_IMPORT ADL_get_operands_names(std::vector<std::string>** ADL_operands_names);
//=========================
bool DLL_IMPORT ADL_encode_vles(
	// input:  VLES in text format.
	//         unknown-immediate (that need ABI-relocation info) 
	//         will be represented by "unknown_val"
	const std::string& vles_text,
	
	// output: coding size (in bits)
	int& num_of_bits,

	// output: code, little endian
	// Assembler sends address of a pointer, and the ADL-encoder updates 
	// the pointer' value to vector that the ADL allocated.
	std::vector<unsigned short>** code,

	// output: ABI-relocation-type to be used in VLES
	std::vector<CADL_relocation_info>** relocation_info,

	// output: messages to print in DT-assembler format, 
	// example:
	// ERROR --- Instruction 1, wrong operand used in operand 1.
	std::vector<CADL_message>** messages
	);
//=========================
void DLL_IMPORT ADL_terminate();
//=========================
bool DLL_IMPORT ADL_set_config(const std::vector<std::string>* adl_switch_text, std::vector<CADL_message>** messages);
//=========================


#endif
#endif
