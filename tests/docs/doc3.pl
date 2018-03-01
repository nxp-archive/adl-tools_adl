$data = {
"cores" => {
"myCore" => {
"doc" => <<DATA
  my Core
  
DATA
,
"type" => "someCore",
"bit_endianness" => "little",
"blocks" => [
"agu",
"dalu",
],
"type_declarations" => [
" enum MsgTypes {
    Read
    ,
    Write
    ,
    LastMsg
} ;",
" enum  {
    Foo = LastMsg + 1
    ,
    Bar
} ;",
],
"parallel_execution" => 4,
"RaMask" => {
"initial" => "0xffffff",
"constant" => 1,
},
"EaMask" => {
"initial" => "0xffffff",
"constant" => 1,
},
"regs" => {
"A" => {
"width" => 24,
"pseudo" => 1,
"shared" => 0,
"read" => {
"alias" => "PC",
},
"write" => {
"alias" => "PC",
},
},
"B" => {
"width" => 32,
"pseudo" => 1,
"shared" => 0,
"read" => {
"alias" => "GPR",
"alias_index" => 1,
},
"write" => {
"alias" => "GPR",
"alias_index" => 1,
},
},
"CCR" => {
"doc" => <<DATA
      Cache control register.
      
DATA
,
"width" => 32,
"fields" => {
"L1E" => {
"bits" => [
[
0,
0,
],
],
},
"L1dHitCnt" => {
"bits" => [
[
7,
4,
],
],
},
"L1dMissCnt" => {
"bits" => [
[
11,
8,
],
],
},
"L2E" => {
"bits" => [
[
1,
1,
],
],
},
"L2dMissCnt" => {
"bits" => [
[
19,
16,
],
],
},
"L2iMissCnt" => {
"bits" => [
[
15,
12,
],
],
},
"WAY8" => {
"bits" => [
[
2,
2,
],
],
},
},
"reserved_mask" => "0b11111111111100000000000000001000",
"shared" => 0,
},
"DEAR" => {
"width" => 32,
"shared" => 0,
},
"ESR" => {
"width" => 32,
"shared" => 0,
},
"FACTIVE" => {
"width" => 32,
"shared" => 0,
},
"FOO" => {
"width" => 32,
"shared" => 0,
},
"HDBCR0" => {
"width" => 32,
"fields" => {
"MMU_ENABLE" => {
"bits" => [
[
0,
0,
],
],
},
},
"reserved_mask" => "0b11111111111111111111111111111110",
"shared" => 0,
},
"IAR" => {
"width" => 32,
"shared" => 0,
},
"MSR" => {
"width" => 32,
"fields" => {
"PR" => {
"bits" => [
[
10,
10,
],
],
},
},
"reserved_mask" => "0b11111111111111111111101111111111",
"shared" => 0,
},
"NIA0" => {
"width" => 24,
"pseudo" => 1,
"shared" => 0,
"read" => {
"alias" => "PC",
},
"write" => {
"alias" => "PC",
},
},
"NIA1" => {
"width" => 24,
"pseudo" => 1,
"shared" => 0,
"read" => {
"alias" => "PC",
},
"write" => {
"alias" => "PC",
},
},
"PC" => {
"doc" => <<DATA
    PC
    
DATA
,
"width" => 24,
"offset" => 2,
"serial" => 1,
"fields" => {
"A" => {
"bits" => [
[
18,
16,
],
],
"reserved" => 1,
},
"B" => {
"bits" => [
[
15,
0,
],
],
"readonly" => 1,
},
},
"reserved_mask" => "0b111111000000000000000011",
"readonly_mask" => "0b000000111111111111111100",
"attributes" => {
"cia" => "",
"nia" => "",
},
"reset" => "3",
"shared" => 0,
},
"PID0" => {
"width" => 32,
"shared" => 0,
},
"PID1" => {
"width" => 32,
"shared" => 0,
},
"PS" => {
"width" => 32,
"pseudo" => 1,
"enumerated" => {
"0" => "a",
"1" => "b",
"2" => "c",
},
"shared" => 0,
"read" => {
<<DATA
{
    return PC ;
}
DATA
,
},
"write" => {
<<DATA
{
    PC = v ;
}
DATA
,
},
},
"TACTIVE" => {
"width" => 32,
"shared" => 0,
},
"a" => {
"width" => 32,
"fields" => {
"f0" => {
"bits" => [
[
14,
5,
],
[
2,
0,
],
],
},
"f1" => {
"bits" => [
[
11,
2,
],
],
},
},
"reserved_mask" => "0b11111111111111111000000000000000",
"shared" => 0,
},
},
"regfiles" => {
"B" => {
"width" => 32,
"pseudo" => 1,
"size" => 8,
"prefix" => "b",
"shared" => 0,
"read" => {
"alias" => {
"reg" => "GPR",
"map" => {
"0" => 8,
"1" => 9,
"2" => 10,
"3" => 11,
"4" => 12,
"5" => 13,
"6" => 14,
"7" => 15,
},
},
},
"write" => {
"alias" => {
"reg" => "GPR",
"map" => {
"0" => 8,
"1" => 9,
"2" => 10,
"3" => 11,
"4" => 12,
"5" => 13,
"6" => 14,
"7" => 15,
},
},
},
},
"GPR" => {
"width" => 32,
"size" => 32,
"prefix" => "r",
"shared" => 0,
},
"GPRS" => {
"width" => 32,
"pseudo" => 1,
"size" => 32,
"shared" => 0,
"read" => {
"alias" => {
"reg" => "GPR",
},
},
"write" => {
"alias" => {
"reg" => "GPR",
},
},
},
"RxTable" => {
"width" => 32,
"size" => 32,
"shared" => 0,
"entries" => {
"0" => {
"syntax" => "b0",
"reg" => "B(0)",
},
"1" => {
"syntax" => "B",
"reg" => "B(1)",
},
"2" => {
"syntax" => "B",
"reg" => "B(2)",
},
"3" => {
"syntax" => "B",
"reg" => "B(3)",
},
"7" => {
"syntax" => "sp",
"read" => <<DATA
{
    return ( B ( 0 ) == 1 ) ? B ( 1 ) : B ( 2 ) ;
}
DATA
,
"write" => <<DATA
{
    if ( GPR ( 3 ) == 1 ) {
        PC = x ;
    } else {
        GPR ( 11 ) = x ;
    }
}
DATA
,
},
},
},
},
"instrfields" => {
"ADCD" => {
"bits" => [
[
5,
5,
],
],
"width" => 1,
"size" => 1,
"mask" => "0x20",
"type" => "imm",
},
"ENC" => {
"bits" => [
[
31,
0,
],
],
"width" => 32,
"size" => 32,
"mask" => "0xffffffff",
"type" => "imm",
},
"F1" => {
"bits" => [
[
31,
16,
],
],
"prefix" => 1,
"width" => 16,
"size" => 4,
"indexed" => 4,
"mask" => "0xffff0000",
"addr" => "pc",
"display" => "hex",
"type" => "imm",
"overlay" => 1,
"signed" => 1,
"inverted" => 1,
"action" => <<DATA
{
    ThisField = bits ( s , s ) ;
}
DATA
,
},
"F2" => {
"pseudo" => 1,
"width" => 16,
"size" => 3,
"mask" => "0x0",
"table" => {
"0" => [
1,
4,
5,
],
"1" => "reserved",
"2" => [
1,
2,
3,
],
"3" => [
4,
4,
4,
],
},
"enumerated" => {
"0" => "a",
"1" => "b",
"2" => "reserved",
"3" => "d",
},
"type" => "mem",
"value" => 0,
"ref" => "IMEM",
},
"F2_imp_bits__15_0_" => {
"bits" => [
[
15,
0,
],
],
"width" => 16,
"size" => 3,
"mask" => "0xffff",
"table" => {
"0" => [
1,
4,
5,
],
"1" => "reserved",
"2" => [
1,
2,
3,
],
"3" => [
4,
4,
4,
],
},
"enumerated" => {
"0" => "a",
"1" => "b",
"2" => "reserved",
"3" => "d",
},
"type" => "mem",
"value" => 0,
"implements" => "F2",
"ref" => "IMEM",
},
"F3" => {
"bits" => [
[
15,
0,
],
],
"prefix" => 1,
"width" => 16,
"size" => 16,
"mask" => "0xffff",
"type" => "imm",
"valid_ranges" => [
[
0,
1,
],
[
6,
12,
],
],
"valid_masks" => [
[
0,
1,
],
[
6,
12,
],
],
},
"JJJJJ" => {
"syntax" => "Da,Db",
"pseudo" => 1,
"width" => 7,
"size" => 8,
"mask" => "0x0",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "GPR",
"fields" => [
[
6,
6,
],
"DaDb" => {
1,
},
[
5,
5,
],
"DaDb" => {
0,
},
],
"instrfields" => {
"Da" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DaDb" => {
"size" => 3,
"bits" => [
[
4,
0,
],
],
"table" => {
"0" => [
0,
4,
],
"1" => [
0,
5,
],
"2" => [
0,
6,
],
"3" => [
0,
7,
],
"4" => [
1,
4,
],
"5" => [
1,
5,
],
"6" => [
1,
6,
],
},
},
"Db" => {
"size" => 4,
"fields" => [
[
7,
4,
],
],
},
},
},
"JJJJJ_imp_bits__18_17_x_4_0_" => {
"syntax" => "Da,Db",
"bits" => [
[
18,
17,
],
[
4,
0,
],
],
"width" => 7,
"size" => 8,
"mask" => "0x6001f",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "JJJJJ",
"ref" => "GPR",
"fields" => [
[
6,
6,
],
"DaDb" => {
1,
},
[
5,
5,
],
"DaDb" => {
0,
},
],
"instrfields" => {
"Da" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DaDb" => {
"size" => 3,
"bits" => [
[
4,
0,
],
],
"table" => {
"0" => [
0,
4,
],
"1" => [
0,
5,
],
"2" => [
0,
6,
],
"3" => [
0,
7,
],
"4" => [
1,
4,
],
"5" => [
1,
5,
],
"6" => [
1,
6,
],
},
},
"Db" => {
"size" => 4,
"fields" => [
[
7,
4,
],
],
},
},
},
"JJJJJ_imp_bits__6_0_" => {
"syntax" => "Da,Db",
"bits" => [
[
6,
0,
],
],
"width" => 7,
"size" => 8,
"mask" => "0x7f",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "JJJJJ",
"ref" => "GPR",
"fields" => [
[
6,
6,
],
"DaDb" => {
1,
},
[
5,
5,
],
"DaDb" => {
0,
},
],
"instrfields" => {
"Da" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DaDb" => {
"size" => 3,
"bits" => [
[
4,
0,
],
],
"table" => {
"0" => [
0,
4,
],
"1" => [
0,
5,
],
"2" => [
0,
6,
],
"3" => [
0,
7,
],
"4" => [
1,
4,
],
"5" => [
1,
5,
],
"6" => [
1,
6,
],
},
},
"Db" => {
"size" => 4,
"fields" => [
[
7,
4,
],
],
},
},
},
"OPCD" => {
"bits" => [
[
4,
0,
],
],
"width" => 5,
"size" => 5,
"mask" => "0x1f",
"type" => "imm",
},
"Opcode" => {
"pseudo" => 1,
"width" => 0,
"size" => 0,
"mask" => "0x0",
"type" => "imm",
},
"POPCD" => {
"bits" => [
[
31,
16,
],
],
"prefix" => 1,
"width" => 16,
"size" => 16,
"mask" => "0xffff0000",
"type" => "imm",
},
"RA" => {
"doc" => <<DATA
      Field used to specify a General Purpose Register to be used as a source.
      
DATA
,
"bits" => [
[
15,
11,
],
],
"width" => 5,
"size" => 2,
"mask" => "0xf800",
"table" => {
"0" => [
0,
1,
],
"1" => "reserved",
"2" => [
2,
3,
],
"3" => "reserved",
},
"type" => "imm",
},
"RS" => {
"doc" => <<DATA
      Field used to specify a General Purpose Register as a target.
      
DATA
,
"bits" => [
[
10,
6,
],
],
"width" => 5,
"size" => 5,
"mask" => "0x7c0",
"type" => "imm",
},
"Res" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"mask" => "0x0",
"type" => "imm",
},
"Src1" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"mask" => "0x0",
"type" => "imm",
},
"Src2" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"mask" => "0x0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_" => {
"bits" => [
[
19,
19,
],
[
15,
15,
],
],
"width" => 2,
"size" => 2,
"mask" => "0x88000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_6_2_" => {
"bits" => [
[
19,
19,
],
[
15,
15,
],
[
6,
2,
],
],
"width" => 7,
"size" => 7,
"mask" => "0x8807c",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__31_16_" => {
"bits" => [
[
31,
16,
],
],
"width" => 16,
"size" => 16,
"mask" => "0xffff0000",
"type" => "imm",
},
"jj" => {
"pseudo" => 1,
"width" => 4,
"size" => 8,
"mask" => "0x0",
"blocks" => [
"dalu",
],
"type" => "imm",
"fields" => [
[
3,
3,
],
"DaDa" => {
1,
},
[
2,
2,
],
"DaDa" => {
0,
},
],
"instrfields" => {
"Da" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DaDa" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
1,
1,
],
"1" => [
3,
3,
],
"2" => [
5,
5,
],
"3" => [
7,
7,
],
},
},
"Db" => {
"size" => 4,
"fields" => [
[
7,
4,
],
],
},
},
},
"jj_imp_bits__18_17_x_1_0_" => {
"bits" => [
[
18,
17,
],
[
1,
0,
],
],
"width" => 4,
"size" => 8,
"mask" => "0x60003",
"blocks" => [
"dalu",
],
"type" => "imm",
"implements" => "jj",
"fields" => [
[
3,
3,
],
"DaDa" => {
1,
},
[
2,
2,
],
"DaDa" => {
0,
},
],
"instrfields" => {
"Da" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DaDa" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
1,
1,
],
"1" => [
3,
3,
],
"2" => [
5,
5,
],
"3" => [
7,
7,
],
},
},
"Db" => {
"size" => 4,
"fields" => [
[
7,
4,
],
],
},
},
},
},
"instrs" => {
"I1" => {
"width" => 32,
"syntax" => "I1 JJJJJ",
"attributes" => {
"usual" => "",
"string" => "d",
"integer" => 3,
"strlist" => [
"d",
"d",
"g",
],
"intlist" => [
1,
2,
3,
],
},
"fields" => {
"VarInstrOpcode_imp_bits__31_16_" => 64250,
"JJJJJ_imp_bits__6_0_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
},
"I2" => {
"width" => 32,
"doc" => <<DATA
      some prefix instruction
      
DATA
,
"syntax" => "I2 F3",
"attributes" => {
"prefix" => "",
},
"fields" => {
"POPCD" => 65535,
"F3" => "",
},
"action" => <<DATA
{
    PC = F3 ;
}
DATA
,
"assembler" => <<DATA
{
}
DATA
,
"prefix" => 1,
"disassemble" => 1,
"outputs" => [
"PC",
],
},
"I3" => {
"width" => 32,
"syntax" => "I3 ENC",
"fields" => {
"ENC" => "",
},
"pseudo" => 1,
"disassemble" => 1,
},
"I4" => {
"width" => 32,
"syntax" => "I4 F2_imp_bits__15_0_",
"fields" => {
"VarInstrOpcode_imp_bits__31_16_" => 1,
"F2_imp_bits__15_0_" => "",
},
"action" => <<DATA
{
    IMEM ( 0 , 4 ) = 3 ;
    GPR ( F2 ) = sincos ( IMEM ( GPR ( F2 ) , 4 ) ) ;
    GPR ( F2 + 1 ) = sincos ( IMEM ( GPR ( F2 + 1 ) , 4 ) ) ;
    raiseException ( yo ) ;
}
DATA
,
"disassemble" => 1,
"type" => "I3",
"inputs" => [
"GPR(F2)",
"GPR(F2 + 1)",
],
"outputs" => [
"GPR(F2)",
"GPR(F2 + 1)",
],
"input_mems" => [
"IMEM",
],
"output_mems" => [
"IMEM",
],
"helpers" => [
"sincos",
],
"raises_exception" => 1,
},
"I5" => {
"width" => 32,
"syntax" => "aaa",
"alias" => {
"target" => "I4",
"sources" => [
],
"destinations" => [
],
},
"disassemble" => 0,
"type" => "I3",
},
"add" => {
"width" => 16,
"syntax" => "add RA, RS",
"attributes" => {
"usual" => "",
},
"fields" => {
"RA" => "",
"RS" => "",
"ADCD" => 1,
"OPCD" => 10,
},
"action" => <<DATA
{
     bits < 32 > addr ;
    eaRegDirect( RS , addr ) ;
     bits < 32 > res ;
    addOp( RA , addr , res ) ;
    GPR ( RA . uint32 (  ) ) = res ;
}
DATA
,
"disassemble" => 1,
"outputs" => [
"GPR(RA . uint32 (  ))",
],
},
"addi" => {
"width" => 16,
"syntax" => "addi RA, RS",
"fields" => {
"RA" => "",
"RS" => "",
"ADCD" => 1,
"OPCD" => 11,
},
"action" => <<DATA
{
     bits < 32 > addr ;
    eaRegDirect( RS , addr ) ;
     bits < 32 > res ;
    subOp( RA , addr , res ) ;
    GPR ( RA . uint32 (  ) ) = res ;
}
DATA
,
"disassemble" => 1,
"outputs" => [
"GPR(RA . uint32 (  ))",
],
},
"mac" => {
"syntax" => "mac +Src1,Src2",
"block" => "dalu",
"fields" => {
"Opcode" => "",
"Src1" => "",
"Src2" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( GPR ( Src1 ) ( 31 , 16 ) , 41 ) , signExtend ( GPR ( Src2 ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
}
DATA
,
"disassemble" => 1,
"inputs" => [
"GPR(Src1)[mask 0xffff0000]/p",
"GPR(Src2)[mask 0xffff0000]/p",
],
},
"mac_pDa_Da_Dn" => {
"width" => 24,
"syntax" => "mac +jj.Da,jj.Db",
"block" => "dalu",
"attributes" => {
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_6_2_" => 88,
"jj_imp_bits__18_17_x_1_0_" => "",
},
"alias" => {
"target" => "mac",
"sources" => [
	"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
	"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
],
"destinations" => [
],
},
"disassemble" => 1,
},
"mac_pDa_Db_Dn" => {
"width" => 24,
"syntax" => "mac +JJJJJ.Da,JJJJJ.Db",
"block" => "dalu",
"attributes" => {
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_" => 0,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
},
"alias" => {
"target" => "mac",
"sources" => [
	"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
	"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
],
"destinations" => [
],
},
"disassemble" => 1,
},
"subi" => {
"width" => 16,
"syntax" => "subi RA, RS",
"fields" => {
"RA" => "",
"RS" => "",
"ADCD" => 0,
"OPCD" => 11,
},
"action" => <<DATA
{
     bits < 32 > addr ;
    eaRegIndirect( RS , addr ) ;
     bits < 32 > res ;
    subOp( RA , addr , res ) ;
    GPR ( RA . uint32 (  ) ) = res ;
}
DATA
,
"disassemble" => 1,
"outputs" => [
"GPR(RA . uint32 (  ))",
],
"input_mems" => [
"Mem",
],
},
},
"subinstrs" => {
"addOp" => {
"syntax" => "addOp",
"attributes" => {
"usual" => "",
},
"fields" => {
"OPCD" => 10,
},
},
"eaRegDirect" => {
"syntax" => "eaRegDirect",
"fields" => {
"ADCD" => 1,
},
},
"eaRegIndirect" => {
"syntax" => "eaRegIndirect",
"fields" => {
"ADCD" => 0,
},
"input_mems" => [
"Mem",
],
},
"subOp" => {
"syntax" => "subOp",
"fields" => {
"OPCD" => 11,
},
},
},
"exceptions" => {
"DTlbMiss" => {
"priority" => 0,
"action" => <<DATA
{
}
DATA
,
},
"Foo" => {
"doc" => <<DATA
Program interrupt.
DATA
,
"priority" => 0,
"action" => <<DATA
{
    FOO = 12 ;
    IAR = 0x0000700 ;
}
DATA
,
},
"ITlbMiss" => {
"priority" => 0,
"action" => <<DATA
{
}
DATA
,
},
"Program" => {
"doc" => <<DATA
Program interrupt.
DATA
,
"fields" => {
"ESR" => {
"doc" => <<DATA

DATA
,
"bits" => 32,
},
},
"priority" => 0,
"action" => <<DATA
{
    FOO = 12 ;
    IAR = 0x0000700 ;
    ESR = p . ESR ;
}
DATA
,
},
"ProtectionFault" => {
"priority" => 0,
"action" => <<DATA
{
}
DATA
,
},
"yo" => {
"priority" => 0,
"action" => <<DATA
{
}
DATA
,
},
},
"contexts" => {
"foo" => {
"thread" => 1,
"active" => <<DATA
{
    return FACTIVE ;
}
DATA
,
"num_contexts" => 3,
"regs" => [
"PC",
],
},
"thread" => {
"active" => <<DATA
{
    return TACTIVE ;
}
DATA
,
"num_contexts" => 2,
"regs" => [
"ESR",
"FOO",
"IAR",
],
"regfiles" => [
"GPR",
],
},
},
"caches" => {
"L1d" => {
"level" => 1,
"type" => "data",
"size" => 32768,
"line_size" => 256,
"assoc" => 4,
"enable-predicate" => <<DATA

DATA
,
"hit" => <<DATA

DATA
,
"miss" => <<DATA

DATA
,
"invalidate-line" => <<DATA

DATA
,
"read-line" => <<DATA

DATA
,
"replace" => <<DATA

DATA
,
"line-access" => <<DATA

DATA
,
"writethrough-predicate" => <<DATA

DATA
,
"number-of-sets" => <<DATA

DATA
,
"number-of-ways" => <<DATA

DATA
,
},
"L1i" => {
"level" => 1,
"type" => "instr",
"size" => 32768,
"line_size" => 256,
"fully_assoc" => 1,
"setfields" => {
"a" => {
"doc" => <<DATA
 Set field A.
 
DATA
,
"bits" => 32,
},
"b" => {
"doc" => <<DATA
 Set field B.
 
DATA
,
"bits" => 32,
},
"c" => {
"doc" => <<DATA
 Set field C.
 
DATA
,
"bits" => 32,
},
},
"wayfields" => {
"x" => {
"doc" => <<DATA

DATA
,
"bits" => 6,
},
"y" => {
"doc" => <<DATA

DATA
,
"bits" => 7,
},
"z" => {
"doc" => <<DATA

DATA
,
"bits" => 8,
},
},
"enable-predicate" => <<DATA

DATA
,
"hit" => <<DATA

DATA
,
"miss" => <<DATA

DATA
,
"invalidate-line" => <<DATA

DATA
,
"read-line" => <<DATA

DATA
,
"replace" => <<DATA
{
     unsigned tmp = L1d ( set ) . b ;
    ThisCache ( set ) . b = ( L1d ( set ) . b % 4 ) ;
    return tmp ;
}
DATA
,
"line-access" => <<DATA
{
    ThisCache ( set , way ) . y ++ ;
}
DATA
,
"writethrough-predicate" => <<DATA

DATA
,
"number-of-sets" => <<DATA

DATA
,
"number-of-ways" => <<DATA

DATA
,
},
"L2" => {
"level" => 2,
"type" => "unified",
"size" => 131072,
"line_size" => 128,
"assoc" => 8,
"enable-predicate" => <<DATA

DATA
,
"hit" => <<DATA

DATA
,
"miss" => <<DATA

DATA
,
"invalidate-line" => <<DATA

DATA
,
"read-line" => <<DATA

DATA
,
"replace" => <<DATA

DATA
,
"line-access" => <<DATA

DATA
,
"writethrough-predicate" => <<DATA

DATA
,
"number-of-sets" => <<DATA
{
    return ( CCR . WAY8 == 0 ) ? 128 : 64 ;
}
DATA
,
"number-of-ways" => <<DATA
{
    return ( CCR . WAY8 == 0 ) ? 4 : 8 ;
}
DATA
,
},
},
"eventbuses" => {
"msg" => {
"fields" => {
"addr" => {
"doc" => <<DATA

DATA
,
"bits" => 32,
},
"data" => {
"doc" => <<DATA

DATA
,
"bits" => 32,
},
"foo" => {
"doc" => <<DATA

DATA
,
"bits" => 20,
},
"type" => {
"doc" => <<DATA

DATA
,
"bits" => 0,
},
},
"action" => <<DATA
{
    CCR = x . data ;
}
DATA
,
},
},
"memories" => {
"IMEM" => {
"instr_mem" => 0,
"size" => 16,
"addr_unit" => 1,
},
"PreFetch" => {
"instr_mem" => 0,
"prefetch_mem" => 1,
"size" => 16,
"addr_unit" => 4,
},
"data" => {
"instr_mem" => 0,
"size" => 512,
"addr_unit" => 1,
"parent" => [
"instr",
256,
],
"read" => <<DATA
{
    return d . swap_bytes (  ) ;
}
DATA
,
"write" => <<DATA
{
    return d . swap_bytes (  ) ;
}
DATA
,
},
"instr" => {
"instr_mem" => 1,
"size" => 1024,
"addr_unit" => 4,
},
"instr2" => {
"instr_mem" => 0,
"size" => 32,
"addr_unit" => 1,
"parent" => [
"instr",
512,
],
},
},
"mmu" => {
"lookups" => {
"TlbCam" => {
"priority" => 0,
"page-size" => {
"bitsize" => 1,
"scale" => 1,
"field" => "SIZE",
"offset" => 0,
"shift" => 0,
},
"entries" => 16,
"fullyassoc" => 1,
"setfields" => {
"Foo" => {
"doc" => <<DATA

DATA
,
"bits" => 0,
},
},
"wayfields" => {
"EPN" => {
"doc" => <<DATA
   Example documentation string for an MMU field.
   
DATA
,
"bits" => 24,
},
"RPN" => {
"doc" => <<DATA

DATA
,
"bits" => 24,
},
"SIZE" => {
"doc" => <<DATA

DATA
,
"bits" => 6,
},
"TID" => {
"doc" => <<DATA

DATA
,
"bits" => 6,
},
"V" => {
"doc" => <<DATA

DATA
,
"bits" => 1,
},
"WIMG" => {
"doc" => <<DATA

DATA
,
"bits" => 4,
},
"x" => {
"doc" => <<DATA

DATA
,
"bits" => 2,
},
"y" => {
"doc" => <<DATA

DATA
,
"bits" => 3,
},
},
"tests" => [
<<DATA

DATA
,
<<DATA

DATA
,
],
"exec-perm" => <<DATA
{
    if ( ! ( ( MSR . PR (  ) ) ? t . SX : t . UX ) ) {
        raiseException ( ProtectionFault ) ;
    }
}
DATA
,
"load-perm" => <<DATA
{
    if ( ! ( ( MSR . PR (  ) ) ? t . SR : t . UR ) ) {
        raiseException ( ProtectionFault ) ;
    }
}
DATA
,
"store-perm" => <<DATA
{
    if ( ! ( ( MSR . PR (  ) ) ? t . SW : t . UW ) ) {
        raiseException ( ProtectionFault ) ;
    }
}
DATA
,
"valid" => <<DATA
{
    return ( V != 0 ) ;
}
DATA
,
"hit" => <<DATA
{
}
DATA
,
"miss" => <<DATA
{
}
DATA
,
"reset" => <<DATA
{
}
DATA
,
},
},
"instr-enable" => <<DATA
{
    return HDBCR0 . MMU_ENABLE (  ) ;
}
DATA
,
"data-enable" => <<DATA
{
    return HDBCR0 . MMU_ENABLE (  ) ;
}
DATA
,
"instr-miss" => <<DATA
{
    raiseException ( ITlbMiss ) ;
}
DATA
,
"data-miss" => <<DATA
{
    DEAR = a ;
    raiseException ( DTlbMiss ) ;
}
DATA
,
"multi-hit" => <<DATA

DATA
,
"aligned-write" => <<DATA
{
     int i = 20 ;
}
DATA
,
"misaligned-write" => <<DATA
{
     int j = 30 ;
}
DATA
,
},
"core-level-hooks" => {
"decode-miss" => <<DATA

DATA
,
"pre-cycle" => <<DATA

DATA
,
"post-cycle" => <<DATA

DATA
,
"pre-fetch" => <<DATA

DATA
,
"post-fetch" => <<DATA

DATA
,
"post-exec" => <<DATA

DATA
,
"post-asm" => <<DATA

DATA
,
"post-packet-asm" => <<DATA

DATA
,
"post-packet" => <<DATA

DATA
,
"active-watch" => <<DATA

DATA
,
"instr-table-watch" => <<DATA

DATA
,
},
"parms" => {
"p1" => {
"value" => "a",
"options" => [
"a",
"b",
"c",
],
},
"p2" => {
"value" => "a1",
"options" => [
"a1",
"b1",
"c1",
],
"watch" => <<DATA
{
    if ( PC == 2 ) {
        p2 = b1 ;
    } else {
        p2 = a1 ;
    }
}
DATA
,
"constant" => 1,
},
"p3" => {
"value" => "true",
"options" => [
"false",
"true",
],
},
},
"aspects" => [
{
"type" => "instr",
"location" => [
"before",
],
"destinations" => [
"I1",
],
"action" => <<DATA
{
    raiseException ( yo ) ;
    return ;
}
DATA
,
},
{
"type" => "instr",
"location" => [
"before",
"after",
],
"destinations" => [
"I1",
],
"action" => <<DATA
{
    raiseException ( yo ) ;
    return ;
}
DATA
,
},
],
"asm_config" => {
"comments" => [
"//",
"#",
";",
],
"line_comments" => [
"#rem",
"#",
],
"line_seps" => [
"\t",
],
"group_seps" => [
[
"[",
"]",
],
[
"\t",
"\t",
],
],
"instrtables" => [
"prefix",
"other",
],
"parms" => {
"L1" => {
"value" => 4294967295,
"type" => "label",
},
"PktCnt" => {
"value" => 0,
"type" => "int",
},
},
},
"helpers" => {
"sincos" => {
"action" => <<DATA
 bits < 32 > sincos ( bits < 32 > x ) {
    return x ;
}
DATA
,
},
},
},
},
"systems" => {
},
};

