$data = {
"cores" => {
"StarCore" => {
"bit_endianness" => "little",
"blocks" => [
"agu",
"dalu",
],
"parallel_execution" => 8,
"RaMask" => {
"initial" => "0xffffffff",
"constant" => 1,
},
"EaMask" => {
"initial" => "0xffffffff",
"constant" => 1,
},
"regs" => {
"BTR0" => {
"doc" => <<DATA
 Back Trace Register
 
DATA
,
"width" => 32,
"serial" => 1,
"shared" => 0,
},
"BTR1" => {
"doc" => <<DATA
 Back Trace Register
 
DATA
,
"width" => 32,
"serial" => 1,
"shared" => 0,
},
"DSPC" => {
"doc" => <<DATA
 Delay Slot Program Counter
 
DATA
,
"width" => 32,
"serial" => 1,
"shared" => 0,
"write" => {
<<DATA
{
    if ( x ( 0 ) == 1 ) {
        LegalPC = false ;
    } else {
        DSPC = x ;
    }
}
DATA
,
},
},
"EMR" => {
"doc" => <<DATA
 Exception and Mode Register
 
DATA
,
"width" => 32,
"serial" => 1,
"fields" => {
"BEM" => {
"bits" => [
[
16,
16,
],
],
"readonly" => 1,
},
"DOVF" => {
"bits" => [
[
2,
2,
],
],
},
"EAP" => {
"bits" => [
[
25,
25,
],
],
},
"GP0" => {
"bits" => [
[
17,
17,
],
],
"readonly" => 1,
},
"GP1" => {
"bits" => [
[
18,
18,
],
],
"readonly" => 1,
},
"GP2" => {
"bits" => [
[
19,
19,
],
],
"readonly" => 1,
},
"GP3" => {
"bits" => [
[
20,
20,
],
],
"readonly" => 1,
},
"GP4" => {
"bits" => [
[
21,
21,
],
],
"readonly" => 1,
},
"GP5" => {
"bits" => [
[
22,
22,
],
],
"readonly" => 1,
},
"GP6" => {
"bits" => [
[
23,
23,
],
],
"readonly" => 1,
},
"H" => {
"bits" => [
[
31,
16,
],
],
},
"ILIN" => {
"bits" => [
[
0,
0,
],
],
},
"ILPR" => {
"bits" => [
[
4,
4,
],
],
},
"ILST" => {
"bits" => [
[
1,
1,
],
],
},
"L" => {
"bits" => [
[
15,
0,
],
],
},
"MEM" => {
"bits" => [
[
26,
26,
],
],
"readonly" => 1,
},
"NMID" => {
"bits" => [
[
3,
3,
],
],
},
"SDD" => {
"bits" => [
[
24,
24,
],
],
},
},
"readonly_mask" => "0b00000100111111110000000000000000",
"reset" => "0x04ff0000",
"shared" => 0,
"read" => {
<<DATA
{
     bits < 32 > emr_exception_mask = 0x07ff00ff ;
     bits < 32 > emr_user_mask = 0x00000004 ;
    if ( IsUserMode (  ) ) {
        return ( EMR & emr_user_mask ) ;
    } else {
        return EMR & emr_exception_mask ;
    }
}
DATA
,
},
"write" => {
<<DATA
{
     bits < 32 > emr_bigendian_mask ;
    if ( EMR . BEM == 1 ) emr_bigendian_mask = 0x00010000 ; else emr_bigendian_mask = 0 ;
     bits < 32 > emr_exception_mask = 0x07ff00ff ;
     bits < 32 > emr_user_mask = 0x00000004 ;
    if ( exception_condition == true || EMR_LOW == true || exception_overflow == true ) {
        if ( IsUserMode (  ) ) {
            EMR = x & emr_user_mask | emr_bigendian_mask ;
        } else {
            EMR = x & emr_exception_mask | emr_bigendian_mask ;
        }
        exception_condition = false ;
        EMR_LOW = false ;
        exception_overflow = false ;
    } else {
        if ( IsUserMode (  ) ) {
            EMR = ( ( ~ ( x & ThisReg ) ) & ThisReg & emr_user_mask ) | emr_bigendian_mask ;
        } else {
            EMR . L = ( ( ~ ( x ( 15 , 0 ) & ThisReg ( 15 , 0 ) ) ) & ThisReg ( 15 , 0 ) ) & emr_user_mask ( 15 , 0 ) ;
            EMR . H = ( ( x ( 31 , 16 ) ) & ( emr_exception_mask ( 31 , 16 ) ) ) | 0x0001 ;
        }
    }
}
DATA
,
},
},
"ESP" => {
"doc" => <<DATA
 Exception-Mode Stack Pointer
 
DATA
,
"width" => 32,
"shared" => 0,
"read" => {
<<DATA
{
    return ( ESP & 0xFFFFFFF8 ) ;
}
DATA
,
},
"write" => {
<<DATA
{
    ESP = ( x & 0xFFFFFFF8 ) ;
}
DATA
,
},
},
"Fetch" => {
"width" => 2,
"serial" => 1,
"enumerated" => {
"0" => "Normal",
"1" => "DelaySlot",
"2" => "DoubleDelaySlot",
"3" => "Jump",
},
"reset" => "0",
"shared" => 0,
},
"GCR" => {
"doc" => <<DATA
 General Configuration Register
 
DATA
,
"width" => 32,
"fields" => {
"BAM" => {
"bits" => [
[
2,
0,
],
],
},
"BAM0" => {
"bits" => [
[
0,
0,
],
],
},
"BAM1" => {
"bits" => [
[
1,
1,
],
],
},
"BAM2" => {
"bits" => [
[
2,
2,
],
],
},
},
"reserved_mask" => "0b11111111111111111111111111111000",
"shared" => 0,
},
"IDCR" => {
"doc" => <<DATA
 Implementation Dependent Configuration Register
 
DATA
,
"width" => 32,
"fields" => {
"DBTB" => {
"bits" => [
[
1,
1,
],
],
},
"DHLB" => {
"bits" => [
[
2,
2,
],
],
},
"DSO" => {
"bits" => [
[
0,
0,
],
],
},
},
"reserved_mask" => "0b11111111111111111111111111111000",
"reset" => "0",
"shared" => 0,
"read" => {
<<DATA
{
    if ( IsUserMode (  ) ) {
        EMR_LOW = true ;
        EMR . ILPR = 1 ;
        raiseException ( Privelege ) ;
        return ThisReg ;
    } else return ( IDCR & 0x00000007 ) ;
}
DATA
,
},
"write" => {
<<DATA
{
    if ( IsUserMode (  ) ) {
        EMR_LOW = true ;
        EMR . ILPR = 1 ;
        raiseException ( Privelege ) ;
    } else IDCR = ( x & 0x00000007 ) ;
}
DATA
,
},
},
"MCTL" => {
"width" => 32,
"fields" => {
"AM_R" => {
"indexed" => {
"width" => 4,
"count" => 8,
},
},
},
"reset" => "0",
"shared" => 0,
},
"NPC" => {
"doc" => <<DATA
 Next Program Counter
 
DATA
,
"width" => 32,
"serial" => 1,
"attributes" => {
"nia" => "",
},
"shared" => 0,
"write" => {
<<DATA
{
    if ( x ( 0 ) == 1 ) {
        LegalPC = false ;
    } else {
        NPC = x ;
    }
}
DATA
,
},
},
"NSP" => {
"doc" => <<DATA
 Normal-Mode Stack Pointer
 
DATA
,
"width" => 32,
"shared" => 0,
"read" => {
<<DATA
{
    return ( NSP & 0xFFFFFFF8 ) ;
}
DATA
,
},
"write" => {
<<DATA
{
    NSP = ( x & 0xFFFFFFF8 ) ;
}
DATA
,
},
},
"NVPC" => {
"doc" => <<DATA
 Next Program Counter
 
DATA
,
"width" => 32,
"serial" => 1,
"shared" => 0,
},
"PC" => {
"doc" => <<DATA
 Program Counter
 
DATA
,
"width" => 32,
"serial" => 1,
"attributes" => {
"cia" => "",
},
"shared" => 0,
"write" => {
<<DATA
{
    if ( x ( 0 ) == 1 ) {
        LegalPC = false ;
    } else {
        PC = x ;
    }
}
DATA
,
},
},
"PC_ST_VLES" => {
"doc" => <<DATA
 Program Counter Start VLES
 
DATA
,
"width" => 32,
"shared" => 0,
"write" => {
<<DATA
{
    if ( x ( 0 ) == 1 ) {
        LegalPC = false ;
    } else {
        PC_ST_VLES = x ;
    }
}
DATA
,
},
},
"SP" => {
"doc" => <<DATA
 Stack Pointer
 
DATA
,
"width" => 32,
"shared" => 0,
"read" => {
<<DATA
{
    if ( SR . EXP == 1 ) return ESP ; else return NSP ;
}
DATA
,
},
"write" => {
<<DATA
{
    if ( SR . EXP == 1 ) ESP = x ; else NSP = x ;
}
DATA
,
},
},
"SR" => {
"doc" => <<DATA
 Status Register
 
DATA
,
"width" => 32,
"fields" => {
"C" => {
"bits" => [
[
0,
0,
],
],
},
"DI" => {
"bits" => [
[
19,
19,
],
],
},
"EXP" => {
"bits" => [
[
18,
18,
],
],
},
"H" => {
"bits" => [
[
31,
16,
],
],
},
"I" => {
"bits" => [
[
25,
21,
],
],
},
"L" => {
"bits" => [
[
15,
0,
],
],
},
"LF" => {
"bits" => [
[
30,
27,
],
],
},
"LF0" => {
"bits" => [
[
27,
27,
],
],
},
"LF1" => {
"bits" => [
[
28,
28,
],
],
},
"LF2" => {
"bits" => [
[
29,
29,
],
],
},
"LF3" => {
"bits" => [
[
30,
30,
],
],
},
"OVE" => {
"bits" => [
[
20,
20,
],
],
},
"PE" => {
"bits" => [
[
17,
17,
],
],
},
"RM" => {
"bits" => [
[
3,
3,
],
],
},
"S" => {
"bits" => [
[
6,
6,
],
],
},
"SCM" => {
"bits" => [
[
5,
4,
],
],
},
"SLF" => {
"bits" => [
[
31,
31,
],
],
},
"SM" => {
"bits" => [
[
2,
2,
],
],
},
"SM2" => {
"bits" => [
[
7,
7,
],
],
},
"T" => {
"bits" => [
[
1,
1,
],
],
},
"VF0" => {
"bits" => [
[
8,
8,
],
],
},
"VF1" => {
"bits" => [
[
9,
9,
],
],
},
"VF2" => {
"bits" => [
[
10,
10,
],
],
},
"VF3" => {
"bits" => [
[
11,
11,
],
],
},
},
"reset" => "0x03E40000",
"shared" => 0,
"read" => {
<<DATA
{
     var user_mask = ( ( ThisReg & 0xFC03FFFF ) | 0x00020000 ) ;
    if ( ThisReg ( 17 ) == 1 ) {
        return user_mask ;
    } else {
        return ThisReg ;
    }
}
DATA
,
},
"write" => {
<<DATA
{
    if ( SR . PE == 1 ) {
        if ( clear_SR_PE == true ) {
            SR = ( x & 0xFC03FFFF ) ;
            clear_SR_PE = false ;
        } else {
            SR = ( x & 0xFC03FFFF ) | 0x00020000 ;
        }
    } else {
        if ( set_SR_PE == enable ) {
            SR = x ;
            set_SR_PE = disable ;
        } else {
            SR = x & 0xFFFDFFFF ;
        }
    }
}
DATA
,
},
},
"VBA" => {
"doc" => <<DATA
 Vector Base Address Register
 
DATA
,
"width" => 32,
"fields" => {
"high" => {
"bits" => [
[
31,
12,
],
],
},
"low" => {
"bits" => [
[
11,
0,
],
],
"readonly" => 1,
},
},
"readonly_mask" => "0b00000000000000000000111111111111",
"shared" => 0,
"read" => {
<<DATA
{
    if ( IsUserMode (  ) ) {
        EMR_LOW = true ;
        EMR . ILPR = 1 ;
        raiseException ( Privelege ) ;
        return ThisReg ;
    } else return ( ThisReg & 0xFFFFF000 ) ;
}
DATA
,
},
"write" => {
<<DATA
{
    if ( IsUserMode (  ) ) {
        EMR_LOW = true ;
        EMR . ILPR = 1 ;
        raiseException ( Privelege ) ;
    } else ThisReg = ( x & 0xFFFFF000 ) ;
}
DATA
,
},
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
"reg" => "R",
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
"reg" => "R",
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
"C1Table" => {
"width" => 32,
"size" => 8,
"shared" => 0,
"entries" => {
"0" => {
"syntax" => "emr",
"reg" => "EMR",
},
"1" => {
"syntax" => "vba",
"reg" => "VBA",
},
"2" => {
"syntax" => "idcr",
"reg" => "IDCR",
},
"3" => {
"syntax" => "gcr",
"reg" => "GCR",
},
"5" => {
"syntax" => "sr",
"reg" => "SR",
},
"7" => {
"syntax" => "mctl",
"reg" => "MCTL",
},
},
},
"C2Table" => {
"width" => 32,
"size" => 16,
"shared" => 0,
"entries" => {
"0" => {
"syntax" => "emr",
"reg" => "EMR",
},
"1" => {
"syntax" => "vba",
"reg" => "VBA",
},
"2" => {
"syntax" => "idcr",
"reg" => "IDCR",
},
"3" => {
"syntax" => "gcr",
"reg" => "GCR",
},
"5" => {
"syntax" => "sr",
"reg" => "SR",
},
"7" => {
"syntax" => "mctl",
"reg" => "MCTL",
},
"8" => {
"syntax" => "SA0",
"reg" => "SA(0)",
},
"9" => {
"syntax" => "LC0",
"reg" => "LC(0)",
},
"10" => {
"syntax" => "SA1",
"reg" => "SA(1)",
},
"11" => {
"syntax" => "LC1",
"reg" => "LC(1)",
},
"12" => {
"syntax" => "SA2",
"reg" => "SA(2)",
},
"13" => {
"syntax" => "LC2",
"reg" => "LC(2)",
},
"14" => {
"syntax" => "SA3",
"reg" => "SA(3)",
},
"15" => {
"syntax" => "LC3",
"reg" => "LC(3)",
},
},
},
"C3Table" => {
"width" => 32,
"size" => 16,
"shared" => 0,
"entries" => {
"0" => {
"syntax" => "b0",
"reg" => "B(0)",
},
"1" => {
"syntax" => "b1",
"reg" => "B(1)",
},
"2" => {
"syntax" => "b2",
"reg" => "B(2)",
},
"3" => {
"syntax" => "b3",
"reg" => "B(3)",
},
"4" => {
"syntax" => "b4",
"reg" => "B(4)",
},
"5" => {
"syntax" => "b5",
"reg" => "B(5)",
},
"6" => {
"syntax" => "b6",
"reg" => "B(6)",
},
"7" => {
"syntax" => "b7",
"reg" => "B(7)",
},
"8" => {
"syntax" => "n0",
"reg" => "N(0)",
},
"9" => {
"syntax" => "n1",
"reg" => "N(1)",
},
"10" => {
"syntax" => "n2",
"reg" => "N(2)",
},
"11" => {
"syntax" => "n3",
"reg" => "N(3)",
},
"12" => {
"syntax" => "m0",
"reg" => "M(0)",
},
"13" => {
"syntax" => "m1",
"reg" => "M(1)",
},
"14" => {
"syntax" => "m2",
"reg" => "M(2)",
},
"15" => {
"syntax" => "m3",
"reg" => "M(3)",
},
},
},
"C4Table" => {
"width" => 40,
"size" => 64,
"shared" => 0,
"entries" => {
"0" => {
"syntax" => "d0",
"reg" => "D(0)",
},
"1" => {
"syntax" => "b0",
"reg" => "B(0)",
},
"2" => {
"syntax" => "d1",
"reg" => "D(1)",
},
"3" => {
"syntax" => "b1",
"reg" => "B(1)",
},
"4" => {
"syntax" => "d2",
"reg" => "D(2)",
},
"5" => {
"syntax" => "b2",
"reg" => "B(2)",
},
"6" => {
"syntax" => "d3",
"reg" => "D(3)",
},
"7" => {
"syntax" => "b3",
"reg" => "B(3)",
},
"8" => {
"syntax" => "d4",
"reg" => "D(4)",
},
"9" => {
"syntax" => "b4",
"reg" => "B(4)",
},
"10" => {
"syntax" => "d5",
"reg" => "D(5)",
},
"11" => {
"syntax" => "b5",
"reg" => "B(5)",
},
"12" => {
"syntax" => "d6",
"reg" => "D(6)",
},
"13" => {
"syntax" => "b6",
"reg" => "B(6)",
},
"14" => {
"syntax" => "d7",
"reg" => "D(7)",
},
"15" => {
"syntax" => "b7",
"reg" => "B(7)",
},
"16" => {
"syntax" => "r0",
"reg" => "R(0)",
},
"17" => {
"syntax" => "n0",
"reg" => "N(0)",
},
"18" => {
"syntax" => "r1",
"reg" => "R(1)",
},
"19" => {
"syntax" => "n1",
"reg" => "N(1)",
},
"20" => {
"syntax" => "r2",
"reg" => "R(2)",
},
"21" => {
"syntax" => "n2",
"reg" => "N(2)",
},
"22" => {
"syntax" => "r3",
"reg" => "R(3)",
},
"23" => {
"syntax" => "n3",
"reg" => "N(3)",
},
"24" => {
"syntax" => "r4",
"reg" => "R(4)",
},
"25" => {
"syntax" => "m0",
"reg" => "M(0)",
},
"26" => {
"syntax" => "r5",
"reg" => "R(5)",
},
"27" => {
"syntax" => "m1",
"reg" => "M(1)",
},
"28" => {
"syntax" => "r6",
"reg" => "R(6)",
},
"29" => {
"syntax" => "m2",
"reg" => "M(2)",
},
"30" => {
"syntax" => "r7",
"reg" => "R(7)",
},
"31" => {
"syntax" => "m3",
"reg" => "M(3)",
},
"32" => {
"syntax" => "d8",
"reg" => "D(8)",
},
"34" => {
"syntax" => "d9",
"reg" => "D(9)",
},
"36" => {
"syntax" => "d10",
"reg" => "D(10)",
},
"38" => {
"syntax" => "d11",
"reg" => "D(11)",
},
"40" => {
"syntax" => "d12",
"reg" => "D(12)",
},
"42" => {
"syntax" => "d13",
"reg" => "D(13)",
},
"44" => {
"syntax" => "d14",
"reg" => "D(14)",
},
"46" => {
"syntax" => "d15",
"reg" => "D(15)",
},
"48" => {
"syntax" => "r8",
"reg" => "R(8)",
},
"50" => {
"syntax" => "r9",
"reg" => "R(9)",
},
"52" => {
"syntax" => "r10",
"reg" => "R(10)",
},
"54" => {
"syntax" => "r11",
"reg" => "R(11)",
},
"56" => {
"syntax" => "r12",
"reg" => "R(12)",
},
"58" => {
"syntax" => "r13",
"reg" => "R(13)",
},
"60" => {
"syntax" => "r14",
"reg" => "R(14)",
},
"62" => {
"syntax" => "r15",
"reg" => "R(15)",
},
},
},
"D" => {
"width" => 40,
"fields" => {
"E" => {
"bits" => [
[
39,
32,
],
],
},
"H" => {
"bits" => [
[
31,
16,
],
],
},
"HH" => {
"bits" => [
[
31,
24,
],
],
},
"HL" => {
"bits" => [
[
23,
16,
],
],
},
"L" => {
"bits" => [
[
15,
0,
],
],
},
"LH" => {
"bits" => [
[
15,
8,
],
],
},
"LL" => {
"bits" => [
[
7,
0,
],
],
},
"WH" => {
"bits" => [
[
39,
36,
],
[
31,
16,
],
],
},
"WL" => {
"bits" => [
[
35,
32,
],
[
15,
0,
],
],
},
},
"size" => 16,
"prefix" => "d",
"shared" => 0,
},
"DRTable" => {
"width" => 40,
"size" => 32,
"shared" => 0,
"entries" => {
"0" => {
"syntax" => "d0",
"reg" => "D(0)",
},
"1" => {
"syntax" => "d1",
"reg" => "D(1)",
},
"2" => {
"syntax" => "d2",
"reg" => "D(2)",
},
"3" => {
"syntax" => "d3",
"reg" => "D(3)",
},
"4" => {
"syntax" => "d4",
"reg" => "D(4)",
},
"5" => {
"syntax" => "d5",
"reg" => "D(5)",
},
"6" => {
"syntax" => "d6",
"reg" => "D(6)",
},
"7" => {
"syntax" => "d7",
"reg" => "D(7)",
},
"8" => {
"syntax" => "r0",
"reg" => "R(0)",
},
"9" => {
"syntax" => "r1",
"reg" => "R(1)",
},
"10" => {
"syntax" => "r2",
"reg" => "R(2)",
},
"11" => {
"syntax" => "r3",
"reg" => "R(3)",
},
"12" => {
"syntax" => "r4",
"reg" => "R(4)",
},
"13" => {
"syntax" => "r5",
"reg" => "R(5)",
},
"14" => {
"syntax" => "r6",
"reg" => "R(6)",
},
"15" => {
"syntax" => "r7",
"reg" => "R(7)",
},
"16" => {
"syntax" => "d8",
"reg" => "D(8)",
},
"17" => {
"syntax" => "d9",
"reg" => "D(9)",
},
"18" => {
"syntax" => "d10",
"reg" => "D(10)",
},
"19" => {
"syntax" => "d11",
"reg" => "D(11)",
},
"20" => {
"syntax" => "d12",
"reg" => "D(12)",
},
"21" => {
"syntax" => "d13",
"reg" => "D(13)",
},
"22" => {
"syntax" => "d14",
"reg" => "D(14)",
},
"23" => {
"syntax" => "d15",
"reg" => "D(15)",
},
"24" => {
"syntax" => "r8",
"reg" => "R(8)",
},
"25" => {
"syntax" => "r9",
"reg" => "R(9)",
},
"26" => {
"syntax" => "r10",
"reg" => "R(10)",
},
"27" => {
"syntax" => "r11",
"reg" => "R(11)",
},
"28" => {
"syntax" => "r12",
"reg" => "R(12)",
},
"29" => {
"syntax" => "r13",
"reg" => "R(13)",
},
"30" => {
"syntax" => "r14",
"reg" => "R(14)",
},
"31" => {
"syntax" => "r15",
"reg" => "R(15)",
},
},
},
"DaXDaYTable" => {
"width" => 16,
"size" => 16,
"shared" => 0,
"entries" => {
"0" => {
"syntax" => "d4.l",
"read" => <<DATA
{
    return ( D ( 4 ) . L ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 4 ) . L = x ;
}
DATA
,
},
"1" => {
"syntax" => "d5.l",
"read" => <<DATA
{
    return ( D ( 5 ) . L ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 5 ) . L = x ;
}
DATA
,
},
"2" => {
"syntax" => "d6.l",
"read" => <<DATA
{
    return ( D ( 6 ) . L ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 6 ) . L = x ;
}
DATA
,
},
"3" => {
"syntax" => "d7.l",
"read" => <<DATA
{
    return ( D ( 7 ) . L ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 7 ) . L = x ;
}
DATA
,
},
"4" => {
"syntax" => "d4.h",
"read" => <<DATA
{
    return ( D ( 4 ) . H ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 4 ) . H = x ;
}
DATA
,
},
"5" => {
"syntax" => "d5.h",
"read" => <<DATA
{
    return ( D ( 5 ) . H ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 5 ) . H = x ;
}
DATA
,
},
"6" => {
"syntax" => "d6.h",
"read" => <<DATA
{
    return ( D ( 6 ) . H ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 6 ) . H = x ;
}
DATA
,
},
"7" => {
"syntax" => "d7.h",
"read" => <<DATA
{
    return ( D ( 7 ) . H ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 7 ) . H = x ;
}
DATA
,
},
"8" => {
"syntax" => "d12.l",
"read" => <<DATA
{
    return ( D ( 12 ) . L ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 12 ) . L = x ;
}
DATA
,
},
"9" => {
"syntax" => "d13.l",
"read" => <<DATA
{
    return ( D ( 13 ) . L ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 13 ) . L = x ;
}
DATA
,
},
"10" => {
"syntax" => "d14.l",
"read" => <<DATA
{
    return ( D ( 14 ) . L ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 14 ) . L = x ;
}
DATA
,
},
"11" => {
"syntax" => "d15.l",
"read" => <<DATA
{
    return ( D ( 15 ) . L ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 15 ) . L = x ;
}
DATA
,
},
"12" => {
"syntax" => "d12.h",
"read" => <<DATA
{
    return ( D ( 12 ) . H ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 12 ) . H = x ;
}
DATA
,
},
"13" => {
"syntax" => "d13.h",
"read" => <<DATA
{
    return ( D ( 13 ) . H ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 13 ) . H = x ;
}
DATA
,
},
"14" => {
"syntax" => "d14.h",
"read" => <<DATA
{
    return ( D ( 14 ) . H ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 14 ) . H = x ;
}
DATA
,
},
"15" => {
"syntax" => "d15.h",
"read" => <<DATA
{
    return ( D ( 15 ) . H ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 15 ) . H = x ;
}
DATA
,
},
},
},
"DeDoTable" => {
"width" => 64,
"size" => 64,
"shared" => 0,
"entries" => {
"0" => {
"syntax" => "d0:d1",
"read" => <<DATA
{
    return concat ( D ( 0 ) ( 31 , 0 ) , D ( 1 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 1 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 1 ) = 0 ;
    D ( 0 ) = signExtend ( x ( 63 , 32 ) , 40 ) ;
    L ( 0 ) = 0 ;
}
DATA
,
},
"1" => {
"syntax" => "b0:b1",
"read" => <<DATA
{
    return concat ( B ( 0 ) , B ( 1 ) ) ;
}
DATA
,
"write" => <<DATA
{
    B ( 1 ) = x ( 31 , 0 ) ;
    B ( 0 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"2" => {
"syntax" => "idcr:vba",
"read" => <<DATA
{
    return concat ( IDCR , VBA ) ;
}
DATA
,
"write" => <<DATA
{
    VBA = x ( 31 , 0 ) ;
    IDCR = x ( 63 , 32 ) ;
}
DATA
,
},
"4" => {
"syntax" => "d2:d3",
"read" => <<DATA
{
    return concat ( D ( 2 ) ( 31 , 0 ) , D ( 3 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 3 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 3 ) = 0 ;
    D ( 2 ) = signExtend ( x ( 63 , 32 ) , 40 ) ;
    L ( 2 ) = 0 ;
}
DATA
,
},
"5" => {
"syntax" => "b2:b3",
"read" => <<DATA
{
    return concat ( B ( 2 ) , B ( 3 ) ) ;
}
DATA
,
"write" => <<DATA
{
    B ( 3 ) = x ( 31 , 0 ) ;
    B ( 2 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"8" => {
"syntax" => "d4:d5",
"read" => <<DATA
{
    return concat ( D ( 4 ) ( 31 , 0 ) , D ( 5 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 5 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 5 ) = 0 ;
    D ( 4 ) = signExtend ( x ( 63 , 32 ) , 40 ) ;
    L ( 4 ) = 0 ;
}
DATA
,
},
"9" => {
"syntax" => "b4:b5",
"read" => <<DATA
{
    return concat ( B ( 4 ) , B ( 5 ) ) ;
}
DATA
,
"write" => <<DATA
{
    B ( 5 ) = x ( 31 , 0 ) ;
    B ( 4 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"12" => {
"syntax" => "d6:d7",
"read" => <<DATA
{
    return concat ( D ( 6 ) ( 31 , 0 ) , D ( 7 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 7 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 7 ) = 0 ;
    D ( 6 ) = signExtend ( x ( 63 , 32 ) , 40 ) ;
    L ( 6 ) = 0 ;
}
DATA
,
},
"13" => {
"syntax" => "b6:b7",
"read" => <<DATA
{
    return concat ( B ( 6 ) , B ( 7 ) ) ;
}
DATA
,
"write" => <<DATA
{
    B ( 7 ) = x ( 31 , 0 ) ;
    B ( 6 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"14" => {
"syntax" => "gcr:mctl",
"read" => <<DATA
{
    return concat ( GCR , MCTL ) ;
}
DATA
,
"write" => <<DATA
{
    MCTL = x ( 31 , 0 ) ;
    GCR = x ( 63 , 32 ) ;
}
DATA
,
},
"16" => {
"syntax" => "r0:r1",
"read" => <<DATA
{
    return concat ( R ( 0 ) , R ( 1 ) ) ;
}
DATA
,
"write" => <<DATA
{
    R ( 1 ) = x ( 31 , 0 ) ;
    R ( 0 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"17" => {
"syntax" => "n0:n1",
"read" => <<DATA
{
    return concat ( N ( 0 ) , N ( 1 ) ) ;
}
DATA
,
"write" => <<DATA
{
    N ( 1 ) = x ( 31 , 0 ) ;
    N ( 0 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"18" => {
"syntax" => "sa0:lc0",
"read" => <<DATA
{
    return concat ( SA ( 0 ) , LC ( 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    LC ( 0 ) = x ( 31 , 0 ) ;
    SA ( 0 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"19" => {
"syntax" => "d0.e:d1.e:d2.e:d3.e",
"read" => <<DATA
{
     bits < 32 > tmp = concat ( zero ( 7 ) , L ( 2 ) , D ( 2 ) . E , zero ( 7 ) , L ( 3 ) , D ( 3 ) . E ) ;
     bits < 16 > tmp2 = concat ( zero ( 7 ) , L ( 1 ) , D ( 1 ) . E ) ;
    return concat ( zero ( 7 ) , L ( 0 ) , D ( 0 ) . E , tmp2 , tmp ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 3 ) = x ( 56 ) ;
    D ( 3 ) . E = x ( 55 , 48 ) ;
    L ( 2 ) = x ( 40 ) ;
    D ( 2 ) . E = x ( 39 , 32 ) ;
    L ( 1 ) = x ( 24 ) ;
    D ( 1 ) . E = x ( 23 , 16 ) ;
    L ( 0 ) = x ( 8 ) ;
    D ( 0 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"20" => {
"syntax" => "r2:r3",
"read" => <<DATA
{
    return concat ( R ( 2 ) , R ( 3 ) ) ;
}
DATA
,
"write" => <<DATA
{
    R ( 3 ) = x ( 31 , 0 ) ;
    R ( 2 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"21" => {
"syntax" => "n2:n3",
"read" => <<DATA
{
    return concat ( N ( 2 ) , N ( 3 ) ) ;
}
DATA
,
"write" => <<DATA
{
    N ( 3 ) = x ( 31 , 0 ) ;
    N ( 2 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"22" => {
"syntax" => "sa1:lc1",
"read" => <<DATA
{
    return concat ( SA ( 1 ) , LC ( 1 ) ) ;
}
DATA
,
"write" => <<DATA
{
    LC ( 1 ) = x ( 31 , 0 ) ;
    SA ( 1 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"23" => {
"syntax" => "btr0:btr1",
"read" => <<DATA
{
    return concat ( BTR0 , BTR1 ) ;
}
DATA
,
"write" => <<DATA
{
    BTR1 = x ( 31 , 0 ) ;
    BTR0 = x ( 63 , 32 ) ;
}
DATA
,
},
"24" => {
"syntax" => "r4:r5",
"read" => <<DATA
{
    return concat ( R ( 4 ) , R ( 5 ) ) ;
}
DATA
,
"write" => <<DATA
{
    R ( 5 ) = x ( 31 , 0 ) ;
    R ( 4 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"25" => {
"syntax" => "m0:m1",
"read" => <<DATA
{
    return concat ( M ( 0 ) , M ( 1 ) ) ;
}
DATA
,
"write" => <<DATA
{
    M ( 1 ) = x ( 31 , 0 ) ;
    M ( 0 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"26" => {
"syntax" => "sa2:lc2",
"read" => <<DATA
{
    return concat ( SA ( 2 ) , LC ( 2 ) ) ;
}
DATA
,
"write" => <<DATA
{
    LC ( 2 ) = x ( 31 , 0 ) ;
    SA ( 2 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"27" => {
"syntax" => "d4.e:d5.e:d6.e:d7.e",
"read" => <<DATA
{
     bits < 32 > tmp = concat ( zero ( 7 ) , L ( 6 ) , D ( 6 ) . E , zero ( 7 ) , L ( 7 ) , D ( 7 ) . E ) ;
     bits < 16 > tmp2 = concat ( zero ( 7 ) , L ( 5 ) , D ( 5 ) . E ) ;
    return concat ( zero ( 7 ) , L ( 4 ) , D ( 4 ) . E , tmp2 , tmp ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 7 ) = x ( 56 ) ;
    D ( 7 ) . E = x ( 55 , 48 ) ;
    L ( 6 ) = x ( 40 ) ;
    D ( 6 ) . E = x ( 39 , 32 ) ;
    L ( 5 ) = x ( 24 ) ;
    D ( 5 ) . E = x ( 23 , 16 ) ;
    L ( 4 ) = x ( 8 ) ;
    D ( 4 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"28" => {
"syntax" => "r6:r7",
"read" => <<DATA
{
    return concat ( R ( 6 ) , R ( 7 ) ) ;
}
DATA
,
"write" => <<DATA
{
    R ( 7 ) = x ( 31 , 0 ) ;
    R ( 6 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"29" => {
"syntax" => "m2:m3",
"read" => <<DATA
{
    return concat ( M ( 2 ) , M ( 3 ) ) ;
}
DATA
,
"write" => <<DATA
{
    M ( 3 ) = x ( 31 , 0 ) ;
    M ( 2 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"30" => {
"syntax" => "sa3:lc3",
"read" => <<DATA
{
    return concat ( SA ( 3 ) , LC ( 3 ) ) ;
}
DATA
,
"write" => <<DATA
{
    LC ( 3 ) = x ( 31 , 0 ) ;
    SA ( 3 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"32" => {
"syntax" => "d8:d9",
"read" => <<DATA
{
    return concat ( D ( 8 ) ( 31 , 0 ) , D ( 9 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 9 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 9 ) = 0 ;
    D ( 8 ) = signExtend ( x ( 63 , 32 ) , 40 ) ;
    L ( 8 ) = 0 ;
}
DATA
,
},
"36" => {
"syntax" => "d10:d11",
"read" => <<DATA
{
    return concat ( D ( 10 ) ( 31 , 0 ) , D ( 11 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 11 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 11 ) = 0 ;
    D ( 10 ) = signExtend ( x ( 63 , 32 ) , 40 ) ;
    L ( 10 ) = 0 ;
}
DATA
,
},
"40" => {
"syntax" => "d12:d13",
"read" => <<DATA
{
    return concat ( D ( 12 ) ( 31 , 0 ) , D ( 13 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 13 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 13 ) = 0 ;
    D ( 12 ) = signExtend ( x ( 63 , 32 ) , 40 ) ;
    L ( 12 ) = 0 ;
}
DATA
,
},
"44" => {
"syntax" => "d14:d15",
"read" => <<DATA
{
    return concat ( D ( 14 ) ( 31 , 0 ) , D ( 15 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 15 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 15 ) = 0 ;
    D ( 14 ) = signExtend ( x ( 63 , 32 ) , 40 ) ;
    L ( 14 ) = 0 ;
}
DATA
,
},
"48" => {
"syntax" => "r8:r9",
"read" => <<DATA
{
    return concat ( R ( 8 ) , R ( 9 ) ) ;
}
DATA
,
"write" => <<DATA
{
    R ( 9 ) = x ( 31 , 0 ) ;
    R ( 8 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"51" => {
"syntax" => "d8.e:d9.e:d10.e:d11.e",
"read" => <<DATA
{
     bits < 32 > tmp = concat ( zero ( 7 ) , L ( 10 ) , D ( 10 ) . E , zero ( 7 ) , L ( 11 ) , D ( 11 ) . E ) ;
     bits < 16 > tmp2 = concat ( zero ( 7 ) , L ( 9 ) , D ( 9 ) . E ) ;
    return concat ( zero ( 7 ) , L ( 8 ) , D ( 8 ) . E , tmp2 , tmp ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 11 ) = x ( 56 ) ;
    D ( 11 ) . E = x ( 55 , 48 ) ;
    L ( 10 ) = x ( 40 ) ;
    D ( 10 ) . E = x ( 39 , 32 ) ;
    L ( 9 ) = x ( 24 ) ;
    D ( 9 ) . E = x ( 23 , 16 ) ;
    L ( 8 ) = x ( 8 ) ;
    D ( 8 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"52" => {
"syntax" => "r10:r11",
"read" => <<DATA
{
    return concat ( R ( 10 ) , R ( 11 ) ) ;
}
DATA
,
"write" => <<DATA
{
    R ( 11 ) = x ( 31 , 0 ) ;
    R ( 10 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"56" => {
"syntax" => "r12:r13",
"read" => <<DATA
{
    return concat ( R ( 12 ) , R ( 13 ) ) ;
}
DATA
,
"write" => <<DATA
{
    R ( 13 ) = x ( 31 , 0 ) ;
    R ( 12 ) = x ( 63 , 32 ) ;
}
DATA
,
},
"59" => {
"syntax" => "d12.e:d13.e:d14.e:d15.e",
"read" => <<DATA
{
     bits < 32 > tmp = concat ( zero ( 7 ) , L ( 14 ) , D ( 14 ) . E , zero ( 7 ) , L ( 15 ) , D ( 15 ) . E ) ;
     bits < 16 > tmp2 = concat ( zero ( 7 ) , L ( 13 ) , D ( 13 ) . E ) ;
    return concat ( zero ( 7 ) , L ( 12 ) , D ( 12 ) . E , tmp2 , tmp ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 15 ) = x ( 56 ) ;
    D ( 15 ) . E = x ( 55 , 48 ) ;
    L ( 14 ) = x ( 40 ) ;
    D ( 14 ) . E = x ( 39 , 32 ) ;
    L ( 13 ) = x ( 24 ) ;
    D ( 13 ) . E = x ( 23 , 16 ) ;
    L ( 12 ) = x ( 8 ) ;
    D ( 12 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"60" => {
"syntax" => "r14:r15",
"read" => <<DATA
{
    return concat ( R ( 14 ) , R ( 15 ) ) ;
}
DATA
,
"write" => <<DATA
{
    R ( 15 ) = x ( 31 , 0 ) ;
    R ( 14 ) = x ( 63 , 32 ) ;
}
DATA
,
},
},
},
"DeTable" => {
"width" => 32,
"size" => 64,
"shared" => 0,
"entries" => {
"0" => {
"syntax" => "d0",
"read" => <<DATA
{
    return ( D ( 0 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 0 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 0 ) = 0 ;
}
DATA
,
},
"1" => {
"syntax" => "b0",
"reg" => "B(0)",
},
"2" => {
"syntax" => "IDCR",
"reg" => "IDCR",
},
"3" => {
"syntax" => "d0.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 1 ) , D ( 1 ) . E , zero ( 7 ) , L ( 0 ) , D ( 0 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 0 ) = x ( 8 ) ;
    D ( 0 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"4" => {
"syntax" => "d2",
"read" => <<DATA
{
    return ( D ( 2 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 2 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 2 ) = 0 ;
}
DATA
,
},
"5" => {
"syntax" => "b2",
"reg" => "B(2)",
},
"7" => {
"syntax" => "d2.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 3 ) , D ( 3 ) . E , zero ( 7 ) , L ( 2 ) , D ( 2 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 2 ) = x ( 8 ) ;
    D ( 2 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"8" => {
"syntax" => "d4",
"read" => <<DATA
{
    return ( D ( 4 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 4 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 4 ) = 0 ;
}
DATA
,
},
"9" => {
"syntax" => "b4",
"reg" => "B(4)",
},
"11" => {
"syntax" => "d4.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 5 ) , D ( 5 ) . E , zero ( 7 ) , L ( 4 ) , D ( 4 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 4 ) = x ( 8 ) ;
    D ( 4 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"12" => {
"syntax" => "d6",
"read" => <<DATA
{
    return ( D ( 6 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 6 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 6 ) = 0 ;
}
DATA
,
},
"13" => {
"syntax" => "b6",
"reg" => "B(6)",
},
"14" => {
"syntax" => "GCR",
"reg" => "GCR",
},
"15" => {
"syntax" => "d6.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 7 ) , D ( 7 ) . E , zero ( 7 ) , L ( 6 ) , D ( 6 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 6 ) = x ( 8 ) ;
    D ( 6 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"16" => {
"syntax" => "r0",
"reg" => "R(0)",
},
"17" => {
"syntax" => "n0",
"reg" => "N(0)",
},
"18" => {
"syntax" => "SA0",
"reg" => "SA(0)",
},
"19" => {
"syntax" => "d0.e:d1.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 1 ) , D ( 1 ) . E , zero ( 7 ) , L ( 0 ) , D ( 0 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 1 ) = x ( 24 ) ;
    D ( 1 ) . E = x ( 23 , 16 ) ;
    L ( 0 ) = x ( 8 ) ;
    D ( 0 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"20" => {
"syntax" => "r2",
"reg" => "R(2)",
},
"21" => {
"syntax" => "n2",
"reg" => "N(2)",
},
"22" => {
"syntax" => "SA1",
"reg" => "SA(1)",
},
"23" => {
"syntax" => "d2.e:d3.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 3 ) , D ( 3 ) . E , zero ( 7 ) , L ( 2 ) , D ( 2 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 3 ) = x ( 24 ) ;
    D ( 3 ) . E = x ( 23 , 16 ) ;
    L ( 2 ) = x ( 8 ) ;
    D ( 2 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"24" => {
"syntax" => "r4",
"reg" => "R(4)",
},
"25" => {
"syntax" => "m0",
"reg" => "M(0)",
},
"26" => {
"syntax" => "SA2",
"reg" => "SA(2)",
},
"27" => {
"syntax" => "d4.e:d5.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 5 ) , D ( 5 ) . E , zero ( 7 ) , L ( 4 ) , D ( 4 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 5 ) = x ( 24 ) ;
    D ( 5 ) . E = x ( 23 , 16 ) ;
    L ( 4 ) = x ( 8 ) ;
    D ( 4 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"28" => {
"syntax" => "r6",
"reg" => "R(6)",
},
"29" => {
"syntax" => "m2",
"reg" => "M(2)",
},
"30" => {
"syntax" => "SA3",
"reg" => "SA(3)",
},
"31" => {
"syntax" => "d6.e:d7.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 7 ) , D ( 7 ) . E , zero ( 7 ) , L ( 6 ) , D ( 6 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 7 ) = x ( 24 ) ;
    D ( 7 ) . E = x ( 23 , 16 ) ;
    L ( 6 ) = x ( 8 ) ;
    D ( 6 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"32" => {
"syntax" => "d8",
"read" => <<DATA
{
    return ( D ( 8 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 8 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 8 ) = 0 ;
}
DATA
,
},
"35" => {
"syntax" => "d8.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 9 ) , D ( 9 ) . E , zero ( 7 ) , L ( 8 ) , D ( 8 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 8 ) = x ( 8 ) ;
    D ( 8 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"36" => {
"syntax" => "d10",
"read" => <<DATA
{
    return ( D ( 10 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 10 ) = signExtend ( x , 40 ) ;
    L ( 10 ) = 0 ;
}
DATA
,
},
"39" => {
"syntax" => "d10.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 11 ) , D ( 11 ) . E , zero ( 7 ) , L ( 10 ) , D ( 10 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 10 ) = x ( 8 ) ;
    D ( 10 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"40" => {
"syntax" => "d12",
"read" => <<DATA
{
    return ( D ( 12 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 12 ) = signExtend ( x , 40 ) ;
    L ( 12 ) = 0 ;
}
DATA
,
},
"43" => {
"syntax" => "d12.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 13 ) , D ( 13 ) . E , zero ( 7 ) , L ( 12 ) , D ( 12 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 12 ) = x ( 8 ) ;
    D ( 12 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"44" => {
"syntax" => "d14",
"read" => <<DATA
{
    return ( D ( 14 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 14 ) = signExtend ( x , 40 ) ;
    L ( 14 ) = 0 ;
}
DATA
,
},
"47" => {
"syntax" => "d14.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 15 ) , D ( 15 ) . E , zero ( 7 ) , L ( 14 ) , D ( 14 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 14 ) = x ( 8 ) ;
    D ( 14 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"48" => {
"syntax" => "r8",
"reg" => "R(8)",
},
"51" => {
"syntax" => "d8.e:d9.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 9 ) , D ( 9 ) . E , zero ( 7 ) , L ( 8 ) , D ( 8 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 9 ) = x ( 24 ) ;
    D ( 9 ) . E = x ( 23 , 16 ) ;
    L ( 8 ) = x ( 8 ) ;
    D ( 8 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"52" => {
"syntax" => "r10",
"reg" => "R(10)",
},
"55" => {
"syntax" => "d10.e:d11.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 11 ) , D ( 11 ) . E , zero ( 7 ) , L ( 10 ) , D ( 10 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 11 ) = x ( 24 ) ;
    D ( 11 ) . E = x ( 23 , 16 ) ;
    L ( 10 ) = x ( 8 ) ;
    D ( 10 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"56" => {
"syntax" => "r12",
"reg" => "R(12)",
},
"59" => {
"syntax" => "d12.e:d13.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 13 ) , D ( 13 ) . E , zero ( 7 ) , L ( 12 ) , D ( 12 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 13 ) = x ( 24 ) ;
    D ( 13 ) . E = x ( 23 , 16 ) ;
    L ( 12 ) = x ( 8 ) ;
    D ( 12 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
"60" => {
"syntax" => "r14",
"reg" => "R(14)",
},
"63" => {
"syntax" => "d14.e:d15.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 15 ) , D ( 15 ) . E , zero ( 7 ) , L ( 14 ) , D ( 14 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 15 ) = x ( 24 ) ;
    D ( 15 ) . E = x ( 23 , 16 ) ;
    L ( 14 ) = x ( 8 ) ;
    D ( 14 ) . E = x ( 7 , 0 ) ;
}
DATA
,
},
},
},
"DoTable" => {
"width" => 32,
"size" => 64,
"shared" => 0,
"entries" => {
"0" => {
"syntax" => "d1",
"read" => <<DATA
{
    return ( D ( 1 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 1 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 1 ) = 0 ;
}
DATA
,
},
"1" => {
"syntax" => "b1",
"reg" => "B(1)",
},
"2" => {
"syntax" => "vba",
"reg" => "VBA",
},
"3" => {
"syntax" => "d1.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 1 ) , D ( 1 ) . E , zero ( 7 ) , L ( 0 ) , D ( 0 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 1 ) = x ( 24 ) ;
    D ( 1 ) . E = x ( 23 , 16 ) ;
}
DATA
,
},
"4" => {
"syntax" => "d3",
"read" => <<DATA
{
    return ( D ( 3 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 3 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 3 ) = 0 ;
}
DATA
,
},
"5" => {
"syntax" => "b3",
"reg" => "B(3)",
},
"7" => {
"syntax" => "d3.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 3 ) , D ( 3 ) . E , zero ( 7 ) , L ( 2 ) , D ( 2 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 3 ) = x ( 24 ) ;
    D ( 3 ) . E = x ( 23 , 16 ) ;
}
DATA
,
},
"8" => {
"syntax" => "d5",
"read" => <<DATA
{
    return ( D ( 5 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 5 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 5 ) = 0 ;
}
DATA
,
},
"9" => {
"syntax" => "b5",
"reg" => "B(5)",
},
"10" => {
"syntax" => "sr",
"reg" => "SR",
},
"11" => {
"syntax" => "d5.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 5 ) , D ( 5 ) . E , zero ( 7 ) , L ( 4 ) , D ( 4 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 5 ) = x ( 24 ) ;
    D ( 5 ) . E = x ( 23 , 16 ) ;
}
DATA
,
},
"12" => {
"syntax" => "d7",
"read" => <<DATA
{
    return ( D ( 7 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 7 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 7 ) = 0 ;
}
DATA
,
},
"13" => {
"syntax" => "b7",
"reg" => "B(7)",
},
"14" => {
"syntax" => "mctl",
"reg" => "MCTL",
},
"15" => {
"syntax" => "d7.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 7 ) , D ( 7 ) . E , zero ( 7 ) , L ( 6 ) , D ( 6 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 7 ) = x ( 24 ) ;
    D ( 7 ) . E = x ( 23 , 16 ) ;
}
DATA
,
},
"16" => {
"syntax" => "r1",
"reg" => "R(1)",
},
"17" => {
"syntax" => "n1",
"reg" => "N(1)",
},
"18" => {
"syntax" => "LC0",
"reg" => "LC(0)",
},
"20" => {
"syntax" => "r3",
"reg" => "R(3)",
},
"21" => {
"syntax" => "n3",
"reg" => "N(3)",
},
"22" => {
"syntax" => "LC1",
"reg" => "LC(1)",
},
"24" => {
"syntax" => "r5",
"reg" => "R(5)",
},
"25" => {
"syntax" => "m1",
"reg" => "M(1)",
},
"26" => {
"syntax" => "LC2",
"reg" => "LC(2)",
},
"28" => {
"syntax" => "r7",
"reg" => "R(7)",
},
"29" => {
"syntax" => "m3",
"reg" => "M(3)",
},
"30" => {
"syntax" => "LC3",
"reg" => "LC(3)",
},
"32" => {
"syntax" => "d9",
"read" => <<DATA
{
    return ( D ( 9 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 9 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 9 ) = 0 ;
}
DATA
,
},
"35" => {
"syntax" => "d9.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 9 ) , D ( 9 ) . E , zero ( 7 ) , L ( 8 ) , D ( 8 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 9 ) = x ( 24 ) ;
    D ( 9 ) . E = x ( 23 , 16 ) ;
}
DATA
,
},
"36" => {
"syntax" => "d11",
"read" => <<DATA
{
    return ( D ( 11 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 11 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 11 ) = 0 ;
}
DATA
,
},
"39" => {
"syntax" => "d11.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 11 ) , D ( 11 ) . E , zero ( 7 ) , L ( 10 ) , D ( 10 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 11 ) = x ( 24 ) ;
    D ( 11 ) . E = x ( 23 , 16 ) ;
}
DATA
,
},
"40" => {
"syntax" => "d13",
"read" => <<DATA
{
    return ( D ( 13 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 13 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 13 ) = 0 ;
}
DATA
,
},
"43" => {
"syntax" => "d13.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 13 ) , D ( 13 ) . E , zero ( 7 ) , L ( 12 ) , D ( 12 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 13 ) = x ( 24 ) ;
    D ( 13 ) . E = x ( 23 , 16 ) ;
}
DATA
,
},
"44" => {
"syntax" => "d15",
"read" => <<DATA
{
    return ( D ( 15 ) ( 31 , 0 ) ) ;
}
DATA
,
"write" => <<DATA
{
    D ( 15 ) = signExtend ( x ( 31 , 0 ) , 40 ) ;
    L ( 15 ) = 0 ;
}
DATA
,
},
"47" => {
"syntax" => "d15.e",
"read" => <<DATA
{
    return concat ( zero ( 7 ) , L ( 15 ) , D ( 15 ) . E , zero ( 7 ) , L ( 14 ) , D ( 14 ) . E ) ;
}
DATA
,
"write" => <<DATA
{
    L ( 15 ) = x ( 24 ) ;
    D ( 15 ) . E = x ( 23 , 16 ) ;
}
DATA
,
},
"48" => {
"syntax" => "r9",
"reg" => "R(9)",
},
"52" => {
"syntax" => "r11",
"reg" => "R(11)",
},
"56" => {
"syntax" => "r13",
"reg" => "R(13)",
},
"60" => {
"syntax" => "r15",
"reg" => "R(15)",
},
},
},
"L" => {
"width" => 1,
"size" => 16,
"prefix" => "l",
"shared" => 0,
},
"LC" => {
"width" => 32,
"size" => 4,
"prefix" => "LC",
"shared" => 0,
},
"M" => {
"width" => 32,
"size" => 4,
"prefix" => "m",
"shared" => 0,
},
"N" => {
"width" => 32,
"size" => 4,
"prefix" => "n",
"shared" => 0,
},
"R" => {
"width" => 32,
"fields" => {
"H" => {
"bits" => [
[
31,
16,
],
],
},
"L" => {
"bits" => [
[
15,
0,
],
],
},
},
"size" => 16,
"prefix" => "r",
"shared" => 0,
},
"RAS" => {
"width" => 32,
"size" => 4,
"shared" => 0,
},
"RxTable" => {
"width" => 32,
"size" => 32,
"shared" => 0,
"entries" => {
"0" => {
"syntax" => "n0",
"reg" => "N(0)",
},
"1" => {
"syntax" => "n1",
"reg" => "N(1)",
},
"2" => {
"syntax" => "n2",
"reg" => "N(2)",
},
"3" => {
"syntax" => "n3",
"reg" => "N(3)",
},
"7" => {
"syntax" => "sp",
"read" => <<DATA
{
    if ( SR . EXP == 1 ) return ESP ; else return NSP ;
}
DATA
,
"write" => <<DATA
{
    if ( SR . EXP == 1 ) {
        ESP = x ;
    } else {
        NSP = x ;
    }
}
DATA
,
},
"8" => {
"syntax" => "r0",
"reg" => "R(0)",
},
"9" => {
"syntax" => "r1",
"reg" => "R(1)",
},
"10" => {
"syntax" => "r2",
"reg" => "R(2)",
},
"11" => {
"syntax" => "r3",
"reg" => "R(3)",
},
"12" => {
"syntax" => "r4",
"reg" => "R(4)",
},
"13" => {
"syntax" => "r5",
"reg" => "R(5)",
},
"14" => {
"syntax" => "r6",
"reg" => "R(6)",
},
"15" => {
"syntax" => "r7",
"reg" => "R(7)",
},
"24" => {
"syntax" => "r8",
"reg" => "R(8)",
},
"25" => {
"syntax" => "r9",
"reg" => "R(9)",
},
"26" => {
"syntax" => "r10",
"reg" => "R(10)",
},
"27" => {
"syntax" => "r11",
"reg" => "R(11)",
},
"28" => {
"syntax" => "r12",
"reg" => "R(12)",
},
"29" => {
"syntax" => "r13",
"reg" => "R(13)",
},
"30" => {
"syntax" => "r14",
"reg" => "R(14)",
},
"31" => {
"syntax" => "r15",
"reg" => "R(15)",
},
},
},
"SA" => {
"width" => 32,
"serial" => 1,
"size" => 4,
"prefix" => "SA",
"shared" => 0,
},
"VALID_RAS" => {
"width" => 1,
"reset" => "0",
"size" => 4,
"shared" => 0,
},
"rxTable" => {
"width" => 32,
"size" => 32,
"shared" => 0,
"entries" => {
"0" => {
"syntax" => "n0",
"reg" => "N(0)",
},
"1" => {
"syntax" => "n1",
"reg" => "N(1)",
},
"2" => {
"syntax" => "n2",
"reg" => "N(2)",
},
"3" => {
"syntax" => "n3",
"reg" => "N(3)",
},
"6" => {
"syntax" => "PC_ST_VLES",
"reg" => "PC_ST_VLES",
},
"7" => {
"syntax" => "sp",
"read" => <<DATA
{
    if ( SR . EXP == 1 ) return ESP ; else return NSP ;
}
DATA
,
"write" => <<DATA
{
    if ( SR . EXP == 1 ) {
        ESP = x ;
    } else {
        NSP = x ;
    }
}
DATA
,
},
"8" => {
"syntax" => "r0",
"reg" => "R(0)",
},
"9" => {
"syntax" => "r1",
"reg" => "R(1)",
},
"10" => {
"syntax" => "r2",
"reg" => "R(2)",
},
"11" => {
"syntax" => "r3",
"reg" => "R(3)",
},
"12" => {
"syntax" => "r4",
"reg" => "R(4)",
},
"13" => {
"syntax" => "r5",
"reg" => "R(5)",
},
"14" => {
"syntax" => "r6",
"reg" => "R(6)",
},
"15" => {
"syntax" => "r7",
"reg" => "R(7)",
},
"24" => {
"syntax" => "r8",
"reg" => "R(8)",
},
"25" => {
"syntax" => "r9",
"reg" => "R(9)",
},
"26" => {
"syntax" => "r10",
"reg" => "R(10)",
},
"27" => {
"syntax" => "r11",
"reg" => "R(11)",
},
"28" => {
"syntax" => "r12",
"reg" => "R(12)",
},
"29" => {
"syntax" => "r13",
"reg" => "R(13)",
},
"30" => {
"syntax" => "r14",
"reg" => "R(14)",
},
"31" => {
"syntax" => "r15",
"reg" => "R(15)",
},
},
},
},
"instrfields" => {
"AAAAA" => {
"pseudo" => 1,
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"CCC" => {
"pseudo" => 1,
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "C1Table",
},
"CCCC" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "C2Table",
},
"CCCC_imp_bits__11_8_" => {
"bits" => [
[
11,
8,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000f00",
"type" => "regfile",
"implements" => "CCCC",
"ref" => "C2Table",
},
"CCC_imp_bits__18_18_x_17_17_x_16_16_" => {
"bits" => [
[
18,
16,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000070000",
"type" => "regfile",
"implements" => "CCC",
"ref" => "C1Table",
},
"CCC_imp_bits__42_40_" => {
"bits" => [
[
42,
40,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000070000000000",
"type" => "regfile",
"implements" => "CCC",
"ref" => "C1Table",
},
"DDDD" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "C3Table",
},
"DDDDD" => {
"pseudo" => 1,
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "C4Table",
},
"DDDDD_imp_bits__16_16_x_11_8_x_4_4_" => {
"bits" => [
[
16,
16,
],
[
11,
8,
],
[
4,
4,
],
],
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x0000000000010f10",
"type" => "regfile",
"implements" => "DDDDD",
"ref" => "C4Table",
},
"DDDDD_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_x_17_17_" => {
"bits" => [
[
32,
32,
],
[
27,
24,
],
[
17,
17,
],
],
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x000000010f020000",
"type" => "regfile",
"implements" => "DDDDD",
"ref" => "C4Table",
},
"DDDDD_imp_bits__48_48_x_43_40_x_33_33_" => {
"bits" => [
[
48,
48,
],
[
43,
40,
],
[
33,
33,
],
],
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x00010f0200000000",
"type" => "regfile",
"implements" => "DDDDD",
"ref" => "C4Table",
},
"DDDD_imp_bits__11_11_x_10_10_x_9_9_x_8_8_" => {
"bits" => [
[
11,
8,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000f00",
"type" => "regfile",
"implements" => "DDDD",
"ref" => "C3Table",
},
"DaH_DbL" => {
"syntax" => "Da.h,Db.l",
"alias" => "JJJJJ",
},
"DaL_DbH" => {
"syntax" => "Da.l,Db.h",
"alias" => "JJJJJ",
},
"DaL_DbL" => {
"syntax" => "Da.l,Db.l",
"alias" => "JJJJJ",
},
"DaX_DaY" => {
"syntax" => "DaX,DaY",
"alias" => "dddd",
},
"DaX_DaYm" => {
"syntax" => "DaX,-DaY",
"alias" => "dddd",
},
"DaXm_DaY" => {
"syntax" => "-DaX,DaY",
"alias" => "dddd",
},
"DaXm_DaYm" => {
"syntax" => "-DaX,-DaY",
"alias" => "dddd",
},
"DbDa" => {
"syntax" => "Db,Da",
"alias" => "JJJJJ",
},
"EEEEE" => {
"pseudo" => 1,
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "DeTable",
},
"EEEEE_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_4_4_x_1_1_" => {
"bits" => [
[
16,
16,
],
[
11,
9,
],
[
4,
4,
],
[
1,
1,
],
],
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x0000000000010e12",
"type" => "regfile",
"implements" => "EEEEE",
"ref" => "DeTable",
},
"ESG" => {
"pseudo" => 1,
"prefix" => 1,
"width" => 8,
"size" => 1,
"shift" => 0,
"indexed" => 1,
"mask" => "0x0000000000000000",
"type" => "imm",
"action" => <<DATA
{
     unsigned pos = ( 10 - Index + get_prefix_size (  ) ) % 6 ;
    ThisField = bits ( pos , pos ) ;
}
DATA
,
},
"ESG_imp_bits__19_19_" => {
"bits" => [
[
19,
19,
],
],
"prefix" => 1,
"width" => 1,
"size" => 1,
"shift" => 0,
"indexed" => 1,
"mask" => "0x0000000000080000",
"type" => "imm",
"action" => <<DATA
{
     unsigned pos = ( 10 - Index + get_prefix_size (  ) ) % 6 ;
    ThisField = bits ( pos , pos ) ;
}
DATA
,
"implements" => "ESG",
},
"ESG_imp_bits__35_35_" => {
"bits" => [
[
35,
35,
],
],
"prefix" => 1,
"width" => 1,
"size" => 1,
"shift" => 0,
"indexed" => 1,
"mask" => "0x0000000800000000",
"type" => "imm",
"action" => <<DATA
{
     unsigned pos = ( 10 - Index + get_prefix_size (  ) ) % 6 ;
    ThisField = bits ( pos , pos ) ;
}
DATA
,
"implements" => "ESG",
},
"ESG_imp_bits__51_51_" => {
"bits" => [
[
51,
51,
],
],
"prefix" => 1,
"width" => 1,
"size" => 1,
"shift" => 0,
"indexed" => 1,
"mask" => "0x0008000000000000",
"type" => "imm",
"action" => <<DATA
{
     unsigned pos = ( 10 - Index + get_prefix_size (  ) ) % 6 ;
    ThisField = bits ( pos , pos ) ;
}
DATA
,
"implements" => "ESG",
},
"Enc1" => {
"pseudo" => 1,
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"Enc1_imp_bits__15_0_" => {
"bits" => [
[
15,
0,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x000000000000ffff",
"type" => "imm",
"implements" => "Enc1",
},
"Enc2" => {
"pseudo" => 1,
"width" => 32,
"size" => 32,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"Enc2_imp_bits__31_0_" => {
"bits" => [
[
31,
0,
],
],
"width" => 32,
"size" => 32,
"shift" => 0,
"mask" => "0x00000000ffffffff",
"type" => "imm",
"implements" => "Enc2",
},
"Enc3" => {
"pseudo" => 1,
"width" => 48,
"size" => 48,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"Enc3_imp_bits__47_0_" => {
"bits" => [
[
47,
0,
],
],
"width" => 48,
"size" => 48,
"shift" => 0,
"mask" => "0x0000ffffffffffff",
"type" => "imm",
"implements" => "Enc3",
},
"FFF" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "D",
},
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => {
"bits" => [
[
16,
16,
],
[
11,
9,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000010e00",
"type" => "regfile",
"implements" => "FFF",
"ref" => "D",
},
"FFF_imp_bits__16_16_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
16,
16,
],
[
2,
0,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000010007",
"type" => "regfile",
"implements" => "FFF",
"ref" => "D",
},
"FFF_imp_bits__16_16_x_9_7_" => {
"bits" => [
[
16,
16,
],
[
9,
7,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000010380",
"type" => "regfile",
"implements" => "FFF",
"ref" => "D",
},
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => {
"bits" => [
[
16,
16,
],
[
9,
7,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000010380",
"type" => "regfile",
"implements" => "FFF",
"ref" => "D",
},
"FFF_imp_bits__32_32_x_18_16_" => {
"bits" => [
[
32,
32,
],
[
18,
16,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000100070000",
"type" => "regfile",
"implements" => "FFF",
"ref" => "D",
},
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => {
"bits" => [
[
32,
32,
],
[
18,
16,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000100070000",
"type" => "regfile",
"implements" => "FFF",
"ref" => "D",
},
"GG" => {
"syntax" => "Dg,Dh",
"pseudo" => 1,
"width" => 4,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
3,
3,
],
{
"DgDh" => 1,
},
[
2,
2,
],
{
"DgDh" => 0,
},
],
"instrfields" => {
"Dg" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DgDh" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
4,
],
"1" => [
1,
5,
],
"2" => [
2,
6,
],
"3" => [
3,
7,
],
},
},
"Dh" => {
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
"GGG" => {
"syntax" => "Ds,Dt",
"pseudo" => 1,
"width" => 5,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
4,
4,
],
{
"DsDt" => 1,
},
[
3,
3,
],
{
"DsDt" => 0,
},
],
"instrfields" => {
"Ds" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DsDt" => {
"size" => 3,
"bits" => [
[
2,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
1,
2,
],
"2" => [
2,
3,
],
"3" => [
3,
4,
],
"4" => [
4,
5,
],
"5" => [
5,
6,
],
"6" => [
6,
7,
],
"7" => [
7,
0,
],
},
},
"Dt" => {
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
"GGG_imp_bits__18_17_x_2_0_" => {
"syntax" => "Ds,Dt",
"bits" => [
[
18,
17,
],
[
2,
0,
],
],
"width" => 5,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000060007",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "GGG",
"ref" => "D",
"fields" => [
[
4,
4,
],
{
"DsDt" => 1,
},
[
3,
3,
],
{
"DsDt" => 0,
},
],
"instrfields" => {
"Ds" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DsDt" => {
"size" => 3,
"bits" => [
[
2,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
1,
2,
],
"2" => [
2,
3,
],
"3" => [
3,
4,
],
"4" => [
4,
5,
],
"5" => [
5,
6,
],
"6" => [
6,
7,
],
"7" => [
7,
0,
],
},
},
"Dt" => {
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
"GGG_imp_bits__18_18_x_17_17_x_2_2_x_1_1_x_0_0_" => {
"syntax" => "Ds,Dt",
"bits" => [
[
18,
17,
],
[
2,
0,
],
],
"width" => 5,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000060007",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "GGG",
"ref" => "D",
"fields" => [
[
4,
4,
],
{
"DsDt" => 1,
},
[
3,
3,
],
{
"DsDt" => 0,
},
],
"instrfields" => {
"Ds" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DsDt" => {
"size" => 3,
"bits" => [
[
2,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
1,
2,
],
"2" => [
2,
3,
],
"3" => [
3,
4,
],
"4" => [
4,
5,
],
"5" => [
5,
6,
],
"6" => [
6,
7,
],
"7" => [
7,
0,
],
},
},
"Dt" => {
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
"GG_imp_bits__16_16_x_18_18_x_9_9_x_8_8_" => {
"syntax" => "Dg,Dh",
"bits" => [
[
16,
16,
],
[
18,
18,
],
[
9,
8,
],
],
"width" => 4,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000050300",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "GG",
"ref" => "D",
"fields" => [
[
3,
3,
],
{
"DgDh" => 1,
},
[
2,
2,
],
{
"DgDh" => 0,
},
],
"instrfields" => {
"Dg" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DgDh" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
4,
],
"1" => [
1,
5,
],
"2" => [
2,
6,
],
"3" => [
3,
7,
],
},
},
"Dh" => {
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
"GG_imp_bits__18_18_x_17_17_x_2_2_x_0_0_" => {
"syntax" => "Dg,Dh",
"bits" => [
[
18,
17,
],
[
2,
2,
],
[
0,
0,
],
],
"width" => 4,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000060005",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "GG",
"ref" => "D",
"fields" => [
[
3,
3,
],
{
"DgDh" => 1,
},
[
2,
2,
],
{
"DgDh" => 0,
},
],
"instrfields" => {
"Dg" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DgDh" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
4,
],
"1" => [
1,
5,
],
"2" => [
2,
6,
],
"3" => [
3,
7,
],
},
},
"Dh" => {
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
"GG_imp_bits__18_18_x_17_17_x_9_9_x_8_8_" => {
"syntax" => "Dg,Dh",
"bits" => [
[
18,
17,
],
[
9,
8,
],
],
"width" => 4,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000060300",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "GG",
"ref" => "D",
"fields" => [
[
3,
3,
],
{
"DgDh" => 1,
},
[
2,
2,
],
{
"DgDh" => 0,
},
],
"instrfields" => {
"Dg" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DgDh" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
4,
],
"1" => [
1,
5,
],
"2" => [
2,
6,
],
"3" => [
3,
7,
],
},
},
"Dh" => {
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
"HHHH" => {
"pseudo" => 1,
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "DRTable",
},
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => {
"bits" => [
[
16,
16,
],
[
11,
8,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x0000000000010f00",
"type" => "regfile",
"implements" => "HHHH",
"ref" => "DRTable",
},
"HHHH_imp_bits__16_16_x_11_8_" => {
"bits" => [
[
16,
16,
],
[
11,
8,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x0000000000010f00",
"type" => "regfile",
"implements" => "HHHH",
"ref" => "DRTable",
},
"HHHH_imp_bits__16_16_x_3_0_" => {
"bits" => [
[
16,
16,
],
[
3,
0,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x000000000001000f",
"type" => "regfile",
"implements" => "HHHH",
"ref" => "DRTable",
},
"HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_" => {
"bits" => [
[
32,
32,
],
[
19,
16,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x00000001000f0000",
"type" => "regfile",
"implements" => "HHHH",
"ref" => "DRTable",
},
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => {
"bits" => [
[
32,
32,
],
[
27,
24,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x000000010f000000",
"type" => "regfile",
"implements" => "HHHH",
"ref" => "DRTable",
},
"HHHH_imp_bits__48_48_x_43_40_" => {
"bits" => [
[
48,
48,
],
[
43,
40,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x00010f0000000000",
"type" => "regfile",
"implements" => "HHHH",
"ref" => "DRTable",
},
"HHHH_imp_bits__48_48_x_43_43_x_42_42_x_41_41_x_40_40_" => {
"bits" => [
[
48,
48,
],
[
43,
40,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x00010f0000000000",
"type" => "regfile",
"implements" => "HHHH",
"ref" => "DRTable",
},
"HighAgu" => {
"pseudo" => 1,
"prefix" => 1,
"width" => 24,
"size" => 3,
"shift" => 0,
"indexed" => 3,
"mask" => "0x0000000000000000",
"blocks" => [
"agu",
],
"type" => "regfield",
"action" => <<DATA
{
     unsigned i = get_agu_dispatch (  ) ;
    ThisField = bits ( ( i + 1 ) * 3 - 1 , i * 3 ) ;
}
DATA
,
"value" => 0,
},
"HighAgu_imp_bits__8_8_x_22_22_x_23_23_x_21_21_" => {
"bits" => [
[
8,
8,
],
[
22,
22,
],
[
23,
23,
],
[
21,
21,
],
],
"prefix" => 1,
"width" => 4,
"size" => 3,
"shift" => 0,
"indexed" => 3,
"mask" => "0x0000000000e00100",
"blocks" => [
"agu",
],
"type" => "regfield",
"action" => <<DATA
{
     unsigned i = get_agu_dispatch (  ) ;
    ThisField = bits ( ( i + 1 ) * 3 - 1 , i * 3 ) ;
}
DATA
,
"value" => 0,
"implements" => "HighAgu",
},
"HighDalu" => {
"pseudo" => 1,
"prefix" => 1,
"width" => 24,
"size" => 3,
"shift" => 0,
"indexed" => 3,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfield",
"action" => <<DATA
{
     unsigned pos = getPacketPosition (  ) ;
     unsigned i = get_dalu_dispatch ( pos ) ;
    ThisField = bits ( ( i + 1 ) * 3 - 1 , i * 3 ) ;
}
DATA
,
"value" => 0,
},
"HighDalu2" => {
"pseudo" => 1,
"prefix" => 1,
"width" => 24,
"size" => 3,
"shift" => 0,
"indexed" => 3,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfield",
"action" => <<DATA
{
     unsigned pos = getPacketPosition (  ) ;
     unsigned i = get_dalu_dispatch ( pos ) ;
    i = i % 2 ;
    ThisField = bits ( ( i + 1 ) * 3 - 1 , i * 3 ) ;
}
DATA
,
"value" => 0,
},
"HighDalu2_imp_bits__12_12_x_10_10_x_7_7_x_11_11_x_9_9_x_6_6_" => {
"bits" => [
[
12,
12,
],
[
10,
10,
],
[
7,
7,
],
[
11,
11,
],
[
9,
9,
],
[
6,
6,
],
],
"prefix" => 1,
"width" => 6,
"size" => 3,
"shift" => 0,
"indexed" => 3,
"mask" => "0x0000000000001ec0",
"blocks" => [
"dalu",
],
"type" => "regfield",
"action" => <<DATA
{
     unsigned pos = getPacketPosition (  ) ;
     unsigned i = get_dalu_dispatch ( pos ) ;
    i = i % 2 ;
    ThisField = bits ( ( i + 1 ) * 3 - 1 , i * 3 ) ;
}
DATA
,
"value" => 0,
"implements" => "HighDalu2",
},
"HighDalu_imp_bits__12_12_x_7_7_x_3_3_x_11_11_x_6_6_x_2_2_x_10_10_x_5_5_x_1_1_x_9_9_x_4_4_x_0_0_" => {
"bits" => [
[
12,
12,
],
[
7,
7,
],
[
3,
3,
],
[
11,
11,
],
[
6,
6,
],
[
2,
2,
],
[
10,
10,
],
[
5,
5,
],
[
1,
1,
],
[
9,
9,
],
[
4,
4,
],
[
0,
0,
],
],
"prefix" => 1,
"width" => 12,
"size" => 3,
"shift" => 0,
"indexed" => 3,
"mask" => "0x0000000000001eff",
"blocks" => [
"dalu",
],
"type" => "regfield",
"action" => <<DATA
{
     unsigned pos = getPacketPosition (  ) ;
     unsigned i = get_dalu_dispatch ( pos ) ;
    ThisField = bits ( ( i + 1 ) * 3 - 1 , i * 3 ) ;
}
DATA
,
"value" => 0,
"implements" => "HighDalu",
},
"HighDes" => {
"pseudo" => 1,
"width" => 1,
"size" => 1,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"HighReg" => {
"pseudo" => 1,
"prefix" => 1,
"width" => 24,
"size" => 3,
"shift" => 0,
"indexed" => 3,
"mask" => "0x0000000000000000",
"blocks" => [
"agu",
"dalu",
],
"type" => "regfield",
"action" => <<DATA
{
     unsigned pos = get_instr_pos (  ) ;
    ThisField = bits ( ( pos + 1 ) * 3 - 1 , pos * 3 ) ;
}
DATA
,
"value" => 0,
},
"HighReg_imp_bits__3_3_x_1_1_x_4_4_x_2_2_x_0_0_" => {
"bits" => [
[
3,
3,
],
[
1,
1,
],
[
4,
4,
],
[
2,
2,
],
[
0,
0,
],
],
"prefix" => 1,
"width" => 5,
"size" => 3,
"shift" => 0,
"indexed" => 3,
"mask" => "0x000000000000001f",
"blocks" => [
"agu",
"dalu",
],
"type" => "regfield",
"action" => <<DATA
{
     unsigned pos = get_instr_pos (  ) ;
    ThisField = bits ( ( pos + 1 ) * 3 - 1 , pos * 3 ) ;
}
DATA
,
"value" => 0,
"implements" => "HighReg",
},
"HighRegister" => {
"pseudo" => 1,
"prefix" => 1,
"width" => 24,
"size" => 3,
"shift" => 0,
"indexed" => 3,
"mask" => "0x0000000000000000",
"blocks" => [
"agu",
"dalu",
],
"type" => "imm",
},
"HighRegister_imp_bits__18_16_" => {
"bits" => [
[
18,
16,
],
],
"prefix" => 1,
"width" => 3,
"size" => 3,
"shift" => 0,
"indexed" => 3,
"mask" => "0x0000000000070000",
"blocks" => [
"agu",
"dalu",
],
"type" => "imm",
"implements" => "HighRegister",
},
"HighRegister_imp_bits__34_32_" => {
"bits" => [
[
34,
32,
],
],
"prefix" => 1,
"width" => 3,
"size" => 3,
"shift" => 0,
"indexed" => 3,
"mask" => "0x0000000700000000",
"blocks" => [
"agu",
"dalu",
],
"type" => "imm",
"implements" => "HighRegister",
},
"HighRegister_imp_bits__50_48_" => {
"bits" => [
[
50,
48,
],
],
"prefix" => 1,
"width" => 3,
"size" => 3,
"shift" => 0,
"indexed" => 3,
"mask" => "0x0007000000000000",
"blocks" => [
"agu",
"dalu",
],
"type" => "imm",
"implements" => "HighRegister",
},
"HighSrc" => {
"pseudo" => 1,
"width" => 1,
"size" => 1,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"JJJ" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "D",
},
"JJJJJ" => {
"syntax" => "Da,Db",
"pseudo" => 1,
"width" => 7,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
6,
6,
],
{
"DaDb" => 1,
},
[
5,
5,
],
{
"DaDb" => 0,
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
"7" => [
1,
7,
],
"8" => [
2,
4,
],
"9" => [
2,
5,
],
"10" => [
2,
6,
],
"11" => [
2,
7,
],
"12" => [
3,
4,
],
"13" => [
3,
5,
],
"14" => [
3,
6,
],
"15" => [
3,
7,
],
"16" => [
0,
0,
],
"17" => [
0,
1,
],
"18" => [
0,
2,
],
"19" => [
0,
3,
],
"20" => [
4,
4,
],
"21" => [
4,
5,
],
"22" => [
4,
6,
],
"23" => [
4,
7,
],
"24" => [
1,
2,
],
"25" => [
1,
3,
],
"26" => [
5,
6,
],
"27" => [
5,
7,
],
"28" => [
2,
2,
],
"29" => [
2,
3,
],
"30" => [
6,
6,
],
"31" => [
6,
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
"JJJJJ_imp_bits__17_17_x_18_18_x_4_0_" => {
"syntax" => "Da,Db",
"bits" => [
[
17,
17,
],
[
18,
18,
],
[
4,
0,
],
],
"width" => 7,
"size" => 8,
"shift" => 0,
"mask" => "0x000000000006001f",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "JJJJJ",
"ref" => "D",
"fields" => [
[
6,
6,
],
{
"DaDb" => 1,
},
[
5,
5,
],
{
"DaDb" => 0,
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
"7" => [
1,
7,
],
"8" => [
2,
4,
],
"9" => [
2,
5,
],
"10" => [
2,
6,
],
"11" => [
2,
7,
],
"12" => [
3,
4,
],
"13" => [
3,
5,
],
"14" => [
3,
6,
],
"15" => [
3,
7,
],
"16" => [
0,
0,
],
"17" => [
0,
1,
],
"18" => [
0,
2,
],
"19" => [
0,
3,
],
"20" => [
4,
4,
],
"21" => [
4,
5,
],
"22" => [
4,
6,
],
"23" => [
4,
7,
],
"24" => [
1,
2,
],
"25" => [
1,
3,
],
"26" => [
5,
6,
],
"27" => [
5,
7,
],
"28" => [
2,
2,
],
"29" => [
2,
3,
],
"30" => [
6,
6,
],
"31" => [
6,
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
"shift" => 0,
"mask" => "0x000000000006001f",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "JJJJJ",
"ref" => "D",
"fields" => [
[
6,
6,
],
{
"DaDb" => 1,
},
[
5,
5,
],
{
"DaDb" => 0,
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
"7" => [
1,
7,
],
"8" => [
2,
4,
],
"9" => [
2,
5,
],
"10" => [
2,
6,
],
"11" => [
2,
7,
],
"12" => [
3,
4,
],
"13" => [
3,
5,
],
"14" => [
3,
6,
],
"15" => [
3,
7,
],
"16" => [
0,
0,
],
"17" => [
0,
1,
],
"18" => [
0,
2,
],
"19" => [
0,
3,
],
"20" => [
4,
4,
],
"21" => [
4,
5,
],
"22" => [
4,
6,
],
"23" => [
4,
7,
],
"24" => [
1,
2,
],
"25" => [
1,
3,
],
"26" => [
5,
6,
],
"27" => [
5,
7,
],
"28" => [
2,
2,
],
"29" => [
2,
3,
],
"30" => [
6,
6,
],
"31" => [
6,
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
"JJJJJ_imp_bits__18_18_x_17_17_x_4_0_" => {
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
"shift" => 0,
"mask" => "0x000000000006001f",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "JJJJJ",
"ref" => "D",
"fields" => [
[
6,
6,
],
{
"DaDb" => 1,
},
[
5,
5,
],
{
"DaDb" => 0,
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
"7" => [
1,
7,
],
"8" => [
2,
4,
],
"9" => [
2,
5,
],
"10" => [
2,
6,
],
"11" => [
2,
7,
],
"12" => [
3,
4,
],
"13" => [
3,
5,
],
"14" => [
3,
6,
],
"15" => [
3,
7,
],
"16" => [
0,
0,
],
"17" => [
0,
1,
],
"18" => [
0,
2,
],
"19" => [
0,
3,
],
"20" => [
4,
4,
],
"21" => [
4,
5,
],
"22" => [
4,
6,
],
"23" => [
4,
7,
],
"24" => [
1,
2,
],
"25" => [
1,
3,
],
"26" => [
5,
6,
],
"27" => [
5,
7,
],
"28" => [
2,
2,
],
"29" => [
2,
3,
],
"30" => [
6,
6,
],
"31" => [
6,
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
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
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
"shift" => 0,
"mask" => "0x000000000006001f",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "JJJJJ",
"ref" => "D",
"fields" => [
[
6,
6,
],
{
"DaDb" => 1,
},
[
5,
5,
],
{
"DaDb" => 0,
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
"7" => [
1,
7,
],
"8" => [
2,
4,
],
"9" => [
2,
5,
],
"10" => [
2,
6,
],
"11" => [
2,
7,
],
"12" => [
3,
4,
],
"13" => [
3,
5,
],
"14" => [
3,
6,
],
"15" => [
3,
7,
],
"16" => [
0,
0,
],
"17" => [
0,
1,
],
"18" => [
0,
2,
],
"19" => [
0,
3,
],
"20" => [
4,
4,
],
"21" => [
4,
5,
],
"22" => [
4,
6,
],
"23" => [
4,
7,
],
"24" => [
1,
2,
],
"25" => [
1,
3,
],
"26" => [
5,
6,
],
"27" => [
5,
7,
],
"28" => [
2,
2,
],
"29" => [
2,
3,
],
"30" => [
6,
6,
],
"31" => [
6,
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
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
17,
17,
],
[
2,
0,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000020007",
"type" => "regfile",
"implements" => "JJJ",
"ref" => "D",
},
"JJJ_imp_bits__18_18_x_2_0_" => {
"bits" => [
[
18,
18,
],
[
2,
0,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000040007",
"type" => "regfile",
"implements" => "JJJ",
"ref" => "D",
},
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
18,
18,
],
[
2,
0,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000040007",
"type" => "regfile",
"implements" => "JJJ",
"ref" => "D",
},
"JJJ_imp_bits__33_33_x_2_0_" => {
"bits" => [
[
33,
33,
],
[
2,
0,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000200000007",
"type" => "regfile",
"implements" => "JJJ",
"ref" => "D",
},
"JJJ_imp_bits__33_33_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
33,
33,
],
[
2,
0,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000200000007",
"type" => "regfile",
"implements" => "JJJ",
"ref" => "D",
},
"JJJ_imp_bits__34_34_x_27_25_" => {
"bits" => [
[
34,
34,
],
[
27,
25,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x000000040e000000",
"type" => "regfile",
"implements" => "JJJ",
"ref" => "D",
},
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => {
"bits" => [
[
34,
34,
],
[
27,
25,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x000000040e000000",
"type" => "regfile",
"implements" => "JJJ",
"ref" => "D",
},
"JJJ_imp_bits__34_34_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
34,
34,
],
[
2,
0,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000400000007",
"type" => "regfile",
"implements" => "JJJ",
"ref" => "D",
},
"KKK" => {
"syntax" => "Dj,Dk",
"pseudo" => 1,
"width" => 5,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
4,
4,
],
{
"DjDk" => 1,
},
[
3,
3,
],
{
"DjDk" => 0,
},
],
"instrfields" => {
"Dj" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DjDk" => {
"size" => 3,
"bits" => [
[
2,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
7,
],
"4" => [
1,
0,
],
"5" => [
3,
2,
],
"6" => [
5,
4,
],
"7" => [
7,
6,
],
},
},
"Dk" => {
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
"KKK_imp_bits__18_18_x_17_17_x_9_9_x_8_8_x_5_5_" => {
"syntax" => "Dj,Dk",
"bits" => [
[
18,
17,
],
[
9,
8,
],
[
5,
5,
],
],
"width" => 5,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000060320",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "KKK",
"ref" => "D",
"fields" => [
[
4,
4,
],
{
"DjDk" => 1,
},
[
3,
3,
],
{
"DjDk" => 0,
},
],
"instrfields" => {
"Dj" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DjDk" => {
"size" => 3,
"bits" => [
[
2,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
7,
],
"4" => [
1,
0,
],
"5" => [
3,
2,
],
"6" => [
5,
4,
],
"7" => [
7,
6,
],
},
},
"Dk" => {
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
"MM" => {
"pseudo" => 1,
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000000000",
"enumerated" => {
"0" => ")+",
"1" => ")-",
"2" => "+n0)",
"3" => ")",
},
"type" => "imm",
},
"MMM" => {
"pseudo" => 1,
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000000000",
"enumerated" => {
"0" => "+n0)",
"1" => ")-",
"2" => ")",
"3" => ")+",
"4" => ")+n0",
"5" => ")+n1",
"6" => ")+n2",
"7" => ")+n3",
},
"type" => "imm",
},
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => {
"bits" => [
[
21,
19,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000380000",
"enumerated" => {
"0" => "+n0)",
"1" => ")-",
"2" => ")",
"3" => ")+",
"4" => ")+n0",
"5" => ")+n1",
"6" => ")+n2",
"7" => ")+n3",
},
"type" => "imm",
"implements" => "MMM",
},
"MMM_imp_bits__5_3_" => {
"bits" => [
[
5,
3,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000000038",
"enumerated" => {
"0" => "+n0)",
"1" => ")-",
"2" => ")",
"3" => ")+",
"4" => ")+n0",
"5" => ")+n1",
"6" => ")+n2",
"7" => ")+n3",
},
"type" => "imm",
"implements" => "MMM",
},
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => {
"bits" => [
[
5,
3,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000000038",
"enumerated" => {
"0" => "+n0)",
"1" => ")-",
"2" => ")",
"3" => ")+",
"4" => ")+n0",
"5" => ")+n1",
"6" => ")+n2",
"7" => ")+n3",
},
"type" => "imm",
"implements" => "MMM",
},
"MM_imp_bits__11_11_x_4_4_" => {
"bits" => [
[
11,
11,
],
[
4,
4,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000000810",
"enumerated" => {
"0" => ")+",
"1" => ")-",
"2" => "+n0)",
"3" => ")",
},
"type" => "imm",
"implements" => "MM",
},
"MM_imp_bits__4_4_x_3_3_" => {
"bits" => [
[
4,
3,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000000018",
"enumerated" => {
"0" => ")+",
"1" => ")-",
"2" => "+n0)",
"3" => ")",
},
"type" => "imm",
"implements" => "MM",
},
"OPCD" => {
"pseudo" => 1,
"width" => 0,
"size" => 0,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"QQ" => {
"syntax" => "De.e",
"pseudo" => 1,
"width" => 3,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"De_temp" => 0,
},
],
"instrfields" => {
"De" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"De_temp" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
],
"1" => [
2,
],
"2" => [
4,
],
"3" => [
6,
],
},
},
},
},
"QQQQQ" => {
"pseudo" => 1,
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "DeDoTable",
},
"QQQQQ_imp_bits__16_16_x_10_10_x_8_8_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
16,
16,
],
[
10,
10,
],
[
8,
8,
],
[
2,
0,
],
],
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x0000000000010507",
"type" => "regfile",
"implements" => "QQQQQ",
"ref" => "DeDoTable",
},
"QQ_imp_bits__32_32_x_26_26_x_25_25_" => {
"syntax" => "De.e",
"bits" => [
[
32,
32,
],
[
26,
25,
],
],
"width" => 3,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000106000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "QQ",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"De_temp" => 0,
},
],
"instrfields" => {
"De" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"De_temp" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
],
"1" => [
2,
],
"2" => [
4,
],
"3" => [
6,
],
},
},
},
},
"QQ_imp_bits__48_48_x_42_42_x_41_41_" => {
"syntax" => "De.e",
"bits" => [
[
48,
48,
],
[
42,
41,
],
],
"width" => 3,
"size" => 4,
"shift" => 0,
"mask" => "0x0001060000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "QQ",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"De_temp" => 0,
},
],
"instrfields" => {
"De" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"De_temp" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
],
"1" => [
2,
],
"2" => [
4,
],
"3" => [
6,
],
},
},
},
},
"RRR" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "R",
},
"RRRR" => {
"pseudo" => 1,
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "RxTable",
},
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => {
"bits" => [
[
18,
18,
],
[
11,
8,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x0000000000040f00",
"type" => "regfile",
"implements" => "RRRR",
"ref" => "RxTable",
},
"RRRR_imp_bits__18_18_x_11_8_" => {
"bits" => [
[
18,
18,
],
[
11,
8,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x0000000000040f00",
"type" => "regfile",
"implements" => "RRRR",
"ref" => "RxTable",
},
"RRR_imp_bits__18_18_x_10_10_x_9_9_x_8_8_" => {
"bits" => [
[
18,
18,
],
[
10,
8,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000040700",
"type" => "regfile",
"implements" => "RRR",
"ref" => "R",
},
"RRR_imp_bits__18_18_x_10_8_" => {
"bits" => [
[
18,
18,
],
[
10,
8,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000040700",
"type" => "regfile",
"implements" => "RRR",
"ref" => "R",
},
"RRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_" => {
"bits" => [
[
18,
18,
],
[
11,
9,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000040e00",
"type" => "regfile",
"implements" => "RRR",
"ref" => "R",
},
"RRR_imp_bits__18_18_x_11_9_" => {
"bits" => [
[
18,
18,
],
[
11,
9,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000040e00",
"type" => "regfile",
"implements" => "RRR",
"ref" => "R",
},
"RRR_imp_bits__18_18_x_2_0_" => {
"bits" => [
[
18,
18,
],
[
2,
0,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000040007",
"type" => "regfile",
"implements" => "RRR",
"ref" => "R",
},
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
18,
18,
],
[
2,
0,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000040007",
"type" => "regfile",
"implements" => "RRR",
"ref" => "R",
},
"RRR_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => {
"bits" => [
[
18,
18,
],
[
5,
3,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000040038",
"type" => "regfile",
"implements" => "RRR",
"ref" => "R",
},
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => {
"bits" => [
[
34,
34,
],
[
18,
16,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000400070000",
"type" => "regfile",
"implements" => "RRR",
"ref" => "R",
},
"RRR_imp_bits__34_34_x_26_24_" => {
"bits" => [
[
34,
34,
],
[
26,
24,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000407000000",
"type" => "regfile",
"implements" => "RRR",
"ref" => "R",
},
"RRR_imp_bits__34_34_x_26_26_x_25_25_x_24_24_" => {
"bits" => [
[
34,
34,
],
[
26,
24,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000407000000",
"type" => "regfile",
"implements" => "RRR",
"ref" => "R",
},
"Res" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"Src1" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"Src2" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"U6" => {
"pseudo" => 1,
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"U6_imp_bits__11_6_" => {
"bits" => [
[
11,
6,
],
],
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x0000000000000fc0",
"type" => "imm",
"implements" => "U6",
},
"VarInstrOpcode_imp_bits__19_19_x_15_0_" => {
"bits" => [
[
19,
19,
],
[
15,
0,
],
],
"width" => 17,
"size" => 17,
"shift" => 0,
"mask" => "0x000000000008ffff",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_2_" => {
"bits" => [
[
19,
19,
],
[
15,
10,
],
[
6,
2,
],
],
"width" => 12,
"size" => 12,
"shift" => 0,
"mask" => "0x000000000008fc7c",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_3_" => {
"bits" => [
[
19,
19,
],
[
15,
10,
],
[
6,
3,
],
],
"width" => 11,
"size" => 11,
"shift" => 0,
"mask" => "0x000000000008fc78",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_5_" => {
"bits" => [
[
19,
19,
],
[
15,
10,
],
[
6,
5,
],
],
"width" => 9,
"size" => 9,
"shift" => 0,
"mask" => "0x000000000008fc60",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_7_4_" => {
"bits" => [
[
19,
19,
],
[
15,
10,
],
[
7,
4,
],
],
"width" => 11,
"size" => 11,
"shift" => 0,
"mask" => "0x000000000008fcf0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_7_6_" => {
"bits" => [
[
19,
19,
],
[
15,
10,
],
[
7,
6,
],
],
"width" => 9,
"size" => 9,
"shift" => 0,
"mask" => "0x000000000008fcc0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_11_x_0_0_" => {
"bits" => [
[
19,
19,
],
[
15,
11,
],
[
0,
0,
],
],
"width" => 7,
"size" => 7,
"shift" => 0,
"mask" => "0x000000000008f801",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_11_x_7_0_" => {
"bits" => [
[
19,
19,
],
[
15,
11,
],
[
7,
0,
],
],
"width" => 14,
"size" => 14,
"shift" => 0,
"mask" => "0x000000000008f8ff",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_0_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
7,
0,
],
],
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x000000000008f0ff",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_3_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
7,
3,
],
],
"width" => 10,
"size" => 10,
"shift" => 0,
"mask" => "0x000000000008f0f8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_4_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
7,
4,
],
],
"width" => 9,
"size" => 9,
"shift" => 0,
"mask" => "0x000000000008f0f0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_5_x_3_3_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
7,
5,
],
[
3,
3,
],
],
"width" => 9,
"size" => 9,
"shift" => 0,
"mask" => "0x000000000008f0e8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_0_" => {
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
13,
10,
],
[
6,
0,
],
],
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x000000000008bc7f",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => {
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
13,
10,
],
[
6,
2,
],
],
"width" => 11,
"size" => 11,
"shift" => 0,
"mask" => "0x000000000008bc7c",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_3_" => {
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
13,
10,
],
[
6,
3,
],
],
"width" => 10,
"size" => 10,
"shift" => 0,
"mask" => "0x000000000008bc78",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => {
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
13,
10,
],
[
6,
5,
],
],
"width" => 8,
"size" => 8,
"shift" => 0,
"mask" => "0x000000000008bc60",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_12_x_7_6_" => {
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
13,
12,
],
[
7,
6,
],
],
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x000000000008b0c0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_" => {
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
13,
10,
],
[
6,
6,
],
],
"width" => 7,
"size" => 7,
"shift" => 0,
"mask" => "0x000000000008bc40",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_2_2_" => {
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
13,
10,
],
[
6,
6,
],
[
2,
2,
],
],
"width" => 8,
"size" => 8,
"shift" => 0,
"mask" => "0x000000000008bc44",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => {
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
13,
10,
],
[
6,
5,
],
],
"width" => 8,
"size" => 8,
"shift" => 0,
"mask" => "0x000000000008bc60",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => {
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
13,
10,
],
[
6,
3,
],
],
"width" => 10,
"size" => 10,
"shift" => 0,
"mask" => "0x000000000008bc78",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => {
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
13,
10,
],
[
6,
2,
],
],
"width" => 11,
"size" => 11,
"shift" => 0,
"mask" => "0x000000000008bc7c",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
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
13,
10,
],
[
6,
0,
],
],
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x000000000008bc7f",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_7_7_x_6_6_x_4_4_x_3_3_" => {
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
13,
10,
],
[
7,
6,
],
[
4,
3,
],
],
"width" => 10,
"size" => 10,
"shift" => 0,
"mask" => "0x000000000008bcd8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => {
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
13,
10,
],
[
7,
3,
],
],
"width" => 11,
"size" => 11,
"shift" => 0,
"mask" => "0x000000000008bcf8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
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
13,
10,
],
[
7,
0,
],
],
"width" => 14,
"size" => 14,
"shift" => 0,
"mask" => "0x000000000008bcff",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_" => {
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
13,
6,
],
],
"width" => 10,
"size" => 10,
"shift" => 0,
"mask" => "0x000000000008bfc0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => {
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
13,
11,
],
[
7,
6,
],
],
"width" => 7,
"size" => 7,
"shift" => 0,
"mask" => "0x000000000008b8c0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_x_5_5_" => {
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
13,
11,
],
[
7,
5,
],
],
"width" => 8,
"size" => 8,
"shift" => 0,
"mask" => "0x000000000008b8e0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => {
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
13,
11,
],
[
8,
6,
],
],
"width" => 8,
"size" => 8,
"shift" => 0,
"mask" => "0x000000000008b9c0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => {
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
13,
11,
],
[
9,
6,
],
],
"width" => 9,
"size" => 9,
"shift" => 0,
"mask" => "0x000000000008bbc0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_7_7_x_6_6_" => {
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
13,
12,
],
[
7,
6,
],
],
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x000000000008b0c0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => {
"bits" => [
[
19,
19,
],
[
15,
10,
],
[
6,
5,
],
],
"width" => 9,
"size" => 9,
"shift" => 0,
"mask" => "0x000000000008fc60",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_" => {
"bits" => [
[
19,
19,
],
[
15,
10,
],
[
6,
4,
],
],
"width" => 10,
"size" => 10,
"shift" => 0,
"mask" => "0x000000000008fc70",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => {
"bits" => [
[
19,
19,
],
[
15,
10,
],
[
6,
3,
],
],
"width" => 11,
"size" => 11,
"shift" => 0,
"mask" => "0x000000000008fc78",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_1_1_" => {
"bits" => [
[
19,
19,
],
[
15,
10,
],
[
6,
3,
],
[
1,
1,
],
],
"width" => 12,
"size" => 12,
"shift" => 0,
"mask" => "0x000000000008fc7a",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => {
"bits" => [
[
19,
19,
],
[
15,
10,
],
[
6,
2,
],
],
"width" => 12,
"size" => 12,
"shift" => 0,
"mask" => "0x000000000008fc7c",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_0_0_" => {
"bits" => [
[
19,
19,
],
[
15,
9,
],
[
0,
0,
],
],
"width" => 9,
"size" => 9,
"shift" => 0,
"mask" => "0x000000000008fe01",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_" => {
"bits" => [
[
19,
19,
],
[
15,
8,
],
],
"width" => 9,
"size" => 9,
"shift" => 0,
"mask" => "0x000000000008ff00",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_" => {
"bits" => [
[
19,
19,
],
[
15,
6,
],
],
"width" => 11,
"size" => 11,
"shift" => 0,
"mask" => "0x000000000008ffc0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_3_3_" => {
"bits" => [
[
19,
19,
],
[
15,
5,
],
[
3,
3,
],
],
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x000000000008ffe8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => {
"bits" => [
[
19,
19,
],
[
15,
3,
],
],
"width" => 14,
"size" => 14,
"shift" => 0,
"mask" => "0x000000000008fff8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
19,
19,
],
[
15,
0,
],
],
"width" => 17,
"size" => 17,
"shift" => 0,
"mask" => "0x000000000008ffff",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => {
"bits" => [
[
19,
19,
],
[
15,
11,
],
[
7,
6,
],
],
"width" => 8,
"size" => 8,
"shift" => 0,
"mask" => "0x000000000008f8c0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => {
"bits" => [
[
19,
19,
],
[
15,
11,
],
[
7,
3,
],
],
"width" => 11,
"size" => 11,
"shift" => 0,
"mask" => "0x000000000008f8f8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
19,
19,
],
[
15,
11,
],
[
7,
0,
],
],
"width" => 14,
"size" => 14,
"shift" => 0,
"mask" => "0x000000000008f8ff",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => {
"bits" => [
[
19,
19,
],
[
15,
11,
],
[
8,
6,
],
],
"width" => 9,
"size" => 9,
"shift" => 0,
"mask" => "0x000000000008f9c0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_9_9_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => {
"bits" => [
[
19,
19,
],
[
15,
11,
],
[
9,
9,
],
[
7,
3,
],
],
"width" => 12,
"size" => 12,
"shift" => 0,
"mask" => "0x000000000008faf8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => {
"bits" => [
[
19,
19,
],
[
15,
11,
],
[
9,
6,
],
],
"width" => 10,
"size" => 10,
"shift" => 0,
"mask" => "0x000000000008fbc0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
7,
7,
],
],
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x000000000008f080",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
7,
6,
],
],
"width" => 7,
"size" => 7,
"shift" => 0,
"mask" => "0x000000000008f0c0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
7,
5,
],
],
"width" => 8,
"size" => 8,
"shift" => 0,
"mask" => "0x000000000008f0e0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_3_3_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
7,
5,
],
[
3,
3,
],
],
"width" => 9,
"size" => 9,
"shift" => 0,
"mask" => "0x000000000008f0e8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
7,
4,
],
],
"width" => 9,
"size" => 9,
"shift" => 0,
"mask" => "0x000000000008f0f0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
7,
3,
],
],
"width" => 10,
"size" => 10,
"shift" => 0,
"mask" => "0x000000000008f0f8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
7,
0,
],
],
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x000000000008f0ff",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_4_4_x_3_3_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
8,
8,
],
[
4,
3,
],
],
"width" => 8,
"size" => 8,
"shift" => 0,
"mask" => "0x000000000008f118",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
8,
7,
],
],
"width" => 7,
"size" => 7,
"shift" => 0,
"mask" => "0x000000000008f180",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
8,
6,
],
],
"width" => 8,
"size" => 8,
"shift" => 0,
"mask" => "0x000000000008f1c0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
8,
5,
],
],
"width" => 9,
"size" => 9,
"shift" => 0,
"mask" => "0x000000000008f1e0",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_3_3_x_2_2_x_0_0_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
8,
5,
],
[
3,
2,
],
[
0,
0,
],
],
"width" => 12,
"size" => 12,
"shift" => 0,
"mask" => "0x000000000008f1ed",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
8,
3,
],
],
"width" => 11,
"size" => 11,
"shift" => 0,
"mask" => "0x000000000008f1f8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => {
"bits" => [
[
19,
19,
],
[
15,
12,
],
[
8,
2,
],
],
"width" => 12,
"size" => 12,
"shift" => 0,
"mask" => "0x000000000008f1fc",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_3_" => {
"bits" => [
[
19,
19,
],
[
15,
3,
],
],
"width" => 14,
"size" => 14,
"shift" => 0,
"mask" => "0x000000000008fff8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__19_19_x_15_9_x_0_0_" => {
"bits" => [
[
19,
19,
],
[
15,
9,
],
[
0,
0,
],
],
"width" => 9,
"size" => 9,
"shift" => 0,
"mask" => "0x000000000008fe01",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_24_x_20_16_x_15_13_" => {
"bits" => [
[
35,
35,
],
[
31,
24,
],
[
20,
13,
],
],
"width" => 17,
"size" => 17,
"shift" => 0,
"mask" => "0x00000008ff1fe000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_24_x_20_19_x_15_13_" => {
"bits" => [
[
35,
35,
],
[
31,
24,
],
[
20,
19,
],
[
15,
13,
],
],
"width" => 14,
"size" => 14,
"shift" => 0,
"mask" => "0x00000008ff18e000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_26_x_20_16_x_15_13_" => {
"bits" => [
[
35,
35,
],
[
31,
26,
],
[
20,
13,
],
],
"width" => 15,
"size" => 15,
"shift" => 0,
"mask" => "0x00000008fc1fe000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_27_x_20_16_x_15_13_" => {
"bits" => [
[
35,
35,
],
[
31,
27,
],
[
20,
13,
],
],
"width" => 14,
"size" => 14,
"shift" => 0,
"mask" => "0x00000008f81fe000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_24_19_x_15_12_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
24,
19,
],
[
15,
12,
],
],
"width" => 15,
"size" => 15,
"shift" => 0,
"mask" => "0x00000008f1f8f000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_24_19_x_15_3_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
24,
19,
],
[
15,
3,
],
],
"width" => 24,
"size" => 24,
"shift" => 0,
"mask" => "0x00000008f1f8fff8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_24_24_x_20_19_x_15_13_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
24,
24,
],
[
20,
19,
],
[
15,
13,
],
],
"width" => 11,
"size" => 11,
"shift" => 0,
"mask" => "0x00000008f118e000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_26_24_x_20_19_x_15_13_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
26,
24,
],
[
20,
19,
],
[
15,
13,
],
],
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x00000008f718e000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
15,
10,
],
],
"width" => 11,
"size" => 11,
"shift" => 0,
"mask" => "0x00000008f000fc00",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
20,
19,
],
[
15,
13,
],
],
"width" => 10,
"size" => 10,
"shift" => 0,
"mask" => "0x00000008f018e000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_20_20_x_19_19_x_18_18_x_16_16_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
20,
18,
],
[
16,
13,
],
],
"width" => 12,
"size" => 12,
"shift" => 0,
"mask" => "0x00000008f01de000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
20,
13,
],
],
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x00000008f01fe000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
23,
23,
],
[
20,
19,
],
[
15,
13,
],
],
"width" => 11,
"size" => 11,
"shift" => 0,
"mask" => "0x00000008f098e000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_18_18_x_16_16_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
23,
23,
],
[
20,
18,
],
[
16,
13,
],
],
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x00000008f09de000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
23,
23,
],
[
20,
13,
],
],
"width" => 14,
"size" => 14,
"shift" => 0,
"mask" => "0x00000008f09fe000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
23,
19,
],
[
15,
13,
],
],
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x00000008f0f8e000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
24,
24,
],
[
20,
19,
],
[
15,
13,
],
],
"width" => 11,
"size" => 11,
"shift" => 0,
"mask" => "0x00000008f118e000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_23_23_x_19_19_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
24,
23,
],
[
19,
19,
],
[
15,
13,
],
],
"width" => 11,
"size" => 11,
"shift" => 0,
"mask" => "0x00000008f188e000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
24,
23,
],
[
20,
19,
],
[
15,
10,
],
],
"width" => 15,
"size" => 15,
"shift" => 0,
"mask" => "0x00000008f198fc00",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
24,
19,
],
[
15,
3,
],
],
"width" => 24,
"size" => 24,
"shift" => 0,
"mask" => "0x00000008f1f8fff8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
28,
],
[
26,
24,
],
[
20,
19,
],
[
15,
13,
],
],
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x00000008f718e000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
27,
],
[
20,
13,
],
],
"width" => 14,
"size" => 14,
"shift" => 0,
"mask" => "0x00000008f81fe000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
27,
],
[
23,
23,
],
[
20,
19,
],
[
15,
13,
],
],
"width" => 12,
"size" => 12,
"shift" => 0,
"mask" => "0x00000008f898e000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_23_23_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
27,
],
[
23,
23,
],
[
20,
13,
],
],
"width" => 15,
"size" => 15,
"shift" => 0,
"mask" => "0x00000008f89fe000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_24_24_x_23_23_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
27,
],
[
24,
23,
],
[
20,
13,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x00000008f99fe000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
24,
],
[
15,
13,
],
],
"width" => 12,
"size" => 12,
"shift" => 0,
"mask" => "0x00000008ff00e000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
24,
],
[
20,
20,
],
[
15,
13,
],
],
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x00000008ff10e000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => {
"bits" => [
[
35,
35,
],
[
31,
24,
],
[
20,
19,
],
[
15,
13,
],
],
"width" => 14,
"size" => 14,
"shift" => 0,
"mask" => "0x00000008ff18e000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => {
"bits" => [
[
35,
35,
],
[
31,
24,
],
[
20,
19,
],
[
15,
3,
],
],
"width" => 24,
"size" => 24,
"shift" => 0,
"mask" => "0x00000008ff18fff8",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_3_3_x_2_2_x_1_1_" => {
"bits" => [
[
35,
35,
],
[
31,
24,
],
[
20,
19,
],
[
15,
13,
],
[
3,
1,
],
],
"width" => 17,
"size" => 17,
"shift" => 0,
"mask" => "0x00000008ff18e00e",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
35,
35,
],
[
31,
22,
],
[
15,
0,
],
],
"width" => 27,
"size" => 27,
"shift" => 0,
"mask" => "0x00000008ffc0ffff",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
35,
35,
],
[
31,
19,
],
[
15,
0,
],
],
"width" => 30,
"size" => 30,
"shift" => 0,
"mask" => "0x00000008fff8ffff",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__51_51_x_47_40_x_34_32_x_31_29_x_15_14_" => {
"bits" => [
[
51,
51,
],
[
47,
40,
],
[
34,
29,
],
[
15,
14,
],
],
"width" => 17,
"size" => 17,
"shift" => 0,
"mask" => "0x0008ff07e000c000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__51_51_x_47_43_x_34_32_x_31_29_x_15_14_" => {
"bits" => [
[
51,
51,
],
[
47,
43,
],
[
34,
29,
],
[
15,
14,
],
],
"width" => 14,
"size" => 14,
"shift" => 0,
"mask" => "0x0008f807e000c000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__51_51_x_47_44_x_34_32_x_31_29_x_15_14_" => {
"bits" => [
[
51,
51,
],
[
47,
44,
],
[
34,
29,
],
[
15,
14,
],
],
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x0008f007e000c000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__51_51_x_47_44_x_34_34_x_32_32_x_31_29_x_15_14_" => {
"bits" => [
[
51,
51,
],
[
47,
44,
],
[
34,
34,
],
[
32,
29,
],
[
15,
14,
],
],
"width" => 12,
"size" => 12,
"shift" => 0,
"mask" => "0x0008f005e000c000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => {
"bits" => [
[
51,
51,
],
[
47,
44,
],
[
34,
29,
],
[
15,
14,
],
],
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x0008f007e000c000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => {
"bits" => [
[
51,
51,
],
[
47,
43,
],
[
34,
29,
],
[
15,
14,
],
],
"width" => 14,
"size" => 14,
"shift" => 0,
"mask" => "0x0008f807e000c000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => {
"bits" => [
[
51,
51,
],
[
47,
43,
],
[
40,
40,
],
[
34,
29,
],
[
15,
14,
],
],
"width" => 15,
"size" => 15,
"shift" => 0,
"mask" => "0x0008f907e000c000",
"type" => "imm",
},
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => {
"bits" => [
[
51,
51,
],
[
47,
40,
],
[
34,
29,
],
[
15,
14,
],
],
"width" => 17,
"size" => 17,
"shift" => 0,
"mask" => "0x0008ff07e000c000",
"type" => "imm",
},
"VarPrefixOpcode_imp_bits__15_12_x_8_5_" => {
"bits" => [
[
15,
12,
],
[
8,
5,
],
],
"prefix" => 1,
"width" => 8,
"size" => 8,
"shift" => 0,
"mask" => "0x000000000000f1e0",
"type" => "imm",
},
"VarPrefixOpcode_imp_bits__15_15_x_13_12_x_11_6_" => {
"bits" => [
[
15,
15,
],
[
13,
6,
],
],
"prefix" => 1,
"width" => 9,
"size" => 9,
"shift" => 0,
"mask" => "0x000000000000bfc0",
"type" => "imm",
},
"VarPrefixOpcode_imp_bits__31_28_x_24_24_x_15_13_" => {
"bits" => [
[
31,
28,
],
[
24,
24,
],
[
15,
13,
],
],
"prefix" => 1,
"width" => 8,
"size" => 8,
"shift" => 0,
"mask" => "0x00000000f100e000",
"type" => "imm",
},
"VlesSize" => {
"pseudo" => 1,
"prefix" => 1,
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"VlesSize_imp_bits__11_9_" => {
"bits" => [
[
11,
9,
],
],
"prefix" => 1,
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000000e00",
"type" => "imm",
"implements" => "VlesSize",
},
"VlesSize_imp_bits__27_25_" => {
"bits" => [
[
27,
25,
],
],
"prefix" => 1,
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x000000000e000000",
"type" => "imm",
"implements" => "VlesSize",
},
"a10" => {
"pseudo" => 1,
"width" => 10,
"size" => 10,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"a10_imp_bits__10_1_" => {
"bits" => [
[
10,
1,
],
],
"width" => 10,
"size" => 10,
"shift" => 0,
"mask" => "0x00000000000007fe",
"type" => "imm",
"implements" => "a10",
},
"a16" => {
"pseudo" => 1,
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
"signed" => 1,
},
"a16_imp_bits__0_0_x_23_21_x_12_1_" => {
"bits" => [
[
0,
0,
],
[
23,
21,
],
[
12,
1,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000e01fff",
"type" => "imm",
"signed" => 1,
"implements" => "a16",
},
"a16_imp_bits__18_18_x_17_17_x_16_16_x_0_0_x_23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_" => {
"bits" => [
[
18,
16,
],
[
0,
0,
],
[
23,
21,
],
[
12,
4,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000e71ff1",
"type" => "imm",
"signed" => 1,
"implements" => "a16",
},
"a16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
23,
21,
],
[
12,
0,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000e01fff",
"type" => "imm",
"signed" => 1,
"implements" => "a16",
},
"a16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => {
"bits" => [
[
39,
37,
],
[
28,
16,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x000000e01fff0000",
"type" => "imm",
"signed" => 1,
"implements" => "a16",
},
"a20" => {
"pseudo" => 1,
"width" => 20,
"size" => 20,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
"signed" => 1,
},
"a20_imp_bits__27_27_x_18_16_x_0_0_x_23_21_x_12_1_" => {
"bits" => [
[
27,
27,
],
[
18,
16,
],
[
0,
0,
],
[
23,
21,
],
[
12,
1,
],
],
"width" => 20,
"size" => 20,
"shift" => 0,
"mask" => "0x0000000008e71fff",
"type" => "imm",
"signed" => 1,
"implements" => "a20",
},
"a20_imp_bits__27_27_x_18_18_x_17_17_x_16_16_x_0_0_x_23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_" => {
"bits" => [
[
27,
27,
],
[
18,
16,
],
[
0,
0,
],
[
23,
21,
],
[
12,
1,
],
],
"width" => 20,
"size" => 20,
"shift" => 0,
"mask" => "0x0000000008e71fff",
"type" => "imm",
"signed" => 1,
"implements" => "a20",
},
"a32" => {
"pseudo" => 1,
"width" => 32,
"size" => 32,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"a32_imp_bits__36_35_x_13_0_x_39_37_x_28_16_" => {
"bits" => [
[
36,
35,
],
[
13,
0,
],
[
39,
37,
],
[
28,
16,
],
],
"width" => 32,
"size" => 32,
"shift" => 0,
"mask" => "0x000000f81fff3fff",
"type" => "imm",
"implements" => "a32",
},
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => {
"bits" => [
[
36,
35,
],
[
13,
0,
],
[
39,
37,
],
[
28,
16,
],
],
"width" => 32,
"size" => 32,
"shift" => 0,
"mask" => "0x000000f81fff3fff",
"type" => "imm",
"implements" => "a32",
},
"a8" => {
"pseudo" => 1,
"width" => 8,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"a8_imp_bits__8_1_" => {
"bits" => [
[
8,
1,
],
],
"width" => 8,
"size" => 8,
"shift" => 0,
"mask" => "0x00000000000001fe",
"type" => "imm",
"implements" => "a8",
},
"a8_imp_bits__8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_" => {
"bits" => [
[
8,
1,
],
],
"width" => 8,
"size" => 8,
"shift" => 0,
"mask" => "0x00000000000001fe",
"type" => "imm",
"implements" => "a8",
},
"ccc" => {
"pseudo" => 1,
"prefix" => 1,
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"ccc_imp_bits__18_16_" => {
"bits" => [
[
18,
16,
],
],
"prefix" => 1,
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000070000",
"type" => "imm",
"implements" => "ccc",
},
"ccc_imp_bits__2_0_" => {
"bits" => [
[
2,
0,
],
],
"prefix" => 1,
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000000007",
"type" => "imm",
"implements" => "ccc",
},
"dddd" => {
"syntax" => "DaX,DaY",
"pseudo" => 1,
"width" => 5,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "DaXDaYTable",
"fields" => [
[
4,
4,
],
[
1,
1,
],
{
"dd" => 1,
},
[
4,
4,
],
[
0,
0,
],
{
"dd" => 0,
},
],
"instrfields" => {
"DaX" => {
"size" => 4,
"fields" => [
[
7,
4,
],
],
},
"DaY" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"dd" => {
"size" => 2,
"bits" => [
[
3,
2,
],
],
"table" => {
"0" => [
0,
0,
],
"1" => [
1,
1,
],
"2" => [
2,
2,
],
"3" => [
3,
3,
],
},
},
},
},
"dddd_imp_bits__17_17_x_6_6_x_5_5_x_4_4_x_3_3_" => {
"syntax" => "DaX,DaY",
"bits" => [
[
17,
17,
],
[
6,
3,
],
],
"width" => 5,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000020078",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "dddd",
"ref" => "DaXDaYTable",
"fields" => [
[
4,
4,
],
[
1,
1,
],
{
"dd" => 1,
},
[
4,
4,
],
[
0,
0,
],
{
"dd" => 0,
},
],
"instrfields" => {
"DaX" => {
"size" => 4,
"fields" => [
[
7,
4,
],
],
},
"DaY" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"dd" => {
"size" => 2,
"bits" => [
[
3,
2,
],
],
"table" => {
"0" => [
0,
0,
],
"1" => [
1,
1,
],
"2" => [
2,
2,
],
"3" => [
3,
3,
],
},
},
},
},
"e_u2" => {
"pseudo" => 1,
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"e_u2_imp_bits__1_1_x_0_0_" => {
"bits" => [
[
1,
0,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000000003",
"type" => "imm",
"implements" => "e_u2",
},
"ee" => {
"syntax" => "Dc,Dd",
"pseudo" => 1,
"width" => 4,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
3,
3,
],
{
"DcDd" => 1,
},
[
2,
2,
],
{
"DcDd" => 0,
},
],
"instrfields" => {
"Dc" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DcDd" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
7,
],
},
},
"Dd" => {
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
"ee_imp_bits__16_16_x_18_18_x_9_9_x_8_8_" => {
"syntax" => "Dc,Dd",
"bits" => [
[
16,
16,
],
[
18,
18,
],
[
9,
8,
],
],
"width" => 4,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000050300",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "ee",
"ref" => "D",
"fields" => [
[
3,
3,
],
{
"DcDd" => 1,
},
[
2,
2,
],
{
"DcDd" => 0,
},
],
"instrfields" => {
"Dc" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DcDd" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
7,
],
},
},
"Dd" => {
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
"ee_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => {
"syntax" => "Dc,Dd",
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
"shift" => 0,
"mask" => "0x0000000000060003",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "ee",
"ref" => "D",
"fields" => [
[
3,
3,
],
{
"DcDd" => 1,
},
[
2,
2,
],
{
"DcDd" => 0,
},
],
"instrfields" => {
"Dc" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"DcDd" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
7,
],
},
},
"Dd" => {
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
"eeeee" => {
"pseudo" => 1,
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "DoTable",
},
"eeeee_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_4_4_x_1_1_" => {
"bits" => [
[
16,
16,
],
[
11,
9,
],
[
4,
4,
],
[
1,
1,
],
],
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x0000000000010e12",
"type" => "regfile",
"implements" => "eeeee",
"ref" => "DoTable",
},
"ff" => {
"syntax" => "Da.e:Db.e",
"pseudo" => 1,
"width" => 3,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"Dae_Dbe" => 1,
},
[
2,
2,
],
{
"Dae_Dbe" => 0,
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
"Dae_Dbe" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
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
"ff_imp_bits__32_32_x_26_26_x_25_25_" => {
"syntax" => "Da.e:Db.e",
"bits" => [
[
32,
32,
],
[
26,
25,
],
],
"width" => 3,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000106000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "ff",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"Dae_Dbe" => 1,
},
[
2,
2,
],
{
"Dae_Dbe" => 0,
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
"Dae_Dbe" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
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
"ff_imp_bits__48_48_x_42_42_x_41_41_" => {
"syntax" => "Da.e:Db.e",
"bits" => [
[
48,
48,
],
[
42,
41,
],
],
"width" => 3,
"size" => 8,
"shift" => 0,
"mask" => "0x0001060000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "ff",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"Dae_Dbe" => 1,
},
[
2,
2,
],
{
"Dae_Dbe" => 0,
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
"Dae_Dbe" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
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
"gg" => {
"syntax" => "Da:Dc",
"pseudo" => 1,
"width" => 3,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"Da_Dc" => 1,
},
[
2,
2,
],
{
"Da_Dc" => 0,
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
"Da_Dc" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
2,
],
"1" => [
1,
3,
],
"2" => [
4,
6,
],
"3" => [
5,
7,
],
},
},
"Dc" => {
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
"gg_imp_bits__16_16_x_10_10_x_9_9_" => {
"syntax" => "Da:Dc",
"bits" => [
[
16,
16,
],
[
10,
9,
],
],
"width" => 3,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000010600",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "gg",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"Da_Dc" => 1,
},
[
2,
2,
],
{
"Da_Dc" => 0,
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
"Da_Dc" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
2,
],
"1" => [
1,
3,
],
"2" => [
4,
6,
],
"3" => [
5,
7,
],
},
},
"Dc" => {
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
"hh" => {
"syntax" => "Da:Db",
"pseudo" => 1,
"width" => 3,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"Da_Db" => 1,
},
[
2,
2,
],
{
"Da_Db" => 0,
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
"Da_Db" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
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
"hh_imp_bits__16_16_x_10_10_x_9_9_" => {
"syntax" => "Da:Db",
"bits" => [
[
16,
16,
],
[
10,
9,
],
],
"width" => 3,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000010600",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "hh",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"Da_Db" => 1,
},
[
2,
2,
],
{
"Da_Db" => 0,
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
"Da_Db" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
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
"hh_imp_bits__16_16_x_4_4_x_2_2_" => {
"syntax" => "Da:Db",
"bits" => [
[
16,
16,
],
[
4,
4,
],
[
2,
2,
],
],
"width" => 3,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000010014",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "hh",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"Da_Db" => 1,
},
[
2,
2,
],
{
"Da_Db" => 0,
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
"Da_Db" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
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
"hh_imp_bits__16_16_x_7_7_x_6_6_" => {
"syntax" => "Da:Db",
"bits" => [
[
16,
16,
],
[
7,
6,
],
],
"width" => 3,
"size" => 8,
"shift" => 0,
"mask" => "0x00000000000100c0",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "hh",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"Da_Db" => 1,
},
[
2,
2,
],
{
"Da_Db" => 0,
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
"Da_Db" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
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
"highbitvit" => {
"pseudo" => 1,
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "D",
},
"highbitvit_imp_bits__18_18_x_16_16_" => {
"bits" => [
[
18,
18,
],
[
16,
16,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000050000",
"type" => "regfile",
"implements" => "highbitvit",
"ref" => "D",
},
"highbitvsl" => {
"pseudo" => 1,
"width" => 1,
"size" => 1,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "D",
},
"highbitvsl_imp_bits__16_16_" => {
"bits" => [
[
16,
16,
],
],
"width" => 1,
"size" => 1,
"shift" => 0,
"mask" => "0x0000000000010000",
"type" => "regfile",
"implements" => "highbitvsl",
"ref" => "D",
},
"jj" => {
"syntax" => "Da,Db",
"pseudo" => 1,
"width" => 4,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
3,
3,
],
{
"DaDa" => 1,
},
[
2,
2,
],
{
"DaDa" => 0,
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
"jj_DaH_DbL" => {
"syntax" => "Da.h,Db.l",
"alias" => "jj",
},
"jj_DaL_DbH" => {
"syntax" => "Da.l,Db.h",
"alias" => "jj",
},
"jj_DaL_DbL" => {
"syntax" => "Da.l,Db.l",
"alias" => "jj",
},
"jj_imp_bits__18_17_x_1_0_" => {
"syntax" => "Da,Db",
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
"shift" => 0,
"mask" => "0x0000000000060003",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "jj",
"ref" => "D",
"fields" => [
[
3,
3,
],
{
"DaDa" => 1,
},
[
2,
2,
],
{
"DaDa" => 0,
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
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => {
"syntax" => "Da,Db",
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
"shift" => 0,
"mask" => "0x0000000000060003",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "jj",
"ref" => "D",
"fields" => [
[
3,
3,
],
{
"DaDa" => 1,
},
[
2,
2,
],
{
"DaDa" => 0,
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
"jjj" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "D",
},
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => {
"bits" => [
[
16,
16,
],
[
10,
8,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000010700",
"type" => "regfile",
"implements" => "jjj",
"ref" => "D",
},
"jjj_imp_bits__16_16_x_2_0_" => {
"bits" => [
[
16,
16,
],
[
2,
0,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000010007",
"type" => "regfile",
"implements" => "jjj",
"ref" => "D",
},
"jjj_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => {
"bits" => [
[
18,
18,
],
[
5,
3,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000040038",
"type" => "regfile",
"implements" => "jjj",
"ref" => "D",
},
"jjj_imp_bits__18_18_x_7_7_x_6_6_x_5_5_" => {
"bits" => [
[
18,
18,
],
[
7,
5,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x00000000000400e0",
"type" => "regfile",
"implements" => "jjj",
"ref" => "D",
},
"jjj_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => {
"bits" => [
[
32,
32,
],
[
18,
16,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000100070000",
"type" => "regfile",
"implements" => "jjj",
"ref" => "D",
},
"jjj_imp_bits__32_32_x_26_26_x_25_25_x_24_24_" => {
"bits" => [
[
32,
32,
],
[
26,
24,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000107000000",
"type" => "regfile",
"implements" => "jjj",
"ref" => "D",
},
"jjj_imp_bits__33_33_x_23_23_x_22_22_x_21_21_" => {
"bits" => [
[
33,
33,
],
[
23,
21,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000200e00000",
"type" => "regfile",
"implements" => "jjj",
"ref" => "D",
},
"jjj_imp_bits__34_34_x_23_23_x_22_22_x_21_21_" => {
"bits" => [
[
34,
34,
],
[
23,
21,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000400e00000",
"type" => "regfile",
"implements" => "jjj",
"ref" => "D",
},
"jjj_imp_bits__34_34_x_27_25_" => {
"bits" => [
[
34,
34,
],
[
27,
25,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x000000040e000000",
"type" => "regfile",
"implements" => "jjj",
"ref" => "D",
},
"jjj_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => {
"bits" => [
[
34,
34,
],
[
27,
25,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x000000040e000000",
"type" => "regfile",
"implements" => "jjj",
"ref" => "D",
},
"jjj_imp_bits__48_48_x_42_42_x_41_41_x_40_40_" => {
"bits" => [
[
48,
48,
],
[
42,
40,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0001070000000000",
"type" => "regfile",
"implements" => "jjj",
"ref" => "D",
},
"k" => {
"syntax" => "Da:Db:Dc:Dd",
"pseudo" => 1,
"width" => 2,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
1,
1,
],
{
"Da_Db_Dc_Dd" => 3,
},
[
1,
1,
],
{
"Da_Db_Dc_Dd" => 2,
},
[
1,
1,
],
{
"Da_Db_Dc_Dd" => 1,
},
[
1,
1,
],
{
"Da_Db_Dc_Dd" => 0,
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
"Da_Db_Dc_Dd" => {
"size" => 3,
"bits" => [
[
0,
0,
],
],
"table" => {
"0" => [
0,
1,
2,
3,
],
"1" => [
4,
5,
6,
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
"Dc" => {
"size" => 4,
"fields" => [
[
11,
8,
],
],
},
"Dd" => {
"size" => 4,
"fields" => [
[
15,
12,
],
],
},
},
},
"k_imp_bits__16_16_x_10_10_" => {
"syntax" => "Da:Db:Dc:Dd",
"bits" => [
[
16,
16,
],
[
10,
10,
],
],
"width" => 2,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000010400",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "k",
"ref" => "D",
"fields" => [
[
1,
1,
],
{
"Da_Db_Dc_Dd" => 3,
},
[
1,
1,
],
{
"Da_Db_Dc_Dd" => 2,
},
[
1,
1,
],
{
"Da_Db_Dc_Dd" => 1,
},
[
1,
1,
],
{
"Da_Db_Dc_Dd" => 0,
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
"Da_Db_Dc_Dd" => {
"size" => 3,
"bits" => [
[
0,
0,
],
],
"table" => {
"0" => [
0,
1,
2,
3,
],
"1" => [
4,
5,
6,
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
"Dc" => {
"size" => 4,
"fields" => [
[
11,
8,
],
],
},
"Dd" => {
"size" => 4,
"fields" => [
[
15,
12,
],
],
},
},
},
"kk" => {
"syntax" => "Dc:Dd",
"pseudo" => 1,
"width" => 3,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"Dc_Dd" => 1,
},
[
2,
2,
],
{
"Dc_Dd" => 0,
},
],
"instrfields" => {
"Dc" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"Dc_Dd" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
7,
],
},
},
"Dd" => {
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
"kk_imp_bits__18_18_x_1_1_x_0_0_" => {
"syntax" => "Dc:Dd",
"bits" => [
[
18,
18,
],
[
1,
0,
],
],
"width" => 3,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000040003",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "kk",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"Dc_Dd" => 1,
},
[
2,
2,
],
{
"Dc_Dd" => 0,
},
],
"instrfields" => {
"Dc" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"Dc_Dd" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
7,
],
},
},
"Dd" => {
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
"kkk" => {
"syntax" => "Dj:Dk",
"pseudo" => 1,
"width" => 4,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
3,
3,
],
{
"Dj_Dk" => 1,
},
[
3,
3,
],
{
"Dj_Dk" => 0,
},
],
"instrfields" => {
"Dj" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"Dj_Dk" => {
"size" => 3,
"bits" => [
[
2,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
7,
],
"4" => [
1,
0,
],
"5" => [
3,
2,
],
"6" => [
5,
4,
],
"7" => [
7,
6,
],
},
},
"Dk" => {
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
"kkk_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => {
"syntax" => "Dj:Dk",
"bits" => [
[
18,
18,
],
[
5,
3,
],
],
"width" => 4,
"size" => 8,
"shift" => 0,
"mask" => "0x0000000000040038",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "kkk",
"ref" => "D",
"fields" => [
[
3,
3,
],
{
"Dj_Dk" => 1,
},
[
3,
3,
],
{
"Dj_Dk" => 0,
},
],
"instrfields" => {
"Dj" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"Dj_Dk" => {
"size" => 3,
"bits" => [
[
2,
0,
],
],
"table" => {
"0" => [
0,
1,
],
"1" => [
2,
3,
],
"2" => [
4,
5,
],
"3" => [
6,
7,
],
"4" => [
1,
0,
],
"5" => [
3,
2,
],
"6" => [
5,
4,
],
"7" => [
7,
6,
],
},
},
"Dk" => {
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
"lpmarkA" => {
"pseudo" => 1,
"prefix" => 1,
"width" => 1,
"size" => 1,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
"value" => 0,
},
"lpmarkA_imp_bits__19_19_" => {
"bits" => [
[
19,
19,
],
],
"prefix" => 1,
"width" => 1,
"size" => 1,
"shift" => 0,
"mask" => "0x0000000000080000",
"type" => "imm",
"value" => 0,
"implements" => "lpmarkA",
},
"lpmarkA_imp_bits__3_3_" => {
"bits" => [
[
3,
3,
],
],
"prefix" => 1,
"width" => 1,
"size" => 1,
"shift" => 0,
"mask" => "0x0000000000000008",
"type" => "imm",
"value" => 0,
"implements" => "lpmarkA",
},
"lpmarkB" => {
"pseudo" => 1,
"prefix" => 1,
"width" => 1,
"size" => 1,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
"value" => 0,
},
"lpmarkB_imp_bits__20_20_" => {
"bits" => [
[
20,
20,
],
],
"prefix" => 1,
"width" => 1,
"size" => 1,
"shift" => 0,
"mask" => "0x0000000000100000",
"type" => "imm",
"value" => 0,
"implements" => "lpmarkB",
},
"lpmarkB_imp_bits__4_4_" => {
"bits" => [
[
4,
4,
],
],
"prefix" => 1,
"width" => 1,
"size" => 1,
"shift" => 0,
"mask" => "0x0000000000000010",
"type" => "imm",
"value" => 0,
"implements" => "lpmarkB",
},
"mm" => {
"pseudo" => 1,
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000000000",
"enumerated" => {
"0" => ")+n0",
"1" => ")+n1",
"2" => ")+n2",
"3" => ")+n3",
},
"type" => "imm",
},
"mm_imp_bits__4_4_x_3_3_" => {
"bits" => [
[
4,
3,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000000018",
"enumerated" => {
"0" => ")+n0",
"1" => ")+n1",
"2" => ")+n2",
"3" => ")+n3",
},
"type" => "imm",
"implements" => "mm",
},
"nn" => {
"pseudo" => 1,
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"nn_imp_bits__25_24_" => {
"bits" => [
[
25,
24,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000003000000",
"type" => "imm",
"implements" => "nn",
},
"nn_imp_bits__9_8_" => {
"bits" => [
[
9,
8,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000000300",
"type" => "imm",
"implements" => "nn",
},
"qq" => {
"syntax" => "Do.e",
"pseudo" => 1,
"width" => 3,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"Do_temp" => 0,
},
],
"instrfields" => {
"Do" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"Do_temp" => {
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
],
"1" => [
3,
],
"2" => [
5,
],
"3" => [
7,
],
},
},
},
},
"qq_imp_bits__32_32_x_26_26_x_25_25_" => {
"syntax" => "Do.e",
"bits" => [
[
32,
32,
],
[
26,
25,
],
],
"width" => 3,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000106000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "qq",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"Do_temp" => 0,
},
],
"instrfields" => {
"Do" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"Do_temp" => {
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
],
"1" => [
3,
],
"2" => [
5,
],
"3" => [
7,
],
},
},
},
},
"qq_imp_bits__48_48_x_42_42_x_41_41_" => {
"syntax" => "Do.e",
"bits" => [
[
48,
48,
],
[
42,
41,
],
],
"width" => 3,
"size" => 4,
"shift" => 0,
"mask" => "0x0001060000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "qq",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"Do_temp" => 0,
},
],
"instrfields" => {
"Do" => {
"size" => 4,
"fields" => [
[
3,
0,
],
],
},
"Do_temp" => {
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
],
"1" => [
3,
],
"2" => [
5,
],
"3" => [
7,
],
},
},
},
},
"qqq" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "R",
},
"qqq_imp_bits__16_16_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
16,
16,
],
[
2,
0,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000010007",
"type" => "regfile",
"implements" => "qqq",
"ref" => "R",
},
"reserved_imp_bits__14_14_" => {
"bits" => [
[
14,
14,
],
],
"width" => 1,
"size" => 1,
"shift" => 0,
"mask" => "0x0000000000004000",
"type" => "imm",
},
"reserved_imp_bits__17_16_" => {
"bits" => [
[
17,
16,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000030000",
"type" => "imm",
},
"reserved_imp_bits__17_17_" => {
"bits" => [
[
17,
17,
],
],
"width" => 1,
"size" => 1,
"shift" => 0,
"mask" => "0x0000000000020000",
"type" => "imm",
},
"reserved_imp_bits__17_17_x_14_14_" => {
"bits" => [
[
17,
17,
],
[
14,
14,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000024000",
"type" => "imm",
},
"reserved_imp_bits__17_17_x_16_16_" => {
"bits" => [
[
17,
16,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000030000",
"type" => "imm",
},
"reserved_imp_bits__17_17_x_16_16_x_14_14_" => {
"bits" => [
[
17,
16,
],
[
14,
14,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000034000",
"type" => "imm",
},
"reserved_imp_bits__18_16_" => {
"bits" => [
[
18,
16,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000070000",
"type" => "imm",
},
"reserved_imp_bits__18_17_" => {
"bits" => [
[
18,
17,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000060000",
"type" => "imm",
},
"reserved_imp_bits__18_17_x_14_14_" => {
"bits" => [
[
18,
17,
],
[
14,
14,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000064000",
"type" => "imm",
},
"reserved_imp_bits__18_18_x_17_17_" => {
"bits" => [
[
18,
17,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000000060000",
"type" => "imm",
},
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => {
"bits" => [
[
18,
17,
],
[
14,
14,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000064000",
"type" => "imm",
},
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => {
"bits" => [
[
18,
16,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000070000",
"type" => "imm",
},
"reserved_imp_bits__33_32_" => {
"bits" => [
[
33,
32,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000300000000",
"type" => "imm",
},
"reserved_imp_bits__33_33_" => {
"bits" => [
[
33,
33,
],
],
"width" => 1,
"size" => 1,
"shift" => 0,
"mask" => "0x0000000200000000",
"type" => "imm",
},
"reserved_imp_bits__33_33_x_32_32_" => {
"bits" => [
[
33,
32,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000300000000",
"type" => "imm",
},
"reserved_imp_bits__34_32_" => {
"bits" => [
[
34,
32,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000700000000",
"type" => "imm",
},
"reserved_imp_bits__34_33_" => {
"bits" => [
[
34,
33,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000600000000",
"type" => "imm",
},
"reserved_imp_bits__34_34_x_33_33_" => {
"bits" => [
[
34,
33,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0000000600000000",
"type" => "imm",
},
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => {
"bits" => [
[
34,
32,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000700000000",
"type" => "imm",
},
"reserved_imp_bits__50_48_" => {
"bits" => [
[
50,
48,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0007000000000000",
"type" => "imm",
},
"reserved_imp_bits__50_49_" => {
"bits" => [
[
50,
49,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0006000000000000",
"type" => "imm",
},
"reserved_imp_bits__50_50_x_49_49_" => {
"bits" => [
[
50,
49,
],
],
"width" => 2,
"size" => 2,
"shift" => 0,
"mask" => "0x0006000000000000",
"type" => "imm",
},
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => {
"bits" => [
[
50,
48,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0007000000000000",
"type" => "imm",
},
"rrr" => {
"pseudo" => 1,
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "R",
},
"rrr_imp_bits__2_2_x_1_1_x_0_0_" => {
"bits" => [
[
2,
0,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000000007",
"type" => "regfile",
"implements" => "rrr",
"ref" => "R",
},
"rrrr" => {
"pseudo" => 1,
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "rxTable",
},
"rrrr_imp_bits__16_16_x_3_0_" => {
"bits" => [
[
16,
16,
],
[
3,
0,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x000000000001000f",
"type" => "regfile",
"implements" => "rrrr",
"ref" => "rxTable",
},
"rrrr_imp_bits__16_16_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
16,
16,
],
[
3,
0,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x000000000001000f",
"type" => "regfile",
"implements" => "rrrr",
"ref" => "rxTable",
},
"rrrr_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => {
"bits" => [
[
32,
32,
],
[
27,
24,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x000000010f000000",
"type" => "regfile",
"implements" => "rrrr",
"ref" => "rxTable",
},
"s13" => {
"pseudo" => 1,
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
"signed" => 1,
},
"s13_imp_bits__12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
12,
0,
],
],
"width" => 13,
"size" => 13,
"shift" => 0,
"mask" => "0x0000000000001fff",
"type" => "imm",
"signed" => 1,
"implements" => "s13",
},
"s15" => {
"pseudo" => 1,
"width" => 15,
"size" => 15,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
"signed" => 1,
},
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
22,
21,
],
[
12,
0,
],
],
"width" => 15,
"size" => 15,
"shift" => 0,
"mask" => "0x0000000000601fff",
"type" => "imm",
"signed" => 1,
"implements" => "s15",
},
"s16" => {
"pseudo" => 1,
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
"signed" => 1,
},
"s16_imp_bits__20_20_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
20,
20,
],
[
22,
21,
],
[
12,
0,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000701fff",
"type" => "imm",
"signed" => 1,
"implements" => "s16",
},
"s16_imp_bits__23_21_x_12_0_" => {
"bits" => [
[
23,
21,
],
[
12,
0,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000e01fff",
"type" => "imm",
"signed" => 1,
"implements" => "s16",
},
"s16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
23,
21,
],
[
12,
0,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000e01fff",
"type" => "imm",
"signed" => 1,
"implements" => "s16",
},
"s16_imp_bits__24_24_x_23_23_x_20_20_x_19_19_x_22_22_x_21_21_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
24,
23,
],
[
20,
19,
],
[
22,
21,
],
[
9,
0,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000001f803ff",
"type" => "imm",
"signed" => 1,
"implements" => "s16",
},
"s16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
36,
35,
],
[
13,
0,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000001800003fff",
"type" => "imm",
"signed" => 1,
"implements" => "s16",
},
"s16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => {
"bits" => [
[
39,
37,
],
[
28,
16,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x000000e01fff0000",
"type" => "imm",
"signed" => 1,
"implements" => "s16",
},
"s32" => {
"pseudo" => 1,
"width" => 32,
"size" => 32,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
"signed" => 1,
},
"s32_imp_bits__36_35_x_13_0_x_39_37_x_28_16_" => {
"bits" => [
[
36,
35,
],
[
13,
0,
],
[
39,
37,
],
[
28,
16,
],
],
"width" => 32,
"size" => 32,
"shift" => 0,
"mask" => "0x000000f81fff3fff",
"type" => "imm",
"signed" => 1,
"implements" => "s32",
},
"s7" => {
"pseudo" => 1,
"width" => 7,
"size" => 7,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
"signed" => 1,
},
"s7_imp_bits__6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
6,
0,
],
],
"width" => 7,
"size" => 7,
"shift" => 0,
"mask" => "0x000000000000007f",
"type" => "imm",
"signed" => 1,
"implements" => "s7",
},
"sa16" => {
"pseudo" => 1,
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
"signed" => 1,
},
"sa16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => {
"bits" => [
[
39,
37,
],
[
28,
16,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x000000e01fff0000",
"type" => "imm",
"signed" => 1,
"implements" => "sa16",
},
"u12" => {
"pseudo" => 1,
"width" => 12,
"size" => 12,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"u12_imp_bits__22_22_x_21_21_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
22,
21,
],
[
9,
0,
],
],
"width" => 12,
"size" => 12,
"shift" => 0,
"mask" => "0x00000000006003ff",
"type" => "imm",
"implements" => "u12",
},
"u16" => {
"pseudo" => 1,
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"u16_imp_bits__23_21_x_12_0_" => {
"bits" => [
[
23,
21,
],
[
12,
0,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000e01fff",
"type" => "imm",
"implements" => "u16",
},
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
23,
21,
],
[
12,
0,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000000000e01fff",
"type" => "imm",
"implements" => "u16",
},
"u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
36,
35,
],
[
13,
0,
],
],
"width" => 16,
"size" => 16,
"shift" => 0,
"mask" => "0x0000001800003fff",
"type" => "imm",
"implements" => "u16",
},
"u3" => {
"pseudo" => 1,
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"u32" => {
"pseudo" => 1,
"width" => 32,
"size" => 32,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"u32_imp_bits__36_35_x_13_0_x_39_37_x_28_16_" => {
"bits" => [
[
36,
35,
],
[
13,
0,
],
[
39,
37,
],
[
28,
16,
],
],
"width" => 32,
"size" => 32,
"shift" => 0,
"mask" => "0x000000f81fff3fff",
"type" => "imm",
"implements" => "u32",
},
"u32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => {
"bits" => [
[
36,
35,
],
[
13,
0,
],
[
39,
37,
],
[
28,
16,
],
],
"width" => 32,
"size" => 32,
"shift" => 0,
"mask" => "0x000000f81fff3fff",
"type" => "imm",
"implements" => "u32",
},
"u3_imp_bits__2_2_x_1_1_x_0_0_" => {
"bits" => [
[
2,
0,
],
],
"width" => 3,
"size" => 3,
"shift" => 0,
"mask" => "0x0000000000000007",
"type" => "imm",
"implements" => "u3",
},
"u4" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"u4_imp_bits__3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
3,
0,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x000000000000000f",
"type" => "imm",
"implements" => "u4",
},
"u5" => {
"pseudo" => 1,
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"u5_imp_bits__20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => {
"bits" => [
[
20,
16,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x00000000001f0000",
"type" => "imm",
"implements" => "u5",
},
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
4,
0,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x000000000000001f",
"type" => "imm",
"implements" => "u5",
},
"u5_imp_bits__8_8_x_3_0_" => {
"bits" => [
[
8,
8,
],
[
3,
0,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x000000000000010f",
"type" => "imm",
"implements" => "u5",
},
"u5_imp_bits__8_8_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
8,
8,
],
[
3,
0,
],
],
"width" => 5,
"size" => 5,
"shift" => 0,
"mask" => "0x000000000000010f",
"type" => "imm",
"implements" => "u5",
},
"u6" => {
"pseudo" => 1,
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "imm",
},
"u6_imp_bits__5_0_" => {
"bits" => [
[
5,
0,
],
],
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x000000000000003f",
"type" => "imm",
"implements" => "u6",
},
"u6_imp_bits__5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
5,
0,
],
],
"width" => 6,
"size" => 6,
"shift" => 0,
"mask" => "0x000000000000003f",
"type" => "imm",
"implements" => "u6",
},
"uuu" => {
"pseudo" => 1,
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"type" => "regfile",
"ref" => "D",
},
"uuu_imp_bits__16_16_x_2_2_x_1_1_x_0_0_" => {
"bits" => [
[
16,
16,
],
[
2,
0,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000010007",
"type" => "regfile",
"implements" => "uuu",
"ref" => "D",
},
"uuu_imp_bits__18_18_x_2_0_" => {
"bits" => [
[
18,
18,
],
[
2,
0,
],
],
"width" => 4,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000040007",
"type" => "regfile",
"implements" => "uuu",
"ref" => "D",
},
"ww" => {
"pseudo" => 1,
"width" => 3,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000000000",
"blocks" => [
"dalu",
],
"type" => "regfile",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"dd" => 0,
},
],
"instrfields" => {
"dd" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
],
"1" => [
1,
],
"2" => [
2,
],
"3" => [
3,
],
"4" => [
4,
],
},
},
},
},
"ww_imp_bits__17_17_x_1_0_" => {
"bits" => [
[
17,
17,
],
[
1,
0,
],
],
"width" => 3,
"size" => 4,
"shift" => 0,
"mask" => "0x0000000000020003",
"blocks" => [
"dalu",
],
"type" => "regfile",
"implements" => "ww",
"ref" => "D",
"fields" => [
[
2,
2,
],
{
"dd" => 0,
},
],
"instrfields" => {
"dd" => {
"size" => 3,
"bits" => [
[
1,
0,
],
],
"table" => {
"0" => [
0,
],
"1" => [
1,
],
"2" => [
2,
],
"3" => [
3,
],
"4" => [
4,
],
},
},
},
},
"yyyyyy" => {
"pseudo" => 1,
"prefix" => 1,
"width" => 8,
"size" => 1,
"shift" => 0,
"indexed" => 1,
"mask" => "0x0000000000000000",
"type" => "regfield",
"value" => 0,
},
"yyyyyy_imp_bits__5_0_" => {
"bits" => [
[
5,
0,
],
],
"prefix" => 1,
"width" => 6,
"size" => 1,
"shift" => 0,
"indexed" => 1,
"mask" => "0x000000000000003f",
"type" => "regfield",
"value" => 0,
"implements" => "yyyyyy",
},
},
"instrs" => {
"ABS2_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "abs2 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1907,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) . H = ABSword ( D ( JJJ ) . H ) ;
    D ( FFF ) . L = ABSword ( D ( JJJ ) . L ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"ABSword",
],
},
"ABS_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "abs FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 1254,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > tmp ;
    tmp = signExtend ( D ( FFF ) , 41 ) ;
    if ( tmp ( 39 ) == 1 ) {
        tmp = ( ~ tmp  ) + 1 ;
    }
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"ACS2H_Da_X_Da_Y_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "acs2h dddd,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_" => 96,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"dddd_imp_bits__17_17_x_6_6_x_5_5_x_4_4_x_3_3_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 32 > tmp_btr0 = BTR0 ;
     bits < 32 > tmp_btr1 = BTR1 ;
    tmp_btr1 >>= 2 ;
     bits < 17 > sum1 = ( sbits < 17 >  ) D ( FFF ) . H + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sum2 = ( sbits < 17 >  ) D ( JJJ ) . H + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
    if ( sum1 . signedGT ( sum2 ) ) {
        tmp_btr1 ( 30 , 30 ) = 0 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sum1 , 40 ) ) ;
    } else {
        tmp_btr1 ( 30 , 30 ) = 1 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sum2 , 40 ) ) ;
    }
     bits < 17 > sub1 = ( sbits < 17 >  ) D ( FFF ) . H - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sub2 = ( sbits < 17 >  ) D ( JJJ ) . H - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
    if ( sub1 . signedGT ( sub2 ) ) {
        tmp_btr1 ( 31 , 31 ) = 0 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sub1 , 40 ) ) ;
    } else {
        tmp_btr1 ( 31 , 31 ) = 1 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sub2 , 40 ) ) ;
    }
    tmp_btr0 >>= 2 ;
    tmp_btr0 ( 31 ) = BTR1 ( 1 ) ;
    tmp_btr0 ( 30 ) = BTR1 ( 0 ) ;
    BTR0 = tmp_btr0 ;
    BTR1 = tmp_btr1 ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"BTR0",
"BTR1",
"D(FFF)[mask 0x00ffff0000]/p",
"D(JJJ)[mask 0x00ffff0000]/p",
"DaXDaYTable(dddd . DaX)",
"DaXDaYTable(dddd . DaY)",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"BTR0",
"BTR1",
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"saturation16_nochk",
"saturation16_noexception",
],
},
"ACS2H_Da_X_mDa_Y_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "acs2h DaX_DaYm,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_" => 100,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"dddd_imp_bits__17_17_x_6_6_x_5_5_x_4_4_x_3_3_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 32 > tmp_btr0 = BTR0 ;
     bits < 32 > tmp_btr1 = BTR1 ;
    tmp_btr1 >>= 2 ;
     bits < 17 > sum1 = ( sbits < 17 >  ) D ( FFF ) . H + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sum2 = ( sbits < 17 >  ) D ( JJJ ) . H + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
     bits < 17 > sub1 = ( sbits < 17 >  ) D ( FFF ) . H - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sub2 = ( sbits < 17 >  ) D ( JJJ ) . H - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
    if ( sub1 . signedGT ( sum2 ) ) {
        tmp_btr1 ( 30 , 30 ) = 0 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sub1 , 40 ) ) ;
    } else {
        tmp_btr1 ( 30 , 30 ) = 1 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sum2 , 40 ) ) ;
    }
    if ( sum1 . signedGT ( sub2 ) ) {
        tmp_btr1 ( 31 , 31 ) = 0 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sum1 , 40 ) ) ;
    } else {
        tmp_btr1 ( 31 , 31 ) = 1 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sub2 , 40 ) ) ;
    }
    tmp_btr0 >>= 2 ;
    tmp_btr0 ( 31 ) = BTR1 ( 1 ) ;
    tmp_btr0 ( 30 ) = BTR1 ( 0 ) ;
    BTR0 = tmp_btr0 ;
    BTR1 = tmp_btr1 ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"BTR0",
"BTR1",
"D(FFF)[mask 0x00ffff0000]/p",
"D(JJJ)[mask 0x00ffff0000]/p",
"DaXDaYTable(dddd . DaX)",
"DaXDaYTable(dddd . DaY)",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"BTR0",
"BTR1",
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"saturation16_nochk",
"saturation16_noexception",
],
},
"ACS2H_mDa_X_Da_Y_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "acs2h DaXm_DaY,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_" => 104,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"dddd_imp_bits__17_17_x_6_6_x_5_5_x_4_4_x_3_3_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 32 > tmp_btr0 = BTR0 ;
     bits < 32 > tmp_btr1 = BTR1 ;
    tmp_btr1 >>= 2 ;
     bits < 17 > sum1 = ( sbits < 17 >  ) D ( FFF ) . H + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sum2 = ( sbits < 17 >  ) D ( JJJ ) . H + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
     bits < 17 > sub1 = ( sbits < 17 >  ) D ( FFF ) . H - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sub2 = ( sbits < 17 >  ) D ( JJJ ) . H - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
    if ( sum1 . signedGT ( sub2 ) ) {
        tmp_btr1 ( 30 , 30 ) = 0 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sum1 , 40 ) ) ;
    } else {
        tmp_btr1 ( 30 , 30 ) = 1 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sub2 , 40 ) ) ;
    }
    if ( sub1 . signedGT ( sum2 ) ) {
        tmp_btr1 ( 31 , 31 ) = 0 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sub1 , 40 ) ) ;
    } else {
        tmp_btr1 ( 31 , 31 ) = 1 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sum2 , 40 ) ) ;
    }
    tmp_btr0 >>= 2 ;
    tmp_btr0 ( 31 ) = BTR1 ( 1 ) ;
    tmp_btr0 ( 30 ) = BTR1 ( 0 ) ;
    BTR0 = tmp_btr0 ;
    BTR1 = tmp_btr1 ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"BTR0",
"BTR1",
"D(FFF)[mask 0x00ffff0000]/p",
"D(JJJ)[mask 0x00ffff0000]/p",
"DaXDaYTable(dddd . DaX)",
"DaXDaYTable(dddd . DaY)",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"BTR0",
"BTR1",
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"saturation16_nochk",
"saturation16_noexception",
],
},
"ACS2H_mDa_X_mDa_Y_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "acs2h DaXm_DaYm,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_" => 108,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"dddd_imp_bits__17_17_x_6_6_x_5_5_x_4_4_x_3_3_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 32 > tmp_btr0 = BTR0 ;
     bits < 32 > tmp_btr1 = BTR1 ;
    tmp_btr1 >>= 2 ;
     bits < 17 > sum1 = ( sbits < 17 >  ) D ( FFF ) . H + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sum2 = ( sbits < 17 >  ) D ( JJJ ) . H + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
     bits < 17 > sub1 = ( sbits < 17 >  ) D ( FFF ) . H - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sub2 = ( sbits < 17 >  ) D ( JJJ ) . H - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
    if ( sub1 . signedGT ( sub2 ) ) {
        tmp_btr1 ( 30 , 30 ) = 0 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sub1 , 40 ) ) ;
    } else {
        tmp_btr1 ( 30 , 30 ) = 1 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sub2 , 40 ) ) ;
    }
    if ( sum1 . signedGT ( sum2 ) ) {
        tmp_btr1 ( 31 , 31 ) = 0 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sum1 , 40 ) ) ;
    } else {
        tmp_btr1 ( 31 , 31 ) = 1 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sum2 , 40 ) ) ;
    }
    tmp_btr0 >>= 2 ;
    tmp_btr0 ( 31 ) = BTR1 ( 1 ) ;
    tmp_btr0 ( 30 ) = BTR1 ( 0 ) ;
    BTR0 = tmp_btr0 ;
    BTR1 = tmp_btr1 ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"BTR0",
"BTR1",
"D(FFF)[mask 0x00ffff0000]/p",
"D(JJJ)[mask 0x00ffff0000]/p",
"DaXDaYTable(dddd . DaX)",
"DaXDaYTable(dddd . DaY)",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"BTR0",
"BTR1",
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"saturation16_nochk",
"saturation16_noexception",
],
},
"ACS2L_Da_X_Da_Y_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "acs2l DaX_DaY,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_" => 97,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"dddd_imp_bits__17_17_x_6_6_x_5_5_x_4_4_x_3_3_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 32 > tmp_btr0 = BTR0 ;
     bits < 32 > tmp_btr1 = BTR1 ;
    tmp_btr1 >>= 2 ;
     bits < 17 > sum1 = ( sbits < 17 >  ) D ( FFF ) . L + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sum2 = ( sbits < 17 >  ) D ( JJJ ) . L + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
     bits < 17 > sub1 = ( sbits < 17 >  ) D ( FFF ) . L - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sub2 = ( sbits < 17 >  ) D ( JJJ ) . L - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
    if ( sum2 . signedGT ( sum1 ) ) {
        tmp_btr1 ( 30 ) = 0 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sum2 , 40 ) ) ;
    } else {
        tmp_btr1 ( 30 ) = 1 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sum1 , 40 ) ) ;
    }
    if ( sub2 . signedGT ( sub1 ) ) {
        tmp_btr1 ( 31 ) = 0 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sub2 , 40 ) ) ;
    } else {
        tmp_btr1 ( 31 ) = 1 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sub1 , 40 ) ) ;
    }
    tmp_btr0 >>= 2 ;
    tmp_btr0 ( 31 ) = BTR1 ( 1 ) ;
    tmp_btr0 ( 30 ) = BTR1 ( 0 ) ;
    BTR0 = tmp_btr0 ;
    BTR1 = tmp_btr1 ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"BTR0",
"BTR1",
"D(FFF)[mask 0x000000ffff]/p",
"D(JJJ)[mask 0x000000ffff]/p",
"DaXDaYTable(dddd . DaX)",
"DaXDaYTable(dddd . DaY)",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"BTR0",
"BTR1",
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"saturation16_nochk",
"saturation16_noexception",
],
},
"ACS2L_Da_X_mDa_Y_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "acs2l DaX_DaYm,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_" => 101,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"dddd_imp_bits__17_17_x_6_6_x_5_5_x_4_4_x_3_3_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 32 > tmp_btr0 = BTR0 ;
     bits < 32 > tmp_btr1 = BTR1 ;
    tmp_btr1 >>= 2 ;
     bits < 17 > sum1 = ( sbits < 17 >  ) D ( FFF ) . L + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sum2 = ( sbits < 17 >  ) D ( JJJ ) . L + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
     bits < 17 > sub1 = ( sbits < 17 >  ) D ( FFF ) . L - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sub2 = ( sbits < 17 >  ) D ( JJJ ) . L - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
    if ( sum2 . signedGT ( sub1 ) ) {
        tmp_btr1 ( 30 , 30 ) = 0 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sum2 , 40 ) ) ;
    } else {
        tmp_btr1 ( 30 , 30 ) = 1 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sub1 , 40 ) ) ;
    }
    if ( sub2 . signedGT ( sum1 ) ) {
        tmp_btr1 ( 31 , 31 ) = 0 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sub2 , 40 ) ) ;
    } else {
        tmp_btr1 ( 31 , 31 ) = 1 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sum1 , 40 ) ) ;
    }
    tmp_btr0 >>= 2 ;
    tmp_btr0 ( 31 ) = BTR1 ( 1 ) ;
    tmp_btr0 ( 30 ) = BTR1 ( 0 ) ;
    BTR0 = tmp_btr0 ;
    BTR1 = tmp_btr1 ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"BTR0",
"BTR1",
"D(FFF)[mask 0x000000ffff]/p",
"D(JJJ)[mask 0x000000ffff]/p",
"DaXDaYTable(dddd . DaX)",
"DaXDaYTable(dddd . DaY)",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"BTR0",
"BTR1",
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"saturation16_nochk",
"saturation16_noexception",
],
},
"ACS2L_mDa_X_Da_Y_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "acs2l DaXm_DaY,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_" => 105,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"dddd_imp_bits__17_17_x_6_6_x_5_5_x_4_4_x_3_3_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 32 > tmp_btr0 = BTR0 ;
     bits < 32 > tmp_btr1 = BTR1 ;
    tmp_btr1 >>= 2 ;
     bits < 17 > sum1 = ( sbits < 17 >  ) D ( FFF ) . L + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sum2 = ( sbits < 17 >  ) D ( JJJ ) . L + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
     bits < 17 > sub1 = ( sbits < 17 >  ) D ( FFF ) . L - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sub2 = ( sbits < 17 >  ) D ( JJJ ) . L - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
    if ( sub2 . signedGT ( sum1 ) ) {
        tmp_btr1 ( 30 , 30 ) = 0 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sub2 , 40 ) ) ;
    } else {
        tmp_btr1 ( 30 , 30 ) = 1 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sum1 , 40 ) ) ;
    }
    if ( sum2 . signedGT ( sub1 ) ) {
        tmp_btr1 ( 31 , 31 ) = 0 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sum2 , 40 ) ) ;
    } else {
        tmp_btr1 ( 31 , 31 ) = 1 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sub1 , 40 ) ) ;
    }
    tmp_btr0 >>= 2 ;
    tmp_btr0 ( 31 ) = BTR1 ( 1 ) ;
    tmp_btr0 ( 30 ) = BTR1 ( 0 ) ;
    BTR0 = tmp_btr0 ;
    BTR1 = tmp_btr1 ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"BTR0",
"BTR1",
"D(FFF)[mask 0x000000ffff]/p",
"D(JJJ)[mask 0x000000ffff]/p",
"DaXDaYTable(dddd . DaX)",
"DaXDaYTable(dddd . DaY)",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"BTR0",
"BTR1",
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"saturation16_nochk",
"saturation16_noexception",
],
},
"ACS2L_mDa_X_mDa_Y_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "acs2l DaXm_DaYm,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_" => 109,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"dddd_imp_bits__17_17_x_6_6_x_5_5_x_4_4_x_3_3_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 32 > tmp_btr0 = BTR0 ;
     bits < 32 > tmp_btr1 = BTR1 ;
    tmp_btr1 >>= 2 ;
     bits < 17 > sum1 = ( sbits < 17 >  ) D ( FFF ) . L + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sum2 = ( sbits < 17 >  ) D ( JJJ ) . L + ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
     bits < 17 > sub1 = ( sbits < 17 >  ) D ( FFF ) . L - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaY ) ;
     bits < 17 > sub2 = ( sbits < 17 >  ) D ( JJJ ) . L - ( sbits < 17 >  ) DaXDaYTable ( dddd . DaX ) ;
    if ( sub2 . signedGT ( sub1 ) ) {
        tmp_btr1 ( 30 ) = 0 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sub2 , 40 ) ) ;
    } else {
        tmp_btr1 ( 30 ) = 1 ;
        D ( FFF ) . H = saturation16_noexception ( signExtend ( sub1 , 40 ) ) ;
    }
    if ( sum2 . signedGT ( sum1 ) ) {
        tmp_btr1 ( 31 ) = 0 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sum2 , 40 ) ) ;
    } else {
        tmp_btr1 ( 31 ) = 1 ;
        D ( FFF ) . L = saturation16_noexception ( signExtend ( sum1 , 40 ) ) ;
    }
    tmp_btr0 >>= 2 ;
    tmp_btr0 ( 31 ) = BTR1 ( 1 ) ;
    tmp_btr0 ( 30 ) = BTR1 ( 0 ) ;
    BTR0 = tmp_btr0 ;
    BTR1 = tmp_btr1 ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"BTR0",
"BTR1",
"D(FFF)[mask 0x000000ffff]/p",
"D(JJJ)[mask 0x000000ffff]/p",
"DaXDaYTable(dddd . DaX)",
"DaXDaYTable(dddd . DaY)",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"BTR0",
"BTR1",
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"saturation16_nochk",
"saturation16_noexception",
],
},
"ADC_Dc_Dd" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "adc ee",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 2938,
"ee_imp_bits__16_16_x_18_18_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 41 > sum = ( sbits < 41 >  ) D ( ee . Dc ) + ( sbits < 41 >  ) D ( ee . Dd ) + zeroExtend ( SR . C , 41 ) ;
    overflow40 ( sum ) ;
     bits < 40 > Gcarry = Carry ( D ( ee . Dc ) , D ( ee . Dd ) , SR . C ) ;
    SR . C = Gcarry ( 40 - 1 ) ;
    D ( ee . Dd ) = sum ( 39 , 0 ) ;
    L ( ee . Dd ) = LnComputation_special_instr ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(ee . Dc)",
"D(ee . Dd)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(ee . Dd)",
"EMR[mask 0x00000004]/p?",
"L(ee . Dd)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"LnComputation_nochk",
"LnComputation_special_instr",
"overflow40",
],
"raises_exception" => 1,
},
"ADD2_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "add2 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 840,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 17 > sumh = ( bits < 17 >  ) D ( FFF ) . H + ( bits < 17 >  ) D ( JJJ ) . H ;
     bits < 17 > suml = ( bits < 17 >  ) D ( FFF ) . L + ( bits < 17 >  ) D ( JJJ ) . L ;
    D ( FFF ) . H = sumh ( 15 , 0 ) ;
    D ( FFF ) . L = suml ( 15 , 0 ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = extension_update_add ( sumh , suml ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
"D(JJJ)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"extension_update_add",
],
},
"ADDA__s16_rx_Rn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "adda #s16,rrrr,RRR",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 76,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"rrrr_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = signExtend ( s16 , 32 ) ;
    if ( rrrr ( 4 ) == 0 && rrrr ( 3 ) == 1 ) {
        R ( RRR ) = Address_by_MCTL ( rrrr , imm , 0 , 0 , 1 , imm ) ;
    } else {
        R ( RRR ) = rxTable ( rrrr ) + imm ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"RxTable(rrrr)?",
"rxTable(rrrr)",
],
"outputs" => [
"R(RRR)",
],
"helpers" => [
"Address_by_MCTL",
"bitreverse",
"preshift",
],
},
"ADDA__u5_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "adda #u5,RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_" => 114,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = zeroExtend ( u5 , 32 ) ;
    if ( RRRR ( 4 ) == 0 && RRRR ( 3 ) == 1 ) {
        RxTable ( RRRR ) = Address_by_MCTL ( RRRR , imm , 1 , 0 , 1 , imm ) ;
    } else {
        RxTable ( RRRR ) = RxTable ( RRRR ) + imm ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"RxTable(RRRR)",
"rxTable(RRRR)?",
],
"outputs" => [
"RxTable(RRRR)",
],
"helpers" => [
"Address_by_MCTL",
"bitreverse",
"preshift",
],
},
"ADDA_rx_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "adda rrrr,RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_" => 225,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"rrrr_imp_bits__16_16_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( RRRR ( 4 ) == 0 && RRRR ( 3 ) == 1 ) {
        RxTable ( RRRR ) = Address_by_MCTL ( RRRR , rxTable ( rrrr ) , 1 , 0 , 1 , rxTable ( rrrr ) ) ;
    } else {
        RxTable ( RRRR ) = RxTable ( RRRR ) + rxTable ( rrrr ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"RxTable(RRRR)",
"rxTable(RRRR)?",
"rxTable(rrrr)",
],
"outputs" => [
"RxTable(RRRR)",
],
"helpers" => [
"Address_by_MCTL",
"bitreverse",
"preshift",
],
},
"ADDL1A_rx_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "addl1a rrrr,RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_" => 224,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"rrrr_imp_bits__16_16_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( RRRR ( 4 ) == 0 && RRRR ( 3 ) == 1 ) {
        RxTable ( RRRR ) = Address_by_MCTL ( RRRR , ( rxTable ( rrrr ) << 1 ) , 1 , 0 , 2 , rxTable ( rrrr ) ) ;
    } else {
        RxTable ( RRRR ) = RxTable ( RRRR ) + ( rxTable ( rrrr ) << 1 ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"RxTable(RRRR)",
"rxTable(RRRR)?",
"rxTable(rrrr)",
],
"outputs" => [
"RxTable(RRRR)",
],
"helpers" => [
"Address_by_MCTL",
"bitreverse",
"preshift",
],
},
"ADDL2A_rx_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "addl2a rrrr,RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_" => 226,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"rrrr_imp_bits__16_16_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( RRRR ( 4 ) == 0 && RRRR ( 3 ) == 1 ) {
        RxTable ( RRRR ) = Address_by_MCTL ( RRRR , ( rxTable ( rrrr ) << 2 ) , 1 , 0 , 4 , rxTable ( rrrr ) ) ;
    } else {
        RxTable ( RRRR ) = RxTable ( RRRR ) + ( rxTable ( rrrr ) << 2 ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"RxTable(RRRR)",
"rxTable(RRRR)?",
"rxTable(rrrr)",
],
"outputs" => [
"RxTable(RRRR)",
],
"helpers" => [
"Address_by_MCTL",
"bitreverse",
"preshift",
],
},
"ADDNC_W__s16_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "addnc.w #s16,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 228,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     sbits < 41 > imm = signExtend ( s16 , 41 ) ;
     bits < 41 > sum = ( sbits < 41 >  ) D ( JJJ ) + imm ;
    overflow40 ( sum ) ;
    saturation32 ( sum , FFF ) ;
    L ( FFF ) = LnComputation ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"ADD_Da_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "add jj,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 280,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 41 > sum = ( sbits < 41 >  ) D ( jj . Da ) + ( sbits < 41 >  ) D ( jj . Db ) ;
    overflow40 ( sum ) ;
     bits < 40 > Gcarry = Carry ( D ( jj . Da ) , D ( jj . Db ) , 0 ) ;
    SR . C = Gcarry ( 40 - 1 ) ;
    saturation32 ( sum , FFF ) ;
    L ( FFF ) = LnComputation ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(jj . Da)",
"D(jj . Db)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"ADD_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "add JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 46,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 41 > sum = ( sbits < 41 >  ) D ( JJJJJ . Da ) + ( sbits < 41 >  ) D ( JJJJJ . Db ) ;
    overflow40 ( sum ) ;
     bits < 40 > Gcarry = Carry ( D ( JJJJJ . Da ) , D ( JJJJJ . Db ) , 0 ) ;
    SR . C = Gcarry ( 40 - 1 ) ;
    saturation32 ( sum , FFF ) ;
    L ( FFF ) = LnComputation ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJJJ . Da)",
"D(JJJJJ . Db)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"ADD_W_Da_H_Dn_H" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "add.w JJJ.h,FFF.h",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1897,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 17 > sum = ( sbits < 17 >  ) D ( JJJ ) . H + ( sbits < 17 >  ) D ( FFF ) . H ;
    D ( FFF ) . H = saturation16_noexception ( signExtend ( sum , 40 ) ) ;
    L ( FFF ) = 0 ;
    sum = ( bits < 17 >  ) D ( JJJ ) . H + ( bits < 17 >  ) D ( FFF ) . H ;
    D ( FFF ) . E = extension_update_high ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffff0000]/p",
"D(JJJ)[mask 0x00ffff0000]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)[mask 0xffffff0000]",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_high",
"saturation16_nochk",
"saturation16_noexception",
],
},
"ADD_W_Da_H_Dn_L" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "add.w JJJ.h,FFF.l",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1889,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 17 > sum = ( sbits < 17 >  ) D ( JJJ ) . H + ( sbits < 17 >  ) D ( FFF ) . L ;
    D ( FFF ) . L = saturation16_noexception ( signExtend ( sum , 40 ) ) ;
    L ( FFF ) = 0 ;
    sum = ( bits < 17 >  ) D ( JJJ ) . H + ( bits < 17 >  ) D ( FFF ) . L ;
    D ( FFF ) . E = extension_update_low ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x000000ffff]/p",
"D(JJJ)[mask 0x00ffff0000]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)[mask 0xff0000ffff]",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_low",
"saturation16_nochk",
"saturation16_noexception",
],
},
"ADD_W_Da_L_Dn_H" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "add.w JJJ.l,FFF.h",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1896,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 17 > sum = ( sbits < 17 >  ) D ( JJJ ) . L + ( sbits < 17 >  ) D ( FFF ) . H ;
    D ( FFF ) . H = saturation16_noexception ( signExtend ( sum , 40 ) ) ;
    L ( FFF ) = 0 ;
    sum = ( bits < 17 >  ) D ( JJJ ) . L + ( bits < 17 >  ) D ( FFF ) . H ;
    D ( FFF ) . E = extension_update_high ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffff0000]/p",
"D(JJJ)[mask 0x000000ffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)[mask 0xffffff0000]",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_high",
"saturation16_nochk",
"saturation16_noexception",
],
},
"ADD_W_Da_L_Dn_L" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "add.w JJJ.l,FFF.l",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1888,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 17 > sum = ( sbits < 17 >  ) D ( JJJ ) . L + ( sbits < 17 >  ) D ( FFF ) . L ;
    D ( FFF ) . L = saturation16_noexception ( signExtend ( sum , 40 ) ) ;
    L ( FFF ) = 0 ;
    sum = ( bits < 17 >  ) D ( JJJ ) . L + ( bits < 17 >  ) D ( FFF ) . L ;
    D ( FFF ) . E = extension_update_low ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x000000ffff]/p",
"D(JJJ)[mask 0x000000ffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)[mask 0xff0000ffff]",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_low",
"saturation16_nochk",
"saturation16_noexception",
],
},
"ADD__u5_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "add #u5,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 58,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > D_imm = zeroExtend ( u5 , 40 ) ;
     bits < 41 > sum = ( sbits < 41 >  ) D_imm + ( sbits < 41 >  ) D ( FFF ) ;
    overflow40 ( sum ) ;
     bits < 40 > Gcarry = Carry ( D_imm , D ( FFF ) , 0 ) ;
    SR . C = Gcarry ( 40 - 1 ) ;
    saturation32 ( sum , FFF ) ;
    L ( FFF ) = LnComputation ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"ADR_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "adr JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 200,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 40 > tmp = round ( D ( FFF ) + D ( JJJ ) ) ;
    D ( FFF ) = tmp ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"round",
"saturate",
],
"raises_exception" => 1,
},
"ANDA_rx_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "anda rrrr,RRRR",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_" => 508,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"rrrr_imp_bits__16_16_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    RxTable ( RRRR ) = rxTable ( rrrr ) & RxTable ( RRRR ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
"rxTable(rrrr)",
],
"outputs" => [
"RxTable(RRRR)",
],
},
"AND_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "and JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_3_" => 880,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = D ( JJJ ) & D ( FFF ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"AND_W__u16__Rn_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "AND_W__u16__Rn_ RRR, u16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCLR_W__u16__Rn_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_",
"value" => "RRR",
},
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "~ u16",
},
],
"parent_action" => <<DATA
{
    DMEM ( R ( RRR ) , 2 ) = DMEM ( R ( RRR ) , 2 ) & ( ~ u16 ) ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"AND_W__u16__SPmu5_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "AND_W__u16__SPmu5_ u5, u16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCLR_W__u16__SPmu5_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "~ u16",
},
"misc" => {
"field" => "u5_imp_bits__20_20_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "u5",
},
],
"parent_action" => <<DATA
{
     bits < 6 > u6 = concat ( u5 , zero ( 1 ) ) ;
     bits < 32 > offset = concat ( zero ( 26 ) , u6 ) ;
    DMEM ( ( SP - offset ) , 2 ) = DMEM ( ( SP - offset ) , 2 ) & ~ u16 ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"AND_W__u16__SPps16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "AND_W__u16__SPps16_ s16, u16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCLR_W__u16__SPps16_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "s16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "s16",
},
"misc" => {
"field" => "u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "~ u16",
},
],
"parent_action" => <<DATA
{
     var offset = signExtend ( s16 , 32 ) ;
    DMEM ( ( SP + offset ) , 2 ) = DMEM ( ( SP + offset ) , 2 ) & ~ u16 ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"AND_W__u16__a16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "AND_W__u16__a16_ a16, u16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCLR_W__u16__a16_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "a16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "a16",
},
"misc" => {
"field" => "u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "~ u16",
},
],
"parent_action" => <<DATA
{
    DMEM ( a16 , 2 ) = DMEM ( a16 , 2 ) & ~ u16 ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "ThreeWords",
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"AND__0_u16__Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "and #u16,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_24_24_x_20_19_x_15_13_" => 252,
"JJJ_imp_bits__34_34_x_27_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_16_" => "",
"u16_imp_bits__23_21_x_12_0_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = zeroExtend ( u16 , 40 ) & D ( JJJ ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"AND___u16__0000_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "and #u16,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_24_24_x_20_19_x_15_13_" => 236,
"JJJ_imp_bits__34_34_x_27_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_16_" => "",
"u16_imp_bits__23_21_x_12_0_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = concat ( signExtend ( u16 , 24 ) , zero ( 16 ) ) & D ( JJJ ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"AND__u16_DR_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "AND__u16_DR_H HHHH, u16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCLR__u16_DR_H" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "HHHH",
},
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "~ u16",
},
],
"parent_action" => <<DATA
{
    if ( HHHH ( 3 ) == 1 ) {
        DRTable ( HHHH ) = DRTable ( HHHH ) & ~ ( concat ( u16 , zero ( 16 ) ) ) ;
    } else {
        DRTable ( HHHH ) = DRTable ( HHHH ) & ~ ( concat ( zero ( 8 ) , u16 , zero ( 16 ) ) ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"AND__u16_DR_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "AND__u16_DR_L HHHH, u16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCLR__u16_DR_L" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "HHHH",
},
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "~ u16",
},
],
"parent_action" => <<DATA
{
    if ( HHHH ( 3 , 3 ) == 1 ) {
        DRTable ( HHHH ) = DRTable ( HHHH ) & ~ ( zeroExtend ( u16 , 32 ) ) ;
    } else {
        DRTable ( HHHH ) = DRTable ( HHHH ) & ~ ( zeroExtend ( u16 , 40 ) ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"ASL2A_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asl2a RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_0_" => 3838,
"RRRR_imp_bits__18_18_x_11_8_" => "",
"reserved_imp_bits__17_16_" => "reserved",
},
"action" => <<DATA
{
    RxTable ( RRRR ) = RxTable ( RRRR ) << 2 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
],
"outputs" => [
"RxTable(RRRR)",
],
},
"ASLA_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asla RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_0_" => 3836,
"RRRR_imp_bits__18_18_x_11_8_" => "",
"reserved_imp_bits__17_16_" => "reserved",
},
"action" => <<DATA
{
    RxTable ( RRRR ) = RxTable ( RRRR ) << 1 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
],
"outputs" => [
"RxTable(RRRR)",
],
},
"ASLL2_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asll2 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1914,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bool shift_left = true ;
     bits < 5 > shift_size = D ( JJJ ) ( 4 , 0 ) ;
    multiple_bit_shift2 ( FFF , shift_size , shift_left ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
"D(JJJ)[mask 0x000000001f]/p",
"EMR/p",
"SR[mask 0x00060080]/p?",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"multiple_bit_shift2",
"overflow_shift_left",
"saturation16_shift",
"set_emr_dovf",
"slice_signExtend16",
],
},
"ASLL2__e_u2_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asll2 #e_u2,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3812,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"e_u2_imp_bits__1_1_x_0_0_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"ASLL2__u4_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asll2 #u4,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_" => 934,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u4_imp_bits__3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bool shift_left = true ;
     bits < 5 > shift_size = zeroExtend ( u4 , 5 ) ;
    multiple_bit_shift2 ( FFF , shift_size , shift_left ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p?",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"multiple_bit_shift2",
"overflow_shift_left",
"saturation16_shift",
"set_emr_dovf",
"slice_signExtend16",
],
},
"ASLLS_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "aslls JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 832,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bool shift_left = true ;
     bool saturation = true ;
    multiple_bit_shift ( FFF , D ( JJJ ) ( 6 , 0 ) , shift_left , saturation ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)/p",
"D(JJJ)[mask 0x000000007f]/p",
"EMR/p",
"SR[mask 0x00060004]/p?",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"multiple_bit_shift",
"overflow_shift_left",
"saturation32_shift",
"set_emr_dovf",
"slice_signExtend",
],
},
"ASLLS__u5_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "aslls #u5,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 223,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bool shift_left = true ;
     bool saturation = true ;
    multiple_bit_shift ( FFF , zeroExtend ( u5 , 7 ) , shift_left , saturation ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)/p",
"EMR/p",
"SR[mask 0x00060004]/p?",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"multiple_bit_shift",
"overflow_shift_left",
"saturation32_shift",
"set_emr_dovf",
"slice_signExtend",
],
},
"ASLL_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asll JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 850,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bool shift_left = true ;
     bool saturation = false ;
     bits < 7 > shift_size = D ( JJJ ) ( 6 , 0 ) ;
    multiple_bit_shift ( FFF , shift_size , shift_left , saturation ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)/p",
"D(JJJ)[mask 0x000000007f]/p",
"EMR/p",
"SR[mask 0x00060004]/p?",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"multiple_bit_shift",
"overflow_shift_left",
"saturation32_shift",
"set_emr_dovf",
"slice_signExtend",
],
},
"ASLL__u5_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asll #u5,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 62,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bool shift_left = true ;
     bool saturation = false ;
    multiple_bit_shift ( FFF , zeroExtend ( u5 , 7 ) , shift_left , saturation ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)/p",
"EMR/p",
"SR[mask 0x00060004]/p?",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"multiple_bit_shift",
"overflow_shift_left",
"saturation32_shift",
"set_emr_dovf",
"slice_signExtend",
],
},
"ASLW_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "aslw JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 866,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 5 > shift = 16 ;
     bool ovf = overflow_shift_left ( D ( JJJ ) , shift ) ;
    D ( FFF ) = D ( JJJ ) << shift ;
    SR . C = D ( JJJ ) ( 40 - shift ) ;
    if ( ovf ) set_emr_dovf (  ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"overflow_shift_left",
"set_emr_dovf",
],
},
"ASR2_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asr2 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 835,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 1 > sbit_hp = D ( JJJ ) ( 31 , 31 ) ;
     bits < 1 > sbit_lp = D ( JJJ ) ( 15 , 15 ) ;
     bits < 16 > tmp_hp = D ( JJJ ) . H >> 1 ;
     bits < 16 > tmp_lp = D ( JJJ ) . L >> 1 ;
    tmp_hp ( 15 ) = sbit_hp ;
    tmp_lp ( 15 ) = sbit_lp ;
    D ( FFF ) . H = tmp_hp ;
    D ( FFF ) . L = tmp_lp ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"ASRA_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asra RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_0_" => 3837,
"RRRR_imp_bits__18_18_x_11_8_" => "",
"reserved_imp_bits__17_16_" => "reserved",
},
"action" => <<DATA
{
    RxTable ( RRRR ) = signExtend ( ( ( RxTable ( RRRR ) >> 1 ) ( 30 , 0 ) ) , 32 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
],
"outputs" => [
"RxTable(RRRR)",
],
},
"ASRR2_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asrr2 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1915,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bool shift_left = false ;
     bits < 5 > shift_size = D ( JJJ ) ( 4 , 0 ) ;
    multiple_bit_shift2 ( FFF , shift_size , shift_left ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
"D(JJJ)[mask 0x000000001f]/p",
"EMR/p",
"SR[mask 0x00060080]/p?",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"multiple_bit_shift2",
"overflow_shift_left",
"saturation16_shift",
"set_emr_dovf",
"slice_signExtend16",
],
},
"ASRR2__e_u2_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asrr2 #e_u2,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3813,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"e_u2_imp_bits__1_1_x_0_0_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"ASRR2__u4_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asrr2 #u4,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_" => 935,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u4_imp_bits__3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bool shift_left = false ;
     bits < 5 > shift_size = zeroExtend ( u4 , 5 ) ;
    multiple_bit_shift2 ( FFF , shift_size , shift_left ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p?",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"multiple_bit_shift2",
"overflow_shift_left",
"saturation16_shift",
"set_emr_dovf",
"slice_signExtend16",
],
},
"ASRRS_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asrrs JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 833,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bool shift_left = false ;
     bool saturation = true ;
    multiple_bit_shift ( FFF , D ( JJJ ) ( 6 , 0 ) , shift_left , saturation ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)/p",
"D(JJJ)[mask 0x000000007f]/p",
"EMR/p",
"SR[mask 0x00060004]/p?",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"multiple_bit_shift",
"overflow_shift_left",
"saturation32_shift",
"set_emr_dovf",
"slice_signExtend",
],
},
"ASRRS__u5_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asrrs #u5,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 219,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bool shift_left = false ;
     bool saturation = true ;
    multiple_bit_shift ( FFF , zeroExtend ( u5 , 7 ) , shift_left , saturation ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)/p",
"EMR/p",
"SR[mask 0x00060004]/p?",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"multiple_bit_shift",
"overflow_shift_left",
"saturation32_shift",
"set_emr_dovf",
"slice_signExtend",
],
},
"ASRR_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asrr JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 851,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bool shift_left = false ;
     bool saturation = false ;
    multiple_bit_shift ( FFF , D ( JJJ ) ( 6 , 0 ) , shift_left , saturation ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)/p",
"D(JJJ)[mask 0x000000007f]/p",
"EMR/p",
"SR[mask 0x00060004]/p?",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"multiple_bit_shift",
"overflow_shift_left",
"saturation32_shift",
"set_emr_dovf",
"slice_signExtend",
],
},
"ASRR__u5_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asrr #u5,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 63,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bool shift_left = false ;
     bool saturation = false ;
    multiple_bit_shift ( FFF , zeroExtend ( u5 , 7 ) , shift_left , saturation ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)/p",
"EMR/p",
"SR[mask 0x00060004]/p?",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"multiple_bit_shift",
"overflow_shift_left",
"saturation32_shift",
"set_emr_dovf",
"slice_signExtend",
],
},
"ASRW_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asrw JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 867,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 5 > shift = 16 ;
     bits < 40 > val = D ( JJJ ) >> shift ;
    D ( FFF ) = slice_signExtend ( val , ( 39 - shift ) ) ;
    SR . C = ( shift == 0 ) ? 0 : D ( JJJ ) ( shift - 1 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)",
"SR/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"slice_signExtend",
],
},
"ASR_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "asr JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 216,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     sbits < 40 > tmp = Shift_Right_Dalu ( D ( JJJ ) , 1 ) ;
    saturation32 ( signExtend ( tmp , 41 ) , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"Shift_Right_Dalu",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
"slice_signExtend",
],
"raises_exception" => 1,
},
"AVGU4_Dg_Dh_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "avgu4 GG,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 383,
"GG_imp_bits__18_18_x_17_17_x_9_9_x_8_8_" => "",
"FFF_imp_bits__16_16_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     var dh = GG . Dh ;
     var dg = GG . Dg ;
     bits < 9 > ll = ( ( bits < 9 >  ) D ( dh ) . LL + ( bits < 9 >  ) D ( dg ) . LL + 1 ) >> 1 ;
     bits < 9 > lh = ( ( bits < 9 >  ) D ( dh ) . LH + ( bits < 9 >  ) D ( dg ) . LH + 1 ) >> 1 ;
     bits < 9 > hl = ( ( bits < 9 >  ) D ( dh ) . HL + ( bits < 9 >  ) D ( dg ) . HL + 1 ) >> 1 ;
     bits < 9 > hh = ( ( bits < 9 >  ) D ( dh ) . HH + ( bits < 9 >  ) D ( dg ) . HH + 1 ) >> 1 ;
    D ( FFF ) . LL = ll ;
    D ( FFF ) . LH = lh ;
    D ( FFF ) . HL = hl ;
    D ( FFF ) . HH = hh ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D[mask 0x00000000ff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"BDEINTRLV_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "bdeintrlv JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_3_" => 1905,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 6 > tmp = concat ( D ( JJJ ) ( 30 ) , D ( JJJ ) ( 28 ) , D ( JJJ ) ( 26 ) , D ( JJJ ) ( 24 ) , D ( JJJ ) ( 22 ) , D ( JJJ ) ( 20 ) ) ;
     bits < 6 > tmp2 = concat ( D ( JJJ ) ( 18 ) , D ( JJJ ) ( 16 ) , D ( JJJ ) ( 14 ) , D ( JJJ ) ( 12 ) , D ( JJJ ) ( 10 ) , D ( JJJ ) ( 8 ) ) ;
    D ( FFF ) . L = concat ( tmp , tmp2 , D ( JJJ ) ( 6 ) , D ( JJJ ) ( 4 ) , D ( JJJ ) ( 2 ) , D ( JJJ ) ( 0 ) ) ;
    tmp = concat ( D ( JJJ ) ( 31 ) , D ( JJJ ) ( 29 ) , D ( JJJ ) ( 27 ) , D ( JJJ ) ( 25 ) , D ( JJJ ) ( 23 ) , D ( JJJ ) ( 21 ) ) ;
    tmp2 = concat ( D ( JJJ ) ( 19 ) , D ( JJJ ) ( 17 ) , D ( JJJ ) ( 15 ) , D ( JJJ ) ( 13 ) , D ( JJJ ) ( 11 ) , D ( JJJ ) ( 9 ) ) ;
    D ( FFF ) . H = concat ( tmp , tmp2 , D ( JJJ ) ( 7 ) , D ( JJJ ) ( 5 ) , D ( JJJ ) ( 3 ) , D ( JJJ ) ( 1 ) ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"BDINTRLV3B_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "bdintrlv3b JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DLU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1830,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"BFD___label" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "bfd a8",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_9_x_0_0_" => 132,
"reserved_imp_bits__18_16_" => "reserved",
"a8_imp_bits__8_1_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 0 ) {
        DSPC = PC_ST_VLES + signExtend ( concat ( a8 , zero ( 1 ) ) , 32 ) ;
        Fetch = Fetch :: DelaySlot ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"PC_ST_VLES?",
"SR[mask 0x00000002]/p",
],
"outputs" => [
"DSPC?",
"Fetch?",
],
},
"BFD__label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bfd a20",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_26_24_x_20_19_x_15_13_" => 732,
"reserved_imp_bits__34_32_" => "reserved",
"a20_imp_bits__27_27_x_18_16_x_0_0_x_23_21_x_12_1_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 0 ) {
        DSPC = PC_ST_VLES + signExtend ( concat ( a20 , zero ( 1 ) ) , 32 ) ;
        Fetch = Fetch :: DelaySlot ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"PC_ST_VLES?",
"SR[mask 0x00000002]/p",
],
"outputs" => [
"DSPC?",
"Fetch?",
],
},
"BF___label" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "bf a8",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_9_x_0_0_" => 133,
"reserved_imp_bits__18_16_" => "reserved",
"a8_imp_bits__8_1_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 0 ) {
        NVPC = PC_ST_VLES + signExtend ( concat ( a8 , zero ( 1 ) ) , 32 ) ;
        jump_instr = true ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"PC_ST_VLES?",
"SR[mask 0x00000002]/p",
],
"outputs" => [
"NVPC?",
],
},
"BF__label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bf a20",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_26_24_x_20_19_x_15_13_" => 764,
"reserved_imp_bits__34_32_" => "reserved",
"a20_imp_bits__27_27_x_18_16_x_0_0_x_23_21_x_12_1_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 0 ) {
        NVPC = PC_ST_VLES + signExtend ( concat ( a20 , zero ( 1 ) ) , 32 ) ;
        jump_instr = true ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"PC_ST_VLES?",
"SR[mask 0x00000002]/p",
],
"outputs" => [
"NVPC?",
],
},
"BINTRLV3B_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "bintrlv3b JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DLU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1829,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"BINTRLV_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "bintrlv JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_3_" => 1904,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 32 > tmp = 0 ;
    for (  int i = 0 ; i <= 15 ; i ++ ) {
        tmp ( 2 * i ) = D ( JJJ ) . L ( i ) ;
        tmp ( ( 2 * i ) + 1 ) = D ( JJJ ) . H ( i ) ;
    }
    D ( FFF ) . H = tmp ( 31 , 16 ) ;
    D ( FFF ) . L = tmp ( 15 , 0 ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"BMCHG_W__u16__Rn_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmchg.w #u16,(RRR)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 589,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    DMEM ( R ( RRR ) , 2 ) = DMEM ( R ( RRR ) , 2 ) ^ ( u16 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMCHG_W__u16__SPmu5_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmchg.w #u16,(sp-u5)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_15_15_x_14_14_x_13_13_" => 21,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"u5_imp_bits__20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 6 > u6 = concat ( u5 , zero ( 1 ) ) ;
     bits < 32 > offset = concat ( zero ( 26 ) , u6 ) ;
    DMEM ( ( SP - offset ) , 2 ) = DMEM ( ( SP - offset ) , 2 ) ^ ( u16 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMCHG_W__u16__SPps16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "bmchg.w #u16,(sp+s16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 14950,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"s16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     var offset = signExtend ( s16 , 32 ) ;
    DMEM ( ( SP + offset ) , 2 ) = ( DMEM ( ( SP + offset ) , 2 ) ) ^ ( u16 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMCHG_W__u16__a16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "bmchg.w #u16,(a16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 14886,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"a16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    DMEM ( a16 , 2 ) = ( DMEM ( a16 , 2 ) ) ^ ( u16 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMCHG__u16_C1_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmchg #u16,CCC.h",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 597,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"CCC_imp_bits__18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    if ( ( SR . PE == 1 ) && ( CCC == 1 || CCC == 2 ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        C1Table ( CCC ) = ( C1Table ( CCC ) ^ ( concat ( u16 , zero ( 16 ) ) ) ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"C1Table(CCC)?",
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"C1Table(CCC)?",
"EMR[mask 0x00000010]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"BMCHG__u16_C1_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmchg #u16,CCC.l",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 581,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"CCC_imp_bits__18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) && ( CCC == 1 || CCC == 2 ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        C1Table ( CCC ) = C1Table ( CCC ) ^ ( zeroExtend ( u16 , 32 ) ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"C1Table(CCC)?",
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"C1Table(CCC)?",
"EMR[mask 0x00000010]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"BMCHG__u16_DR_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmchg #u16,HHHH.h",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_15_15_x_14_14_x_13_13_" => 173,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( HHHH ( 3 ) == 1 ) {
        DRTable ( HHHH ) = DRTable ( HHHH ) ^ ( concat ( u16 , zero ( 16 ) ) ) ;
    } else {
        DRTable ( HHHH ) = DRTable ( HHHH ) ^ ( concat ( zero ( 8 ) , u16 , zero ( 16 ) ) ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"BMCHG__u16_DR_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmchg #u16,HHHH.l",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_15_15_x_14_14_x_13_13_" => 165,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( HHHH ( 3 , 3 ) == 1 ) {
        DRTable ( HHHH ) = DRTable ( HHHH ) ^ ( zeroExtend ( u16 , 32 ) ) ;
    } else {
        DRTable ( HHHH ) = DRTable ( HHHH ) ^ ( zeroExtend ( u16 , 40 ) ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"BMCLR_W__u16__Rn_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmclr.w #u16,(RRR)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 525,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    DMEM ( R ( RRR ) , 2 ) = DMEM ( R ( RRR ) , 2 ) & ( ~ u16 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMCLR_W__u16__SPmu5_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmclr.w #u16,(sp-u5)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_15_15_x_14_14_x_13_13_" => 5,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"u5_imp_bits__20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 6 > u6 = concat ( u5 , zero ( 1 ) ) ;
     bits < 32 > offset = concat ( zero ( 26 ) , u6 ) ;
    DMEM ( ( SP - offset ) , 2 ) = DMEM ( ( SP - offset ) , 2 ) & ~ u16 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMCLR_W__u16__SPps16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "bmclr.w #u16,(sp+s16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 14438,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"s16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     var offset = signExtend ( s16 , 32 ) ;
    DMEM ( ( SP + offset ) , 2 ) = DMEM ( ( SP + offset ) , 2 ) & ~ u16 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMCLR_W__u16__a16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "bmclr.w #u16,(a16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 14374,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"a16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    DMEM ( a16 , 2 ) = DMEM ( a16 , 2 ) & ~ u16 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMCLR__u16_C1_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmclr #u16,CCC.h",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 533,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"CCC_imp_bits__18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    if ( ( SR . PE == 1 ) && ( CCC == 1 || CCC == 2 ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        C1Table ( CCC ) = ( C1Table ( CCC ) & ( ~ ( concat ( u16 , zero ( 16 ) ) ) ) ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"C1Table(CCC)?",
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"C1Table(CCC)?",
"EMR[mask 0x00000010]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"BMCLR__u16_C1_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmclr #u16,CCC.l",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 517,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"CCC_imp_bits__18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) && ( CCC == 1 || CCC == 2 ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        C1Table ( CCC ) = C1Table ( CCC ) & ~ ( zeroExtend ( u16 , 32 ) ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"C1Table(CCC)?",
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"C1Table(CCC)?",
"EMR[mask 0x00000010]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"BMCLR__u16_DR_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmclr #u16,HHHH.h",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_15_15_x_14_14_x_13_13_" => 141,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( HHHH ( 3 ) == 1 ) {
        DRTable ( HHHH ) = DRTable ( HHHH ) & ~ ( concat ( u16 , zero ( 16 ) ) ) ;
    } else {
        DRTable ( HHHH ) = DRTable ( HHHH ) & ~ ( concat ( zero ( 8 ) , u16 , zero ( 16 ) ) ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"BMCLR__u16_DR_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmclr #u16,HHHH.l",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_15_15_x_14_14_x_13_13_" => 133,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( HHHH ( 3 , 3 ) == 1 ) {
        DRTable ( HHHH ) = DRTable ( HHHH ) & ~ ( zeroExtend ( u16 , 32 ) ) ;
    } else {
        DRTable ( HHHH ) = DRTable ( HHHH ) & ~ ( zeroExtend ( u16 , 40 ) ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"BMSET_W__u16__Rn_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmset.w #u16,(RRR)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 557,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    DMEM ( R ( RRR ) , 2 ) = DMEM ( R ( RRR ) , 2 ) | u16 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMSET_W__u16__SPmu5_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmset.w #u16,(sp-u5)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_15_15_x_14_14_x_13_13_" => 13,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"u5_imp_bits__20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 6 > u6 = concat ( u5 , zero ( 1 ) ) ;
     bits < 32 > offset = concat ( zero ( 26 ) , u6 ) ;
    DMEM ( ( SP - offset ) , 2 ) = DMEM ( ( SP - offset ) , 2 ) | u16 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMSET_W__u16__SPps16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "bmset.w #u16,(sp+s16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 14694,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"s16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    DMEM ( ( SP + signExtend ( s16 , 32 ) ) , 2 ) = ( DMEM ( ( SP + signExtend ( s16 , 32 ) ) , 2 ) ) | ( u16 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMSET_W__u16__a16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "bmset.w #u16,(a16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 14630,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"a16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    DMEM ( a16 , 2 ) = DMEM ( a16 , 2 ) | u16 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMSET__u16_C1_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmset #u16,CCC.h",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 565,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"CCC_imp_bits__18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) && ( CCC == 1 || CCC == 2 ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        C1Table ( CCC ) = C1Table ( CCC ) | concat ( u16 , zero ( 16 ) ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"C1Table(CCC)?",
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"C1Table(CCC)?",
"EMR[mask 0x00000010]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"BMSET__u16_C1_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmset #u16,CCC.l",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 549,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"CCC_imp_bits__18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) && ( CCC == 1 || CCC == 2 ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        C1Table ( CCC ) = C1Table ( CCC ) | concat ( zero ( 16 ) , u16 ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"C1Table(CCC)?",
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"C1Table(CCC)?",
"EMR[mask 0x00000010]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"BMSET__u16_DR_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmset #u16,HHHH.h",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_15_15_x_14_14_x_13_13_" => 157,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( HHHH ( 3 ) == 1 ) {
        DRTable ( HHHH ) = DRTable ( HHHH ) | concat ( u16 , zero ( 16 ) ) ;
    } else {
        DRTable ( HHHH ) = DRTable ( HHHH ) | concat ( zero ( 8 ) , u16 , zero ( 16 ) ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"BMSET__u16_DR_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmset #u16,HHHH.l",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_15_15_x_14_14_x_13_13_" => 149,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( HHHH ( 3 ) == 1 ) {
        DRTable ( HHHH ) = DRTable ( HHHH ) | concat ( zero ( 16 ) , u16 ) ;
    } else {
        DRTable ( HHHH ) = DRTable ( HHHH ) | concat ( zero ( 24 ) , u16 ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"BMTSET_W__u16__Rn_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtset.w #u16,(RRR)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 717,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 16 > src = DMEM ( R ( RRR ) , 2 ) ;
     bits < 16 > res = src | u16 ;
    if ( res == src ) SR . T = 1 ; else SR . T = 0 ;
    DMEM ( R ( RRR ) , 2 ) = res ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMTSET_W__u16__SPmu5_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtset.w #u16,(sp-u5)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_15_15_x_14_14_x_13_13_" => 53,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"u5_imp_bits__20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 6 > u6 = concat ( u5 , zero ( 1 ) ) ;
     bits < 32 > offset = concat ( zero ( 26 ) , u6 ) ;
     bits < 16 > src = DMEM ( ( SP - offset ) , 2 ) ;
     bits < 16 > res = src | u16 ;
    if ( res == src ) SR . T = 1 ; else SR . T = 0 ;
    DMEM ( ( SP - offset ) , 2 ) = res ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMTSET_W__u16__SPps16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "bmtset.w #u16,(sp+s16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 15974,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"s16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 16 > src = DMEM ( ( SP + signExtend ( s16 , 32 ) ) , 2 ) ;
     bits < 16 > res = src | u16 ;
    if ( res == src ) SR . T = 1 ; else SR . T = 0 ;
    DMEM ( ( SP + signExtend ( s16 , 32 ) ) , 2 ) = res ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMTSET_W__u16__a16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "bmtset.w #u16,(a16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 15910,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"a16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 16 > src = DMEM ( a16 , 2 ) ;
     bits < 16 > res = src | u16 ;
    if ( res == src ) SR . T = 1 ; else SR . T = 0 ;
    DMEM ( a16 , 2 ) = res ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"BMTSET__u16_DR_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtset #u16,HHHH.h",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_15_15_x_14_14_x_13_13_" => 237,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( HHHH ( 3 ) == 1 ) {
         bits < 32 > tmp_r = DRTable ( HHHH ) | concat ( u16 , zero ( 16 ) ) ;
        if ( tmp_r == DRTable ( HHHH ) ) SR . T = 1 ; else SR . T = 0 ;
        DRTable ( HHHH ) = tmp_r ;
    } else {
         bits < 40 > tmp_d = DRTable ( HHHH ) | concat ( zero ( 8 ) , u16 , zero ( 16 ) ) ;
        if ( tmp_d == DRTable ( HHHH ) ) SR . T = 1 ; else SR . T = 0 ;
        DRTable ( HHHH ) = tmp_d ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
"SR/p",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
"SR[mask 0x00000002]/p",
],
},
"BMTSET__u16_DR_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtset #u16,HHHH.l",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_15_15_x_14_14_x_13_13_" => 229,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( HHHH ( 3 ) == 1 ) {
         bits < 32 > tmp_r = DRTable ( HHHH ) | zeroExtend ( u16 , 32 ) ;
        if ( tmp_r == DRTable ( HHHH ) ) SR . T = 1 ; else SR . T = 0 ;
        DRTable ( HHHH ) = tmp_r ;
    } else {
         bits < 40 > tmp_d = DRTable ( HHHH ) | zeroExtend ( u16 , 40 ) ;
        if ( tmp_d == DRTable ( HHHH ) ) SR . T = 1 ; else SR . T = 0 ;
        DRTable ( HHHH ) = tmp_d ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
"SR/p",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
"SR[mask 0x00000002]/p",
],
},
"BMTSTC_W__u16__Rn_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtstc.w #u16,(RRR)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 653,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( ( DMEM ( R ( RRR ) , 2 ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
"input_mems" => [
"DMEM",
],
},
"BMTSTC_W__u16__SPmu5_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtstc.w #u16,(sp-u5)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_15_15_x_14_14_x_13_13_" => 37,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"u5_imp_bits__20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 6 > u6 = concat ( u5 , zero ( 1 ) ) ;
     bits < 32 > offset = concat ( zero ( 26 ) , u6 ) ;
    if ( ( DMEM ( ( SP - offset ) , 2 ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
"input_mems" => [
"DMEM",
],
},
"BMTSTC_W__u16__SPps16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "bmtstc.w #u16,(sp+s16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 15462,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"s16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     var offset = signExtend ( s16 , 32 ) ;
    if ( ( DMEM ( ( SP + offset ) , 2 ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
"input_mems" => [
"DMEM",
],
},
"BMTSTC_W__u16__a16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "bmtstc.w #u16,(a16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 15398,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"a16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( ( DMEM ( a16 , 2 ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
"input_mems" => [
"DMEM",
],
},
"BMTSTC__u16_C1_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtstc #u16,CCC.h",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 661,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"CCC_imp_bits__18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    if ( ( SR . PE == 1 ) && ( CCC == 1 || CCC == 2 ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        if ( ( C1Table ( CCC ) ( 31 , 16 ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"C1Table(CCC)[mask 0xffff0000]/p?",
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
"SR[mask 0x00000002]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"BMTSTC__u16_C1_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtstc #u16,CCC.l",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 645,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"CCC_imp_bits__18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    if ( ( SR . PE == 1 ) && ( CCC == 1 || CCC == 2 ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        if ( ( C1Table ( CCC ) ( 15 , 0 ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"C1Table(CCC)[mask 0x0000ffff]/p?",
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
"SR[mask 0x00000002]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"BMTSTC__u16_DR_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtstc #u16,HHHH.h",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_15_15_x_14_14_x_13_13_" => 205,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( ( DRTable ( HHHH ) ( 31 , 16 ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)[mask 0x00ffff0000]/p",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"BMTSTC__u16_DR_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtstc #u16,HHHH.l",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_15_15_x_14_14_x_13_13_" => 197,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( ( DRTable ( HHHH ) ( 15 , 0 ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)[mask 0x000000ffff]/p",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"BMTSTS_W__u16__Rn_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtsts.w #u16,(RRR)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 685,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( ( ~ ( DMEM ( R ( RRR ) , 2 ) ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
"input_mems" => [
"DMEM",
],
},
"BMTSTS_W__u16__SPmu5_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtsts.w #u16,(sp-u5)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_15_15_x_14_14_x_13_13_" => 45,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"u5_imp_bits__20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 6 > u6 = concat ( u5 , zero ( 1 ) ) ;
     bits < 32 > offset = concat ( zero ( 26 ) , u6 ) ;
    if ( ( ( ~ ( DMEM ( ( SP - offset ) , 2 ) ) ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
"input_mems" => [
"DMEM",
],
},
"BMTSTS_W__u16__SPps16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "bmtsts.w #u16,(sp+s16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 15718,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"s16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     var offset = signExtend ( s16 , 32 ) ;
    if ( ( ( ~ ( DMEM ( ( SP + offset ) , 2 ) ) ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
"input_mems" => [
"DMEM",
],
},
"BMTSTS_W__u16__a16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "bmtsts.w #u16,(a16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 15654,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"a16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( ( ~ ( DMEM ( a16 , 2 ) ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
"input_mems" => [
"DMEM",
],
},
"BMTSTS__u16_C1_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtsts #u16,CCC.h",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 693,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"CCC_imp_bits__18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    if ( ( SR . PE == 1 ) && ( CCC == 1 || CCC == 2 ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        if ( ( ( ~ ( C1Table ( CCC ) ( 31 , 16 ) ) ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"C1Table(CCC)[mask 0xffff0000]/p?",
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
"SR[mask 0x00000002]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"BMTSTS__u16_C1_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtsts #u16,CCC.l",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 677,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"CCC_imp_bits__18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    if ( ( SR . PE == 1 ) && ( CCC == 1 || CCC == 2 ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        if ( ( ( ~ ( C1Table ( CCC ) ( 15 , 0 ) ) ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"C1Table(CCC)[mask 0x0000ffff]/p?",
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
"SR[mask 0x00000002]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"BMTSTS__u16_DR_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtsts #u16,HHHH.h",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_15_15_x_14_14_x_13_13_" => 221,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( ( ( ~ ( DRTable ( HHHH ) ( 31 , 16 ) ) ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)[mask 0x00ffff0000]/p",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"BMTSTS__u16_DR_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bmtsts #u16,HHHH.l",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_15_15_x_14_14_x_13_13_" => 213,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( ( ( ~ ( DRTable ( HHHH ) ( 15 , 0 ) ) ) & u16 ) == 0x0000 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)[mask 0x000000ffff]/p",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"BRAD___label" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "brad a10",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_11_x_0_0_" => 34,
"reserved_imp_bits__18_16_" => "reserved",
"a10_imp_bits__10_1_" => "",
},
"action" => <<DATA
{
    DSPC = PC_ST_VLES + signExtend ( concat ( a10 , zero ( 1 ) ) , 32 ) ;
    Fetch = Fetch :: DelaySlot ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"PC_ST_VLES",
],
"outputs" => [
"DSPC",
"Fetch",
],
},
"BRAD__label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "brad a20",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_26_24_x_20_19_x_15_13_" => 540,
"reserved_imp_bits__34_32_" => "reserved",
"a20_imp_bits__27_27_x_18_16_x_0_0_x_23_21_x_12_1_" => "",
},
"action" => <<DATA
{
    DSPC = PC_ST_VLES + signExtend ( concat ( a20 , zero ( 1 ) ) , 32 ) ;
    Fetch = Fetch :: DelaySlot ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"PC_ST_VLES",
],
"outputs" => [
"DSPC",
"Fetch",
],
},
"BRA___label" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "bra a10",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_11_x_0_0_" => 35,
"reserved_imp_bits__18_16_" => "reserved",
"a10_imp_bits__10_1_" => "",
},
"action" => <<DATA
{
    jump_instr = true ;
    NVPC = PC_ST_VLES + signExtend ( concat ( a10 , zero ( 1 ) ) , 32 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"PC_ST_VLES",
],
"outputs" => [
"NVPC",
],
},
"BRA__label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bra a20",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_26_24_x_20_19_x_15_13_" => 572,
"reserved_imp_bits__34_32_" => "reserved",
"a20_imp_bits__27_27_x_18_16_x_0_0_x_23_21_x_12_1_" => "",
},
"action" => <<DATA
{
    jump_instr = true ;
    NVPC = PC_ST_VLES + signExtend ( concat ( a20 , zero ( 1 ) ) , 32 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"PC_ST_VLES",
],
"outputs" => [
"NVPC",
],
},
"BREAK_label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "break a16",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_24_x_20_16_x_15_13_" => 8220,
"reserved_imp_bits__34_32_" => "reserved",
"a16_imp_bits__0_0_x_23_21_x_12_1_" => "",
},
"action" => <<DATA
{
     uint32_t active_loop = FindActiveLoop (  ) ;
    jump_instr = true ;
    NVPC = PC_ST_VLES + signExtend ( concat ( a16 , zero ( 1 ) ) , 32 ) ;
    clear_SR_LF ( active_loop ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"PC_ST_VLES",
"SR[mask 0x78020000]/p",
],
"outputs" => [
"NVPC",
"SR[mask 0x78000000]/p?",
],
"helpers" => [
"FindActiveLoop",
"clear_SR_LF",
],
},
"BREV4_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "brev4 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DLU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1831,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"BREV_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "brev JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_3_" => 143,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 40 > tmp = bitreverse ( D ( JJJ ) ( 31 , 0 ) ) ;
    tmp = concat ( D ( FFF ) . E , tmp ) ;
    D ( FFF ) = tmp ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0xff00000000]/p",
"D(JJJ)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"bitreverse",
],
},
"BSRD___label" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "bsrd a8",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_0_0_" => 130,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
"a8_imp_bits__8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_" => "",
},
"action" => <<DATA
{
    subroutine_instr = delayed ;
    DSPC = PC_ST_VLES + signExtend ( concat ( a8 , zero ( 1 ) ) , 32 ) ;
    Fetch = Fetch :: DelaySlot ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"PC_ST_VLES",
],
"outputs" => [
"DSPC",
"Fetch",
],
},
"BSRD__label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bsrd a20",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 604,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"a20_imp_bits__27_27_x_18_18_x_17_17_x_16_16_x_0_0_x_23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_" => "",
},
"action" => <<DATA
{
    subroutine_instr = delayed ;
    DSPC = PC_ST_VLES + signExtend ( concat ( a20 , zero ( 1 ) ) , 32 ) ;
    Fetch = Fetch :: DelaySlot ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"PC_ST_VLES",
],
"outputs" => [
"DSPC",
"Fetch",
],
},
"BSR___label" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "bsr a8",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_0_0_" => 131,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
"a8_imp_bits__8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_" => "",
},
"action" => <<DATA
{
    subroutine_instr = regular ;
    jump_instr = true ;
    NVPC = PC_ST_VLES + signExtend ( concat ( a8 , zero ( 1 ) ) , 32 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"PC_ST_VLES",
],
"outputs" => [
"NVPC",
],
},
"BSR__label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bsr a20",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 636,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"a20_imp_bits__27_27_x_18_18_x_17_17_x_16_16_x_0_0_x_23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_" => "",
},
"action" => <<DATA
{
    subroutine_instr = regular ;
    jump_instr = true ;
    NVPC = PC_ST_VLES + signExtend ( concat ( a20 , zero ( 1 ) ) , 32 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"PC_ST_VLES",
],
"outputs" => [
"NVPC",
],
},
"BTD___label" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "btd a8",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_9_x_0_0_" => 128,
"reserved_imp_bits__18_16_" => "reserved",
"a8_imp_bits__8_1_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 1 ) {
        DSPC = PC_ST_VLES + signExtend ( concat ( a8 , zero ( 1 ) ) , 32 ) ;
        Fetch = Fetch :: DelaySlot ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"PC_ST_VLES?",
"SR[mask 0x00000002]/p",
],
"outputs" => [
"DSPC?",
"Fetch?",
],
},
"BTD__label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "btd a20",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_26_24_x_20_19_x_15_13_" => 668,
"reserved_imp_bits__34_32_" => "reserved",
"a20_imp_bits__27_27_x_18_16_x_0_0_x_23_21_x_12_1_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 1 ) {
        DSPC = PC_ST_VLES + signExtend ( concat ( a20 , zero ( 1 ) ) , 32 ) ;
        Fetch = Fetch :: DelaySlot ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"PC_ST_VLES?",
"SR[mask 0x00000002]/p",
],
"outputs" => [
"DSPC?",
"Fetch?",
],
},
"BT___label" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "bt a8",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_9_x_0_0_" => 129,
"reserved_imp_bits__18_16_" => "reserved",
"a8_imp_bits__8_1_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 1 ) {
        NVPC = PC_ST_VLES + signExtend ( concat ( a8 , zero ( 1 ) ) , 32 ) ;
        jump_instr = true ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"PC_ST_VLES?",
"SR[mask 0x00000002]/p",
],
"outputs" => [
"NVPC?",
],
},
"BT__label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "bt a20",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_26_24_x_20_19_x_15_13_" => 700,
"reserved_imp_bits__34_32_" => "reserved",
"a20_imp_bits__27_27_x_18_16_x_0_0_x_23_21_x_12_1_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 1 ) {
        NVPC = PC_ST_VLES + signExtend ( concat ( a20 , zero ( 1 ) ) , 32 ) ;
        jump_instr = true ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"PC_ST_VLES?",
"SR[mask 0x00000002]/p",
],
"outputs" => [
"NVPC?",
],
},
"CAST_F_Da_WH_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "cast.f JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1824,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"CAST_F_Da_WL_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "cast.f JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1825,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"CLB_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "clb JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_3_" => 834,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 32 > res = 0 ;
     var src = D ( JJJ ) ;
    if ( src == 0 ) {
        res = 0 ;
    } else {
        if ( src ( 39 ) == 0 ) {
            src = ~ src ;
        }
        res = 9 - NumberOfOnes ( src ) ;
    }
    D ( FFF ) = signExtend ( res , 40 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"NumberOfOnes",
],
},
"CLIP20__e_u2_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "clip20 #e_u2,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DLU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3591,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"e_u2_imp_bits__1_1_x_0_0_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"CLIP20__u4_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "clip20 #u4,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DLU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_" => 913,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u4_imp_bits__3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"CLIP2__u4_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "clip2 #u4,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_" => 444,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u4_imp_bits__3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bool dovf = false ;
     bits < 20 > low_src = zeroExtend ( ( D ( FFF ) . L ) , 20 ) ;
     bits < 20 > high_src = zeroExtend ( ( D ( FFF ) . H ) , 20 ) ;
    if ( ( D ( FFF ) . L ) . signedLT ( 0 ) ) {
        low_src = 0 ;
        dovf = true ;
    }
    if ( ( D ( FFF ) . H ) . signedLT ( 0 ) ) {
        high_src = 0 ;
        dovf = true ;
    }
     bits < 1 > last_low = ( u4 == 0 ) ? 0 : low_src ( u4 - 1 ) ;
     bits < 1 > last_high = ( u4 == 0 ) ? 0 : high_src ( u4 - 1 ) ;
    low_src >>= u4 ;
    high_src >>= u4 ;
    low_src = low_src + last_low ;
    high_src = high_src + last_high ;
    D ( FFF ) . WL = saturation8_nochk ( zeroExtend ( low_src , 40 ) ) ;
    D ( FFF ) . WH = saturation8_nochk ( zeroExtend ( high_src , 40 ) ) ;
    if ( dovf ) set_emr_dovf (  ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
],
"helpers" => [
"IsUserMode",
"saturation8_nochk",
"set_emr_dovf",
],
},
"CLIP__u5_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "clip #u5,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 218,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > src = D ( FFF ) ;
    if ( D ( FFF ) . signedLT ( 0 ) ) {
        src = 0 ;
        set_emr_dovf (  ) ;
    }
    SR . C = ( u5 == 0 ) ? 0 : D ( FFF ) ( u5 - 1 ) ;
     bits < 1 > carry = ( u5 == 0 ) ? 0 : src ( u5 - 1 ) ;
    src >>= u5 ;
    src = src + carry ;
    D ( FFF ) = saturation8_nochk ( src ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"saturation8_nochk",
"set_emr_dovf",
],
},
"CMPEQA_W__s16_Rn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpeqa.w #s16,RRR",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_27_x_20_16_x_15_13_" => 1068,
"RRR_imp_bits__34_34_x_26_24_" => "",
"reserved_imp_bits__33_32_" => "reserved",
"s16_imp_bits__23_21_x_12_0_" => "",
},
"action" => <<DATA
{
    if ( R ( RRR ) == signExtend ( s16 , 32 ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPEQA_W__u5_Rn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "cmpeqa.w #u5,RRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_4_" => 199,
"RRR_imp_bits__18_18_x_11_9_" => "",
"reserved_imp_bits__17_16_" => "reserved",
"u5_imp_bits__8_8_x_3_0_" => "",
},
"action" => <<DATA
{
    if ( R ( RRR ) == zeroExtend ( u5 , 32 ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"R(RRR)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPEQA_rx_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "cmpeqa rrrr,RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_4_" => 234,
"RRRR_imp_bits__18_18_x_11_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"rrrr_imp_bits__16_16_x_3_0_" => "",
},
"action" => <<DATA
{
    if ( rxTable ( rrrr ) == RxTable ( RRRR ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
"SR/p",
"rxTable(rrrr)",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPEQD_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpeqd JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1815040,
"jjj_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"JJJ_imp_bits__33_33_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    if ( D ( JJJ ) == D ( jjj ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
"D(jjj)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPEQD__s16_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpeqd #s16,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_" => 1252,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__24_24_x_23_23_x_20_20_x_19_19_x_22_22_x_21_21_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( D ( JJJ ) == signExtend ( s16 , 40 ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPEQD__u12_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpeqd #u12,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_" => 3556,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"u12_imp_bits__22_22_x_21_21_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( D ( JJJ ) == zeroExtend ( u12 , 40 ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPEQ_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "cmpeq JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 204,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    if ( D ( JJJ ) == D ( FFF ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPEQ_L_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "cmpeq.l JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 205,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    if ( concat ( D ( JJJ ) . H , D ( JJJ ) . L ) == concat ( D ( FFF ) . H , D ( FFF ) . L ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
"D(JJJ)[mask 0x00ffffffff]/p",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPEQ_W__s16_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpeq.w #s16,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 1684,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( D ( FFF ) == signExtend ( s16 , 40 ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(FFF)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPEQ_W__u5_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "cmpeq.w #u5,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 209,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( D ( FFF ) == zeroExtend ( u5 , 40 ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPGED_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpged JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1815424,
"jjj_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"JJJ_imp_bits__33_33_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    if ( D ( JJJ ) . signedGE ( D ( jjj ) ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
"D(jjj)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPGED__s16_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpged #s16,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_" => 1255,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__24_24_x_23_23_x_20_20_x_19_19_x_22_22_x_21_21_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > imm = signExtend ( s16 , 40 ) ;
    if ( D ( JJJ ) . signedGE ( imm ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPGED__u12_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpged #u12,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_" => 3559,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"u12_imp_bits__22_22_x_21_21_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > imm = zeroExtend ( u12 , 40 ) ;
    if ( D ( JJJ ) . signedGE ( imm ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPGTA_W__s16_Rn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpgta.w #s16,RRR",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 1076,
"RRR_imp_bits__34_34_x_26_26_x_25_25_x_24_24_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"s16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( R ( RRR ) . signedGT ( signExtend ( s16 , 32 ) ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPGTA_W__u5_Rn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "cmpgta.w #u5,RRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_" => 235,
"RRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
"u5_imp_bits__8_8_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( R ( RRR ) . signedGT ( zeroExtend ( u5 , 32 ) ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"R(RRR)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPGTA_rx_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "cmpgta rrrr,RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_" => 233,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"rrrr_imp_bits__16_16_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( RxTable ( RRRR ) . signedGT ( rxTable ( rrrr ) ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
"SR/p",
"rxTable(rrrr)",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPGTD_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpgtd JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1815296,
"jjj_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"JJJ_imp_bits__33_33_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    if ( D ( JJJ ) . signedGT ( D ( jjj ) ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
"D(jjj)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPGTD__s16_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpgtd #s16,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_" => 1254,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__24_24_x_23_23_x_20_20_x_19_19_x_22_22_x_21_21_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > imm = signExtend ( s16 , 40 ) ;
    if ( D ( JJJ ) . signedGT ( imm ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPGTD__u12_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpgtd #u12,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_" => 3558,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"u12_imp_bits__22_22_x_21_21_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > imm = zeroExtend ( u12 , 40 ) ;
    if ( D ( JJJ ) . signedGT ( imm ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPGT_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "cmpgt JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 206,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    if ( D ( FFF ) . signedGT ( D ( JJJ ) ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPGT_W__s16_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpgt.w #s16,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 1748,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > src = signExtend ( s16 , 40 ) ;
    if ( D ( FFF ) . signedGT ( src ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(FFF)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPGT_W__u5_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "cmpgt.w #u5,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 213,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > src = zeroExtend ( u5 , 40 ) ;
    if ( D ( FFF ) . signedGT ( src ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPHIA_rx_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "cmphia rrrr,RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_" => 232,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"rrrr_imp_bits__16_16_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 33 > val1 = zeroExtend ( RxTable ( RRRR ) , 33 ) ;
     bits < 33 > val2 = zeroExtend ( rxTable ( rrrr ) , 33 ) ;
    if ( val1 . signedGT ( val2 ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
"SR/p",
"rxTable(rrrr)",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPHI_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "cmphi JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 207,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 33 > src1 = zeroExtend ( D ( JJJ ) ( 31 , 0 ) , 40 ) ;
     bits < 33 > src2 = zeroExtend ( D ( FFF ) ( 31 , 0 ) , 40 ) ;
    if ( src2 . signedGT ( src1 ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
"D(JJJ)[mask 0x00ffffffff]/p",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"CMPLED__s16_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpled #s16,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_" => 1250,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__24_24_x_23_23_x_20_20_x_19_19_x_22_22_x_21_21_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > imm = signExtend ( s16 , 40 ) ;
    if ( D ( JJJ ) . signedLE ( imm ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPLED__u12_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpled #u12,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_" => 3554,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"u12_imp_bits__22_22_x_21_21_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > imm = zeroExtend ( u12 , 40 ) ;
    if ( D ( JJJ ) . signedLE ( imm ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPLTD__s16_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpltd #s16,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_" => 1251,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__24_24_x_23_23_x_20_20_x_19_19_x_22_22_x_21_21_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > imm = signExtend ( s16 , 40 ) ;
    if ( D ( JJJ ) . signedLT ( imm ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPLTD__u12_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpltd #u12,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_" => 3555,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"u12_imp_bits__22_22_x_21_21_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > imm = zeroExtend ( u12 , 40 ) ;
    if ( D ( JJJ ) . signedLT ( imm ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPNED_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpned JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1815168,
"jjj_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"JJJ_imp_bits__33_33_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    if ( D ( JJJ ) != D ( jjj ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
"D(jjj)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPNED__s16_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpned #s16,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_" => 1253,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__24_24_x_23_23_x_20_20_x_19_19_x_22_22_x_21_21_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( D ( JJJ ) != signExtend ( s16 , 40 ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
],
},
"CMPNED__u12_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cmpned #u12,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_" => 3557,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"u12_imp_bits__22_22_x_21_21_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( D ( JJJ ) != zeroExtend ( u12 , 40 ) ) {
        D ( FFF ) = 0xffffffffffull ;
    } else {
        D ( FFF ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
],
},
"COB_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "cob JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DLU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1828,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = zeroExtend ( NumberOfOnes32 ( D ( JJJ ) ( 31 , 0 ) ) , 40 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"NumberOfOnes32",
],
},
"CONTD_label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "contd a16",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_24_x_20_16_x_15_13_" => 8732,
"reserved_imp_bits__34_32_" => "reserved",
"a16_imp_bits__0_0_x_23_21_x_12_1_" => "",
},
"action" => <<DATA
{
     uint32_t active_loop = FindActiveLoop (  ) ;
    if ( ( ( LC ( active_loop ) & 0xFFFFFFFE ) != 0 ) && ( LC ( active_loop ) ( 31 ) == 0 ) ) {
        DSPC = SA ( active_loop ) ;
        Fetch = Fetch :: DelaySlot ;
        LC ( active_loop ) = LC ( active_loop ) - 1 ;
    } else {
        DSPC = PC_ST_VLES + signExtend ( concat ( a16 , zero ( 1 ) ) , 32 ) ;
        Fetch = Fetch :: DelaySlot ;
        clear_SR_LF ( active_loop ) ;
        LC ( active_loop ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"LC",
"PC_ST_VLES?",
"SA?",
"SR[mask 0x78020000]/p",
],
"outputs" => [
"DSPC",
"Fetch",
"LC",
"SR[mask 0x78000000]/p?",
],
"helpers" => [
"FindActiveLoop",
"clear_SR_LF",
],
},
"CONT_label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "cont a16",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_24_x_20_16_x_15_13_" => 8988,
"reserved_imp_bits__34_32_" => "reserved",
"a16_imp_bits__0_0_x_23_21_x_12_1_" => "",
},
"action" => <<DATA
{
     uint32_t active_loop = FindActiveLoop (  ) ;
    jump_instr = true ;
    if ( ( ( LC ( active_loop ) & 0xFFFFFFFE ) != 0 ) && ( LC ( active_loop ) ( 31 ) == 0 ) ) {
        NVPC = SA ( active_loop ) ;
        LC ( active_loop ) = LC ( active_loop ) - 1 ;
    } else {
        NVPC = PC_ST_VLES + signExtend ( concat ( a16 , zero ( 1 ) ) , 32 ) ;
        clear_SR_LF ( active_loop ) ;
        LC ( active_loop ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"LC",
"PC_ST_VLES?",
"SA?",
"SR[mask 0x78020000]/p",
],
"outputs" => [
"LC",
"NVPC",
"SR[mask 0x78000000]/p?",
],
"helpers" => [
"FindActiveLoop",
"clear_SR_LF",
],
},
"DEBUG" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "debug",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_APC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40560,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( EMR . SDD == 0 ) {
        halt (  ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR[mask 0x01000000]/p",
"SR/p",
],
"helpers" => [
"IsUserMode",
],
},
"DEBUGEV" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "debugev",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_APC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40561,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    halt (  ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"DECA_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "DECA_Rx",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"aliases" => {
"SUBA__u5_Rx" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => 1,
},
],
"parent_action" => <<DATA
{
     bits < 32 > imm = zeroExtend ( u5 , 32 ) ;
    if ( RRRR ( 4 ) == 0 && RRRR ( 3 ) == 1 ) {
        RxTable ( RRRR ) = Address_by_MCTL ( RRRR , imm , 1 , 1 , 1 , imm ) ;
    } else {
        RxTable ( RRRR ) = RxTable ( RRRR ) - imm ;
    }
}
DATA
,
},
},
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"RxTable(RRRR)",
"rxTable(RRRR)?",
],
"outputs" => [
"RxTable(RRRR)",
],
"helpers" => [
"Address_by_MCTL",
"bitreverse",
"preshift",
],
},
"DECEQA_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "deceqa RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 3830,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
     bits < 32 > val = RxTable ( RRRR ) - 1 ;
    RxTable ( RRRR ) = val ;
    if ( val == 0 ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
"SR/p",
],
"outputs" => [
"RxTable(RRRR)",
"SR[mask 0x00000002]/p",
],
},
"DECEQ_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "deceq FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 1261,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 40 > imm = 0x0000000001ull ;
     bits < 41 > sub = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) imm ;
    overflow40 ( sub ) ;
    Borrow ( sub , imm , D ( FFF ) ) ;
    D ( FFF ) = sub ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
    SR . T = ( sub == 0 ) ? 1 : 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000003]/p",
],
"helpers" => [
"Borrow",
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"DECGEA_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "decgea RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 3831,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
     bits < 32 > val = RxTable ( RRRR ) - 1 ;
    RxTable ( RRRR ) = val ;
    if ( val . signedGE ( 0 ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
"SR/p",
],
"outputs" => [
"RxTable(RRRR)",
"SR[mask 0x00000002]/p",
],
},
"DECGE_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "decge FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 1260,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 40 > imm = 0x0000000001ull ;
     bits < 41 > sub = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) imm ;
    overflow40 ( sub ) ;
    Borrow ( sub , imm , D ( FFF ) ) ;
    D ( FFF ) = sub ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
    SR . T = ( sub . signedGE ( 0 ) ) ? 1 : 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000003]/p",
],
"helpers" => [
"Borrow",
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"DF05_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df05.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506827,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF06_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df06.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506829,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF09_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df09.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506835,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF0A_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df0a.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506837,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF0B_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df0b.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506839,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF0D_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df0d.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506843,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF0E_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df0e.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506845,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF0F_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df0f.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506847,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF11_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df11.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506851,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF12_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df12.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506853,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF13_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df13.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506855,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF15_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df15.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506859,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF16_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df16.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506861,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF17_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df17.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506863,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF18_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df18.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506865,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF19_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df19.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506867,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF1A_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df1a.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506869,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF1B_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df1b.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506871,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF1C_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df1c.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506873,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF1D_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df1d.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506875,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF1E_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df1e.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506877,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DF1F_V__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "df1f.v (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506879,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DFETCHW__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "dfetchw (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506857,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DFETCH__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "dfetch (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506825,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DFL2C0__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "dfl2c0 (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506817,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DFL2C1__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "dfl2c1 (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506819,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DFL2C2__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "dfl2c2 (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506821,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DFL2C3__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "dfl2c3 (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506823,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DFLUSH__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "dflush (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506833,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DI" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "di",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_APC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40829,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        SR . DI = 1 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
"SR[mask 0x00080000]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"DIVP0_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "divp0 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 681,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"DIVP1_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "divp1 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 682,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"DIVP2_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "divp2 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 683,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"DIVP3_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "divp3 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 685,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"DIV_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "div JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 202,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 40 > tmp ;
     bits < 40 > op1 = ( D ( FFF ) << 1 ) ;
     bits < 40 > op2 = ( D ( JJJ ) & 0xFFFFFF0000ull ) ;
    if ( ( ( D ( FFF ) ^ D ( JJJ ) ) >> 39 ) == 1 ) {
        tmp = op1 + ( bits < 40 >  ) SR . C + op2 ;
    } else {
        tmp = op1 + ( bits < 40 >  ) SR . C - op2 ;
    }
    if ( ( tmp >> 39 ) == 0 ) SR . C = 1 ; else SR . C = 0 ;
    if ( ( D ( FFF ) ( 39 ) ^ D ( FFF ) ( 38 ) ) == 1 ) set_emr_dovf (  ) ;
    D ( FFF ) = tmp ;
    L ( FFF ) = LnComputation_special_instr ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)",
"EMR/p",
"SR[mask 0x00060035]/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"LnComputation_nochk",
"LnComputation_special_instr",
"set_emr_dovf",
],
},
"DMACSS_Dc_Dd_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "dmacss ee,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 381,
"ee_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( ee . Dc ) ( 31 , 16 ) , 40 ) , signExtend ( D ( ee . Dd ) ( 31 , 16 ) , 40 ) ) ) << 1 ;
     bits < 41 > tmp = signExtend ( ( ( D ( FFF ) >> 16 ) ( 23 , 0 ) ) , 41 ) ;
    tmp = ( sbits < 41 >  ) tmp + ( sbits < 41 >  ) mult ;
    D ( FFF ) = tmp ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(ee . Dc)[mask 0x00ffff0000]/p",
"D(ee . Dd)[mask 0x00ffff0000]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"DMACSU_Dc_Dd_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "dmacsu ee,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 380,
"ee_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( ee . Dc ) ( 31 , 16 ) , 40 ) , zeroExtend ( D ( ee . Dd ) ( 15 , 0 ) , 40 ) ) ) << 1 ;
     bits < 41 > tmp = signExtend ( ( ( D ( FFF ) >> 16 ) ( 23 , 0 ) ) , 41 ) ;
    tmp = ( sbits < 41 >  ) tmp + ( sbits < 41 >  ) mult ;
    D ( FFF ) = tmp ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(ee . Dc)[mask 0x00ffff0000]/p",
"D(ee . Dd)[mask 0x000000ffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"DMAC_mDa_H_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "dmac -JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_" => 91,
"jjj_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"DMAC_pDa_H_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "dmac JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_" => 89,
"jjj_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"DMALLOC__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "dmalloc (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506849,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DMPY_Da_H_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "dmpy JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_" => 94,
"jjj_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"DOALIGN_Ds_Dt_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "doalign GGG,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_3_" => 890,
"GGG_imp_bits__18_17_x_2_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
    switch ( ( GCR . BAM ) . uint32 (  ) ) {
        case 1 : D ( FFF ) = concat ( D ( FFF ) ( 39 , 32 ) , D ( GGG . Dt ) ( 7 , 0 ) , D ( GGG . Ds ) ( 31 , 8 ) ) ;
        break ;
        case 2 : D ( FFF ) = concat ( D ( FFF ) ( 39 , 32 ) , D ( GGG . Dt ) ( 15 , 0 ) , D ( GGG . Ds ) ( 31 , 16 ) ) ;
        break ;
        case 3 : D ( FFF ) = concat ( D ( FFF ) ( 39 , 32 ) , D ( GGG . Dt ) ( 23 , 0 ) , D ( GGG . Ds ) ( 31 , 24 ) ) ;
        break ;
        case 5 : D ( FFF ) = concat ( D ( FFF ) ( 39 , 32 ) , D ( GGG . Ds ) ( 23 , 0 ) , D ( GGG . Dt ) ( 31 , 24 ) ) ;
        break ;
        case 6 : D ( FFF ) = concat ( D ( FFF ) ( 39 , 32 ) , D ( GGG . Ds ) ( 15 , 0 ) , D ( GGG . Dt ) ( 31 , 16 ) ) ;
        break ;
        case 7 : D ( FFF ) = concat ( D ( FFF ) ( 39 , 32 ) , D ( GGG . Ds ) ( 7 , 0 ) , D ( GGG . Dt ) ( 31 , 8 ) ) ;
        break ;
        default : D ( FFF ) = concat ( D ( FFF ) ( 39 , 32 ) , D ( GGG . Ds ) ( 31 , 0 ) ) ;
        break ;
    }
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0xff00000000]/p?",
"D(GGG . Ds)[mask 0x00ffffffff]/p?",
"D(GGG . Dt)[mask 0x00ffffffff]/p?",
"GCR[mask 0x00000007]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"DOENSHn_DR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "doenshnn HHHH",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_7_4_" => 628,
"reserved_imp_bits__18_17_" => "reserved",
"HHHH_imp_bits__16_16_x_3_0_" => "",
"nn_imp_bits__9_8_" => "",
},
"action" => <<DATA
{
    LC ( nn ) = DRTable ( HHHH ) ( 32 - 1 , 0 ) ;
    set_SR_LF ( nn ) ;
    SR . SLF = 1 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"DRTable(HHHH)[mask 0x00ffffffff]/p",
"SR/p",
],
"outputs" => [
"LC(nn)",
"SR[mask 0xf8000000]/p",
],
"helpers" => [
"set_SR_LF",
],
},
"DOENSHn__u16" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "doenshnn u16",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_26_x_20_16_x_15_13_" => 2340,
"reserved_imp_bits__34_32_" => "reserved",
"nn_imp_bits__25_24_" => "",
"u16_imp_bits__23_21_x_12_0_" => "",
},
"action" => <<DATA
{
    LC ( nn ) = zeroExtend ( u16 , 32 ) ;
    set_SR_LF ( nn ) ;
    SR . SLF = 1 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"SR/p",
],
"outputs" => [
"LC(nn)",
"SR[mask 0xf8000000]/p",
],
"helpers" => [
"set_SR_LF",
],
},
"DOENSHn__u6" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "doenshnn u6",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_7_6_" => 149,
"reserved_imp_bits__18_16_" => "reserved",
"nn_imp_bits__9_8_" => "",
"u6_imp_bits__5_0_" => "",
},
"action" => <<DATA
{
    LC ( nn ) = zeroExtend ( u6 , 32 ) ;
    set_SR_LF ( nn ) ;
    SR . SLF = 1 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"SR/p",
],
"outputs" => [
"LC(nn)",
"SR[mask 0xf8000000]/p",
],
"helpers" => [
"set_SR_LF",
],
},
"DOENn_DR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "doennn HHHH",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_7_4_" => 612,
"reserved_imp_bits__18_17_" => "reserved",
"HHHH_imp_bits__16_16_x_3_0_" => "",
"nn_imp_bits__9_8_" => "",
},
"action" => <<DATA
{
    LC ( nn ) = DRTable ( HHHH ) ( 32 - 1 , 0 ) ;
    set_SR_LF ( nn ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"DRTable(HHHH)[mask 0x00ffffffff]/p",
"SR/p",
],
"outputs" => [
"LC(nn)",
"SR[mask 0x78000000]/p?",
],
"helpers" => [
"set_SR_LF",
],
},
"DOENn__u16" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "doennn u16",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_26_x_20_16_x_15_13_" => 2084,
"reserved_imp_bits__34_32_" => "reserved",
"nn_imp_bits__25_24_" => "",
"u16_imp_bits__23_21_x_12_0_" => "",
},
"action" => <<DATA
{
    LC ( nn ) = zeroExtend ( u16 , 32 ) ;
    set_SR_LF ( nn ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"SR/p",
],
"outputs" => [
"LC(nn)",
"SR[mask 0x78000000]/p?",
],
"helpers" => [
"set_SR_LF",
],
},
"DOENn__u6" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "doennn u6",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_7_6_" => 145,
"reserved_imp_bits__18_16_" => "reserved",
"nn_imp_bits__9_8_" => "",
"u6_imp_bits__5_0_" => "",
},
"action" => <<DATA
{
    LC ( nn ) = zeroExtend ( u6 , 32 ) ;
    set_SR_LF ( nn ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"SR/p",
],
"outputs" => [
"LC(nn)",
"SR[mask 0x78000000]/p?",
],
"helpers" => [
"set_SR_LF",
],
},
"DOSETUPn_label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "DOSETUPn_labelnn a16",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_26_x_20_16_x_15_13_" => 2588,
"reserved_imp_bits__34_32_" => "reserved",
"nn_imp_bits__25_24_" => "",
"a16_imp_bits__0_0_x_23_21_x_12_1_" => "",
},
"action" => <<DATA
{
     bits < 17 > disp = a16 * 2 ;
    SA ( nn ) = PC_ST_VLES + signExtend ( disp , 32 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"PC_ST_VLES",
],
"outputs" => [
"SA(nn)",
],
},
"DQUERY__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "dquery (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506831,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"DSYNC__EA_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "dsync (RRRMMM",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 73506841,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"MMM_imp_bits__21_21_x_20_20_x_19_19_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"EI" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "ei",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_APC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40828,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        SR . DI = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
"SR[mask 0x00080000]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"EORA_rx_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "eora rrrr,RRRR",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_" => 510,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"rrrr_imp_bits__16_16_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    RxTable ( RRRR ) = rxTable ( rrrr ) ^ RxTable ( RRRR ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
"rxTable(rrrr)",
],
"outputs" => [
"RxTable(RRRR)",
],
},
"EOR_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "eor JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_3_" => 882,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = D ( FFF ) ^ D ( JJJ ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"EOR_W__u16__Rn_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "EOR_W__u16__Rn_ u16, RRR",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCHG_W__u16__Rn_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_",
"value" => "RRR",
},
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "u16",
},
],
"parent_action" => <<DATA
{
    DMEM ( R ( RRR ) , 2 ) = DMEM ( R ( RRR ) , 2 ) ^ ( u16 ) ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"EOR_W__u16__SPmu5_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "EOR_W__u16__SPmu5_ u16, u5",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCHG_W__u16__SPmu5_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "u16",
},
"misc" => {
"field" => "u5_imp_bits__20_20_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "u5",
},
],
"parent_action" => <<DATA
{
     bits < 6 > u6 = concat ( u5 , zero ( 1 ) ) ;
     bits < 32 > offset = concat ( zero ( 26 ) , u6 ) ;
    DMEM ( ( SP - offset ) , 2 ) = DMEM ( ( SP - offset ) , 2 ) ^ ( u16 ) ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"EOR_W__u16__SPps16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "EOR_W__u16__SPps16_ u16, s16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCHG_W__u16__SPps16_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "s16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "s16",
},
"misc" => {
"field" => "u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "u16",
},
],
"parent_action" => <<DATA
{
     var offset = signExtend ( s16 , 32 ) ;
    DMEM ( ( SP + offset ) , 2 ) = ( DMEM ( ( SP + offset ) , 2 ) ) ^ ( u16 ) ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"EOR_W__u16__a16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "EOR_W__u16__a16_ u16, a16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCHG_W__u16__a16_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "a16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "a16",
},
"misc" => {
"field" => "u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "u16",
},
],
"parent_action" => <<DATA
{
    DMEM ( a16 , 2 ) = ( DMEM ( a16 , 2 ) ) ^ ( u16 ) ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "ThreeWords",
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"EOR__u16_DR_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "EOR__u16_DR_H HHHH, u16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCHG__u16_DR_H" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "HHHH",
},
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "u16",
},
],
"parent_action" => <<DATA
{
    if ( HHHH ( 3 ) == 1 ) {
        DRTable ( HHHH ) = DRTable ( HHHH ) ^ ( concat ( u16 , zero ( 16 ) ) ) ;
    } else {
        DRTable ( HHHH ) = DRTable ( HHHH ) ^ ( concat ( zero ( 8 ) , u16 , zero ( 16 ) ) ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"EOR__u16_DR_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "EOR__u16_DR_L HHHH, u16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCHG__u16_DR_L" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "HHHH",
},
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "u16",
},
],
"parent_action" => <<DATA
{
    if ( HHHH ( 3 , 3 ) == 1 ) {
        DRTable ( HHHH ) = DRTable ( HHHH ) ^ ( zeroExtend ( u16 , 32 ) ) ;
    } else {
        DRTable ( HHHH ) = DRTable ( HHHH ) ^ ( zeroExtend ( u16 , 40 ) ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"ESG_Prefix" => {
"width" => 16,
"syntax" => "ESG_Prefix reserved_imp_bits__14_14_, yyyyyy_imp_bits__5_0_",
"attributes" => {
"prefix" => "",
"esg_prefix" => "",
},
"fields" => {
"VarPrefixOpcode_imp_bits__15_15_x_13_12_x_11_6_" => 192,
"reserved_imp_bits__14_14_" => "reserved",
"yyyyyy_imp_bits__5_0_" => "",
},
"action" => <<DATA
{
}
DATA
,
"prefix" => 1,
"disassemble" => 1,
},
"EXTRACTU_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "extractu JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_24_19_x_15_3_" => 1716224,
"jjj_imp_bits__34_34_x_27_25_" => "",
"JJJ_imp_bits__33_33_x_2_0_" => "",
"FFF_imp_bits__32_32_x_18_16_" => "",
},
"action" => <<DATA
{
     bool sign_extract = false ;
    extract_f ( FFF , jjj , D ( JJJ ) ( 13 , 8 ) , D ( JJJ ) ( 5 , 0 ) , sign_extract ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)[mask 0x0000003f3f]/p",
"D(jjj)",
],
"outputs" => [
"D(FFF)?",
"L(FFF)",
],
"helpers" => [
"extract_f",
"modulo_slice",
"slice_signExtend",
],
},
"EXTRACTU__U6__u6_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "extractu #U6,#u6,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_24_19_x_15_12_" => 3480,
"jjj_imp_bits__34_34_x_27_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_16_" => "",
"U6_imp_bits__11_6_" => "",
"u6_imp_bits__5_0_" => "",
},
"action" => <<DATA
{
     bool sign_extract = false ;
    extract_f ( FFF , jjj , U6 , u6 , sign_extract ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(jjj)",
],
"outputs" => [
"D(FFF)?",
"L(FFF)",
],
"helpers" => [
"extract_f",
"modulo_slice",
"slice_signExtend",
],
},
"EXTRACT_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "extract JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_24_19_x_15_3_" => 1716736,
"jjj_imp_bits__34_34_x_27_25_" => "",
"JJJ_imp_bits__33_33_x_2_0_" => "",
"FFF_imp_bits__32_32_x_18_16_" => "",
},
"action" => <<DATA
{
     bool sign_extract = true ;
    extract_f ( FFF , jjj , D ( JJJ ) ( 13 , 8 ) , D ( JJJ ) ( 5 , 0 ) , sign_extract ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)[mask 0x0000003f3f]/p",
"D(jjj)",
],
"outputs" => [
"D(FFF)?",
"L(FFF)",
],
"helpers" => [
"extract_f",
"modulo_slice",
"slice_signExtend",
],
},
"EXTRACT__U6__u6_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "extract #U6,#u6,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_24_19_x_15_12_" => 3481,
"jjj_imp_bits__34_34_x_27_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_16_" => "",
"U6_imp_bits__11_6_" => "",
"u6_imp_bits__5_0_" => "",
},
"action" => <<DATA
{
     bool sign_extract = true ;
    extract_f ( FFF , jjj , U6 , u6 , sign_extract ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(jjj)",
],
"outputs" => [
"D(FFF)?",
"L(FFF)",
],
"helpers" => [
"extract_f",
"modulo_slice",
"slice_signExtend",
],
},
"FBTB" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "fbtb",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_APC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40573,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"IADDNC_W__s16_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "iaddnc.w #s16,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 1876,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > D_imm = signExtend ( s16 , 40 ) ;
     bits < 41 > sum = ( sbits < 41 >  ) D_imm + ( sbits < 41 >  ) D ( FFF ) ;
    overflow40 ( sum ) ;
    D ( FFF ) = sum ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(FFF)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"IADD_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "iadd JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 856,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > sum = ( sbits < 41 >  ) D ( JJJ ) + ( sbits < 41 >  ) D ( FFF ) ;
    overflow40 ( sum ) ;
    D ( FFF ) = sum ( 39 , 0 ) ;
     bits < 40 > Gcarry = Carry ( D ( JJJ ) , D ( FFF ) , 0 ) ;
    SR . C = Gcarry ( 40 - 1 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"IADD__u5_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "iadd #u5,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 211,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > D_imm = zeroExtend ( u5 , 40 ) ;
     bits < 41 > sum = ( sbits < 41 >  ) D_imm + ( sbits < 41 >  ) D ( FFF ) ;
    overflow40 ( sum ) ;
    D ( FFF ) = sum ( 39 , 0 ) ;
     bits < 40 > Gcarry = Carry ( D_imm , D ( FFF ) , 0 ) ;
    SR . C = Gcarry ( 40 - 1 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"IDMPYS_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "idmpys jjj,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 2035712,
"jjj_imp_bits__34_34_x_23_23_x_22_22_x_21_21_" => "",
"JJJ_imp_bits__33_33_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 40 > mult1 = signExtend ( signedMultiply ( ( zeroExtend ( D ( JJJ ) . H , 40 ) ) , ( signExtend ( D ( jjj ) . L , 40 ) ) ) , 40 ) ;
     bits < 40 > mult2 = signExtend ( signedMultiply ( ( zeroExtend ( D ( JJJ ) . L , 40 ) ) , ( signExtend ( D ( jjj ) . H , 40 ) ) ) , 40 ) ;
     bits < 40 > res = ( mult1 + mult2 ) ;
    res <<= 16 ;
    D ( FFF ) = res ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
"D(jjj)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"IDMPYU_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "idmpyu jjj,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1970176,
"jjj_imp_bits__34_34_x_23_23_x_22_22_x_21_21_" => "",
"JJJ_imp_bits__33_33_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 40 > mult1 = ( ( zeroExtend ( D ( JJJ ) . L , 40 ) ) * ( zeroExtend ( D ( jjj ) . H , 40 ) ) ) ;
     bits < 40 > mult2 = ( ( zeroExtend ( D ( JJJ ) . H , 40 ) ) * ( zeroExtend ( D ( jjj ) . L , 40 ) ) ) ;
     bits < 40 > res = ( mult1 + mult2 ) ;
    res <<= 16 ;
    D ( FFF ) = res ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
"D(jjj)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"ILLEGAL" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "illegal",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_APC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40572,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    DMEM ( ESP , 4 ) = PC_ST_VLES ;
    DMEM ( ESP + 4 , 4 ) = SR ;
    SP = SP + 8 ;
    jump_instr = true ;
    NVPC = concat ( VBA ( 31 , 12 ) , 0x080 ) ;
    trap_f (  ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"ESP?",
"NSP?",
"PC_ST_VLES",
"SP",
"SR/p",
"VBA[mask 0xfffff000]/p",
],
"outputs" => [
"EMR/p?",
"ESP?",
"NSP?",
"NVPC",
"SP",
"SR[mask 0xfbe60043]/p",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"trap_f",
],
"raises_exception" => 1,
},
"IMACLHUU_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "imaclhuu jjj,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1839104,
"JJJ_imp_bits__34_34_x_2_2_x_1_1_x_0_0_" => "",
"jjj_imp_bits__33_33_x_23_23_x_22_22_x_21_21_" => "",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( zeroExtend ( D ( jjj ) . L , 41 ) * zeroExtend ( D ( JJJ ) . H , 41 ) ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = tmp ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(FFF)",
"D(JJJ)[mask 0x00ffff0000]/p",
"D(jjj)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"IMACSU2_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "imacsu2 JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_" => 88,
"jjj_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 17 > mult_high = signExtend ( signedMultiply ( zeroExtend ( D ( jjj ) ( 23 , 16 ) , 16 ) , signExtend ( D ( JJJ ) ( 23 , 16 ) , 16 ) ) , 17 ) ;
     bits < 17 > mult_low = signExtend ( signedMultiply ( zeroExtend ( D ( jjj ) ( 7 , 0 ) , 16 ) , signExtend ( D ( JJJ ) ( 7 , 0 ) , 16 ) ) , 17 ) ;
     bits < 17 > sum_high = ( sbits < 17 >  ) D ( FFF ) . H + ( sbits < 17 >  ) mult_high ;
     bits < 17 > sum_low = ( sbits < 17 >  ) D ( FFF ) . L + ( sbits < 17 >  ) mult_low ;
    D ( FFF ) . WH = signExtend ( saturation16_noexception ( signExtend ( sum_high , 40 ) ) , 20 ) ;
    D ( FFF ) . WL = signExtend ( saturation16_noexception ( signExtend ( sum_low , 40 ) ) , 20 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
"D(JJJ)[mask 0x0000ff00ff]/p",
"D(jjj)[mask 0x0000ff00ff]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"saturation16_nochk",
"saturation16_noexception",
],
},
"IMACUS_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "imacus jjj,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1576960,
"JJJ_imp_bits__34_34_x_2_2_x_1_1_x_0_0_" => "",
"jjj_imp_bits__33_33_x_23_23_x_22_22_x_21_21_" => "",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = signExtend ( signedMultiply ( zeroExtend ( D ( jjj ) . L , 40 ) , signExtend ( D ( JJJ ) . H , 40 ) ) , 41 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = tmp ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(FFF)",
"D(JJJ)[mask 0x00ffff0000]/p",
"D(jjj)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"IMACUU_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "imacuu jjj,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1642496,
"jjj_imp_bits__34_34_x_23_23_x_22_22_x_21_21_" => "",
"JJJ_imp_bits__33_33_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( zeroExtend ( D ( jjj ) . L , 41 ) * zeroExtend ( D ( JJJ ) . L , 41 ) ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = tmp ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(FFF)",
"D(JJJ)[mask 0x000000ffff]/p",
"D(jjj)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"IMPYHLUU_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "impyhluu jjj,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1904640,
"JJJ_imp_bits__34_34_x_2_2_x_1_1_x_0_0_" => "",
"jjj_imp_bits__33_33_x_23_23_x_22_22_x_21_21_" => "",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 32 > imult = ( ( zeroExtend ( D ( JJJ ) . L , 40 ) ) * ( zeroExtend ( D ( jjj ) . H , 40 ) ) ) ;
    D ( FFF ) = zeroExtend ( imult , 40 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)[mask 0x000000ffff]/p",
"D(jjj)[mask 0x00ffff0000]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"IMPYSU2_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "impysu2 JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_" => 92,
"jjj_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"IMPYSU2_Dm_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "impysu2 ww,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_2_2_" => 184,
"jjj_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"ww_imp_bits__17_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
    D ( FFF ) . WH = signExtend ( signedMultiply ( ( signExtend ( D ( ww ) ( 23 , 16 ) , 20 ) ) , ( zeroExtend ( D ( jjj ) ( 23 , 16 ) , 20 ) ) ) , 20 ) ;
    D ( FFF ) . WL = signExtend ( signedMultiply ( ( signExtend ( D ( ww ) ( 7 , 0 ) , 20 ) ) , ( zeroExtend ( D ( jjj ) ( 7 , 0 ) , 20 ) ) ) , 20 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(jjj)[mask 0x0000ff00ff]/p",
"D(ww)[mask 0x0000ff00ff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"IMPYSU_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "impysu jjj,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1708032,
"JJJ_imp_bits__34_34_x_2_2_x_1_1_x_0_0_" => "",
"jjj_imp_bits__33_33_x_23_23_x_22_22_x_21_21_" => "",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = signExtend ( signedMultiply ( ( zeroExtend ( D ( JJJ ) . L , 40 ) ) , ( signExtend ( D ( jjj ) . H , 40 ) ) ) , 40 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)[mask 0x000000ffff]/p",
"D(jjj)[mask 0x00ffff0000]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"IMPYUU_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "impyuu jjj,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1773568,
"JJJ_imp_bits__34_34_x_2_2_x_1_1_x_0_0_" => "",
"jjj_imp_bits__33_33_x_23_23_x_22_22_x_21_21_" => "",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 32 > imult = ( ( zeroExtend ( D ( JJJ ) . L , 40 ) ) * ( zeroExtend ( D ( jjj ) . L , 40 ) ) ) ;
    D ( FFF ) = zeroExtend ( imult , 40 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(JJJ)[mask 0x000000ffff]/p",
"D(jjj)[mask 0x000000ffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"IMPY_W__s16_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "impy.w #s16,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_24_x_20_19_x_15_13_" => 2004,
"reserved_imp_bits__34_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_16_" => "",
"s16_imp_bits__23_21_x_12_0_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = signedMultiply ( signExtend ( D ( FFF ) . L , 40 ) , signExtend ( s16 , 40 ) ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(FFF)[mask 0x000000ffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"INCA_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "INCA_Rx",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"aliases" => {
"ADDA__u5_Rx" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => 1,
},
],
"parent_action" => <<DATA
{
     bits < 32 > imm = zeroExtend ( u5 , 32 ) ;
    if ( RRRR ( 4 ) == 0 && RRRR ( 3 ) == 1 ) {
        RxTable ( RRRR ) = Address_by_MCTL ( RRRR , imm , 1 , 0 , 1 , imm ) ;
    } else {
        RxTable ( RRRR ) = RxTable ( RRRR ) + imm ;
    }
}
DATA
,
},
},
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"RxTable(RRRR)",
"rxTable(RRRR)?",
],
"outputs" => [
"RxTable(RRRR)",
],
"helpers" => [
"Address_by_MCTL",
"bitreverse",
"preshift",
],
},
"INC_F_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "inc.f FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 1255,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > sum = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) 0x0000010000ull ;
    overflow40 ( sum ) ;
     bits < 40 > Gcarry = Carry ( D ( FFF ) , ( sbits < 41 >  ) 0x0000010000ull , 0 ) ;
    SR . C = Gcarry ( 40 - 1 ) ;
    saturation32 ( sum , FFF ) ;
    L ( FFF ) = LnComputation ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"INSERT_Da_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "insert JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_24_19_x_15_3_" => 1748992,
"jjj_imp_bits__34_34_x_27_25_" => "",
"JJJ_imp_bits__33_33_x_2_0_" => "",
"FFF_imp_bits__32_32_x_18_16_" => "",
},
"action" => <<DATA
{
     bits < 6 > width = D ( JJJ ) ( 13 , 8 ) ;
     bits < 6 > offset = D ( JJJ ) ( 5 , 0 ) ;
     bits < 6 > width_s = ( width > 40 ) ? 40 : width ;
     bits < 6 > offset_s = ( offset > 39 ) ? 39 : offset ;
     bits < 7 > sum = ( bits < 7 >  ) width + ( bits < 7 >  ) offset ;
    sum = ( sum > 40 ) ? 40 : sum ;
    if ( width != 0 ) {
        D ( FFF ) = slice_write_dalu ( D ( FFF ) , D ( jjj ) ( width_s . uint32 (  ) - 1 , 0 ) , sum , offset ) ;
    }
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(FFF)?",
"D(JJJ)[mask 0x0000003f3f]/p",
"D(jjj)/p?",
],
"outputs" => [
"D(FFF)?",
"L(FFF)",
],
"helpers" => [
"slice_write_dalu",
],
},
"INSERT__U6__u6_Db_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "insert #U6,#u6,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_28_x_24_19_x_15_12_" => 3544,
"jjj_imp_bits__34_34_x_27_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_16_" => "",
"U6_imp_bits__11_6_" => "",
"u6_imp_bits__5_0_" => "",
},
"action" => <<DATA
{
     bits < 6 > width_s = ( U6 > 40 ) ? 40 : U6 ;
     bits < 6 > offset_s = ( u6 > 39 ) ? 39 : u6 ;
     bits < 7 > sum = ( bits < 7 >  ) U6 + ( bits < 7 >  ) u6 ;
    sum = ( sum > 40 ) ? 40 : sum ;
    if ( U6 != 0 ) {
        D ( FFF ) = slice_write_dalu ( D ( FFF ) , D ( jjj ) ( width_s . uint32 (  ) - 1 , 0 ) , sum , u6 ) ;
    }
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(FFF)?",
"D(jjj)/p?",
],
"outputs" => [
"D(FFF)?",
"L(FFF)",
],
"helpers" => [
"slice_write_dalu",
],
},
"ISUB_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "isub JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 857,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > sub = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) D ( JJJ ) ;
    overflow40 ( sub ) ;
    D ( FFF ) = sub ( 39 , 0 ) ;
    Borrow ( sub , D ( JJJ ) , D ( FFF ) ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"Borrow",
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"ISUB__u5_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "isub #u5,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 215,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > imm = zeroExtend ( u5 , 40 ) ;
     bits < 41 > sub = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) imm ;
    overflow40 ( sub ) ;
    D ( FFF ) = sub ( 39 , 0 ) ;
    Borrow ( sub , imm , D ( FFF ) ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"Borrow",
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"JFD_Rn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "jfd RRR",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_11_x_7_0_" => 4966,
"RRR_imp_bits__18_18_x_10_8_" => "",
"reserved_imp_bits__17_16_" => "reserved",
},
"action" => <<DATA
{
    if ( SR . T == 0 ) {
        DSPC = R ( RRR ) ;
        Fetch = Fetch :: DelaySlot ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"R(RRR)?",
"SR[mask 0x00000002]/p",
],
"outputs" => [
"DSPC?",
"Fetch?",
],
},
"JFD_label" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "jfd a32",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_40_x_34_32_x_31_29_x_15_14_" => 13958,
"reserved_imp_bits__50_48_" => "reserved",
"a32_imp_bits__36_35_x_13_0_x_39_37_x_28_16_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 0 ) {
        DSPC = a32 ;
        Fetch = Fetch :: DelaySlot ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"SR[mask 0x00000002]/p",
],
"outputs" => [
"DSPC?",
"Fetch?",
],
},
"JF_Rn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "jf RRR",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_11_x_7_0_" => 4967,
"RRR_imp_bits__18_18_x_10_8_" => "",
"reserved_imp_bits__17_16_" => "reserved",
},
"action" => <<DATA
{
    if ( SR . T == 0 ) {
        NVPC = R ( RRR ) ;
        jump_instr = true ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"R(RRR)?",
"SR[mask 0x00000002]/p",
],
"outputs" => [
"NVPC?",
],
},
"JF_label" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "jf a32",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_40_x_34_32_x_31_29_x_15_14_" => 14214,
"reserved_imp_bits__50_48_" => "reserved",
"a32_imp_bits__36_35_x_13_0_x_39_37_x_28_16_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 0 ) {
        NVPC = a32 ;
        jump_instr = true ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"SR[mask 0x00000002]/p",
],
"outputs" => [
"NVPC?",
],
},
"JMPD_Rn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "jmpd RRR",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_11_x_7_0_" => 4960,
"RRR_imp_bits__18_18_x_10_8_" => "",
"reserved_imp_bits__17_16_" => "reserved",
},
"action" => <<DATA
{
    DSPC = R ( RRR ) ;
    Fetch = Fetch :: DelaySlot ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"R(RRR)",
],
"outputs" => [
"DSPC",
"Fetch",
],
},
"JMPD_label" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "jmpd a32",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_40_x_34_32_x_31_29_x_15_14_" => 12422,
"reserved_imp_bits__50_48_" => "reserved",
"a32_imp_bits__36_35_x_13_0_x_39_37_x_28_16_" => "",
},
"action" => <<DATA
{
    DSPC = a32 ;
    Fetch = Fetch :: DelaySlot ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"DSPC",
"Fetch",
],
},
"JMP_Rn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "jmp RRR",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_11_x_7_0_" => 4961,
"RRR_imp_bits__18_18_x_10_8_" => "",
"reserved_imp_bits__17_16_" => "reserved",
},
"action" => <<DATA
{
    jump_instr = true ;
    NVPC = R ( RRR ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"R(RRR)",
],
"outputs" => [
"NVPC",
],
},
"JMP_label" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "jmp a32",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_40_x_34_32_x_31_29_x_15_14_" => 12678,
"reserved_imp_bits__50_48_" => "reserved",
"a32_imp_bits__36_35_x_13_0_x_39_37_x_28_16_" => "",
},
"action" => <<DATA
{
    if ( a32 + 2 == 0 ) {
         ;
        halt (  ) ;
    }
    jump_instr = true ;
    NVPC = a32 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"NVPC",
],
},
"JSRD_Rn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "jsrd RRR",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 4962,
"RRR_imp_bits__18_18_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    subroutine_instr = delayed ;
    DSPC = R ( RRR ) ;
    Fetch = Fetch :: DelaySlot ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"R(RRR)",
],
"outputs" => [
"DSPC",
"Fetch",
],
},
"JSRD_label" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "jsrd a32",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 12934,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    subroutine_instr = delayed ;
    DSPC = a32 ;
    Fetch = Fetch :: DelaySlot ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"DSPC",
"Fetch",
],
},
"JSR_Rn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "jsr RRR",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 4963,
"RRR_imp_bits__18_18_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    subroutine_instr = regular ;
    jump_instr = true ;
    NVPC = R ( RRR ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"R(RRR)",
],
"outputs" => [
"NVPC",
],
},
"JSR_label" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "jsr a32",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 13190,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    subroutine_instr = regular ;
    jump_instr = true ;
    NVPC = a32 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"NVPC",
],
},
"JTD_Rn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "jtd RRR",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_11_x_7_0_" => 4964,
"RRR_imp_bits__18_18_x_10_8_" => "",
"reserved_imp_bits__17_16_" => "reserved",
},
"action" => <<DATA
{
    if ( SR . T == 1 ) {
        DSPC = R ( RRR ) ;
        Fetch = Fetch :: DelaySlot ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"R(RRR)?",
"SR[mask 0x00000002]/p",
],
"outputs" => [
"DSPC?",
"Fetch?",
],
},
"JTD_label" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "jtd a32",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_40_x_34_32_x_31_29_x_15_14_" => 13446,
"reserved_imp_bits__50_48_" => "reserved",
"a32_imp_bits__36_35_x_13_0_x_39_37_x_28_16_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 1 ) {
        DSPC = a32 ;
        Fetch = Fetch :: DelaySlot ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"SR[mask 0x00000002]/p",
],
"outputs" => [
"DSPC?",
"Fetch?",
],
},
"JT_Rn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "jt RRR",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_11_x_7_0_" => 4965,
"RRR_imp_bits__18_18_x_10_8_" => "",
"reserved_imp_bits__17_16_" => "reserved",
},
"action" => <<DATA
{
    if ( SR . T == 1 ) {
        NVPC = R ( RRR ) ;
        jump_instr = true ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"R(RRR)?",
"SR[mask 0x00000002]/p",
],
"outputs" => [
"NVPC?",
],
},
"JT_label" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "jt a32",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_40_x_34_32_x_31_29_x_15_14_" => 13702,
"reserved_imp_bits__50_48_" => "reserved",
"a32_imp_bits__36_35_x_13_0_x_39_37_x_28_16_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 1 ) {
        NVPC = a32 ;
        jump_instr = true ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"SR[mask 0x00000002]/p",
],
"outputs" => [
"NVPC?",
],
},
"LSLL2_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "lsll2 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1874,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 5 > shift = D ( JJJ ) ( 4 , 0 ) ;
     bits < 5 > neg_val = ( ~ shift  ) + 1 ;
    shift = ( shift ( 4 ) == 0 ) ? shift : ( neg_val ) ;
     bits < 6 > shift_temp = zeroExtend ( shift , 6 ) ;
     bits < 6 > abs_shift = ( shift_temp . signedGT ( 16 ) ) ? 16 : shift_temp ;
    if ( D ( JJJ ) ( 4 ) == 1 ) {
        D ( FFF ) . L = ( D ( FFF ) . L ) >> abs_shift ;
        D ( FFF ) . H = ( D ( FFF ) . H ) >> abs_shift ;
    } else {
        D ( FFF ) . L = D ( FFF ) . L << abs_shift ;
        D ( FFF ) . H = D ( FFF ) . H << abs_shift ;
    }
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
"D(JJJ)[mask 0x000000001f]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"LSLL_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "lsll JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 848,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bool shift_left = true ;
    multiple_bitwise_shift ( FFF , D ( JJJ ) ( 6 , 0 ) , shift_left ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)[mask 0x000000007f]/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"multiple_bitwise_shift",
],
},
"LSR2_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "lsr2 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1912,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) . H = ( bits < 16 >  ) D ( JJJ ) . H >> 1 ;
    D ( FFF ) . L = ( bits < 16 >  ) D ( JJJ ) . L >> 1 ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"LSRA_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "lsra RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_0_" => 3839,
"RRRR_imp_bits__18_18_x_11_8_" => "",
"reserved_imp_bits__17_16_" => "reserved",
},
"action" => <<DATA
{
    RxTable ( RRRR ) = ( bits < 32 >  ) RxTable ( RRRR ) >> 1 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
],
"outputs" => [
"RxTable(RRRR)",
],
},
"LSRR2_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "lsrr2 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1875,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 5 > shift = D ( JJJ ) ( 4 , 0 ) ;
     bits < 5 > neg_val = ( ~ shift  ) + 1 ;
    shift = ( shift ( 4 ) == 0 ) ? shift : ( neg_val ) ;
     bits < 6 > shift_temp = zeroExtend ( shift , 6 ) ;
     bits < 6 > abs_shift = ( shift_temp . signedGT ( 16 ) ) ? 16 : shift_temp ;
    if ( D ( JJJ ) ( 4 ) == 0 ) {
        D ( FFF ) . L = D ( FFF ) . L >> abs_shift ;
        D ( FFF ) . H = D ( FFF ) . H >> abs_shift ;
    } else {
        D ( FFF ) . L = D ( FFF ) . L << abs_shift ;
        D ( FFF ) . H = D ( FFF ) . H << abs_shift ;
    }
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
"D(JJJ)[mask 0x000000001f]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"LSRR2__e_u2_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "lsrr2 #e_u2,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3584,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"e_u2_imp_bits__1_1_x_0_0_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"LSRR2__u4_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "lsrr2 #u4,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_" => 936,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u4_imp_bits__3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    D ( FFF ) . L = D ( FFF ) . L >> u4 ;
    D ( FFF ) . H = D ( FFF ) . H >> u4 ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"LSRR_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "lsrr JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 849,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bool shift_left = false ;
    multiple_bitwise_shift ( FFF , D ( JJJ ) ( 6 , 0 ) , shift_left ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)[mask 0x000000007f]/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"multiple_bitwise_shift",
],
},
"LSRR_L_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "lsrr.l JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 858,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 7 > shift = D ( JJJ ) ( 6 , 0 ) ;
     bits < 7 > neg_val = ( ~ shift  ) + 1 ;
    shift = ( shift ( 6 ) == 0 ) ? shift : ( neg_val ) ;
     bits < 6 > abs_shift = ( shift . signedGT ( 32 ) ) ? 32 : shift ;
     bool limit = ( shift . signedGT ( 32 ) ) ? true : false ;
    if ( D ( JJJ ) ( 6 ) == 0 ) {
         bits < 32 > tmp = ( D ( FFF ) ( 31 , 0 ) ) >> abs_shift ;
        D ( FFF ) = zeroExtend ( tmp , 40 ) ;
        SR . C = ( abs_shift == 0 || ( limit == true ) ) ? 0 : D ( FFF ) ( abs_shift - 1 ) ;
    } else {
         bits < 32 > tmp = ( D ( FFF ) ( 31 , 0 ) ) << abs_shift ;
        D ( FFF ) = zeroExtend ( tmp , 40 ) ;
        SR . C = ( abs_shift == 0 || ( limit == true ) ) ? 0 : D ( FFF ) ( 32 - abs_shift ) ;
    }
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)/p",
"D(JJJ)[mask 0x000000007f]/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
},
"LSRR_L__u5_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "lsrr.l #u5,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 217,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > tmp = ( D ( FFF ) ( 31 , 0 ) ) >> u5 ;
    D ( FFF ) = zeroExtend ( tmp , 40 ) ;
    SR . C = ( u5 == 0 ) ? 0 : D ( FFF ) ( u5 - 1 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
},
"LSRR__u5_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "lsrr #u5,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 221,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = D ( FFF ) >> u5 ;
    SR . C = ( u5 == 0 ) ? 0 : D ( FFF ) ( u5 - 1 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"SR/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
},
"LSRW_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "lsrw JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 865,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    SR . C = D ( JJJ ) ( 15 ) ;
    D ( FFF ) = ( bits < 40 >  ) D ( JJJ ) >> 16 ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
},
"LSR_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "lsr FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DBS" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 1262,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    SR . C = D ( FFF ) ( 0 ) ;
    D ( FFF ) = D ( FFF ) >> 1 ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
},
"MAC2R_mDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac2r -jj,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3590,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MAC2R_mDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac2r -JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 454,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MAC2R_pDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac2r +jj,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3589,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MAC2R_pDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac2r +JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 453,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MAC2_mDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac2 -jj,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3587,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MAC2_mDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac2 -JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 451,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MAC2_pDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac2 +jj,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3586,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MAC2_pDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac2 +JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 450,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MACCIM_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "maccim JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_" => 223,
"jjj_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MACD_mDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macd -jj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3549,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MACD_mDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macd -JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_" => 429,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MACD_pDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macd +jj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3548,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MACD_pDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macd +JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_" => 428,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MACIM_Da_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macim jj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3551,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MACIM_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macim JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_" => 431,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MACRE_Da_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macre jj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3550,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MACRE_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macre JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_" => 430,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MACSU_Dc_Dd_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macsu ee,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 284,
"ee_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 41 > mult = signExtend ( signedMultiply ( signExtend ( D ( ee . Dc ) ( 31 , 16 ) , 40 ) , zeroExtend ( D ( ee . Dd ) ( 15 , 0 ) , 40 ) ) , 41 ) << 1 ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = tmp ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(ee . Dc)[mask 0x00ffff0000]/p",
"D(ee . Dd)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"MACUS_Dc_Dd_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macus ee,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 376,
"ee_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 41 > mult = signExtend ( signedMultiply ( zeroExtend ( D ( ee . Dc ) ( 15 , 0 ) , 40 ) , signExtend ( D ( ee . Dd ) ( 31 , 16 ) , 40 ) ) , 41 ) << 1 ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = tmp ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(ee . Dc)[mask 0x000000ffff]/p",
"D(ee . Dd)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"MACUU_Dc_Dd_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macuu ee,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 377,
"ee_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 41 > mult = ( zeroExtend ( D ( ee . Dc ) ( 15 , 0 ) , 40 ) * zeroExtend ( D ( ee . Dd ) ( 15 , 0 ) , 40 ) ) << 1 ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = tmp ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(ee . Dc)[mask 0x000000ffff]/p",
"D(ee . Dd)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"MACX_mDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macx -jj,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3645,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MACX_mDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macx -JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 461,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MACX_pDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macx +jj,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3644,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MACX_pDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macx +JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 460,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MAC__s16_Da_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "mac #s16,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 244,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJ ) ( 31 , 16 ) , 41 ) , signExtend ( s16 , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(FFF)",
"D(JJJ)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC__s16_Da_L_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "mac #s16,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_MAC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_24_24_x_23_23_x_19_19_x_15_15_x_14_14_x_13_13_" => 204,
"JJJ_imp_bits__34_34_x_27_27_x_26_26_x_25_25_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__20_20_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJ ) ( 15 , 0 ) , 41 ) , signExtend ( s16 , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(FFF)",
"D(JJJ)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MARK" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mark",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_APC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40562,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MAX2VIT_D0_D6" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "max2vit D0,D6highbitvit",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 54361,
"highbitvit_imp_bits__18_18_x_16_16_" => "",
"reserved_imp_bits__17_17_" => "reserved",
},
"action" => <<DATA
{
     bits < 4 > Db = ( highbitvit ( 0 ) == 0 ) ? 6 : 14 ;
     bits < 4 > Da = ( highbitvit ( 1 ) == 0 ) ? 0 : 8 ;
    info ( 1 , " max2vit, Db = " << Db ) ;
    info ( 1 , " max2vit, Da = " << Da ) ;
    if ( ( D ( Da ) . H ) . signedGT ( D ( Db ) . H ) ) {
        D ( Db ) . H = D ( Da ) . H ;
        SR . VF3 = 0 ;
        D ( Db ) . E = D ( Da ) . E ;
    } else {
        SR . VF3 = 1 ;
    }
    if ( ( D ( Da ) . L ) . signedGT ( D ( Db ) . L ) ) {
        D ( Db ) . L = D ( Da ) . L ;
        SR . VF2 = 0 ;
    } else {
        SR . VF2 = 1 ;
    }
    L ( Db ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D[mask 0x00ffffffff]/p",
"SR/p",
],
"outputs" => [
"D[mask 0x00ffff0000]?",
"L",
"SR[mask 0x00000c00]/p",
],
},
"MAX2VIT_D4_D2" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "max2vit D4,D2highbitvit",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 54360,
"highbitvit_imp_bits__18_18_x_16_16_" => "",
"reserved_imp_bits__17_17_" => "reserved",
},
"action" => <<DATA
{
     bits < 4 > Db = ( highbitvit ( 0 ) == 0 ) ? 2 : 10 ;
     bits < 4 > Da = ( highbitvit ( 1 ) == 0 ) ? 4 : 12 ;
    info ( 1 , " max2vit, Db = " << Db ) ;
    info ( 1 , " max2vit, Da = " << Da ) ;
    if ( ( D ( Da ) . H ) . signedGT ( D ( Db ) . H ) ) {
        D ( Db ) . H = D ( Da ) . H ;
        SR . VF1 = 0 ;
        D ( Db ) . E = D ( Da ) . E ;
    } else {
        SR . VF1 = 1 ;
    }
    if ( ( D ( Da ) . L ) . signedGT ( D ( Db ) . L ) ) {
        D ( Db ) . L = D ( Da ) . L ;
        SR . VF0 = 0 ;
    } else {
        SR . VF0 = 1 ;
    }
    L ( Db ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D[mask 0x00ffffffff]/p",
"SR/p",
],
"outputs" => [
"D[mask 0x00ffff0000]?",
"L",
"SR[mask 0x00000300]/p",
],
},
"MAX2_Dg_Dh" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "max2 GG",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 2943,
"GG_imp_bits__16_16_x_18_18_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
},
"action" => <<DATA
{
     var dg = GG . Dg ;
     var dh = GG . Dh ;
    if ( ( D ( dg ) . H ) . signedGT ( D ( dh ) . H ) ) {
        D ( dh ) . H = D ( dg ) . H ;
        D ( dh ) . E = D ( dg ) . E ;
    }
    if ( ( D ( dg ) . L ) . signedGT ( D ( dh ) . L ) ) D ( dh ) . L = D ( dg ) . L ;
    L ( GG . Dh ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D[mask 0x00ffffffff]/p",
],
"outputs" => [
"D[mask 0x00ffff0000]?",
"L(GG . Dh)",
],
},
"MAXM_Dg_Dh" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "maxm GG",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 2942,
"GG_imp_bits__16_16_x_18_18_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 40 > abs_dg = ABSd ( D ( GG . Dg ) ) ;
     bits < 40 > abs_dh = ABSd ( D ( GG . Dh ) ) ;
    if ( abs_dg > abs_dh ) D ( GG . Dh ) = D ( GG . Dg ) ; else if ( D ( GG . Dg ) == ( 0 - D ( GG . Dh ) ) ) D ( GG . Dh ) = abs_dg ;
    L ( GG . Dh ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(GG . Dg)",
"D(GG . Dh)",
],
"outputs" => [
"D(GG . Dh)?",
"L(GG . Dh)",
],
"helpers" => [
"ABSd",
],
},
"MAX_Dg_Dh" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "max GG",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 2940,
"GG_imp_bits__16_16_x_18_18_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
},
"action" => <<DATA
{
    if ( ( D ( GG . Dg ) ) . signedGT ( D ( GG . Dh ) ) ) {
        D ( GG . Dh ) = D ( GG . Dg ) ;
    }
    L ( GG . Dh ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(GG . Dg)",
"D(GG . Dh)",
],
"outputs" => [
"D(GG . Dh)?",
"L(GG . Dh)",
],
},
"MIN2_Dg_Dh" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "min2 GG",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 2937,
"GG_imp_bits__16_16_x_18_18_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
},
"action" => <<DATA
{
     var dg = GG . Dg ;
     var dh = GG . Dh ;
    if ( ( D ( dg ) . H ) . signedLT ( D ( dh ) . H ) ) {
        D ( dh ) . H = D ( dg ) . H ;
        D ( dh ) . E = D ( dg ) . E ;
    }
    if ( ( D ( dg ) . L ) . signedLT ( D ( dh ) . L ) ) D ( dh ) . L = D ( dg ) . L ;
    L ( GG . Dh ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D[mask 0x00ffffffff]/p",
],
"outputs" => [
"D[mask 0x00ffff0000]?",
"L(GG . Dh)",
],
},
"MIN_Dg_Dh" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "min GG",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 2941,
"GG_imp_bits__16_16_x_18_18_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
},
"action" => <<DATA
{
     var dg = GG . Dg ;
     var dh = GG . Dh ;
    if ( ( D ( dg ) ) . signedLT ( D ( dh ) ) ) D ( dh ) = D ( dg ) ;
    L ( GG . Dh ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D",
],
"outputs" => [
"D?",
"L(GG . Dh)",
],
},
"MOVE2_2BF__EA__Da" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move2.2bf (RRRMMM,jjj",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 242,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVE2_2B_Da__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move2.2b jjj,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 64,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 16 > mem_data = concat ( D ( jjj ) . HL , D ( jjj ) . LL ) ;
    DMEM ( EAtable ( MMM , RRR , 1 ) , 2 ) = mem_data ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D(jjj)[mask 0x0000ff00ff]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE2_2B__EA__Da" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move2.2b (RRRMMM,jjj",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 68,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 16 > mem_data = DMEM ( EAtable ( MMM , RRR , 1 ) , 2 ) ;
    D ( jjj ) . WL = signExtend ( mem_data ( 7 , 0 ) , 20 ) ;
    D ( jjj ) . WH = signExtend ( mem_data ( 15 , 8 ) , 20 ) ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(jjj)",
"L(jjj)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE2_2F__EA__Da" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move2.2f (RRRMMM,jjj",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 71,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVE2_4BF__EA__Da_Db" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move2.4bf (RRRMMM,hh",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 489,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVE2_4B_Da_Db__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move2.4b hh,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_" => 134,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"hh_imp_bits__16_16_x_7_7_x_6_6_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 4 > Da = hh . Da ;
     bits < 4 > Db = hh . Db ;
     bits < 8 > Db_LL = D ( Db ) . LL ;
     bits < 8 > Db_HL = D ( Db ) . HL ;
     bits < 8 > Da_LL = D ( Da ) . LL ;
     bits < 8 > Da_HL = D ( Da ) . HL ;
     bits < 32 > mem_data = concat ( Da_HL , Da_LL , Db_HL , Db_LL ) ;
    DMEM ( EAtable ( MMM , RRR , 2 ) , 4 ) = mem_data ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D[mask 0x00000000ff]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE2_4B__EA__Da_Db" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move2.4b (RRRMMM,hh",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 138,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 4 > Da = hh . Da ;
     bits < 4 > Db = hh . Db ;
     bits < 32 > mem_data = DMEM ( EAtable ( MMM , RRR , 2 ) , 4 ) ;
    D ( Db ) . WL = signExtend ( mem_data ( 7 , 0 ) , 20 ) ;
    D ( Db ) . WH = signExtend ( mem_data ( 15 , 8 ) , 20 ) ;
    L ( Db ) = 0 ;
    D ( Da ) . WL = signExtend ( mem_data ( 23 , 16 ) , 20 ) ;
    D ( Da ) . WH = signExtend ( mem_data ( 31 , 24 ) , 20 ) ;
    L ( Da ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D[mask 0x0f0000ffff]/p",
"L",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE2_4F__EA__Da_Db" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move2.4f (RRRMMM,hh",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 142,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVE2_8BF__EA__Da_Db_Dc_Dd" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move2.8bf (RRRMMM,k",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => 987,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"k_imp_bits__16_16_x_10_10_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVE2_8B_Da_Db_Dc_Dd__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move2.8b k,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => 259,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"k_imp_bits__16_16_x_10_10_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 4 > Da = k . Da ;
     bits < 4 > Db = k . Db ;
     bits < 4 > Dc = k . Dc ;
     bits < 4 > Dd = k . Dd ;
     bits < 8 > Dd_LL = D ( Dd ) . LL ;
     bits < 8 > Dd_HL = D ( Dd ) . HL ;
     bits < 8 > Dc_LL = D ( Dc ) . LL ;
     bits < 8 > Dc_HL = D ( Dc ) . HL ;
     bits < 8 > Db_LL = D ( Db ) . LL ;
     bits < 8 > Db_HL = D ( Db ) . HL ;
     bits < 8 > Da_LL = D ( Da ) . LL ;
     bits < 8 > Da_HL = D ( Da ) . HL ;
     bits < 32 > mem_data1 = concat ( Da_HL , Da_LL , Db_HL , Db_LL ) ;
     bits < 32 > mem_data2 = concat ( Dc_HL , Dc_LL , Dd_HL , Dd_LL ) ;
     bits < 64 > mem_data = concat ( mem_data1 , mem_data2 ) ;
    DMEM ( EAtable ( MMM , RRR , 3 ) , 8 ) = mem_data ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D[mask 0x00000000ff]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE2_8B__EA__Da_Db_Dc_Dd" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move2.8b (RRRMMM,k",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => 263,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"k_imp_bits__16_16_x_10_10_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 4 > Da = k . Da ;
     bits < 4 > Db = k . Db ;
     bits < 4 > Dc = k . Dc ;
     bits < 4 > Dd = k . Dd ;
     bits < 64 > mem_data = DMEM ( EAtable ( MMM , RRR , 3 ) , 8 ) ;
    D ( Dd ) . WL = signExtend ( mem_data ( 7 , 0 ) , 20 ) ;
    D ( Dd ) . WH = signExtend ( mem_data ( 15 , 8 ) , 20 ) ;
    L ( Dd ) = 0 ;
    D ( Dc ) . WL = signExtend ( mem_data ( 23 , 16 ) , 20 ) ;
    D ( Dc ) . WH = signExtend ( mem_data ( 31 , 24 ) , 20 ) ;
    L ( Dc ) = 0 ;
    D ( Db ) . WL = signExtend ( mem_data ( 39 , 32 ) , 20 ) ;
    D ( Db ) . WH = signExtend ( mem_data ( 47 , 40 ) , 20 ) ;
    L ( Db ) = 0 ;
    D ( Da ) . WL = signExtend ( mem_data ( 55 , 48 ) , 20 ) ;
    D ( Da ) . WH = signExtend ( mem_data ( 63 , 56 ) , 20 ) ;
    L ( Da ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D[mask 0x0f0000ffff]/p",
"L",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVEF_Rq_Rn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "movef qqq,RRR",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 619,
"RRR_imp_bits__18_18_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"qqq_imp_bits__16_16_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 0 ) R ( RRR ) = R ( qqq ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"R(qqq)?",
"SR[mask 0x00000002]/p",
],
"outputs" => [
"R(RRR)?",
],
},
"MOVEPU_B__Rnps13__DR" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "movepu.b (RRR+s13),HHHH",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 756,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s13_imp_bits__12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = signExtend ( s13 , 32 ) ;
     bits < 32 > eff_mem_add = ( imm  ) + R ( RRR ) ;
    write_mem_2DRTable_zeroextend ( eff_mem_add , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2DRTable_zeroextend",
],
},
"MOVEP_B_DR__Rnps13_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "movep.b HHHH,(RRR+s13)",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 660,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s13_imp_bits__12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = signExtend ( s13 , 32 ) ;
     bits < 32 > eff_mem_add = ( imm  ) + R ( RRR ) ;
    write_DRTable_2mem ( eff_mem_add , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)[mask 0x00000000ff]/p",
"R(RRR)",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"write_DRTable_2mem",
],
},
"MOVEP_B__Rnps13__DR" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "movep.b (RRR+s13),HHHH",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 692,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s13_imp_bits__12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = signExtend ( s13 , 32 ) ;
     bits < 32 > eff_mem_add = ( imm  ) + R ( RRR ) ;
    write_mem_2DRTable ( eff_mem_add , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2DRTable",
],
},
"MOVEP_L_DR__Rnps13_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "movep.l HHHH,(RRR+s13)",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 596,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s13_imp_bits__12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = signExtend ( s13 , 32 ) ;
     bits < 32 > eff_mem_add = ( imm  ) + R ( RRR ) ;
    write_DRTable_2mem ( eff_mem_add , HHHH , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)[mask 0x00ffffffff]/p",
"R(RRR)",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"write_DRTable_2mem",
],
},
"MOVEP_L__Rnps13__DR" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "movep.l (RRR+s13),HHHH",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 628,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s13_imp_bits__12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = signExtend ( s13 , 32 ) ;
     bits < 32 > eff_mem_add = ( imm  ) + R ( RRR ) ;
    write_mem_2DRTable ( eff_mem_add , HHHH , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2DRTable",
],
},
"MOVEP_W_DR__Rnps13_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "movep.w HHHH,(RRR+s13)",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 532,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s13_imp_bits__12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = signExtend ( s13 , 32 ) ;
     bits < 32 > eff_mem_add = ( imm  ) + R ( RRR ) ;
    write_DRTable_2mem ( eff_mem_add , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)[mask 0x000000ffff]/p",
"R(RRR)",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"write_DRTable_2mem",
],
},
"MOVEP_W__Rnps13__DR" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "movep.w (RRR+s13),HHHH",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 564,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s13_imp_bits__12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = signExtend ( s13 , 32 ) ;
     bits < 32 > eff_mem_add = ( imm  ) + R ( RRR ) ;
    write_mem_2DRTable ( eff_mem_add , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2DRTable",
],
},
"MOVER2_2BF_Da__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mover2.2bf jjj,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 241,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVER2_2F_Da__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mover2.2f jjj,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 240,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVER2_4BF_Da_Db__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mover2.4bf hh,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 492,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVER2_4F_Da_Db__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mover2.4f hh,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 488,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVER2_8BF_Da_Db_Dc_Dd__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mover2.8bf k,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => 979,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"k_imp_bits__16_16_x_10_10_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVER_2BF_Da_Db__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mover.2bf hh,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 493,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVER_2F_Da_Db__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mover.2f hh,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 147,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVER_2L_Da_Db__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mover.2l hh,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 130,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVER_4BF_Da_Db_Dc_Dd__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mover.4bf k,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => 983,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"k_imp_bits__16_16_x_10_10_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVER_4F_Da_Db_Dc_Dd__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mover.4f k,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => 271,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"k_imp_bits__16_16_x_10_10_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVER_BF_Da__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mover.bf jjj,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 243,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVER_F_Db__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mover.f jjj,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 76,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVER_F_Db__Rnps15_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "mover.f jjj,(RRR+s15)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 157,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"jjj_imp_bits__32_32_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"MOVER_F_Db__SPps15_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "mover.f jjj,(sp+s15)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_23_23_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 244,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"jjj_imp_bits__32_32_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"MOVER_F_Db__a16_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "mover.f jjj,(a16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 364,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"jjj_imp_bits__32_32_x_26_26_x_25_25_x_24_24_" => "",
"a16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"MOVER_F_Db__a32_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "mover.f jjj,(a32)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 1766,
"reserved_imp_bits__50_50_x_49_49_" => "reserved",
"jjj_imp_bits__48_48_x_42_42_x_41_41_x_40_40_" => "",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
},
"MOVER_L_Db__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mover.l jjj,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 72,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MOVES_2F_Da_Db__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "moves.2f hh,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 13,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 40 > value_Da = scale ( hh . Da ) ;
     bits < 40 > value_Db = scale ( hh . Db ) ;
    value_Da = sat_moves ( L ( hh . Da ) , value_Da ) ;
    value_Db = sat_moves ( L ( hh . Db ) , value_Db ) ;
     bits < 32 > mem_data = concat ( value_Da ( 31 , 16 ) , value_Db ( 31 , 16 ) ) ;
    DMEM ( EAtable ( MMM , RRR , 2 ) , 4 ) = mem_data ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D(hh . Da)",
"D(hh . Db)",
"L(hh . Da)",
"L(hh . Db)",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
"SR[mask 0x00020034]/p",
],
"outputs" => [
"R(RRR)?",
"SR[mask 0x00000040]/p?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
"sat_moves",
"scale",
],
},
"MOVES_4F_Da_Db_Dc_Dd__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "moves.4f k,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => 19,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"k_imp_bits__16_16_x_10_10_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 40 > value_Da = scale ( k . Da ) ;
     bits < 40 > value_Db = scale ( k . Db ) ;
     bits < 40 > value_Dc = scale ( k . Dc ) ;
     bits < 40 > value_Dd = scale ( k . Dd ) ;
    value_Da = sat_moves ( L ( k . Da ) , value_Da ) ;
    value_Db = sat_moves ( L ( k . Db ) , value_Db ) ;
    value_Dc = sat_moves ( L ( k . Dc ) , value_Dc ) ;
    value_Dd = sat_moves ( L ( k . Dd ) , value_Dd ) ;
     bits < 64 > mem_data = concat ( value_Da ( 31 , 16 ) , value_Db ( 31 , 16 ) , value_Dc ( 31 , 16 ) , value_Dd ( 31 , 16 ) ) ;
    DMEM ( EAtable ( MMM , RRR , 3 ) , 8 ) = mem_data ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D(k . Da)",
"D(k . Db)",
"D(k . Dc)",
"D(k . Dd)",
"L(k . Da)",
"L(k . Db)",
"L(k . Dc)",
"L(k . Dd)",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
"SR[mask 0x00020034]/p",
],
"outputs" => [
"R(RRR)?",
"SR[mask 0x00000040]/p?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
"sat_moves",
"scale",
],
},
"MOVES_F_Db__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "moves.f jjj,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 1,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 40 > value = scale ( jjj ) ;
    value = sat_moves ( L ( jjj ) , value ) ;
    DMEM ( EAtable ( MMM , RRR , 1 ) , 2 ) = value ( 31 , 16 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D(jjj)",
"L(jjj)",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
"SR[mask 0x00020034]/p",
],
"outputs" => [
"R(RRR)?",
"SR[mask 0x00000040]/p?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
"sat_moves",
"scale",
],
},
"MOVES_F_Db__Rnps15_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "moves.f jjj,(RRR+s15)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 52,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"jjj_imp_bits__32_32_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = signExtend ( s15 , 32 ) ;
     bits < 32 > eff_mem_add ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , imm , 0 , 2 , imm ) ;
    } else {
        eff_mem_add = imm + R ( RRR ) ;
    }
     bits < 40 > value = scale ( jjj ) ;
    value = sat_moves ( L ( jjj ) , value ) ;
    DMEM ( eff_mem_add , 2 ) = value ( 31 , 16 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"B?",
"D(jjj)",
"L(jjj)",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
"SR[mask 0x00020034]/p",
],
"outputs" => [
"SR[mask 0x00000040]/p?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"sat_moves",
"scale",
],
},
"MOVES_F_Db__SPps15_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "moves.f jjj,(sp+s15)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_23_23_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 500,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"jjj_imp_bits__32_32_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = ( sbits < 32 >  ) SP + ( sbits < 32 >  ) s15 ;
     bits < 40 > value = scale ( jjj ) ;
    value = sat_moves ( L ( jjj ) , value ) ;
    DMEM ( eff_mem_add , 2 ) = value ( 31 , 16 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(jjj)",
"ESP?",
"L(jjj)",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000040]/p?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"sat_moves",
"scale",
],
},
"MOVES_F_Db__a16_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "moves.f jjj,(a16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 124,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"jjj_imp_bits__32_32_x_26_26_x_25_25_x_24_24_" => "",
"a16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > value = scale ( jjj ) ;
    value = sat_moves ( L ( jjj ) , value ) ;
    DMEM ( a16 , 2 ) = value ( 31 , 16 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(jjj)",
"L(jjj)",
"SR[mask 0x00020034]/p",
],
"outputs" => [
"SR[mask 0x00000040]/p?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"sat_moves",
"scale",
],
},
"MOVES_F_Db__a32_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "moves.f jjj,(a32)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 102,
"reserved_imp_bits__50_50_x_49_49_" => "reserved",
"jjj_imp_bits__48_48_x_42_42_x_41_41_x_40_40_" => "",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 40 > value = scale ( jjj ) ;
    value = sat_moves ( L ( jjj ) , value ) ;
    DMEM ( a32 , 2 ) = value ( 31 , 16 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"D(jjj)",
"L(jjj)",
"SR[mask 0x00020034]/p",
],
"outputs" => [
"SR[mask 0x00000040]/p?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"sat_moves",
"scale",
],
},
"MOVES_L_Db__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "moves.l jjj,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 3,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 40 > value = scale ( jjj ) ;
    value = sat_moves ( L ( jjj ) , value ) ;
    DMEM ( EAtable ( MMM , RRR , 2 ) , 4 ) = value ( 31 , 0 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D(jjj)",
"L(jjj)",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
"SR[mask 0x00020034]/p",
],
"outputs" => [
"R(RRR)?",
"SR[mask 0x00000040]/p?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
"sat_moves",
"scale",
],
},
"MOVET_Rq_Rn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "movet qqq,RRR",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 618,
"RRR_imp_bits__18_18_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"qqq_imp_bits__16_16_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 1 ) R ( RRR ) = R ( qqq ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"R(qqq)?",
"SR[mask 0x00000002]/p",
],
"outputs" => [
"R(RRR)?",
],
},
"MOVEU2_2B__EA__Da" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "moveu2.2b (RRRMMM,jjj",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 69,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 16 > mem_data = DMEM ( EAtable ( MMM , RRR , 1 ) , 2 ) ;
    D ( jjj ) . WL = zeroExtend ( mem_data ( 7 , 0 ) , 20 ) ;
    D ( jjj ) . WH = zeroExtend ( mem_data ( 15 , 8 ) , 20 ) ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(jjj)",
"L(jjj)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVEU2_4B__EA__Da_Db" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "moveu2.4b (RRRMMM,hh",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_" => 135,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"hh_imp_bits__16_16_x_7_7_x_6_6_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 32 > mem_data = DMEM ( EAtable ( MMM , RRR , 2 ) , 4 ) ;
     bits < 4 > Da = hh . Da ;
     bits < 4 > Db = hh . Db ;
    D ( Db ) . WL = zeroExtend ( mem_data ( 7 , 0 ) , 20 ) ;
    D ( Db ) . WH = zeroExtend ( mem_data ( 15 , 8 ) , 20 ) ;
    D ( Da ) . WL = zeroExtend ( mem_data ( 23 , 16 ) , 20 ) ;
    D ( Da ) . WH = zeroExtend ( mem_data ( 31 , 24 ) , 20 ) ;
    L ( Da ) = 0 ;
    L ( Db ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D[mask 0x0f0000ffff]/p",
"L",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVEU2_8B__EA__Da_Db_Dc_Dd" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "moveu2.8b (RRRMMM,k",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => 267,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"k_imp_bits__16_16_x_10_10_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 4 > Da = k . Da ;
     bits < 4 > Db = k . Db ;
     bits < 4 > Dc = k . Dc ;
     bits < 4 > Dd = k . Dd ;
     bits < 64 > mem_data = DMEM ( EAtable ( MMM , RRR , 3 ) , 8 ) ;
    D ( Dd ) . WL = zeroExtend ( mem_data ( 7 , 0 ) , 20 ) ;
    D ( Dd ) . WH = zeroExtend ( mem_data ( 15 , 8 ) , 20 ) ;
    L ( Dd ) = 0 ;
    D ( Dc ) . WL = zeroExtend ( mem_data ( 23 , 16 ) , 20 ) ;
    D ( Dc ) . WH = zeroExtend ( mem_data ( 31 , 24 ) , 20 ) ;
    L ( Dc ) = 0 ;
    D ( Db ) . WL = zeroExtend ( mem_data ( 39 , 32 ) , 20 ) ;
    D ( Db ) . WH = zeroExtend ( mem_data ( 47 , 40 ) , 20 ) ;
    L ( Db ) = 0 ;
    D ( Da ) . WL = zeroExtend ( mem_data ( 55 , 48 ) , 20 ) ;
    D ( Da ) . WH = zeroExtend ( mem_data ( 63 , 56 ) , 20 ) ;
    L ( Da ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D[mask 0x0f0000ffff]/p",
"L",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVEU_4B__EA__Da_Db_Dc_Dd" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "moveu.4b (RRRMMM,k",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => 31,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"k_imp_bits__16_16_x_10_10_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 32 > mem_data = DMEM ( EAtable ( MMM , RRR , 2 ) , 4 ) ;
    D ( k . Dd ) = zeroExtend ( mem_data ( 7 , 0 ) , 40 ) ;
    D ( k . Dc ) = zeroExtend ( mem_data ( 15 , 8 ) , 40 ) ;
    D ( k . Db ) = zeroExtend ( mem_data ( 23 , 16 ) , 40 ) ;
    D ( k . Da ) = zeroExtend ( mem_data ( 31 , 24 ) , 40 ) ;
    L ( k . Da ) = 0 ;
    L ( k . Db ) = 0 ;
    L ( k . Dc ) = 0 ;
    L ( k . Dd ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(k . Da)",
"D(k . Db)",
"D(k . Dc)",
"D(k . Dd)",
"L(k . Da)",
"L(k . Db)",
"L(k . Dc)",
"L(k . Dd)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVEU_B__Ea__Db" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "moveu.b (RRRmm,jjj",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_x_5_5_" => 131,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"mm_imp_bits__4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 3 > mm_2MMM = mm + 4 ;
    D ( jjj ) = zeroExtend ( DMEM ( EAtable ( mm_2MMM , RRR , 0 ) , 1 ) , 40 ) ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(jjj)",
"L(jjj)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVEU_B__Rnps15__DR" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "moveu.b (RRR+s15),HHHH",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 84,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add ;
     bits < 32 > imm = signExtend ( s15 , 32 ) ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , imm , 0 , 1 , imm ) ;
    } else {
        eff_mem_add = imm + R ( RRR ) ;
    }
    write_mem_2DRTable_zeroextend ( eff_mem_add , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_mem_2DRTable_zeroextend",
],
},
"MOVEU_B__SPps15__DR" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "moveu.b (sp+s15),HHHH",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 740,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = ( sbits < 32 >  ) SP + ( sbits < 32 >  ) s15 ;
    write_mem_2DRTable_zeroextend ( eff_mem_add , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2DRTable_zeroextend",
],
},
"MOVEU_B__a16__DR" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "moveu.b (a16),HHHH",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 356,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"a16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    write_mem_2DRTable_zeroextend ( zeroExtend ( a16 , 32 ) , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2DRTable_zeroextend",
],
},
"MOVEU_B__a32__DR" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "moveu.b (a32),HHHH",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 166,
"reserved_imp_bits__50_50_x_49_49_" => "reserved",
"HHHH_imp_bits__48_48_x_43_43_x_42_42_x_41_41_x_40_40_" => "",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    write_mem_2DRTable_zeroextend ( a32 , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2DRTable_zeroextend",
],
},
"MOVEU_B__ea__DR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "moveu.b (RRRMM,HHHH",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_" => 77,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"MM_imp_bits__4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    write_mem_2DRTable_zeroextend ( EAtable ( ea_2EA ( MM ) , RRR , 0 ) , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"ea_2EA",
"increase_size",
"preshift",
"write_mem_2DRTable_zeroextend",
],
},
"MOVEU_L__u32_Db" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "moveu.l #u32,jjj",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 1574,
"reserved_imp_bits__50_50_x_49_49_" => "reserved",
"jjj_imp_bits__48_48_x_42_42_x_41_41_x_40_40_" => "",
"u32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    D ( jjj ) = zeroExtend ( u32 , 40 ) ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"D(jjj)",
"L(jjj)",
],
},
"MOVEU_W__EA__DR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "moveu.w (RRRMMM,HHHH",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_7_7_x_6_6_" => 7,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    write_mem_2DRTable_zeroextend ( EAtable ( MMM , RRR , 1 ) , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
"write_mem_2DRTable_zeroextend",
],
},
"MOVEU_W__EA__Db_H" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "moveu.w (RRRMMM,jjj.h",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 77,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    D ( jjj ) . H = DMEM ( EAtable ( MMM , RRR , 1 ) , 2 ) ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(jjj)[mask 0x00ffff0000]/p",
"L(jjj)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVEU_W__EA__Db_L" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "moveu.w (RRRMMM,jjj.l",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 73,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    D ( jjj ) . L = DMEM ( EAtable ( MMM , RRR , 1 ) , 2 ) ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(jjj)[mask 0x000000ffff]/p",
"L(jjj)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVEU_W__Rnps15__DR" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "moveu.w (RRR+s15),HHHH",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 116,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = signExtend ( s15 , 32 ) ;
     bits < 32 > eff_mem_add ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , ( imm ) , 0 , 2 , imm ) ;
    } else {
        eff_mem_add = ( imm  ) + R ( RRR ) ;
    }
    write_mem_2DRTable_zeroextend ( eff_mem_add , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_mem_2DRTable_zeroextend",
],
},
"MOVEU_W__SPps15__C4" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "moveu.w (sp+s15),DDDDD",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_18_18_x_16_16_x_15_15_x_14_14_x_13_13_" => 500,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"DDDDD_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_x_17_17_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = ( sbits < 32 >  ) SP + ( sbits < 32 >  ) s15 ;
    write_mem_2C4Table_zeroextend ( eff_mem_add , DDDDD , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"C4Table(DDDDD)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2C4Table_zeroextend",
],
},
"MOVEU_W__a16__C4" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "moveu.w (a16),DDDDD",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_20_20_x_19_19_x_18_18_x_16_16_x_15_15_x_14_14_x_13_13_" => 188,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"DDDDD_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_x_17_17_" => "",
"a16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    write_mem_2C4Table_zeroextend ( zeroExtend ( a16 , 32 ) , DDDDD , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"outputs" => [
"C4Table(DDDDD)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2C4Table_zeroextend",
],
},
"MOVEU_W__a32__DR" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "moveu.w (a32),HHHH",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 230,
"reserved_imp_bits__50_50_x_49_49_" => "reserved",
"HHHH_imp_bits__48_48_x_43_43_x_42_42_x_41_41_x_40_40_" => "",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    write_mem_2DRTable_zeroextend ( a32 , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2DRTable_zeroextend",
],
},
"MOVEU_W__u16_Db_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "moveu.w #u16,jjj.h",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 821,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"jjj_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    D ( jjj ) . H = u16 ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"outputs" => [
"D(jjj)[mask 0x00ffff0000]/p",
"L(jjj)",
],
},
"MOVEU_W__u16_Db_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "moveu.w #u16,jjj.l",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 805,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"jjj_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    D ( jjj ) . L = u16 ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"outputs" => [
"D(jjj)[mask 0x000000ffff]/p",
"L(jjj)",
],
},
"MOVEU_W__u16_Rn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "moveu.w #u16,RRR",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 829,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    R ( RRR ) = zeroExtend ( u16 , 32 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"outputs" => [
"R(RRR)",
],
},
"MOVE_2F__EA__Da_Db" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.2f (RRRMMM,hh",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 29,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 32 > mem_data = DMEM ( EAtable ( MMM , RRR , 2 ) , 4 ) ;
    D ( hh . Db ) = signExtend ( concat ( mem_data ( 15 , 0 ) , zero ( 16 ) ) , 40 ) ;
    D ( hh . Da ) = signExtend ( concat ( mem_data ( 31 , 16 ) , zero ( 16 ) ) , 40 ) ;
    L ( hh . Da ) = 0 ;
    L ( hh . Db ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(hh . Da)",
"D(hh . Db)",
"L(hh . Da)",
"L(hh . Db)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_2L_BTR0_BTR1__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.2l btr0:btr1,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_" => 578,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_16_16_x_14_14_" => "reserved",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 64 > mem_data = concat ( BTR0 , BTR1 ) ;
    DMEM ( EAtable ( MMM , RRR , 3 ) , 8 ) = mem_data ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"BTR0",
"BTR1",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_2L_Da_Db_Dc_Dd" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.2l hh,kk",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_3_3_" => 3233,
"kk_imp_bits__18_18_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"hh_imp_bits__16_16_x_4_4_x_2_2_" => "",
},
"action" => <<DATA
{
    D ( kk . Dc ) = signExtend ( D ( hh . Da ) ( 31 , 0 ) , 40 ) ;
    D ( kk . Dd ) = signExtend ( D ( hh . Db ) ( 31 , 0 ) , 40 ) ;
    L ( kk . Dc ) = 0 ;
    L ( kk . Dd ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(hh . Da)[mask 0x00ffffffff]/p",
"D(hh . Db)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(kk . Dc)",
"D(kk . Dd)",
"L(kk . Dc)",
"L(kk . Dd)",
],
},
"MOVE_2L_Da_Db__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.2l hh,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 192,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 32 > Da = D ( hh . Da ) ( 32 - 1 , 0 ) ;
     bits < 32 > Db = D ( hh . Db ) ( 32 - 1 , 0 ) ;
     bits < 64 > mem_data = concat ( Da , Db ) ;
    DMEM ( EAtable ( MMM , RRR , 3 ) , 8 ) = mem_data ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D(hh . Da)[mask 0x00ffffffff]/p",
"D(hh . Db)[mask 0x00ffffffff]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_2L_Da_Dc__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.2l gg,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 134,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"gg_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 32 > Da = D ( gg . Da ) ( 32 - 1 , 0 ) ;
     bits < 32 > Dc = D ( gg . Dc ) ( 32 - 1 , 0 ) ;
     bits < 64 > mem_data = concat ( Da , Dc ) ;
    DMEM ( EAtable ( MMM , RRR , 3 ) , 8 ) = mem_data ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D(gg . Da)[mask 0x00ffffffff]/p",
"D(gg . Dc)[mask 0x00ffffffff]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_2L__EA__Da_Db" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.2l (RRRMMM,hh",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 196,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 64 > mem_data = DMEM ( EAtable ( MMM , RRR , 3 ) , 8 ) ;
    D ( hh . Db ) = signExtend ( mem_data ( 31 , 0 ) , 40 ) ;
    D ( hh . Da ) = signExtend ( mem_data ( 63 , 32 ) , 40 ) ;
    L ( hh . Da ) = 0 ;
    L ( hh . Db ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(hh . Da)",
"D(hh . Db)",
"L(hh . Da)",
"L(hh . Db)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_2L__EA__Da_Dc" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.2l (RRRMMM,gg",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 150,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"gg_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 64 > mem_data = DMEM ( EAtable ( MMM , RRR , 3 ) , 8 ) ;
    D ( gg . Dc ) = signExtend ( mem_data ( 31 , 0 ) , 40 ) ;
    D ( gg . Da ) = signExtend ( mem_data ( 63 , 32 ) , 40 ) ;
    L ( gg . Da ) = 0 ;
    L ( gg . Dc ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(gg . Da)",
"D(gg . Dc)",
"L(gg . Da)",
"L(gg . Dc)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_2W_Da_Db__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.2w hh,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 9,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    DMEM ( EAtable ( MMM , RRR , 2 ) , 4 ) = concat ( D ( hh . Da ) ( 15 , 0 ) , D ( hh . Db ) ( 15 , 0 ) ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D(hh . Da)[mask 0x000000ffff]/p",
"D(hh . Db)[mask 0x000000ffff]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_2W__EA__Da_Db" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.2w (RRRMMM,hh",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 25,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 32 > mem_data = DMEM ( EAtable ( MMM , RRR , 2 ) , 4 ) ;
    D ( hh . Db ) = signExtend ( mem_data ( 15 , 0 ) , 40 ) ;
    D ( hh . Da ) = signExtend ( mem_data ( 31 , 16 ) , 40 ) ;
    L ( hh . Da ) = 0 ;
    L ( hh . Db ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(hh . Da)",
"D(hh . Db)",
"L(hh . Da)",
"L(hh . Db)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_4B_Da_Db_Dc_Dd__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.4b k,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => 412,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"k_imp_bits__16_16_x_10_10_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    DMEM ( EAtable ( MMM , RRR , 2 ) , 4 ) = concat ( ( D ( k . Da ) ( 7 , 0 ) ) , ( D ( k . Db ) ( 7 , 0 ) ) , ( D ( k . Dc ) ( 7 , 0 ) ) , ( D ( k . Dd ) ( 7 , 0 ) ) ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D(k . Da)[mask 0x00000000ff]/p",
"D(k . Db)[mask 0x00000000ff]/p",
"D(k . Dc)[mask 0x00000000ff]/p",
"D(k . Dd)[mask 0x00000000ff]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_4B__EA__Da_Db_Dc_Dd" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.4b (RRRMMM,k",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => 27,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"k_imp_bits__16_16_x_10_10_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 32 > mem_data = DMEM ( EAtable ( MMM , RRR , 2 ) , 4 ) ;
    D ( k . Dd ) = signExtend ( mem_data ( 7 , 0 ) , 40 ) ;
    D ( k . Dc ) = signExtend ( mem_data ( 15 , 8 ) , 40 ) ;
    D ( k . Db ) = signExtend ( mem_data ( 23 , 16 ) , 40 ) ;
    D ( k . Da ) = signExtend ( mem_data ( 31 , 24 ) , 40 ) ;
    L ( k . Da ) = 0 ;
    L ( k . Db ) = 0 ;
    L ( k . Dc ) = 0 ;
    L ( k . Dd ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(k . Da)",
"D(k . Db)",
"D(k . Dc)",
"D(k . Dd)",
"L(k . Da)",
"L(k . Db)",
"L(k . Dc)",
"L(k . Dd)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_4F__EA__Da_Db_Dc_Dd" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.4f (RRRMMM,k",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => 23,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"k_imp_bits__16_16_x_10_10_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 64 > mem_data = DMEM ( EAtable ( MMM , RRR , 3 ) , 8 ) ;
    D ( k . Dd ) = signExtend ( concat ( mem_data ( 15 , 0 ) , zero ( 16 ) ) , 40 ) ;
    D ( k . Dc ) = signExtend ( concat ( mem_data ( 31 , 16 ) , zero ( 16 ) ) , 40 ) ;
    D ( k . Db ) = signExtend ( concat ( mem_data ( 47 , 32 ) , zero ( 16 ) ) , 40 ) ;
    D ( k . Da ) = signExtend ( concat ( mem_data ( 63 , 48 ) , zero ( 16 ) ) , 40 ) ;
    L ( k . Da ) = 0 ;
    L ( k . Db ) = 0 ;
    L ( k . Dc ) = 0 ;
    L ( k . Dd ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(k . Da)",
"D(k . Db)",
"D(k . Dc)",
"D(k . Dd)",
"L(k . Da)",
"L(k . Db)",
"L(k . Dc)",
"L(k . Dd)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_4W_Da_Db_Dc_Dd__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.4w k,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => 400,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"k_imp_bits__16_16_x_10_10_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    DMEM ( EAtable ( MMM , RRR , 3 ) , 8 ) = concat ( D ( k . Da ) ( 15 , 0 ) , D ( k . Db ) ( 15 , 0 ) , D ( k . Dc ) ( 15 , 0 ) , D ( k . Dd ) ( 15 , 0 ) ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D(k . Da)[mask 0x000000ffff]/p",
"D(k . Db)[mask 0x000000ffff]/p",
"D(k . Dc)[mask 0x000000ffff]/p",
"D(k . Dd)[mask 0x000000ffff]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_4W__EA__Da_Db_Dc_Dd" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.4w (RRRMMM,k",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_9_9_x_8_8_x_7_7_x_6_6_" => 404,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"k_imp_bits__16_16_x_10_10_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 64 > mem_data = DMEM ( EAtable ( MMM , RRR , 3 ) , 8 ) ;
    D ( k . Dd ) = signExtend ( mem_data ( 15 , 0 ) , 40 ) ;
    D ( k . Dc ) = signExtend ( mem_data ( 31 , 16 ) , 40 ) ;
    D ( k . Db ) = signExtend ( mem_data ( 47 , 32 ) , 40 ) ;
    D ( k . Da ) = signExtend ( mem_data ( 63 , 48 ) , 40 ) ;
    L ( k . Da ) = 0 ;
    L ( k . Db ) = 0 ;
    L ( k . Dc ) = 0 ;
    L ( k . Dd ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(k . Da)",
"D(k . Db)",
"D(k . Dc)",
"D(k . Dd)",
"L(k . Da)",
"L(k . Db)",
"L(k . Dc)",
"L(k . Dd)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_B_BTR1_HH__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.b btr1.hh,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_" => 602,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_16_16_x_14_14_" => "reserved",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    DMEM ( EAtable ( MMM , RRR , 0 ) , 1 ) = BTR1 ( 31 , 24 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"BTR1[mask 0xff000000]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_B_DR__Rnps15_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.b HHHH,(RRR+s15)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 20,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = signExtend ( s15 , 32 ) ;
     bits < 32 > eff_mem_add ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , imm , 0 , 1 , imm ) ;
    } else {
        eff_mem_add = ( imm  ) + R ( RRR ) ;
    }
    write_DRTable_2mem ( eff_mem_add , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"B?",
"DRTable(HHHH)[mask 0x00000000ff]/p",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_DRTable_2mem",
],
},
"MOVE_B_DR__SPps15_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.b HHHH,(sp+s15)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 228,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = ( sbits < 32 >  ) SP + ( sbits < 32 >  ) s15 ;
    write_DRTable_2mem ( eff_mem_add , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)[mask 0x00000000ff]/p",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"write_DRTable_2mem",
],
},
"MOVE_B_DR__a16_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.b HHHH,(a16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 100,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"a16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    write_DRTable_2mem ( a16 , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)[mask 0x00000000ff]/p",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"write_DRTable_2mem",
],
},
"MOVE_B_DR__a32_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.b HHHH,(a32)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 38,
"reserved_imp_bits__50_50_x_49_49_" => "reserved",
"HHHH_imp_bits__48_48_x_43_43_x_42_42_x_41_41_x_40_40_" => "",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    write_DRTable_2mem ( a32 , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"DRTable(HHHH)[mask 0x00000000ff]/p",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"write_DRTable_2mem",
],
},
"MOVE_B_DR__ea_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.b HHHH,(RRRMM",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_" => 76,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"MM_imp_bits__4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    write_DRTable_2mem ( EAtable ( ea_2EA ( MM ) , RRR , 0 ) , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"DRTable(HHHH)[mask 0x00000000ff]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"ea_2EA",
"increase_size",
"preshift",
"write_DRTable_2mem",
],
},
"MOVE_B_Db__Ea_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.b jjj,(RRRmm",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_x_5_5_" => 157,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"mm_imp_bits__4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 3 > mm_2MMM = mm + 4 ;
    DMEM ( EAtable ( mm_2MMM , RRR , 0 ) , 1 ) = D ( jjj ) ( 7 , 0 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D(jjj)[mask 0x00000000ff]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_B__Ea__Db" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.b (RRRmm,jjj",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_x_5_5_" => 156,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"mm_imp_bits__4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 3 > mm_2MMM = mm + 4 ;
    D ( jjj ) = signExtend ( DMEM ( EAtable ( mm_2MMM , RRR , 0 ) , 1 ) , 40 ) ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(jjj)",
"L(jjj)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_B__SPps15__DR" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.b (sp+s15),HHHH",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 756,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = ( sbits < 32 >  ) SP + ( sbits < 32 >  ) s15 ;
    write_mem_2DRTable ( eff_mem_add , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2DRTable",
],
},
"MOVE_B__a16__DR" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.b (a16),HHHH",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 372,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"a16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    write_mem_2DRTable ( zeroExtend ( a16 , 32 ) , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2DRTable",
],
},
"MOVE_B__ea__DR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.b (RRRMM,HHHH",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_" => 79,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"MM_imp_bits__4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    write_mem_2DRTable ( EAtable ( ea_2EA ( MM ) , RRR , 0 ) , HHHH , 1 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"ea_2EA",
"increase_size",
"preshift",
"write_mem_2DRTable",
],
},
"MOVE_F_Db__Ea_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.f jjj,(RRRmm",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_x_5_5_" => 130,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"mm_imp_bits__4_4_x_3_3_" => "",
},
"action" => <<DATA
{
     bits < 3 > mm_2MMM = mm + 4 ;
    DMEM ( EAtable ( mm_2MMM , RRR , 1 ) , 2 ) = D ( jjj ) . H ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D(jjj)[mask 0x00ffff0000]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_F_Db__ea_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.f jjj,(RRRMM",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_3_3_" => 147,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MM_imp_bits__11_11_x_4_4_" => "",
},
"action" => <<DATA
{
    DMEM ( EAtable ( ea_2EA ( MM ) , RRR , 1 ) , 2 ) = D ( jjj ) . H ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D(jjj)[mask 0x00ffff0000]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"ea_2EA",
"increase_size",
"preshift",
],
},
"MOVE_F__EA__Db" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.f (RRRMMM,jjj",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_7_7_x_6_6_" => 9,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"jjj_imp_bits__16_16_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    D ( jjj ) = signExtend ( concat ( DMEM ( EAtable ( MMM , RRR , 1 ) , 2 ) , zero ( 16 ) ) , 40 ) ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"D(jjj)",
"L(jjj)",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_F__Rnps15__Db" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.f (RRR+s15),jjj",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 116,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"jjj_imp_bits__32_32_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = signExtend ( s15 , 32 ) ;
     bits < 32 > eff_mem_add ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , imm , 0 , 2 , imm ) ;
    } else {
        eff_mem_add = imm + R ( RRR ) ;
    }
    D ( jjj ) = signExtend ( concat ( DMEM ( eff_mem_add , 2 ) , zero ( 16 ) ) , 40 ) ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
],
"outputs" => [
"D(jjj)",
"L(jjj)",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
],
},
"MOVE_F__SPps15__Db" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.f (sp+s15),jjj",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_23_23_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 484,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"jjj_imp_bits__32_32_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = ( sbits < 32 >  ) SP + ( sbits < 32 >  ) s15 ;
    D ( jjj ) = signExtend ( concat ( DMEM ( eff_mem_add , 2 ) , zero ( 16 ) ) , 40 ) ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"D(jjj)",
"L(jjj)",
],
"input_mems" => [
"DMEM",
],
},
"MOVE_F__a16__Db" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.f (a16),jjj",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 108,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"jjj_imp_bits__32_32_x_26_26_x_25_25_x_24_24_" => "",
"a16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    D ( jjj ) = signExtend ( concat ( DMEM ( a16 , 2 ) , zero ( 16 ) ) , 40 ) ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"outputs" => [
"D(jjj)",
"L(jjj)",
],
"input_mems" => [
"DMEM",
],
},
"MOVE_F__a32__Db" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.f (a32),jjj",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 358,
"reserved_imp_bits__50_50_x_49_49_" => "reserved",
"jjj_imp_bits__48_48_x_42_42_x_41_41_x_40_40_" => "",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    D ( jjj ) = signExtend ( concat ( DMEM ( a32 , 2 ) , zero ( 16 ) ) , 40 ) ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"D(jjj)",
"L(jjj)",
],
"input_mems" => [
"DMEM",
],
},
"MOVE_F__s16_Db" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.f #s16,jjj",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 1036,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"jjj_imp_bits__32_32_x_26_26_x_25_25_x_24_24_" => "",
"s16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    D ( jjj ) = signExtend ( concat ( s16 , zero ( 16 ) ) , 40 ) ;
    L ( jjj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"outputs" => [
"D(jjj)",
"L(jjj)",
],
},
"MOVE_L_BTR0__a32_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.l BTR0,(a32)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 4358,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    DMEM ( a32 , 4 ) = BTR0 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"BTR0",
],
"output_mems" => [
"DMEM",
],
},
"MOVE_L_BTR1__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l btr1,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_" => 586,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_16_16_x_14_14_" => "reserved",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    DMEM ( EAtable ( MMM , RRR , 2 ) , 4 ) = BTR1 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"BTR1",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_L_BTR1__a32_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.l BTR1,(a32)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 5382,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    DMEM ( a32 , 4 ) = BTR1 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"BTR1",
],
"output_mems" => [
"DMEM",
],
},
"MOVE_L_C2_Db" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l CCCC,jjj",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_3_" => 397,
"reserved_imp_bits__18_17_" => "reserved",
"jjj_imp_bits__16_16_x_2_0_" => "",
"CCCC_imp_bits__11_8_" => "",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) && ( ( CCCC == 1 ) || ( CCCC == 2 ) ) ) {
        EMR . ILPR = 1 ;
        raiseException ( Privelege ) ;
    } else {
        D ( jjj ) = signExtend ( C2Table ( CCCC ) , 40 ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"C2Table(CCCC)?",
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"D(jjj)?",
"EMR[mask 0x00000010]/p?",
],
"helpers" => [
"IsUserMode",
],
"raises_exception" => 1,
},
"MOVE_L_C3__Rn_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l DDDD,(RRR)",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 290,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
"DDDD_imp_bits__11_11_x_10_10_x_9_9_x_8_8_" => "",
},
"action" => <<DATA
{
    DMEM ( R ( RRR ) , 4 ) = C3Table ( DDDD ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"C3Table(DDDD)",
"R(RRR)",
],
"output_mems" => [
"DMEM",
],
},
"MOVE_L_C4_Df" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l DDDDD,uuu",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_5_x_3_3_" => 197,
"uuu_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"DDDDD_imp_bits__16_16_x_11_8_x_4_4_" => "",
},
"action" => <<DATA
{
    D ( uuu ) = signExtend ( C4Table ( DDDDD ) ( 31 , 0 ) , 40 ) ;
    L ( uuu ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"C4Table(DDDDD)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(uuu)",
"L(uuu)",
],
},
"MOVE_L_C4__SPps15_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.l DDDDD,(sp+s15)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_18_18_x_16_16_x_15_15_x_14_14_x_13_13_" => 228,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"DDDDD_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_x_17_17_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = ( sbits < 32 >  ) SP + ( sbits < 32 >  ) s15 ;
    write_C4Table_2mem ( eff_mem_add , DDDDD , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"C4Table(DDDDD)[mask 0x00ffffffff]/p",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"write_C4Table_2mem",
],
},
"MOVE_L_C4__a16_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.l DDDDD,(a16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_20_20_x_19_19_x_18_18_x_16_16_x_15_15_x_14_14_x_13_13_" => 44,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"DDDDD_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_x_17_17_" => "",
"a16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    write_C4Table_2mem ( zeroExtend ( a16 , 32 ) , DDDDD , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"C4Table(DDDDD)[mask 0x00ffffffff]/p",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"write_C4Table_2mem",
],
},
"MOVE_L_DR__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l HHHH,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_12_x_7_6_" => 2,
"RRR_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"HHHH_imp_bits__16_16_x_11_8_" => "",
"MMM_imp_bits__5_3_" => "",
},
"action" => <<DATA
{
    write_DRTable_2mem ( EAtable ( MMM , RRR , 2 ) , HHHH , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"DRTable(HHHH)[mask 0x00ffffffff]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
"write_DRTable_2mem",
],
},
"MOVE_L_DR__RnpRr_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l HHHH,(RRR+rrr)",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_" => 41,
"RRR_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"rrr_imp_bits__2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , ( R ( rrr ) << 2 ) , 0 , 4 , R ( rrr ) ) ;
    } else {
        eff_mem_add = ( R ( rrr ) << 2 ) + R ( RRR ) ;
    }
    write_DRTable_2mem ( eff_mem_add , HHHH , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"DRTable(HHHH)[mask 0x00ffffffff]/p",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
"R(rrr)",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_DRTable_2mem",
],
},
"MOVE_L_DR__Rnps15_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.l HHHH,(RRR+s15)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 36,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = signExtend ( s15 , 32 ) ;
     bits < 32 > eff_mem_add ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , imm , 0 , 4 , imm ) ;
    } else {
        eff_mem_add = ( imm  ) + R ( RRR ) ;
    }
    write_DRTable_2mem ( eff_mem_add , HHHH , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"B?",
"DRTable(HHHH)[mask 0x00ffffffff]/p",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_DRTable_2mem",
],
},
"MOVE_L_DR__Rnpu3_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l HHHH,(RRR+u3)",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_" => 45,
"RRR_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"u3_imp_bits__2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = zeroExtend ( u3 , 32 ) ;
     bits < 32 > eff_mem_add ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , ( imm << 2 ) , 0 , 4 , imm ) ;
    } else {
        eff_mem_add = ( imm << 2 ) + R ( RRR ) ;
    }
    write_DRTable_2mem ( eff_mem_add , HHHH , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"DRTable(HHHH)[mask 0x00ffffffff]/p",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_DRTable_2mem",
],
},
"MOVE_L_DR__SPmu6_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l HHHH,(sp-u6)",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_" => 61,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"u6_imp_bits__5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = SP - u6 * 4 ;
    write_DRTable_2mem ( eff_mem_add , HHHH , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"DRTable(HHHH)[mask 0x00ffffffff]/p",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"write_DRTable_2mem",
],
},
"MOVE_L_DR__a32_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.l HHHH,(a32)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 70,
"reserved_imp_bits__50_50_x_49_49_" => "reserved",
"HHHH_imp_bits__48_48_x_43_43_x_42_42_x_41_41_x_40_40_" => "",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    write_DRTable_2mem ( a32 , HHHH , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"DRTable(HHHH)[mask 0x00ffffffff]/p",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"write_DRTable_2mem",
],
},
"MOVE_L_Da_E_Db_E__SPps15_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.l ff,(sp+s15)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_24_24_x_23_23_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 1508,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"ff_imp_bits__32_32_x_26_26_x_25_25_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem = ( sbits < 32 >  ) SP + ( sbits < 32 >  ) s15 ;
     bits < 4 > Db_entry = ff . Db ;
     bits < 4 > Da_entry = ff . Da ;
     bits < 32 > value = concat ( zero ( 7 ) , L ( ff . Db ) , D ( Db_entry ) . E , zero ( 7 ) , L ( ff . Da ) , D ( Da_entry ) . E ) ;
    DMEM ( eff_mem , 4 ) = value ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D[mask 0xff00000000]/p",
"ESP?",
"L(ff . Da)",
"L(ff . Db)",
"NSP?",
"SP",
"SR/p",
],
"output_mems" => [
"DMEM",
],
},
"MOVE_L_Da_E_Db_E__a32_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.l ff,(a32)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 1030,
"reserved_imp_bits__50_50_x_49_49_" => "reserved",
"ff_imp_bits__48_48_x_42_42_x_41_41_" => "",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 4 > Db_entry = ff . Db ;
     bits < 4 > Da_entry = ff . Da ;
     bits < 32 > value = concat ( zero ( 7 ) , L ( ff . Db ) , D ( Db_entry ) . E , zero ( 7 ) , L ( ff . Da ) , D ( Da_entry ) . E ) ;
    DMEM ( a32 , 4 ) = value ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"D[mask 0xff00000000]/p",
"L(ff . Da)",
"L(ff . Db)",
],
"output_mems" => [
"DMEM",
],
},
"MOVE_L_Db_C2" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l jjj,CCCC",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_3_" => 396,
"reserved_imp_bits__18_17_" => "reserved",
"jjj_imp_bits__16_16_x_2_0_" => "",
"CCCC_imp_bits__11_8_" => "",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) && ( ( CCCC == 1 ) || ( CCCC == 2 ) ) ) {
        EMR . ILPR = 1 ;
        raiseException ( Privelege ) ;
    } else C2Table ( CCCC ) = ( D ( jjj ) ( 31 , 0 ) ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(jjj)[mask 0x00ffffffff]/p?",
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"C2Table(CCCC)?",
"EMR[mask 0x00000010]/p?",
],
"helpers" => [
"IsUserMode",
],
"raises_exception" => 1,
},
"MOVE_L_Df_C4" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l uuu,DDDDD",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_5_x_3_3_" => 196,
"uuu_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"DDDDD_imp_bits__16_16_x_11_8_x_4_4_" => "",
},
"action" => <<DATA
{
    if ( ( DDDDD ( 0 ) == 0 ) && ( DDDDD ( 4 ) == 0 ) ) {
        C4Table ( DDDDD ) = signExtend ( D ( uuu ) ( 31 , 0 ) , 40 ) ;
        L ( concat ( DDDDD ( 5 ) , DDDDD ( 3 , 1 ) ) ) = 0 ;
    } else {
        C4Table ( DDDDD ) = D ( uuu ) ( 31 , 0 ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(uuu)[mask 0x00ffffffff]/p",
],
"outputs" => [
"C4Table(DDDDD)",
"L?",
],
},
"MOVE_L__EA__DR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l (RRRMMM,HHHH",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_12_x_7_6_" => 6,
"RRR_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"HHHH_imp_bits__16_16_x_11_8_" => "",
"MMM_imp_bits__5_3_" => "",
},
"action" => <<DATA
{
    write_mem_2DRTable ( EAtable ( MMM , RRR , 2 ) , HHHH , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
"write_mem_2DRTable",
],
},
"MOVE_L__Rn__C3" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l (RRR),DDDD",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 291,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
"DDDD_imp_bits__11_11_x_10_10_x_9_9_x_8_8_" => "",
},
"action" => <<DATA
{
    C3Table ( DDDD ) = DMEM ( R ( RRR ) , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"R(RRR)",
],
"outputs" => [
"C3Table(DDDD)",
],
"input_mems" => [
"DMEM",
],
},
"MOVE_L__RnpRr__DR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l (RRR+rrr),HHHH",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_" => 43,
"RRR_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"rrr_imp_bits__2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , ( R ( rrr ) << 2 ) , 0 , 4 , R ( rrr ) ) ;
    } else {
        eff_mem_add = ( R ( rrr ) << 2 ) + R ( RRR ) ;
    }
    write_mem_2DRTable ( eff_mem_add , HHHH , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
"R(rrr)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_mem_2DRTable",
],
},
"MOVE_L__Rnps15__DR" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.l (RRR+s15),HHHH",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 100,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = signExtend ( s15 , 32 ) ;
     bits < 32 > eff_mem_add ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , imm , 0 , 4 , imm ) ;
    } else {
        eff_mem_add = ( imm  ) + R ( RRR ) ;
    }
    write_mem_2DRTable ( eff_mem_add , HHHH , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_mem_2DRTable",
],
},
"MOVE_L__Rnpu3__DR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l (RRR+u3),HHHH",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_" => 47,
"RRR_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"u3_imp_bits__2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = zeroExtend ( u3 , 32 ) ;
     bits < 32 > eff_mem_add ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , ( imm << 2 ) , 0 , 4 , imm ) ;
    } else {
        eff_mem_add = ( imm << 2 ) + R ( RRR ) ;
    }
    write_mem_2DRTable ( eff_mem_add , HHHH , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_mem_2DRTable",
],
},
"MOVE_L__SPmu6__DR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.l (sp-u6),HHHH",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_" => 63,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"u6_imp_bits__5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = SP - u6 * 4 ;
    write_mem_2DRTable ( eff_mem_add , HHHH , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2DRTable",
],
},
"MOVE_L__SPps15__C4" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.l (sp+s15),DDDDD",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_18_18_x_16_16_x_15_15_x_14_14_x_13_13_" => 484,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"DDDDD_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_x_17_17_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = ( sbits < 32 >  ) SP + ( sbits < 32 >  ) s15 ;
    write_mem_2C4Table ( eff_mem_add , DDDDD , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"C4Table(DDDDD)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2C4Table",
],
},
"MOVE_L__SPps15__De_E" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.l (sp+s15),QQ",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_24_24_x_23_23_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 1524,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"QQ_imp_bits__32_32_x_26_26_x_25_25_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = ( sbits < 32 >  ) SP + ( sbits < 32 >  ) s15 ;
     bits < 32 > value = DMEM ( eff_mem_add , 4 ) ;
     bits < 4 > entry = QQ . De ;
    D ( entry ) . E = value ( 7 , 0 ) ;
    L ( QQ . De ) = value ( 8 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"D[mask 0xff00000000]/p",
"L(QQ . De)",
],
"input_mems" => [
"DMEM",
],
},
"MOVE_L__SPps15__Do_E" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.l (sp+s15),qq",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_24_24_x_23_23_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_x_15_15_x_14_14_x_13_13_" => 2036,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"qq_imp_bits__32_32_x_26_26_x_25_25_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = ( sbits < 32 >  ) SP + ( sbits < 32 >  ) s15 ;
     bits < 32 > value = DMEM ( eff_mem_add , 4 ) ;
     bits < 4 > entry = qq . Do ;
    D ( entry ) . E = value ( 23 , 16 ) ;
    L ( qq . Do ) = value ( 24 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"D[mask 0xff00000000]/p",
"L(qq . Do)",
],
"input_mems" => [
"DMEM",
],
},
"MOVE_L__a16__C4" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.l (a16),DDDDD",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_20_20_x_19_19_x_18_18_x_16_16_x_15_15_x_14_14_x_13_13_" => 172,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"DDDDD_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_x_17_17_" => "",
"a16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    write_mem_2C4Table ( zeroExtend ( a16 , 32 ) , DDDDD , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"outputs" => [
"C4Table(DDDDD)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2C4Table",
],
},
"MOVE_L__a32__BTR0" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.l (a32),BTR0",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 4870,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    BTR0 = DMEM ( a32 , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"BTR0",
],
"input_mems" => [
"DMEM",
],
},
"MOVE_L__a32__BTR1" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.l (a32),BTR1",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 5894,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    BTR1 = DMEM ( a32 , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"BTR1",
],
"input_mems" => [
"DMEM",
],
},
"MOVE_L__a32__DR" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.l (a32),HHHH",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_44_x_34_32_x_31_29_x_15_14_" => 198,
"reserved_imp_bits__50_49_" => "reserved",
"HHHH_imp_bits__48_48_x_43_40_" => "",
"a32_imp_bits__36_35_x_13_0_x_39_37_x_28_16_" => "",
},
"action" => <<DATA
{
    write_mem_2DRTable ( a32 , HHHH , 4 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2DRTable",
],
},
"MOVE_L__a32__De_E" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.l (a32),QQ",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 1542,
"reserved_imp_bits__50_50_x_49_49_" => "reserved",
"QQ_imp_bits__48_48_x_42_42_x_41_41_" => "",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 32 > value = DMEM ( a32 , 4 ) ;
     bits < 4 > entry = QQ . De ;
    D ( entry ) . E = value ( 7 , 0 ) ;
    L ( QQ . De ) = value ( 8 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"D[mask 0xff00000000]/p",
"L(QQ . De)",
],
"input_mems" => [
"DMEM",
],
},
"MOVE_L__a32__Do_E" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.l (a32),qq",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 1798,
"reserved_imp_bits__50_50_x_49_49_" => "reserved",
"qq_imp_bits__48_48_x_42_42_x_41_41_" => "",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 32 > value = DMEM ( a32 , 4 ) ;
     bits < 4 > entry = qq . Do ;
    D ( entry ) . E = value ( 23 , 16 ) ;
    L ( qq . Do ) = value ( 24 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"D[mask 0xff00000000]/p",
"L(qq . Do)",
],
"input_mems" => [
"DMEM",
],
},
"MOVE_L__s32_C4" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.l #s32,DDDDD",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_44_x_34_34_x_32_32_x_31_29_x_15_14_" => 390,
"reserved_imp_bits__50_49_" => "reserved",
"DDDDD_imp_bits__48_48_x_43_40_x_33_33_" => "",
"s32_imp_bits__36_35_x_13_0_x_39_37_x_28_16_" => "",
},
"action" => <<DATA
{
    if ( ( DDDDD ( 0 , 0 ) == 0 ) && ( DDDDD ( 4 , 4 ) == 0 ) ) {
        C4Table ( DDDDD ) = signExtend ( s32 , 40 ) ;
        L ( concat ( DDDDD ( 5 , 5 ) , DDDDD ( 3 , 1 ) ) ) = 0 ;
    } else {
        C4Table ( DDDDD ) = s32 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"C4Table(DDDDD)",
"L?",
],
},
"MOVE_L__u32_C1" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.l #u32,CCC",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_43_x_34_32_x_31_29_x_15_14_" => 1638,
"reserved_imp_bits__50_48_" => "reserved",
"CCC_imp_bits__42_40_" => "",
"u32_imp_bits__36_35_x_13_0_x_39_37_x_28_16_" => "",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) && ( ( CCC == 1 ) || ( CCC == 2 ) ) ) {
        EMR_LOW = true ;
        EMR . ILPR = 1 ;
        raiseException ( Privelege ) ;
    } else {
        C1Table ( CCC ) = u32 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"C1Table(CCC)?",
"EMR[mask 0x00000010]/p?",
],
"helpers" => [
"IsUserMode",
],
"raises_exception" => 1,
},
"MOVE_W_BTR1_H__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.w btr1.h,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_" => 594,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_16_16_x_14_14_" => "reserved",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    DMEM ( EAtable ( MMM , RRR , 1 ) , 2 ) = BTR1 ( 31 , 16 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"BTR1[mask 0xffff0000]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"MOVE_W_C3__Rn_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.w DDDD,(RRR)",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 288,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
"DDDD_imp_bits__11_11_x_10_10_x_9_9_x_8_8_" => "",
},
"action" => <<DATA
{
    DMEM ( R ( RRR ) , 2 ) = C3Table ( DDDD ) ( 15 , 0 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"C3Table(DDDD)[mask 0x0000ffff]/p",
"R(RRR)",
],
"output_mems" => [
"DMEM",
],
},
"MOVE_W_C4__SPps15_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.w DDDDD,(sp+s15)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_18_18_x_16_16_x_15_15_x_14_14_x_13_13_" => 100,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"DDDDD_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_x_17_17_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = ( sbits < 32 >  ) SP + ( sbits < 32 >  ) s15 ;
    write_C4Table_2mem ( eff_mem_add , DDDDD , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"C4Table(DDDDD)[mask 0x000000ffff]/p",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"write_C4Table_2mem",
],
},
"MOVE_W_C4__a16_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.w DDDDD,(a16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_20_20_x_19_19_x_18_18_x_16_16_x_15_15_x_14_14_x_13_13_" => 36,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"DDDDD_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_x_17_17_" => "",
"a16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    write_C4Table_2mem ( zeroExtend ( a16 , 32 ) , DDDDD , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"C4Table(DDDDD)[mask 0x000000ffff]/p",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"write_C4Table_2mem",
],
},
"MOVE_W_DR__EA_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.w HHHH,(RRRMMM",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_7_7_x_6_6_" => 0,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    write_DRTable_2mem ( EAtable ( MMM , RRR , 1 ) , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"DRTable(HHHH)[mask 0x000000ffff]/p",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"R(RRR)?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
"write_DRTable_2mem",
],
},
"MOVE_W_DR__RnpRr_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.w HHHH,(RRR+rrr)",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_" => 40,
"RRR_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"rrr_imp_bits__2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , ( R ( rrr ) << 1 ) , 0 , 2 , R ( rrr ) ) ;
    } else {
        eff_mem_add = ( R ( rrr ) << 1 ) + R ( RRR ) ;
    }
    write_DRTable_2mem ( eff_mem_add , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"DRTable(HHHH)[mask 0x000000ffff]/p",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
"R(rrr)",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_DRTable_2mem",
],
},
"MOVE_W_DR__Rnps15_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.w HHHH,(RRR+s15)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 4,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add ;
     bits < 32 > imm = signExtend ( s15 , 32 ) ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , ( imm ) , 0 , 2 , imm ) ;
    } else {
        eff_mem_add = ( imm  ) + R ( RRR ) ;
    }
    write_DRTable_2mem ( eff_mem_add , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"B?",
"DRTable(HHHH)[mask 0x000000ffff]/p",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_DRTable_2mem",
],
},
"MOVE_W_DR__Rnpu3_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.w HHHH,(RRR+u3)",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_" => 44,
"RRR_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"u3_imp_bits__2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add ;
     bits < 32 > imm = zeroExtend ( u3 , 32 ) ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , ( imm << 1 ) , 0 , 2 , imm ) ;
    } else {
        eff_mem_add = ( imm << 1 ) + R ( RRR ) ;
    }
    write_DRTable_2mem ( eff_mem_add , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"DRTable(HHHH)[mask 0x000000ffff]/p",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_DRTable_2mem",
],
},
"MOVE_W_DR__SPmu6_" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.w HHHH,(sp-u6)",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_" => 60,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"u6_imp_bits__5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = SP - ( u6 * 2 ) ;
    write_DRTable_2mem ( eff_mem_add , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"DRTable(HHHH)[mask 0x000000ffff]/p",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"write_DRTable_2mem",
],
},
"MOVE_W_DR__a32_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.w HHHH,(a32)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 6,
"reserved_imp_bits__50_50_x_49_49_" => "reserved",
"HHHH_imp_bits__48_48_x_43_43_x_42_42_x_41_41_x_40_40_" => "",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    write_DRTable_2mem ( a32 , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"DRTable(HHHH)[mask 0x000000ffff]/p",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"write_DRTable_2mem",
],
},
"MOVE_W__EA__DR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.w (RRRMMM,HHHH",
"block" => "agu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_7_7_x_6_6_" => 4,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
    write_mem_2DRTable ( EAtable ( MMM , RRR , 1 ) , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(RRR)?",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
"R(RRR)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
"write_mem_2DRTable",
],
},
"MOVE_W__Rn__C3" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.w (RRR),DDDD",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 289,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
"DDDD_imp_bits__11_11_x_10_10_x_9_9_x_8_8_" => "",
},
"action" => <<DATA
{
    C3Table ( DDDD ) = signExtend ( DMEM ( R ( RRR ) , 2 ) , 32 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"R(RRR)",
],
"outputs" => [
"C3Table(DDDD)",
],
"input_mems" => [
"DMEM",
],
},
"MOVE_W__RnpRr__DR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.w (RRR+rrr),HHHH",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_" => 42,
"RRR_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"rrr_imp_bits__2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , ( R ( rrr ) << 1 ) , 0 , 2 , R ( rrr ) ) ;
    } else {
        eff_mem_add = ( R ( rrr ) << 1 ) + R ( RRR ) ;
    }
    write_mem_2DRTable ( eff_mem_add , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
"R(rrr)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_mem_2DRTable",
],
},
"MOVE_W__Rnps15__DR" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.w (RRR+s15),HHHH",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 68,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_" => "reserved",
"HHHH_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add ;
     bits < 32 > imm = signExtend ( s15 , 32 ) ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , ( imm ) , 0 , 2 , imm ) ;
    } else {
        eff_mem_add = ( imm  ) + R ( RRR ) ;
    }
    write_mem_2DRTable ( eff_mem_add , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_mem_2DRTable",
],
},
"MOVE_W__Rnpu3__DR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.w (RRR+u3),HHHH",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_" => 46,
"RRR_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"u3_imp_bits__2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add ;
     bits < 32 > imm = zeroExtend ( u3 , 32 ) ;
    if ( RRR ( 3 ) == 0 ) {
        eff_mem_add = Address_by_mctl2 ( RRR ( 2 , 0 ) , ( imm << 1 ) , 0 , 2 , imm ) ;
    } else {
        eff_mem_add = ( imm << 1 ) + R ( RRR ) ;
    }
    write_mem_2DRTable ( eff_mem_add , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"R(RRR)?",
"R(RRR ( 2 , 0 ))?",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"Address_by_mctl2",
"bitreverse",
"preshift",
"write_mem_2DRTable",
],
},
"MOVE_W__SPmu6__DR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.w (sp-u6),HHHH",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_" => 62,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"u6_imp_bits__5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = SP - ( u6 * 2 ) ;
    write_mem_2DRTable ( eff_mem_add , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2DRTable",
],
},
"MOVE_W__SPps15__C4" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.w (sp+s15),DDDDD",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_23_23_x_20_20_x_19_19_x_18_18_x_16_16_x_15_15_x_14_14_x_13_13_" => 356,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"DDDDD_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_x_17_17_" => "",
"s15_imp_bits__22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = ( sbits < 32 >  ) SP + ( sbits < 32 >  ) s15 ;
    write_mem_2C4Table ( eff_mem_add , DDDDD , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"C4Table(DDDDD)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2C4Table",
],
},
"MOVE_W__a16__C4" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.w (a16),DDDDD",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_20_20_x_19_19_x_18_18_x_16_16_x_15_15_x_14_14_x_13_13_" => 164,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"DDDDD_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_x_17_17_" => "",
"a16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    write_mem_2C4Table ( zeroExtend ( a16 , 32 ) , DDDDD , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"outputs" => [
"C4Table(DDDDD)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2C4Table",
],
},
"MOVE_W__a32__DR" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.w (a32),HHHH",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 134,
"reserved_imp_bits__50_50_x_49_49_" => "reserved",
"HHHH_imp_bits__48_48_x_43_43_x_42_42_x_41_41_x_40_40_" => "",
"a32_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_x_39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
    write_mem_2DRTable ( a32 , HHHH , 2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"outputs" => [
"DRTable(HHHH)",
"L?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"write_mem_2DRTable",
],
},
"MOVE_W__s16_C4" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.w #s16,DDDDD",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_20_20_x_19_19_x_18_18_x_16_16_x_15_15_x_14_14_x_13_13_" => 260,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"DDDDD_imp_bits__32_32_x_27_27_x_26_26_x_25_25_x_24_24_x_17_17_" => "",
"s16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( ( DDDDD ( 0 , 0 ) == 0 ) && ( DDDDD ( 4 , 4 ) == 0 ) ) {
        C4Table ( DDDDD ) = signExtend ( s16 , 40 ) ;
        L ( concat ( DDDDD ( 5 , 5 ) , DDDDD ( 3 , 1 ) ) ) = 0 ;
    } else {
        C4Table ( DDDDD ) = signExtend ( s16 , 32 ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"outputs" => [
"C4Table(DDDDD)",
"L?",
],
},
"MOVE_W__s16__Rn_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.w #s16,(RRR)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 813,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
"s16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    DMEM ( R ( RRR ) , 2 ) = s16 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
],
"output_mems" => [
"DMEM",
],
},
"MOVE_W__s16__SPmu5_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "move.w #s16,(sp-u5)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_15_15_x_14_14_x_13_13_" => 197,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"s16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"u5_imp_bits__20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = SP - u5 * 2 ;
    DMEM ( eff_mem_add , 2 ) = s16 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"output_mems" => [
"DMEM",
],
},
"MOVE_W__s16__SPpsa16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.w #s16,(sp+sa16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 14502,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"sa16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > eff_mem_add = ( sbits < 32 >  ) SP + ( sbits < 32 >  ) sa16 ;
    DMEM ( eff_mem_add , 2 ) = s16 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"output_mems" => [
"DMEM",
],
},
"MOVE_W__s16__a16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "move.w #s16,(a16)",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__51_51_x_47_47_x_46_46_x_45_45_x_44_44_x_43_43_x_42_42_x_41_41_x_40_40_x_34_34_x_33_33_x_32_32_x_31_31_x_30_30_x_29_29_x_15_15_x_14_14_" => 14470,
"reserved_imp_bits__50_50_x_49_49_x_48_48_" => "reserved",
"a16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    DMEM ( a16 , 2 ) = s16 ;
}
DATA
,
"disassemble" => 1,
"type" => "ThreeWords",
"output_mems" => [
"DMEM",
],
},
"MOVE_W__s7_DR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "move.w #s7,HHHH",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_" => 25,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"HHHH_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"s7_imp_bits__6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( HHHH ( 3 ) == 0 ) {
        DRTable ( HHHH ) = signExtend ( s7 , 40 ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    } else {
        DRTable ( HHHH ) = signExtend ( s7 , 32 ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"MPY2R_Da_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpy2r jj,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3588,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPY2R_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpy2r JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 452,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPY2_Da_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpy2 jj,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3585,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPY2_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpy2 JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 449,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPYCIM_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpycim JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_" => 222,
"jjj_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPYD_mDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyd -jj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3545,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPYD_mDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyd -JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_" => 425,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPYD_pDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyd +jj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3544,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPYD_pDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyd +JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_" => 424,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPYIM_Da_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyim jj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3547,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPYIM_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyim JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_" => 427,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPYRE_Da_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyre jj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3546,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPYRE_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyre JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_" => 426,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPYR_Da_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyr jj,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 282,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj . Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj . Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
    D ( FFF ) = round ( mult ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(jj . Da)[mask 0x00ffff0000]/p",
"D(jj . Db)[mask 0x00ffff0000]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MPYR_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyr JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 37,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ . Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ . Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
    D ( FFF ) = round ( mult ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJJJ . Da)[mask 0x00ffff0000]/p",
"D(JJJJJ . Db)[mask 0x00ffff0000]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MPYR_Da_H_Da_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyr jj_DaH_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_2_" => 3837,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj . Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj . Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
    D ( FFF ) = round ( mult ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(jj . Da)[mask 0x00ffff0000]/p",
"D(jj . Db)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MPYR_Da_H_Db_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyr DaH_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_5_" => 469,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ . Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ . Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
    D ( FFF ) = round ( mult ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(JJJJJ . Da)[mask 0x00ffff0000]/p",
"D(JJJJJ . Db)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MPYR_Da_L_Da_H_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyr jj_DaL_DbH,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_2_" => 3838,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj . Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( jj . Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
    D ( FFF ) = round ( mult ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(jj . Da)[mask 0x000000ffff]/p",
"D(jj . Db)[mask 0x00ffff0000]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MPYR_Da_L_Da_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyr jj_DaL_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_2_" => 3839,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj . Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( jj . Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
    D ( FFF ) = round ( mult ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(jj . Da)[mask 0x000000ffff]/p",
"D(jj . Db)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MPYR_Da_L_Db_H_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyr DaL_DbH,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_5_" => 470,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ . Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( JJJJJ . Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
    D ( FFF ) = round ( mult ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(JJJJJ . Da)[mask 0x000000ffff]/p",
"D(JJJJJ . Db)[mask 0x00ffff0000]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MPYR_Da_L_Db_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyr DaL_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_5_" => 471,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ . Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( JJJJJ . Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
    D ( FFF ) = round ( mult ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(JJJJJ . Da)[mask 0x000000ffff]/p",
"D(JJJJJ . Db)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MPYSU_Dc_Dd_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpysu ee,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 378,
"ee_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 32 > mult = signedMultiply ( ( signExtend ( D ( ee . Dc ) ( 31 , 16 ) , 40 ) ) , ( zeroExtend ( D ( ee . Dd ) ( 15 , 0 ) , 40 ) ) ) ;
    D ( FFF ) = ( signExtend ( mult , 40 ) ) << 1 ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(ee . Dc)[mask 0x00ffff0000]/p",
"D(ee . Dd)[mask 0x000000ffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"MPYUS_Dc_Dd_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyus ee,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 285,
"ee_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 32 > mult = signedMultiply ( ( signExtend ( D ( ee . Dd ) ( 31 , 16 ) , 40 ) ) , ( zeroExtend ( D ( ee . Dc ) ( 15 , 0 ) , 40 ) ) ) ;
    D ( FFF ) = ( signExtend ( mult , 40 ) ) << 1 ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(ee . Dc)[mask 0x000000ffff]/p",
"D(ee . Dd)[mask 0x00ffff0000]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"MPYUU_Dc_Dd_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyuu ee,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 379,
"ee_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 40 > mult = signedMultiply ( ( zeroExtend ( D ( ee . Dc ) ( 15 , 0 ) , 40 ) ) , ( zeroExtend ( D ( ee . Dd ) ( 15 , 0 ) , 40 ) ) ) ;
    D ( FFF ) = ( signExtend ( mult ( 32 , 0 ) , 40 ) ) << 1 ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(ee . Dc)[mask 0x000000ffff]/p",
"D(ee . Dd)[mask 0x000000ffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"MPYX_Da_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyx jj,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 3646,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPYX_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpyx JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MPY" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 462,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"MPY_Da_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpy jj,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 348,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( jj . Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj . Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ;
    saturation32 ( mult , FFF ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(jj . Da)[mask 0x00ffff0000]/p",
"D(jj . Db)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MPY_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpy JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 33,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( JJJJJ . Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ . Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ;
    saturation32 ( mult , FFF ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJJJ . Da)[mask 0x00ffff0000]/p",
"D(JJJJJ . Db)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MPY_Da_H_Da_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpy jj_DaH_DbL.h,FFF.l,jj(jj_DaH_DbL)",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_2_" => 3834,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( jj . Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj . Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ;
    saturation32 ( mult , FFF ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(jj . Da)[mask 0x00ffff0000]/p",
"D(jj . Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MPY_Da_H_Db_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpy DaH_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_5_" => 464,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( JJJJJ . Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ . Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ;
    saturation32 ( mult , FFF ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(JJJJJ . Da)[mask 0x00ffff0000]/p",
"D(JJJJJ . Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MPY_Da_L_Da_H_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpy jj_DaL_DbH,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_2_" => 3835,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( jj . Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( jj . Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ;
    saturation32 ( mult , FFF ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(jj . Da)[mask 0x000000ffff]/p",
"D(jj . Db)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MPY_Da_L_Da_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpy jj_DaL_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_2_" => 3836,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( jj . Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( jj . Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ;
    saturation32 ( mult , FFF ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(jj . Da)[mask 0x000000ffff]/p",
"D(jj . Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MPY_Da_L_Db_H_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpy DaL_DbH,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_5_" => 465,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( JJJJJ . Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( JJJJJ . Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ;
    saturation32 ( mult , FFF ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(JJJJJ . Da)[mask 0x000000ffff]/p",
"D(JJJJJ . Db)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MPY_Da_L_Db_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mpy DaL_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_5_" => 466,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( JJJJJ . Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( JJJJJ . Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ;
    saturation32 ( mult , FFF ) ;
    L ( FFF ) = LnComputation ( mult ) ;
}
DATA
,
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(JJJJJ . Da)[mask 0x000000ffff]/p",
"D(JJJJJ . Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"NAND_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "nand JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DLU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1916,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = ~ ( D ( FFF ) & D ( JJJ ) ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"NEG2_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "neg2 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1908,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 17 > zero = 0x0 ;
     bits < 17 > subh = zero - ( bits < 17 >  ) D ( JJJ ) . H ;
     bits < 17 > subl = zero - ( bits < 17 >  ) D ( JJJ ) . L ;
    D ( FFF ) . H = subh ( 15 , 0 ) ;
    D ( FFF ) . L = subl ( 15 , 0 ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = extension_update_sub ( subh , subl ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"extension_update_sub",
],
},
"NEG_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "neg FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 1252,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > sum = ( sbits < 41 >  ) ( 0x0 ) - ( sbits < 41 >  ) D ( FFF ) ;
    overflow40 ( sum ) ;
    saturation32 ( sum , FFF ) ;
    L ( FFF ) = LnComputation ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"NEOR_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "neor JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DLU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1820,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = ~ ( D ( FFF ) ^ D ( JJJ ) ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"NOR_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "nor JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DLU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1821,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = ~ ( D ( FFF ) | D ( JJJ ) ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"NOT_DR_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "NOT_DR_H HHHH",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCHG__u16_DR_H" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "HHHH",
},
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => 65535,
},
],
"parent_action" => <<DATA
{
    if ( HHHH ( 3 ) == 1 ) {
        DRTable ( HHHH ) = DRTable ( HHHH ) ^ ( concat ( u16 , zero ( 16 ) ) ) ;
    } else {
        DRTable ( HHHH ) = DRTable ( HHHH ) ^ ( concat ( zero ( 8 ) , u16 , zero ( 16 ) ) ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"NOT_DR_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "NOT_DR_L HHHH",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCHG__u16_DR_L" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "HHHH",
},
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => 65535,
},
],
"parent_action" => <<DATA
{
    if ( HHHH ( 3 , 3 ) == 1 ) {
        DRTable ( HHHH ) = DRTable ( HHHH ) ^ ( zeroExtend ( u16 , 32 ) ) ;
    } else {
        DRTable ( HHHH ) = DRTable ( HHHH ) ^ ( zeroExtend ( u16 , 40 ) ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"NOT_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "not JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_3_" => 864,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = ~ ( D ( JJJ ) ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"NOT_W__Rn_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "NOT_W__Rn_ RRR",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCHG_W__u16__Rn_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_",
"value" => "RRR",
},
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => 65535,
},
],
"parent_action" => <<DATA
{
    DMEM ( R ( RRR ) , 2 ) = DMEM ( R ( RRR ) , 2 ) ^ ( u16 ) ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"NOT_W__SPmu5_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "NOT_W__SPmu5_ u5",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCHG_W__u16__SPmu5_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => 65535,
},
"misc" => {
"field" => "u5_imp_bits__20_20_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "u5",
},
],
"parent_action" => <<DATA
{
     bits < 6 > u6 = concat ( u5 , zero ( 1 ) ) ;
     bits < 32 > offset = concat ( zero ( 26 ) , u6 ) ;
    DMEM ( ( SP - offset ) , 2 ) = DMEM ( ( SP - offset ) , 2 ) ^ ( u16 ) ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"NOT_W__SPps16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "NOT_W__SPps16_ s16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCHG_W__u16__SPps16_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "s16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "s16",
},
"misc" => {
"field" => "u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => 65535,
},
],
"parent_action" => <<DATA
{
     var offset = signExtend ( s16 , 32 ) ;
    DMEM ( ( SP + offset ) , 2 ) = ( DMEM ( ( SP + offset ) , 2 ) ) ^ ( u16 ) ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"NOT_W__a16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "NOT_W__a16_ a16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMCHG_W__u16__a16_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "a16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "a16",
},
"misc" => {
"field" => "u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => 65535,
},
],
"parent_action" => <<DATA
{
    DMEM ( a16 , 2 ) = ( DMEM ( a16 , 2 ) ) ^ ( u16 ) ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "ThreeWords",
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"ORA_rx_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "ora rrrr,RRRR",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_" => 509,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"rrrr_imp_bits__16_16_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    RxTable ( RRRR ) = rxTable ( rrrr ) | RxTable ( RRRR ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
"rxTable(rrrr)",
],
"outputs" => [
"RxTable(RRRR)",
],
},
"OR_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "or JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_3_" => 883,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = ( D ( JJJ ) | D ( FFF ) ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"OR_W__u16__Rn_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "OR_W__u16__Rn_ u16, RRR",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMSET_W__u16__Rn_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_",
"value" => "RRR",
},
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "u16",
},
],
"parent_action" => <<DATA
{
    DMEM ( R ( RRR ) , 2 ) = DMEM ( R ( RRR ) , 2 ) | u16 ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"R(RRR)",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"OR_W__u16__SPmu5_" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "OR_W__u16__SPmu5_ u5, u16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMSET_W__u16__SPmu5_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "u16",
},
"misc" => {
"field" => "u5_imp_bits__20_20_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "u5",
},
],
"parent_action" => <<DATA
{
     bits < 6 > u6 = concat ( u5 , zero ( 1 ) ) ;
     bits < 32 > offset = concat ( zero ( 26 ) , u6 ) ;
    DMEM ( ( SP - offset ) , 2 ) = DMEM ( ( SP - offset ) , 2 ) | u16 ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"OR_W__u16__SPps16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "OR_W__u16__SPps16_ u16, s16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMSET_W__u16__SPps16_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "s16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "s16",
},
"misc" => {
"field" => "u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "u16",
},
],
"parent_action" => <<DATA
{
    DMEM ( ( SP + signExtend ( s16 , 32 ) ) , 2 ) = ( DMEM ( ( SP + signExtend ( s16 , 32 ) ) , 2 ) ) | ( u16 ) ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "ThreeWords",
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"OR_W__u16__a16_" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "OR_W__u16__a16_ u16, a16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"three_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMSET_W__u16__a16_" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "a16_imp_bits__39_39_x_38_38_x_37_37_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "a16",
},
"misc" => {
"field" => "u16_imp_bits__36_36_x_35_35_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "u16",
},
],
"parent_action" => <<DATA
{
    DMEM ( a16 , 2 ) = DMEM ( a16 , 2 ) | u16 ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "ThreeWords",
"input_mems" => [
"DMEM",
],
"output_mems" => [
"DMEM",
],
},
"OR__u16_DR_H" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "OR__u16_DR_H HHHH, u16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMSET__u16_DR_H" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "HHHH",
},
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "u16",
},
],
"parent_action" => <<DATA
{
    if ( HHHH ( 3 ) == 1 ) {
        DRTable ( HHHH ) = DRTable ( HHHH ) | concat ( u16 , zero ( 16 ) ) ;
    } else {
        DRTable ( HHHH ) = DRTable ( HHHH ) | concat ( zero ( 8 ) , u16 , zero ( 16 ) ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"OR__u16_DR_L" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "OR__u16_DR_L HHHH, u16",
"block" => "agu",
"attributes" => {
"Type3" => "",
"two_word" => "",
"instr_tbl_BMU" => "",
},
"aliases" => {
"BMSET__u16_DR_L" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "HHHH_imp_bits__32_32_x_19_19_x_18_18_x_17_17_x_16_16_",
"value" => "HHHH",
},
"misc" => {
"field" => "u16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => "u16",
},
],
"parent_action" => <<DATA
{
    if ( HHHH ( 3 ) == 1 ) {
        DRTable ( HHHH ) = DRTable ( HHHH ) | concat ( zero ( 16 ) , u16 ) ;
    } else {
        DRTable ( HHHH ) = DRTable ( HHHH ) | concat ( zero ( 24 ) , u16 ) ;
        L ( concat ( HHHH ( 4 , 4 ) , HHHH ( 2 , 0 ) ) ) = 0 ;
    }
}
DATA
,
},
},
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"DRTable(HHHH)",
],
"outputs" => [
"DRTable(HHHH)",
"L?",
],
},
"OW_HR_Prefix" => {
"width" => 16,
"syntax" => "OW_HR_Prefix VlesSize=VlesSize, Highreg=HighReg",
"attributes" => {
"prefix" => "",
},
"fields" => {
"VarPrefixOpcode_imp_bits__15_12_x_8_5_" => 158,
"VlesSize_imp_bits__11_9_" => "",
"HighReg_imp_bits__3_3_x_1_1_x_4_4_x_2_2_x_0_0_" => "",
},
"action" => <<DATA
{
    prefix_grouping = true ;
}
DATA
,
"prefix" => 1,
"disassemble" => 1,
},
"OW_Prefix" => {
"width" => 16,
"syntax" => "OW_Prefix: VlesSize=VlesSize, lpmarks=(lpmarkA,lpmarkB), cond_exec = ccc",
"attributes" => {
"prefix" => "",
},
"fields" => {
"VarPrefixOpcode_imp_bits__15_12_x_8_5_" => 150,
"VlesSize_imp_bits__11_9_" => "",
"lpmarkB_imp_bits__4_4_" => "",
"lpmarkA_imp_bits__3_3_" => "",
"ccc_imp_bits__2_0_" => "",
},
"action" => <<DATA
{
    if ( VlesSize == 0 ) {
        prefix_grouping = false ;
        nop_prefix = true ;
        reset_prefix_size (  ) ;
    } else {
        prefix_grouping = true ;
    }
    init_cond_exec ( ccc ) ;
}
DATA
,
"prefix" => 1,
"disassemble" => 1,
"helpers" => [
"init_cond_exec",
"reset_prefix_size",
],
},
"OneWord" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "OneWord ESG_imp_bits__19_19_, HighRegister_imp_bits__18_16_, Enc1_imp_bits__15_0_",
"fields" => {
"ESG_imp_bits__19_19_" => "",
"HighRegister_imp_bits__18_16_" => "",
"Enc1_imp_bits__15_0_" => "",
},
"pseudo" => 1,
"disassemble" => 1,
},
"PACK4B_Ds_Dt_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "pack4b GGG,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1770,
"GGG_imp_bits__18_18_x_17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"PACK_2F_Dj_Dk_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "pack.2f KKK,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_7_7_x_6_6_x_4_4_x_3_3_" => 684,
"KKK_imp_bits__18_18_x_17_17_x_9_9_x_8_8_x_5_5_" => "",
"FFF_imp_bits__16_16_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     unsigned j = KKK . Dj ;
     unsigned k = KKK . Dk ;
    D ( FFF ) . L = D ( j ) . H ;
    D ( FFF ) . H = D ( k ) . H ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D[mask 0x00ffff0000]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"PACK_2W_Dj_Dk_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "pack.2w KKK,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_7_7_x_6_6_x_4_4_x_3_3_" => 676,
"KKK_imp_bits__18_18_x_17_17_x_9_9_x_8_8_x_5_5_" => "",
"FFF_imp_bits__16_16_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"action" => <<DATA
{
     unsigned j = KKK . Dj ;
     unsigned k = KKK . Dk ;
    D ( FFF ) . L = D ( j ) . L ;
    D ( FFF ) . H = D ( k ) . L ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D[mask 0x000000ffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"PF05_V_Rn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pf05.v RRR",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 587374603,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PF05_V_label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pf05.v a16",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_3_3_x_2_2_x_1_1_" => 71981,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"a16_imp_bits__18_18_x_17_17_x_16_16_x_0_0_x_23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PF06_V_Rn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pf06.v RRR",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 587374605,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PF06_V_label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pf06.v a16",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_3_3_x_2_2_x_1_1_" => 71982,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"a16_imp_bits__18_18_x_17_17_x_16_16_x_0_0_x_23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PFETCH_Rn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pfetch RRR",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 587374601,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PFETCH_label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pfetch a16",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_3_3_x_2_2_x_1_1_" => 71980,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"a16_imp_bits__18_18_x_17_17_x_16_16_x_0_0_x_23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PFL2C0_Rn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pfl2c0 RRR",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 587374593,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PFL2C0_label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pfl2c0 a16",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_3_3_x_2_2_x_1_1_" => 71976,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"a16_imp_bits__18_18_x_17_17_x_16_16_x_0_0_x_23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PFL2C1_Rn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pfl2c1 RRR",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 587374595,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PFL2C1_label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pfl2c1 a16",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_3_3_x_2_2_x_1_1_" => 71977,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"a16_imp_bits__18_18_x_17_17_x_16_16_x_0_0_x_23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PFL2C2_Rn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pfl2c2 RRR",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 587374597,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PFL2C2_label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pfl2c2 a16",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_3_3_x_2_2_x_1_1_" => 71978,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"a16_imp_bits__18_18_x_17_17_x_16_16_x_0_0_x_23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PFL2C3_Rn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pfl2c3 RRR",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 587374599,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PFL2C3_label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pfl2c3 a16",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_3_3_x_2_2_x_1_1_" => 71979,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"a16_imp_bits__18_18_x_17_17_x_16_16_x_0_0_x_23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"POPN_2L_De_Do" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "popn.2l QQQQQ",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_STK" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_9_9_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1261,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"QQQQQ_imp_bits__16_16_x_10_10_x_8_8_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    check_exception_DxTable ( QQQQQ ) ;
    mem_write_popn2l_DeDoTable ( QQQQQ ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"NSP",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"DeDoTable(QQQQQ)",
"EMR[mask 0x00000010]/p?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"check_exception_DxTable",
"mem_write_popn2l_DeDoTable",
],
"raises_exception" => 1,
},
"POPN_De" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "popn EEEEE",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_STK" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_3_3_x_2_2_x_0_0_" => 1163,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"EEEEE_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_4_4_x_1_1_" => "",
},
"action" => <<DATA
{
    check_exception_DxTable ( EEEEE ) ;
    mem_write_popn_DeTable ( EEEEE ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"NSP",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"DeTable(EEEEE)",
"EMR[mask 0x00000010]/p?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"check_exception_DxTable",
"mem_write_popn_DeTable",
],
"raises_exception" => 1,
},
"POPN_Do" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "popn eeeee",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_STK" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_3_3_x_2_2_x_0_0_" => 1227,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"eeeee_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_4_4_x_1_1_" => "",
},
"action" => <<DATA
{
    check_exception_DxTable ( eeeee ) ;
    mem_write_popn_DoTable ( eeeee ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"NSP",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"DoTable(eeeee)",
"EMR[mask 0x00000010]/p?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"check_exception_DxTable",
"mem_write_popn_DoTable",
],
"raises_exception" => 1,
},
"POP_2L_De_Do" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "pop.2l QQQQQ",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_STK" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_9_9_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1229,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"QQQQQ_imp_bits__16_16_x_10_10_x_8_8_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    check_exception_DxTable ( QQQQQ ) ;
    mem_write_pop2l_DeDoTable ( QQQQQ ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"DeDoTable(QQQQQ)",
"EMR[mask 0x00000010]/p?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"check_exception_DxTable",
"mem_write_pop2l_DeDoTable",
],
"raises_exception" => 1,
},
"POP_De" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "pop EEEEE",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_STK" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_3_3_x_2_2_x_0_0_" => 1161,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"EEEEE_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_4_4_x_1_1_" => "",
},
"action" => <<DATA
{
    check_exception_DxTable ( EEEEE ) ;
    mem_write_pop_DeTable ( EEEEE ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"DeTable(EEEEE)",
"EMR[mask 0x00000010]/p?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"check_exception_DxTable",
"mem_write_pop_DeTable",
],
"raises_exception" => 1,
},
"POP_Do" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "pop eeeee",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_STK" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_3_3_x_2_2_x_0_0_" => 1225,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"eeeee_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_4_4_x_1_1_" => "",
},
"action" => <<DATA
{
    check_exception_DxTable ( eeeee ) ;
    mem_write_pop_DoTable ( eeeee ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"DoTable(eeeee)",
"EMR[mask 0x00000010]/p?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"check_exception_DxTable",
"mem_write_pop_DoTable",
],
"raises_exception" => 1,
},
"PQUERY_Rn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pquery RRR",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_23_23_x_22_22_x_21_21_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 587374607,
"RRR_imp_bits__34_34_x_18_18_x_17_17_x_16_16_" => "",
"reserved_imp_bits__33_33_x_32_32_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PQUERY_label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "pquery a16",
"block" => "agu",
"attributes" => {
"Type13" => "",
"two_word" => "",
"instr_tbl_ACH" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_x_3_3_x_2_2_x_1_1_" => 71983,
"reserved_imp_bits__34_34_x_33_33_x_32_32_" => "reserved",
"a16_imp_bits__18_18_x_17_17_x_16_16_x_0_0_x_23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
},
"PUSHN_2L_De_Do" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "pushn.2l QQQQQ",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_STK" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_9_9_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1231,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"QQQQQ_imp_bits__16_16_x_10_10_x_8_8_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    check_exception_DxTable ( QQQQQ ) ;
    mem_write_pushn2l_DeDoTable ( QQQQQ ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"DeDoTable(QQQQQ)",
"EMR/p",
"NSP",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"check_exception_DxTable",
"mem_write_pushn2l_DeDoTable",
],
"raises_exception" => 1,
},
"PUSHN_De" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "pushn EEEEE",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_STK" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_3_3_x_2_2_x_0_0_" => 1162,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"EEEEE_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_4_4_x_1_1_" => "",
},
"action" => <<DATA
{
    check_exception_DxTable ( EEEEE ) ;
    mem_write_pushn_DeTable ( EEEEE ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"DeTable(EEEEE)",
"EMR/p",
"NSP",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"check_exception_DxTable",
"mem_write_pushn_DeTable",
],
"raises_exception" => 1,
},
"PUSHN_Do" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "pushn eeeee",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_STK" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_3_3_x_2_2_x_0_0_" => 1226,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"eeeee_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_4_4_x_1_1_" => "",
},
"action" => <<DATA
{
    check_exception_DxTable ( eeeee ) ;
    mem_write_pushn_DoTable ( eeeee ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"DoTable(eeeee)",
"EMR/p",
"NSP",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"check_exception_DxTable",
"mem_write_pushn_DoTable",
],
"raises_exception" => 1,
},
"PUSH_2L_De_Do" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "push.2l QQQQQ",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_STK" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_9_9_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 1230,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"QQQQQ_imp_bits__16_16_x_10_10_x_8_8_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    check_exception_DxTable ( QQQQQ ) ;
    mem_write_push2l_DeDoTable ( QQQQQ ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"DeDoTable(QQQQQ)",
"EMR/p",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"check_exception_DxTable",
"mem_write_push2l_DeDoTable",
],
"raises_exception" => 1,
},
"PUSH_De" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "push EEEEE",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_STK" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_3_3_x_2_2_x_0_0_" => 1160,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"EEEEE_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_4_4_x_1_1_" => "",
},
"action" => <<DATA
{
    check_exception_DxTable ( EEEEE ) ;
    mem_write_push_DeTable ( EEEEE ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"DeTable(EEEEE)",
"EMR/p",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"check_exception_DxTable",
"mem_write_push_DeTable",
],
"raises_exception" => 1,
},
"PUSH_Do" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "push eeeee",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_STK" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_7_7_x_6_6_x_5_5_x_3_3_x_2_2_x_0_0_" => 1224,
"reserved_imp_bits__18_18_x_17_17_" => "reserved",
"eeeee_imp_bits__16_16_x_11_11_x_10_10_x_9_9_x_4_4_x_1_1_" => "",
},
"action" => <<DATA
{
    check_exception_DxTable ( eeeee ) ;
    mem_write_push_DoTable ( eeeee ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"DoTable(eeeee)",
"EMR/p",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"check_exception_DxTable",
"mem_write_push_DoTable",
],
"raises_exception" => 1,
},
"RND_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "rnd JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 217,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 40 > tmp = round ( D ( JJJ ) ) ;
    D ( FFF ) = tmp ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"round",
"saturate",
],
"raises_exception" => 1,
},
"ROL_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "rol FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_0_" => 1250,
"reserved_imp_bits__18_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > D_tmp = ( bits < 41 >  ) D ( FFF ) << 1 ;
    D ( FFF ) = concat ( D_tmp ( 39 , 1 ) , SR . C ) ;
    SR . C = D_tmp ( 40 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"SR[mask 0x00020001]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
},
"ROR_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "ror FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_0_" => 1251,
"reserved_imp_bits__18_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > D_tmp = concat ( SR . C , D ( FFF ) ) ;
    SR . C = D ( FFF ) ( 0 ) ;
    D_tmp = D_tmp >> 1 ;
    D ( FFF ) = D_tmp ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"SR[mask 0x00020001]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
},
"RTE" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "rte",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40819,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        jump_instr = true ;
        NVPC = DMEM ( SP - 8 , 4 ) ;
        SR = DMEM ( SP - 4 , 4 ) ;
        SP = SP - 8 ;
        clear_emr_nmid (  ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"ESP?",
"NSP?",
"SP?",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000018]/p",
"ESP?",
"NSP?",
"NVPC?",
"SP?",
"SR?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"clear_emr_nmid",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"RTED" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "rted",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40818,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        DSPC = DMEM ( SP - 8 , 4 ) ;
        Fetch = Fetch :: DelaySlot ;
        SR = DMEM ( SP - 4 , 4 ) ;
        SP = SP - 8 ;
        clear_emr_nmid (  ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"ESP?",
"NSP?",
"SP?",
"SR/p",
],
"outputs" => [
"DSPC?",
"EMR[mask 0x00000018]/p",
"ESP?",
"Fetch?",
"NSP?",
"SP?",
"SR?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"clear_emr_nmid",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"RTEKRI" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "rtekri",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40831,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        jump_instr = true ;
        NVPC = DMEM ( SP - 8 , 4 ) ;
        SR = DMEM ( SP - 4 , 4 ) ;
        SP = SP - 8 ;
        VALID_RAS ( 0 ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"ESP?",
"NSP?",
"SP?",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
"ESP?",
"NSP?",
"NVPC?",
"SP?",
"SR?",
"VALID_RAS(0)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"RTEKRID" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "rtekrid",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40830,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        DSPC = DMEM ( SP - 8 , 4 ) ;
        Fetch = Fetch :: DelaySlot ;
        SR = DMEM ( SP - 4 , 4 ) ;
        SP = SP - 8 ;
        VALID_RAS ( 0 ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"ESP?",
"NSP?",
"SP?",
"SR/p",
],
"outputs" => [
"DSPC?",
"EMR[mask 0x00000010]/p?",
"ESP?",
"Fetch?",
"NSP?",
"SP?",
"SR?",
"VALID_RAS(0)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"RTERI" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "rteri",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40823,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        jump_instr = true ;
        NVPC = DMEM ( SP - 8 , 4 ) ;
        SR = DMEM ( SP - 4 , 4 ) ;
        SP = SP - 8 ;
        clear_emr_nmid (  ) ;
        VALID_RAS ( 0 ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"ESP?",
"NSP?",
"SP?",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000018]/p",
"ESP?",
"NSP?",
"NVPC?",
"SP?",
"SR?",
"VALID_RAS(0)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"clear_emr_nmid",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"RTERID" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "rterid",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40822,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    } else {
        DSPC = DMEM ( SP - 8 , 4 ) ;
        Fetch = Fetch :: DelaySlot ;
        SR = DMEM ( SP - 4 , 4 ) ;
        SP = SP - 8 ;
        clear_emr_nmid (  ) ;
        VALID_RAS ( 0 ) = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"ESP?",
"NSP?",
"SP?",
"SR/p",
],
"outputs" => [
"DSPC?",
"EMR[mask 0x00000018]/p",
"ESP?",
"Fetch?",
"NSP?",
"SP?",
"SR?",
"VALID_RAS(0)?",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"clear_emr_nmid",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"RTPE" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "rtpe",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40569,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"RTS" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "rts",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40817,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    jump_instr = true ;
    if ( VALID_RAS ( 0 ) == 1 ) {
        NVPC = RAS ( 0 ) ;
    } else {
        NVPC = DMEM ( SP - 8 , 4 ) ;
    }
    SP = SP - 8 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"ESP?",
"NSP?",
"RAS(0)?",
"SP",
"SR/p",
"VALID_RAS(0)",
],
"outputs" => [
"ESP?",
"NSP?",
"NVPC",
"SP",
],
"input_mems" => [
"DMEM",
],
},
"RTSD" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "rtsd",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40816,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( VALID_RAS ( 0 ) == 1 ) {
        DSPC = RAS ( 0 ) ;
    } else {
        DSPC = DMEM ( SP - 8 , 4 ) ;
    }
    SP = SP - 8 ;
    Fetch = Fetch :: DelaySlot ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"ESP?",
"NSP?",
"RAS(0)?",
"SP",
"SR/p",
"VALID_RAS(0)",
],
"outputs" => [
"DSPC",
"ESP?",
"Fetch",
"NSP?",
"SP",
],
"input_mems" => [
"DMEM",
],
},
"RTSTK" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "rtstk",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40821,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    jump_instr = true ;
    NVPC = DMEM ( SP - 8 , 4 ) ;
    SP = SP - 8 ;
    if ( SR . PE == 0 ) clear_emr_nmid (  ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000008]/p?",
"ESP?",
"NSP?",
"NVPC",
"SP",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"clear_emr_nmid",
],
},
"RTSTKD" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "rtstkd",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40820,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    DSPC = DMEM ( SP - 8 , 4 ) ;
    Fetch = Fetch :: DelaySlot ;
    SP = SP - 8 ;
    if ( SR . PE == 0 ) clear_emr_nmid (  ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"DSPC",
"EMR[mask 0x00000008]/p?",
"ESP?",
"Fetch",
"NSP?",
"SP",
],
"input_mems" => [
"DMEM",
],
"helpers" => [
"IsUserMode",
"clear_emr_nmid",
],
},
"SAD4_Dg_Dh_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sad4 GG,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_1_1_" => 1719,
"GG_imp_bits__18_18_x_17_17_x_2_2_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 4 > Dg = GG . Dg ;
     bits < 4 > Dh = GG . Dh ;
     bits < 9 > hh = ( bits < 9 >  ) D ( Dh ) . HH - ( bits < 9 >  ) D ( Dg ) . HH ;
     bits < 9 > hl = ( bits < 9 >  ) D ( Dh ) . HL - ( bits < 9 >  ) D ( Dg ) . HL ;
     bits < 9 > lh = ( bits < 9 >  ) D ( Dh ) . LH - ( bits < 9 >  ) D ( Dg ) . LH ;
     bits < 9 > ll = ( bits < 9 >  ) D ( Dh ) . LL - ( bits < 9 >  ) D ( Dg ) . LL ;
    hh = ABSbyte ( hh ) ;
    hl = ABSbyte ( hl ) ;
    lh = ABSbyte ( lh ) ;
    ll = ABSbyte ( ll ) ;
     bits < 10 > tmp = lh + ll ;
     bits < 10 > tmp2 = hh + hl ;
     bits < 17 > res = ( bits < 17 >  ) D ( FFF ) . L + ( bits < 17 >  ) tmp + ( bits < 17 >  ) tmp2 ;
    D ( FFF ) = zeroExtend ( res ( 15 , 0 ) , 40 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D[mask 0x00ff000000]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"ABSbyte",
],
},
"SAT2_W_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sat2.w JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1909,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) . WL = saturation20_nochk ( signExtend ( ( D ( JJJ ) . WL ) , 40 ) ) ;
    D ( FFF ) . WH = saturation20_nochk ( signExtend ( ( D ( JJJ ) . WH ) , 40 ) ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"saturation20_nochk",
],
},
"SATU20_B_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "satu20.b JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 891,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"SATU2_B_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "satu2.b JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 881,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 16 > pos_val = 0x00ff ;
     bits < 16 > neg_val = 0x0000 ;
     bool sat = false ;
    if ( ( D ( JJJ ) . L ) . signedGT ( pos_val ) ) {
        sat = true ;
        D ( FFF ) . L = pos_val ;
    } else {
        if ( ( D ( JJJ ) . L ) . signedLT ( neg_val ) ) {
            sat = true ;
            D ( FFF ) . L = neg_val ;
        } else {
            D ( FFF ) . L = D ( JJJ ) . L ;
        }
    }
    if ( ( D ( JJJ ) . H ) . signedGT ( pos_val ) ) {
        sat = true ;
        D ( FFF ) . H = pos_val ;
    } else {
        if ( ( D ( JJJ ) . H ) . signedLT ( neg_val ) ) {
            sat = true ;
            D ( FFF ) . H = neg_val ;
        } else {
            D ( FFF ) . H = D ( JJJ ) . H ;
        }
    }
    if ( sat ) {
        EMR_LOW = true ;
        EMR . DOVF = 1 ;
    }
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
],
},
"SAT_F_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sat.f JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 219,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 40 > pos_val = 0x007fffffffull ;
     bits < 40 > neg_val = 0xff80000000ull ;
    if ( D ( JJJ ) . signedGT ( pos_val ) ) {
        D ( FFF ) = 0x007fff0000ull ;
        set_emr_dovf (  ) ;
    } else if ( D ( JJJ ) . signedLT ( neg_val ) ) {
        D ( FFF ) = neg_val ;
        set_emr_dovf (  ) ;
    } else {
        D ( FFF ) = D ( JJJ ) & 0xFFFFFF0000ull ;
    }
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"set_emr_dovf",
],
},
"SAT_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sat.l FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 1253,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 40 > pos_val = 0x007fffffffull ;
     bits < 40 > neg_val = 0xff80000000ull ;
     bool sat = false ;
    if ( ( D ( FFF ) . signedGT ( pos_val ) ) ) {
        sat = true ;
        D ( FFF ) = pos_val ;
    } else {
        if ( ( D ( FFF ) ) . signedLT ( neg_val ) ) {
            sat = true ;
            D ( FFF ) = neg_val ;
        }
    }
    if ( sat ) {
        exception_overflow = true ;
        EMR . DOVF = 1 ;
    }
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
],
},
"SBC_Dc_Dd" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sbc ee",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 2939,
"ee_imp_bits__16_16_x_18_18_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
},
"action" => <<DATA
{
     bits < 40 > dval_pcarry = ( bits < 40 >  ) D ( ee . Dc ) + 0x0000000001ull ;
     bits < 40 > subtract = ( SR . C == 0 ) ? D ( ee . Dc ) : dval_pcarry ;
     bits < 41 > sub = ( sbits < 41 >  ) ( D ( ee . Dd ) ) - ( sbits < 41 >  ) subtract ;
    overflow40 ( sub ) ;
    Borrow ( sub , subtract , D ( ee . Dd ) ) ;
    D ( ee . Dd ) = sub ( 39 , 0 ) ;
    L ( ee . Dd ) = LnComputation_special_instr ( sub ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(ee . Dc)",
"D(ee . Dd)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(ee . Dd)",
"EMR[mask 0x00000004]/p?",
"L(ee . Dd)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"Borrow",
"IsUserMode",
"LnComputation_nochk",
"LnComputation_special_instr",
"overflow40",
],
"raises_exception" => 1,
},
"SBR_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sbr JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 201,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 40 > tmp = round ( D ( FFF ) - D ( JJJ ) ) ;
    D ( FFF ) = tmp ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"round",
"saturate",
],
"raises_exception" => 1,
},
"SDMOVE2_2F__EA__Da_Db" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sdmove2.2f (RRRMMM,hh",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 494,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"SETALIGN_Rn_GCR" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "setalign RRR,GCR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 6469,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    GCR = concat ( GCR ( 31 , 2 ) , R ( RRR ) ( 1 , 0 ) ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"GCR[mask 0xfffffffc]/p",
"R(RRR)[mask 0x00000003]/p",
],
"outputs" => [
"GCR",
],
},
"SKIPLS_label" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "skipls a16",
"block" => "agu",
"attributes" => {
"Type4" => "",
"two_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_24_x_20_16_x_15_13_" => 8476,
"reserved_imp_bits__34_32_" => "reserved",
"a16_imp_bits__0_0_x_23_21_x_12_1_" => "",
},
"action" => <<DATA
{
     uint32_t active_loop = FindActiveLoop (  ) ;
    if ( LC ( active_loop ) . signedLT ( 1 ) ) {
        jump_instr = true ;
        NVPC = PC_ST_VLES + signExtend ( concat ( a16 , zero ( 1 ) ) , 32 ) ;
        clear_SR_LF ( active_loop ) ;
        SR . SLF = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"LC",
"PC_ST_VLES?",
"SR[mask 0x78020000]/p",
],
"outputs" => [
"NVPC?",
"SR[mask 0xf8000000]/p?",
],
"helpers" => [
"FindActiveLoop",
"clear_SR_LF",
],
},
"SOD2AAII_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sod2aaii JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_4_4_x_3_3_" => 196,
"jjj_imp_bits__18_18_x_7_7_x_6_6_x_5_5_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 17 > sumh = ( bits < 17 >  ) D ( JJJ ) . H + ( bits < 17 >  ) D ( jjj ) . H ;
     bits < 17 > suml = ( bits < 17 >  ) D ( JJJ ) . L + ( bits < 17 >  ) D ( jjj ) . L ;
    D ( FFF ) . H = saturation16_noexception ( signExtend ( sumh , 40 ) ) ;
    D ( FFF ) . L = saturation16_noexception ( signExtend ( suml , 40 ) ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = extension_update_add ( sumh , suml ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
"D(jjj)[mask 0x00ffffffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_add",
"saturation16_nochk",
"saturation16_noexception",
],
},
"SOD2AAXX_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sod2aaxx JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_4_4_x_3_3_" => 197,
"jjj_imp_bits__18_18_x_7_7_x_6_6_x_5_5_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 17 > sumh = ( bits < 17 >  ) D ( JJJ ) . L + ( bits < 17 >  ) D ( jjj ) . H ;
     bits < 17 > suml = ( bits < 17 >  ) D ( JJJ ) . H + ( bits < 17 >  ) D ( jjj ) . L ;
    D ( FFF ) . H = saturation16_noexception ( signExtend ( sumh , 40 ) ) ;
    D ( FFF ) . L = saturation16_noexception ( signExtend ( suml , 40 ) ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = extension_update_add ( sumh , suml ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
"D(jjj)[mask 0x00ffffffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_add",
"saturation16_nochk",
"saturation16_noexception",
],
},
"SOD2ASII_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sod2asii JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_4_4_x_3_3_" => 198,
"jjj_imp_bits__18_18_x_7_7_x_6_6_x_5_5_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 17 > sumh = ( bits < 17 >  ) D ( jjj ) . H + ( bits < 17 >  ) D ( JJJ ) . H ;
     bits < 17 > subl = ( bits < 17 >  ) D ( jjj ) . L - ( bits < 17 >  ) D ( JJJ ) . L ;
    D ( FFF ) . H = saturation16_noexception ( signExtend ( sumh , 40 ) ) ;
    D ( FFF ) . L = saturation16_noexception ( signExtend ( subl , 40 ) ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = extension_update_mixed ( sumh , subl ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
"D(jjj)[mask 0x00ffffffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_mixed",
"saturation16_nochk",
"saturation16_noexception",
],
},
"SOD2ASXX_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sod2asxx JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_4_4_x_3_3_" => 199,
"jjj_imp_bits__18_18_x_7_7_x_6_6_x_5_5_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 17 > suml = ( bits < 17 >  ) D ( jjj ) . L + ( bits < 17 >  ) D ( JJJ ) . H ;
     bits < 17 > subh = ( bits < 17 >  ) D ( jjj ) . H - ( bits < 17 >  ) D ( JJJ ) . L ;
    D ( FFF ) . H = saturation16_noexception ( signExtend ( subh , 40 ) ) ;
    D ( FFF ) . L = saturation16_noexception ( signExtend ( suml , 40 ) ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = extension_update_mixed_xx ( suml , subh ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
"D(jjj)[mask 0x00ffffffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_mixed_xx",
"saturation16_nochk",
"saturation16_noexception",
],
},
"SOD2SAII_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sod2saii JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_4_4_x_3_3_" => 204,
"jjj_imp_bits__18_18_x_7_7_x_6_6_x_5_5_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 17 > suml = ( bits < 17 >  ) D ( jjj ) . L + ( bits < 17 >  ) D ( JJJ ) . L ;
     bits < 17 > subh = ( bits < 17 >  ) D ( jjj ) . H - ( bits < 17 >  ) D ( JJJ ) . H ;
    D ( FFF ) . H = saturation16_noexception ( signExtend ( subh , 40 ) ) ;
    D ( FFF ) . L = saturation16_noexception ( signExtend ( suml , 40 ) ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = extension_update_mixed_xx ( suml , subh ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
"D(jjj)[mask 0x00ffffffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_mixed_xx",
"saturation16_nochk",
"saturation16_noexception",
],
},
"SOD2SAXX_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sod2saxx JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_4_4_x_3_3_" => 205,
"jjj_imp_bits__18_18_x_7_7_x_6_6_x_5_5_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 17 > subl = ( sbits < 17 >  ) D ( jjj ) . L - ( sbits < 17 >  ) D ( JJJ ) . H ;
     bits < 17 > sumh = ( sbits < 17 >  ) D ( jjj ) . H + ( sbits < 17 >  ) D ( JJJ ) . L ;
     bits < 17 > subl2 = ( bits < 17 >  ) D ( jjj ) . L - ( bits < 17 >  ) D ( JJJ ) . H ;
     bits < 17 > sumh2 = ( bits < 17 >  ) D ( jjj ) . H + ( bits < 17 >  ) D ( JJJ ) . L ;
    D ( FFF ) . H = saturation16_noexception ( signExtend ( sumh , 40 ) ) ;
    D ( FFF ) . L = saturation16_noexception ( signExtend ( subl , 40 ) ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = extension_update_mixed ( sumh2 , subl2 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
"D(jjj)[mask 0x00ffffffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_mixed",
"saturation16_nochk",
"saturation16_noexception",
],
},
"SOD2SSII_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sod2ssii JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_4_4_x_3_3_" => 206,
"jjj_imp_bits__18_18_x_7_7_x_6_6_x_5_5_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 17 > subl = ( sbits < 17 >  ) D ( jjj ) . L - ( sbits < 17 >  ) D ( JJJ ) . L ;
     bits < 17 > subh = ( sbits < 17 >  ) D ( jjj ) . H - ( sbits < 17 >  ) D ( JJJ ) . H ;
    D ( FFF ) . H = saturation16_noexception ( signExtend ( subh , 40 ) ) ;
    D ( FFF ) . L = saturation16_noexception ( signExtend ( subl , 40 ) ) ;
    L ( FFF ) = 0 ;
    subl = ( bits < 17 >  ) D ( jjj ) . L - ( bits < 17 >  ) D ( JJJ ) . L ;
    subh = ( bits < 17 >  ) D ( jjj ) . H - ( bits < 17 >  ) D ( JJJ ) . H ;
    D ( FFF ) . E = extension_update_sub ( subh , subl ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
"D(jjj)[mask 0x00ffffffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_sub",
"saturation16_nochk",
"saturation16_noexception",
],
},
"SOD2SSXX_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sod2ssxx JJJ,jjj,FFF",
"block" => "dalu",
"attributes" => {
"Type14" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_8_8_x_4_4_x_3_3_" => 207,
"jjj_imp_bits__18_18_x_7_7_x_6_6_x_5_5_" => "",
"JJJ_imp_bits__17_17_x_2_2_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_11_11_x_10_10_x_9_9_" => "",
},
"action" => <<DATA
{
     bits < 17 > subl = ( sbits < 17 >  ) D ( jjj ) . L - ( sbits < 17 >  ) D ( JJJ ) . H ;
     bits < 17 > subh = ( sbits < 17 >  ) D ( jjj ) . H - ( sbits < 17 >  ) D ( JJJ ) . L ;
    D ( FFF ) . H = saturation16_noexception ( signExtend ( subh , 40 ) ) ;
    D ( FFF ) . L = saturation16_noexception ( signExtend ( subl , 40 ) ) ;
    L ( FFF ) = 0 ;
    subl = ( bits < 17 >  ) D ( jjj ) . L - ( bits < 17 >  ) D ( JJJ ) . H ;
    subh = ( bits < 17 >  ) D ( jjj ) . H - ( bits < 17 >  ) D ( JJJ ) . L ;
    D ( FFF ) . E = extension_update_sub ( subh , subl ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
"D(jjj)[mask 0x00ffffffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_sub",
"saturation16_nochk",
"saturation16_noexception",
],
},
"STOP" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "stop",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_APC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40825,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    halt (  ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"SUB2_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sub2 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 841,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 17 > subh = ( bits < 17 >  ) D ( FFF ) . H - ( bits < 17 >  ) D ( JJJ ) . H ;
     bits < 17 > subl = ( bits < 17 >  ) D ( FFF ) . L - ( bits < 17 >  ) D ( JJJ ) . L ;
    D ( FFF ) . H = subh ( 15 , 0 ) ;
    D ( FFF ) . L = subl ( 15 , 0 ) ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = extension_update_sub ( subh , subl ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
"D(JJJ)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"extension_update_sub",
],
},
"SUBA__u5_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "suba #u5,RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_" => 115,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 32 > imm = zeroExtend ( u5 , 32 ) ;
    if ( RRRR ( 4 ) == 0 && RRRR ( 3 ) == 1 ) {
        RxTable ( RRRR ) = Address_by_MCTL ( RRRR , imm , 1 , 1 , 1 , imm ) ;
    } else {
        RxTable ( RRRR ) = RxTable ( RRRR ) - imm ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"RxTable(RRRR)",
"rxTable(RRRR)?",
],
"outputs" => [
"RxTable(RRRR)",
],
"helpers" => [
"Address_by_MCTL",
"bitreverse",
"preshift",
],
},
"SUBA_rx_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "suba rrrr,RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_" => 227,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"rrrr_imp_bits__16_16_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    if ( RRRR ( 4 ) == 0 && RRRR ( 3 ) == 1 ) {
        RxTable ( RRRR ) = Address_by_MCTL ( RRRR , rxTable ( rrrr ) , 1 , 1 , 1 , rxTable ( rrrr ) ) ;
    } else {
        RxTable ( RRRR ) = RxTable ( RRRR ) - rxTable ( rrrr ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"M?",
"MCTL/p?",
"RxTable(RRRR)",
"rxTable(RRRR)?",
"rxTable(rrrr)",
],
"outputs" => [
"RxTable(RRRR)",
],
"helpers" => [
"Address_by_MCTL",
"bitreverse",
"preshift",
],
},
"SUBL_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "subl JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 203,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 41 > src = ( bits < 41 >  ) D ( FFF ) << 1 ;
     bits < 41 > sum = ( sbits < 41 >  ) ( src  ) - ( sbits < 41 >  ) D ( JJJ ) ;
    overflow40 ( sum ) ;
    Borrow ( sum , D ( JJJ ) , src ) ;
    saturation32 ( sum , FFF ) ;
    L ( FFF ) = LnComputation ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"Borrow",
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"SUBNC_W__s16_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "subnc.w #s16,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 1940,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     sbits < 41 > imm = signExtend ( s16 , 41 ) ;
     bits < 41 > sum = ( sbits < 41 >  ) D ( FFF ) - imm ;
    overflow40 ( sum ) ;
    saturation32 ( sum , FFF ) ;
    L ( FFF ) = LnComputation ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"inputs" => [
"D(FFF)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"SUB_W_Da_H_Dn_H" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sub.w JJJ.h,FFF.h",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1899,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 17 > sub = ( sbits < 17 >  ) D ( FFF ) . H - ( sbits < 17 >  ) D ( JJJ ) . H ;
    D ( FFF ) . H = saturation16_noexception ( signExtend ( sub , 40 ) ) ;
    L ( FFF ) = 0 ;
    sub = ( bits < 17 >  ) D ( FFF ) . H - ( bits < 17 >  ) D ( JJJ ) . H ;
    D ( FFF ) . E = extension_update_high_sub ( sub ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffff0000]/p",
"D(JJJ)[mask 0x00ffff0000]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)[mask 0xffffff0000]",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_high_sub",
"saturation16_nochk",
"saturation16_noexception",
],
},
"SUB_W_Da_H_Dn_L" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sub.w JJJ.h,FFF.l",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1891,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 17 > sub = ( sbits < 17 >  ) D ( FFF ) . L - ( sbits < 17 >  ) D ( JJJ ) . H ;
    D ( FFF ) . L = saturation16_noexception ( signExtend ( sub , 40 ) ) ;
    L ( FFF ) = 0 ;
    sub = ( bits < 17 >  ) D ( FFF ) . L - ( bits < 17 >  ) D ( JJJ ) . H ;
    D ( FFF ) . E = extension_update_low_sub ( sub ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x000000ffff]/p",
"D(JJJ)[mask 0x00ffff0000]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)[mask 0xff0000ffff]",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_low_sub",
"saturation16_nochk",
"saturation16_noexception",
],
},
"SUB_W_Da_L_Dn_H" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sub.w JJJ.l,FFF.h",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1898,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 17 > sub = ( sbits < 17 >  ) D ( FFF ) . H - ( sbits < 17 >  ) D ( JJJ ) . L ;
    D ( FFF ) . H = saturation16_noexception ( signExtend ( sub , 40 ) ) ;
    L ( FFF ) = 0 ;
    sub = ( bits < 17 >  ) D ( FFF ) . H - ( bits < 17 >  ) D ( JJJ ) . L ;
    D ( FFF ) . E = extension_update_high_sub ( sub ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffff0000]/p",
"D(JJJ)[mask 0x000000ffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)[mask 0xffffff0000]",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_high_sub",
"saturation16_nochk",
"saturation16_noexception",
],
},
"SUB_W_Da_L_Dn_L" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sub.w JJJ.l,FFF.l",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1890,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 17 > sub = ( sbits < 17 >  ) D ( FFF ) . L - ( sbits < 17 >  ) D ( JJJ ) . L ;
    D ( FFF ) . L = saturation16_noexception ( signExtend ( sub , 40 ) ) ;
    L ( FFF ) = 0 ;
    sub = ( bits < 17 >  ) D ( FFF ) . L - ( bits < 17 >  ) D ( JJJ ) . L ;
    D ( FFF ) . E = extension_update_low_sub ( sub ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x000000ffff]/p",
"D(JJJ)[mask 0x000000ffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"D(FFF)[mask 0xff0000ffff]",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"extension_update_low_sub",
"saturation16_nochk",
"saturation16_noexception",
],
},
"SUB__u5_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sub #u5,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 59,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
     bits < 40 > imm = zeroExtend ( u5 , 40 ) ;
     bits < 41 > sum = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) imm ;
    overflow40 ( sum ) ;
    Borrow ( sum , imm , D ( FFF ) ) ;
    saturation32 ( sum , FFF ) ;
    L ( FFF ) = LnComputation ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"Borrow",
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"SUMOVE2_2F__EA__Da_Db" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sumove2.2f (RRRMMM,hh",
"block" => "agu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_8_8_x_7_7_x_6_6_" => 490,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"hh_imp_bits__16_16_x_10_10_x_9_9_" => "",
"MMM_imp_bits__5_5_x_4_4_x_3_3_" => "",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"SWAPB2_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "swapb2 JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_3_" => 1911,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) . HL = D ( JJJ ) . HH ;
    D ( FFF ) . HH = D ( JJJ ) . HL ;
    D ( FFF ) . LL = D ( JJJ ) . LH ;
    D ( FFF ) . LH = D ( JJJ ) . LL ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"SWAPB_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "swapb JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_3_" => 1910,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) . LL = D ( JJJ ) . HH ;
    D ( FFF ) . LH = D ( JJJ ) . HL ;
    D ( FFF ) . HL = D ( JJJ ) . LH ;
    D ( FFF ) . HH = D ( JJJ ) . LL ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"SWAP_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "swap JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_10_x_6_3_" => 1913,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) . H = D ( JJJ ) . L ;
    D ( FFF ) . L = D ( JJJ ) . H ;
    D ( FFF ) . E = 0xff ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"SXTA_B_rx_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sxta.b rrrr,RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_" => 236,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"rrrr_imp_bits__16_16_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    RxTable ( RRRR ) = signExtend ( rxTable ( rrrr ) ( 7 , 0 ) , 32 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"rxTable(rrrr)[mask 0x000000ff]/p",
],
"outputs" => [
"RxTable(RRRR)",
],
},
"SXTA_W_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sxta.w RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 3833,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    RxTable ( RRRR ) = signExtend ( RxTable ( RRRR ) ( 15 , 0 ) , 32 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)[mask 0x0000ffff]/p",
],
"outputs" => [
"RxTable(RRRR)",
],
},
"SXT_B_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sxt.b JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_3_" => 220,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = signExtend ( D ( JJJ ) . LL , 40 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00000000ff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"SXT_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sxt.l FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_0_" => 1249,
"reserved_imp_bits__18_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = signExtend ( concat ( D ( FFF ) . H , D ( FFF ) . L ) , 40 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"SXT_W_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sxt.w JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_3_" => 222,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = signExtend ( D ( JJJ ) . L , 40 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x000000ffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"SYNCIO" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "syncio",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_APC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40826,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"SYNCM" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "syncm",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_APC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40827,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( IsUserMode (  ) ) {
        set_emr_ilpr (  ) ;
        raiseException ( Privelege ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_ilpr",
],
"raises_exception" => 1,
},
"TFRA_OSP_Rn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tfra osp,RRR",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_3_" => 4942,
"RRR_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_16_" => "reserved",
},
"action" => <<DATA
{
    if ( SR . PE == 1 ) {
        EMR_LOW = true ;
        EMR . ILPR = 1 ;
        raiseException ( Privelege ) ;
    } else {
        if ( SR . EXP == 1 ) {
            R ( RRR ) = NSP ;
        } else {
            R ( RRR ) = ESP ;
        }
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"ESP?",
"NSP?",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
"R(RRR)?",
],
"helpers" => [
"IsUserMode",
],
"raises_exception" => 1,
},
"TFRA_Rn_OSP" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tfra RRR,osp",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_3_" => 4943,
"RRR_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_16_" => "reserved",
},
"action" => <<DATA
{
    if ( SR . PE == 1 ) {
        EMR_LOW = true ;
        EMR . ILPR = 1 ;
        raiseException ( Privelege ) ;
    } else {
        if ( SR . EXP == 1 ) {
            NSP = R ( RRR ) & 0xFFFFFFF8 ;
        } else {
            ESP = R ( RRR ) & 0xFFFFFFF8 ;
        }
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"R(RRR)?",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
"ESP?",
"NSP?",
],
"helpers" => [
"IsUserMode",
],
"raises_exception" => 1,
},
"TFRA_rx_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tfra rrrr,RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_4_" => 238,
"RRRR_imp_bits__18_18_x_11_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"rrrr_imp_bits__16_16_x_3_0_" => "",
},
"action" => <<DATA
{
    RxTable ( RRRR ) = rxTable ( rrrr ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"rxTable(rrrr)",
],
"outputs" => [
"RxTable(RRRR)",
],
},
"TFRF_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tfrf JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 843,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 0 ) {
        D ( FFF ) = D ( JJJ ) ;
        L ( FFF ) = LnComputation_special_instr ( D ( JJJ ) ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)?",
"SR[mask 0x00000036]/p",
],
"outputs" => [
"D(FFF)?",
"L(FFF)?",
],
"helpers" => [
"LnComputation_nochk",
"LnComputation_special_instr",
],
},
"TFRT_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tfrt JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 842,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    if ( SR . T == 1 ) {
        D ( FFF ) = D ( JJJ ) ;
        L ( FFF ) = LnComputation_special_instr ( D ( JJJ ) ) ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)?",
"SR[mask 0x00000036]/p",
],
"outputs" => [
"D(FFF)?",
"L(FFF)?",
],
"helpers" => [
"LnComputation_nochk",
"LnComputation_special_instr",
],
},
"TFR_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tfr JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 218,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = D ( JJJ ) ;
    L ( FFF ) = LnComputation_special_instr ( D ( JJJ ) ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)",
"SR[mask 0x00000034]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"LnComputation_nochk",
"LnComputation_special_instr",
],
},
"TFR_W_Da_H_Dn_H" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tfr.w JJJ.h,FFF.h",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 687,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) . H = D ( JJJ ) . H ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0xff ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffff0000]/p",
],
"outputs" => [
"D(FFF)[mask 0xffffff0000]/p",
"L(FFF)",
],
},
"TFR_W_Da_H_Dn_L" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tfr.w JJJ.h,FFF.l",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 679,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) . L = D ( JJJ ) . H ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0xff ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00ffff0000]/p",
],
"outputs" => [
"D(FFF)[mask 0xff0000ffff]/p",
"L(FFF)",
],
},
"TFR_W_Da_L_Dn_H" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tfr.w JJJ.l,FFF.h",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 686,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) . H = D ( JJJ ) . L ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0xff ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x000000ffff]/p",
],
"outputs" => [
"D(FFF)[mask 0xffffff0000]/p",
"L(FFF)",
],
},
"TFR_W_Da_L_Dn_L" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tfr.w JJJ.l,FFF.l",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 678,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) . L = D ( JJJ ) . L ;
    L ( FFF ) = 0 ;
    D ( FFF ) . E = 0xff ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x000000ffff]/p",
],
"outputs" => [
"D(FFF)[mask 0xff0000ffff]/p",
"L(FFF)",
],
},
"TFR__s16_Dn" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "tfr #s16,FFF",
"block" => "dalu",
"attributes" => {
"Type4" => "",
"two_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__35_35_x_31_31_x_30_30_x_29_29_x_28_28_x_27_27_x_26_26_x_25_25_x_24_24_x_20_20_x_19_19_x_15_15_x_14_14_x_13_13_" => 1812,
"reserved_imp_bits__34_34_x_33_33_" => "reserved",
"FFF_imp_bits__32_32_x_18_18_x_17_17_x_16_16_" => "",
"s16_imp_bits__23_23_x_22_22_x_21_21_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = signExtend ( s16 , 40 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "TwoWords",
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"TRAP" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "trap",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40574,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    trap_instr = true ;
    jump_instr = true ;
    NVPC = concat ( VBA ( 31 , 12 ) , 0x000 ) ;
    trap_f (  ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"SR/p",
"VBA[mask 0xfffff000]/p",
],
"outputs" => [
"EMR/p?",
"NVPC",
"SR[mask 0xfbe60043]/p",
],
"helpers" => [
"IsUserMode",
"trap_f",
],
"raises_exception" => 1,
},
"TRAP1" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "trap1",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40575,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    trap_instr = true ;
    jump_instr = true ;
    NVPC = concat ( VBA ( 31 , 12 ) , 0x010 ) ;
    trap_f (  ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"SR/p",
"VBA[mask 0xfffff000]/p",
],
"outputs" => [
"EMR/p?",
"NVPC",
"SR[mask 0xfbe60043]/p",
],
"helpers" => [
"IsUserMode",
"trap_f",
],
"raises_exception" => 1,
},
"TRAP2" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "trap2",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40570,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    trap_instr = true ;
    jump_instr = true ;
    NVPC = concat ( VBA ( 31 , 12 ) , 0x020 ) ;
    trap_f (  ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"SR/p",
"VBA[mask 0xfffff000]/p",
],
"outputs" => [
"EMR/p?",
"NVPC",
"SR[mask 0xfbe60043]/p",
],
"helpers" => [
"IsUserMode",
"trap_f",
],
"raises_exception" => 1,
},
"TRAP3" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "trap3",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_COF" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40571,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    trap_instr = true ;
    jump_instr = true ;
    NVPC = concat ( VBA ( 31 , 12 ) , 0x030 ) ;
    trap_f (  ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"EMR/p",
"SR/p",
"VBA[mask 0xfffff000]/p",
],
"outputs" => [
"EMR/p?",
"NVPC",
"SR[mask 0xfbe60043]/p",
],
"helpers" => [
"IsUserMode",
"trap_f",
],
"raises_exception" => 1,
},
"TSTEQA_L_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tsteqa.l RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 3825,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( RxTable ( RRRR ) == 0 ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"TSTEQA_W_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tsteqa.w RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 3824,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    if ( RxTable ( RRRR ) ( 15 , 0 ) == 0 ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)[mask 0x0000ffff]/p",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"TSTEQ_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tsteq FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 1257,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    if ( D ( FFF ) == 0 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"TSTEQ_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tsteq.l FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 1259,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    if ( D ( FFF ) ( 31 , 0 ) == 0 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x00ffffffff]/p",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"TSTGEA_L_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tstgea.l RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_0_" => 3827,
"RRRR_imp_bits__18_18_x_11_8_" => "",
"reserved_imp_bits__17_16_" => "reserved",
},
"action" => <<DATA
{
    if ( RxTable ( RRRR ) . signedGE ( 0 ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"TSTGE_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tstge FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 1256,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    if ( ( D ( FFF ) ( 40 - 1 , 40 - 1 ) ) == 0 ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)[mask 0x8000000000]/p",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"TSTGTA_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tstgta RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_12_x_7_0_" => 3826,
"RRRR_imp_bits__18_18_x_11_8_" => "",
"reserved_imp_bits__17_16_" => "reserved",
},
"action" => <<DATA
{
    if ( RxTable ( RRRR ) . signedGT ( 0 ) ) {
        SR . T = 1 ;
    } else {
        SR . T = 0 ;
    }
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"TSTGT_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "tstgt FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 1258,
"reserved_imp_bits__18_18_x_17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
    if ( D ( FFF ) . signedGT ( 0 ) ) SR . T = 1 ; else SR . T = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"SR/p",
],
"outputs" => [
"SR[mask 0x00000002]/p",
],
},
"TW_HR_Prefix" => {
"width" => 32,
"syntax" => "TW_HR_Prefix VlesSize_imp_bits__27_25_, HighAgu_imp_bits__8_8_x_22_22_x_23_23_x_21_21_, lpmarkB_imp_bits__20_20_, lpmarkA_imp_bits__19_19_, ccc_imp_bits__18_16_, HighDalu2_imp_bits__12_12_x_10_10_x_7_7_x_11_11_x_9_9_x_6_6_, yyyyyy_imp_bits__5_0_",
"attributes" => {
"prefix" => "",
"tw_prefix" => "",
},
"fields" => {
"VarPrefixOpcode_imp_bits__31_28_x_24_24_x_15_13_" => 61,
"VlesSize_imp_bits__27_25_" => "",
"HighAgu_imp_bits__8_8_x_22_22_x_23_23_x_21_21_" => "",
"lpmarkB_imp_bits__20_20_" => "",
"lpmarkA_imp_bits__19_19_" => "",
"ccc_imp_bits__18_16_" => "",
"HighDalu2_imp_bits__12_12_x_10_10_x_7_7_x_11_11_x_9_9_x_6_6_" => "",
"yyyyyy_imp_bits__5_0_" => "",
},
"action" => <<DATA
{
    prefix_grouping = true ;
    init_cond_exec ( ccc ) ;
    setup_dispatch ( PC . uint32 (  ) ) ;
}
DATA
,
"prefix" => 1,
"disassemble" => 1,
"inputs" => [
"PC",
],
"input_mems" => [
"PMEM",
],
"helpers" => [
"calc_instr_size",
"end_of_set",
"first_available_dalu",
"get_dalu_dispatch",
"get_esg_relevant_instruction_bit",
"get_prefix_size",
"init_cond_exec",
"is_dalu",
"is_esg",
"reset_dalu",
"setup_dispatch",
"type1_end_of_set",
],
},
"TW_Prefix" => {
"width" => 32,
"syntax" => "TW_Prefix VlesSize_imp_bits__27_25_, HighAgu_imp_bits__8_8_x_22_22_x_23_23_x_21_21_, lpmarkB_imp_bits__20_20_, lpmarkA_imp_bits__19_19_, ccc_imp_bits__18_16_, HighDalu_imp_bits__12_12_x_7_7_x_3_3_x_11_11_x_6_6_x_2_2_x_10_10_x_5_5_x_1_1_x_9_9_x_4_4_x_0_0_",
"attributes" => {
"prefix" => "",
"tw_prefix" => "",
},
"fields" => {
"VarPrefixOpcode_imp_bits__31_28_x_24_24_x_15_13_" => 53,
"VlesSize_imp_bits__27_25_" => "",
"HighAgu_imp_bits__8_8_x_22_22_x_23_23_x_21_21_" => "",
"lpmarkB_imp_bits__20_20_" => "",
"lpmarkA_imp_bits__19_19_" => "",
"ccc_imp_bits__18_16_" => "",
"HighDalu_imp_bits__12_12_x_7_7_x_3_3_x_11_11_x_6_6_x_2_2_x_10_10_x_5_5_x_1_1_x_9_9_x_4_4_x_0_0_" => "",
},
"action" => <<DATA
{
    prefix_grouping = true ;
    init_cond_exec ( ccc ) ;
    setup_dispatch ( PC . uint32 (  ) ) ;
}
DATA
,
"prefix" => 1,
"disassemble" => 1,
"inputs" => [
"PC",
],
"input_mems" => [
"PMEM",
],
"helpers" => [
"calc_instr_size",
"end_of_set",
"first_available_dalu",
"get_dalu_dispatch",
"get_esg_relevant_instruction_bit",
"get_prefix_size",
"init_cond_exec",
"is_dalu",
"is_esg",
"reset_dalu",
"setup_dispatch",
"type1_end_of_set",
],
},
"ThreeWords" => {
"width" => 52,
"fetch_width" => 48,
"syntax" => "ThreeWords ESG_imp_bits__51_51_, HighRegister_imp_bits__50_48_, Enc3_imp_bits__47_0_",
"fields" => {
"ESG_imp_bits__51_51_" => "",
"HighRegister_imp_bits__50_48_" => "",
"Enc3_imp_bits__47_0_" => "",
},
"pseudo" => 1,
"disassemble" => 1,
},
"TwoWords" => {
"width" => 36,
"fetch_width" => 32,
"syntax" => "TwoWords ESG_imp_bits__35_35_, HighRegister_imp_bits__34_32_, Enc2_imp_bits__31_0_",
"fields" => {
"ESG_imp_bits__35_35_" => "",
"HighRegister_imp_bits__34_32_" => "",
"Enc2_imp_bits__31_0_" => "",
},
"pseudo" => 1,
"disassemble" => 1,
},
"UNPACK_2W_Df_Dj_Dk" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "unpack.2w uuu,kkk",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_" => 824,
"kkk_imp_bits__18_18_x_5_5_x_4_4_x_3_3_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"uuu_imp_bits__16_16_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    D ( kkk . Dj ) = signExtend ( D ( uuu ) . L , 40 ) ;
    D ( kkk . Dk ) = signExtend ( D ( uuu ) . H , 40 ) ;
    L ( kkk . Dk ) = 0 ;
    L ( kkk . Dj ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(uuu)[mask 0x00ffffffff]/p",
],
"outputs" => [
"D(kkk . Dj)",
"D(kkk . Dk)",
"L(kkk . Dj)",
"L(kkk . Dk)",
],
},
"VSL_2F_D1_D3__Rn_pN0" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "VSL_2F_D1_D3__Rn_pN0 RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_, reserved_imp_bits__17_17_, highbitvsl_imp_bits__16_16_",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 6470,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"highbitvsl_imp_bits__16_16_" => "",
},
"action" => <<DATA
{
     bits < 4 > dentry1 = ( highbitvsl == 1 ) ? 9 : 1 ;
     bits < 4 > dentry3 = ( highbitvsl == 1 ) ? 11 : 3 ;
     bits < 16 > dreg1 = D ( dentry1 ) . H ;
     bits < 16 > dreg3 = D ( dentry3 ) . H ;
     bits < 16 > word0 ;
     bits < 16 > word1 ;
    if ( SR . VF1 == 1 ) {
        word0 = dreg3 << 1 ;
    } else {
        word0 = dreg1 << 1 ;
    }
    if ( SR . VF3 == 1 ) {
        word1 = ( dreg3 << 1 ) + 1 ;
    } else {
        word1 = ( dreg1 << 1 ) + 1 ;
    }
     bits < 32 > mem_data ;
    if ( EMR . BEM == 0 ) {
        mem_data = concat ( word0 , word1 ) ;
    } else {
        mem_data = concat ( word1 , word0 ) ;
    }
    DMEM ( EAtable ( 4 , RRR , 2 ) , 4 ) = mem_data ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D[mask 0x00ffff0000]/p",
"EMR[mask 0x00010000]/p",
"M?",
"MCTL/p",
"N(0)",
"R(RRR)?",
"SR/p",
],
"outputs" => [
"R(RRR)",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"IsUserMode",
"bitreverse",
"increase_size",
"preshift",
],
},
"VSL_2W_D1_D3__Rn_pN0" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "VSL_2W_D1_D3__Rn_pN0 RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_, reserved_imp_bits__17_17_, highbitvsl_imp_bits__16_16_",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 6468,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"highbitvsl_imp_bits__16_16_" => "",
},
"action" => <<DATA
{
     bits < 4 > dentry1 = ( highbitvsl == 1 ) ? 9 : 1 ;
     bits < 4 > dentry3 = ( highbitvsl == 1 ) ? 11 : 3 ;
     bits < 16 > dreg1 = D ( dentry1 ) . L ;
     bits < 16 > dreg3 = D ( dentry3 ) . L ;
     bits < 16 > word0 ;
     bits < 16 > word1 ;
    if ( SR . VF0 == 1 ) {
        word0 = dreg3 << 1 ;
    } else {
        word0 = dreg1 << 1 ;
    }
    if ( SR . VF2 == 1 ) {
        word1 = ( dreg3 << 1 ) + 1 ;
    } else {
        word1 = ( dreg1 << 1 ) + 1 ;
    }
     bits < 32 > mem_data ;
    if ( EMR . BEM == 0 ) {
        mem_data = concat ( word0 , word1 ) ;
    } else {
        mem_data = concat ( word1 , word0 ) ;
    }
    DMEM ( EAtable ( 4 , RRR , 2 ) , 4 ) = mem_data ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D[mask 0x000000ffff]/p",
"EMR[mask 0x00010000]/p",
"M?",
"MCTL/p",
"N(0)",
"R(RRR)?",
"SR/p",
],
"outputs" => [
"R(RRR)",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"IsUserMode",
"bitreverse",
"increase_size",
"preshift",
],
},
"VSL_4F_D2_D6_D1_D3__Rn_pN0" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "VSL_4F_D2_D6_D1_D3__Rn_pN0 RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_, reserved_imp_bits__17_17_, highbitvsl_imp_bits__16_16_",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 6466,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"highbitvsl_imp_bits__16_16_" => "",
},
"action" => <<DATA
{
     bits < 4 > dentry2 = ( highbitvsl == 1 ) ? 10 : 2 ;
     bits < 4 > dentry6 = ( highbitvsl == 1 ) ? 14 : 6 ;
     bits < 4 > dentry1 = ( highbitvsl == 1 ) ? 9 : 1 ;
     bits < 4 > dentry3 = ( highbitvsl == 1 ) ? 11 : 3 ;
     bits < 16 > dreg1 = D ( dentry1 ) . H ;
     bits < 16 > dreg3 = D ( dentry3 ) . H ;
     bits < 16 > word0 = D ( dentry2 ) . H ;
     bits < 16 > word1 = D ( dentry6 ) . H ;
     bits < 16 > word2 ;
     bits < 16 > word3 ;
    if ( SR . VF1 == 1 ) {
        word2 = dreg3 << 1 ;
    } else {
        word2 = dreg1 << 1 ;
    }
    if ( SR . VF3 == 1 ) {
        word3 = ( dreg3 << 1 ) + 1 ;
    } else {
        word3 = ( dreg1 << 1 ) + 1 ;
    }
     bits < 64 > mem_data ;
    mem_data = concat ( word1 , word0 , word3 , word2 ) ;
    DMEM ( EAtable ( 4 , RRR , 3 ) , 8 ) = mem_data ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D[mask 0x00ffff0000]/p",
"M?",
"MCTL/p",
"N(0)",
"R(RRR)?",
"SR[mask 0x00000a00]/p",
],
"outputs" => [
"R(RRR)",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"VSL_4W_D2_D6_D1_D3__Rn_pN0" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "VSL_4W_D2_D6_D1_D3__Rn_pN0 RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_, reserved_imp_bits__17_17_, highbitvsl_imp_bits__16_16_",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_MOVE" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_" => 6464,
"RRR_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"highbitvsl_imp_bits__16_16_" => "",
},
"action" => <<DATA
{
     bits < 4 > dentry2 = ( highbitvsl == 1 ) ? 10 : 2 ;
     bits < 4 > dentry6 = ( highbitvsl == 1 ) ? 14 : 6 ;
     bits < 4 > dentry1 = ( highbitvsl == 1 ) ? 9 : 1 ;
     bits < 4 > dentry3 = ( highbitvsl == 1 ) ? 11 : 3 ;
     bits < 16 > dreg1 = D ( dentry1 ) . L ;
     bits < 16 > dreg3 = D ( dentry3 ) . L ;
     bits < 16 > word0 = D ( dentry2 ) . L ;
     bits < 16 > word1 = D ( dentry6 ) . L ;
     bits < 16 > word2 ;
     bits < 16 > word3 ;
    if ( SR . VF0 == 1 ) {
        word2 = dreg3 << 1 ;
    } else {
        word2 = dreg1 << 1 ;
    }
    if ( SR . VF2 == 1 ) {
        word3 = ( dreg3 << 1 ) + 1 ;
    } else {
        word3 = ( dreg1 << 1 ) + 1 ;
    }
     bits < 64 > mem_data ;
    mem_data = concat ( word1 , word0 , word3 , word2 ) ;
    DMEM ( EAtable ( 4 , RRR , 3 ) , 8 ) = mem_data ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"B?",
"D[mask 0x000000ffff]/p",
"M?",
"MCTL/p",
"N(0)",
"R(RRR)?",
"SR[mask 0x00000500]/p",
],
"outputs" => [
"R(RRR)",
],
"output_mems" => [
"DMEM",
],
"helpers" => [
"EAtable",
"bitreverse",
"increase_size",
"preshift",
],
},
"VTRACE__0__2_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "vtrace #0,#2,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1892,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 31 > tmp = ( D ( FFF ) >> 1 ) ( 30 , 0 ) ;
     bits < 5 > index = vtrace_index ( D ( FFF ) ( 31 , 0 ) , 0 , 2 ) ;
    D ( FFF ) = concat ( D ( FFF ) . E , D ( JJJ ) ( index , index ) , tmp ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"vtrace_index",
],
},
"VTRACE__0__3_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "vtrace #0,#3,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1893,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 31 > tmp = ( D ( FFF ) >> 1 ) ( 30 , 0 ) ;
     bits < 5 > index = vtrace_index ( D ( FFF ) ( 31 , 0 ) , 0 , 3 ) ;
    D ( FFF ) = concat ( D ( FFF ) . E , D ( JJJ ) ( index , index ) , tmp ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"vtrace_index",
],
},
"VTRACE__0__4_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "vtrace #0,#4,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1894,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 31 > tmp = ( D ( FFF ) >> 1 ) ( 30 , 0 ) ;
     bits < 5 > index = vtrace_index ( D ( FFF ) ( 31 , 0 ) , 0 , 4 ) ;
    D ( FFF ) = concat ( D ( FFF ) . E , D ( JJJ ) ( index , index ) , tmp ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"vtrace_index",
],
},
"VTRACE__1__4_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "vtrace #1,#4,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1895,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 31 > tmp = ( D ( FFF ) >> 1 ) ( 30 , 0 ) ;
     bits < 5 > index = vtrace_index ( D ( FFF ) ( 31 , 0 ) , 1 , 4 ) ;
    D ( FFF ) = concat ( D ( FFF ) . E , D ( JJJ ) ( index , index ) , tmp ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"vtrace_index",
],
},
"VTRACE__2__4_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "vtrace #2,#4,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1900,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 31 > tmp = ( D ( FFF ) >> 1 ) ( 30 , 0 ) ;
     bits < 5 > index = vtrace_index ( D ( FFF ) ( 31 , 0 ) , 2 , 4 ) ;
    D ( FFF ) = concat ( D ( FFF ) . E , D ( JJJ ) ( index , index ) , tmp ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"vtrace_index",
],
},
"VTRACE__3__4_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "vtrace #3,#4,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1901,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 31 > tmp = ( D ( FFF ) >> 1 ) ( 30 , 0 ) ;
     bits < 5 > index = vtrace_index ( D ( FFF ) ( 31 , 0 ) , 3 , 4 ) ;
    D ( FFF ) = concat ( D ( FFF ) . E , D ( JJJ ) ( index , index ) , tmp ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"vtrace_index",
],
},
"VTRACE__4__4_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "vtrace #4,#4,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1902,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 31 > tmp = ( D ( FFF ) >> 1 ) ( 30 , 0 ) ;
     bits < 5 > index = vtrace_index ( D ( FFF ) ( 31 , 0 ) , 4 , 4 ) ;
    D ( FFF ) = concat ( D ( FFF ) . E , D ( JJJ ) ( index , index ) , tmp ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"vtrace_index",
],
},
"VTRACE__5__4_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "vtrace #5,#4,JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type12" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_" => 1903,
"JJJ_imp_bits__18_18_x_2_2_x_1_1_x_0_0_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
},
"action" => <<DATA
{
     bits < 31 > tmp = ( D ( FFF ) >> 1 ) ( 30 , 0 ) ;
     bits < 5 > index = vtrace_index ( D ( FFF ) ( 31 , 0 ) , 5 , 4 ) ;
    D ( FFF ) = concat ( D ( FFF ) . E , D ( JJJ ) ( index , index ) , tmp ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJ)/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
"helpers" => [
"vtrace_index",
],
},
"WAIT" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "wait",
"block" => "agu",
"attributes" => {
"Type4" => "",
"one_word" => "",
"instr_tbl_APC" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_11_11_x_10_10_x_9_9_x_8_8_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 40824,
"reserved_imp_bits__18_18_x_17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"ZXTA_B_rx_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "zxta.b rrrr,RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_" => 237,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_" => "reserved",
"rrrr_imp_bits__16_16_x_3_3_x_2_2_x_1_1_x_0_0_" => "",
},
"action" => <<DATA
{
    RxTable ( RRRR ) = zeroExtend ( rxTable ( rrrr ) ( 7 , 0 ) , 32 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"rxTable(rrrr)[mask 0x000000ff]/p",
],
"outputs" => [
"RxTable(RRRR)",
],
},
"ZXTA_W_Rx" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "zxta.w RRRR",
"block" => "agu",
"attributes" => {
"Type2" => "",
"one_word" => "",
"instr_tbl_AAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_14_14_x_13_13_x_12_12_x_7_7_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_x_1_1_x_0_0_" => 3832,
"RRRR_imp_bits__18_18_x_11_11_x_10_10_x_9_9_x_8_8_" => "",
"reserved_imp_bits__17_17_x_16_16_" => "reserved",
},
"action" => <<DATA
{
    RxTable ( RRRR ) = zeroExtend ( RxTable ( RRRR ) ( 15 , 0 ) , 32 ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"RxTable(RRRR)[mask 0x0000ffff]/p",
],
"outputs" => [
"RxTable(RRRR)",
],
},
"ZXT_B_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "zxt.b JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_3_" => 221,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = zeroExtend ( D ( JJJ ) . LL , 40 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x00000000ff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"ZXT_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "zxt.l FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_0_" => 1248,
"reserved_imp_bits__18_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) . E = 0 ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"outputs" => [
"D(FFF)[mask 0xff00000000]/p",
"L(FFF)",
],
},
"ZXT_W_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "zxt.w JJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_3_" => 223,
"JJJ_imp_bits__18_18_x_2_0_" => "",
"reserved_imp_bits__17_17_x_14_14_" => "reserved",
"FFF_imp_bits__16_16_x_9_7_" => "",
},
"action" => <<DATA
{
    D ( FFF ) = zeroExtend ( D ( JJJ ) . L , 40 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(JJJ)[mask 0x000000ffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"imac_m_family" => {
"syntax" => "imac_m_family OPCD, Src1, Src2, Res",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( Src1 ) ( 15 , 0 ) , 41 ) , signExtend ( D ( Src2 ) ( 15 , 0 ) , 41 ) ) ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( Res ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( Res ) = tmp ( 39 , 0 ) ;
    L ( Res ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Res)",
"D(Src1)[mask 0x000000ffff]/p",
"D(Src2)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(Res)",
"EMR[mask 0x00000004]/p?",
"L(Res)",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"IMAC_mDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "imac -jj,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 347,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"imac_m_family" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 347,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( jj.Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( jj.Db ) ( 15 , 0 ) , 41 ) ) ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = tmp ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
},
},
"parent" => "imac_m_family",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x000000ffff]/p",
"D(jj.Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"IMAC_mDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "imac -JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 42,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"imac_m_family" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 42,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( JJJJJ.Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( JJJJJ.Db ) ( 15 , 0 ) , 41 ) ) ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = tmp ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
},
},
"parent" => "imac_m_family",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x000000ffff]/p",
"D(JJJJJ.Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"imac_p_family" => {
"syntax" => "imac_p_family OPCD, Src1, Src2, Res",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( Src1 ) ( 15 , 0 ) , 41 ) , signExtend ( D ( Src2 ) ( 15 , 0 ) , 41 ) ) ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( Res ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( Res ) = tmp ( 39 , 0 ) ;
    L ( Res ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Res)",
"D(Src1)[mask 0x000000ffff]/p",
"D(Src2)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(Res)",
"EMR[mask 0x00000004]/p?",
"L(Res)",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"IMAC_pDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "imac jj,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 345,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"imac_p_family" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 345,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( jj.Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( jj.Db ) ( 15 , 0 ) , 41 ) ) ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = tmp ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
},
},
"parent" => "imac_p_family",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x000000ffff]/p",
"D(jj.Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"IMAC_pDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "imac JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 40,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"imac_p_family" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 40,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( signedMultiply ( signExtend ( D ( JJJJJ.Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( JJJJJ.Db ) ( 15 , 0 ) , 41 ) ) ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = tmp ( 39 , 0 ) ;
    L ( FFF ) = 0 ;
}
DATA
,
},
},
"parent" => "imac_p_family",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x000000ffff]/p",
"D(JJJJJ.Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"overflow40",
],
"raises_exception" => 1,
},
"impy_family" => {
"syntax" => "impy_family OPCD, Src1, Src2, Res",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
    D ( Res ) = signedMultiply ( signExtend ( D ( Src1 ) . L , 40 ) , signExtend ( D ( Src2 ) . L , 40 ) ) ;
    L ( Res ) = 0 ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Src1)[mask 0x000000ffff]/p",
"D(Src2)[mask 0x000000ffff]/p",
],
"outputs" => [
"D(Res)",
"L(Res)",
],
},
"IMPY_Da_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "impy jj,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 349,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"impy_family" => {
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
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 349,
},
],
"parent_action" => <<DATA
{
    D ( FFF ) = signedMultiply ( signExtend ( D ( jj.Da ) . L , 40 ) , signExtend ( D ( jj.Db ) . L , 40 ) ) ;
    L ( FFF ) = 0 ;
}
DATA
,
},
},
"parent" => "impy_family",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(jj.Da)[mask 0x000000ffff]/p",
"D(jj.Db)[mask 0x000000ffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"IMPY_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "impy JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 41,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"impy_family" => {
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
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 41,
},
],
"parent_action" => <<DATA
{
    D ( FFF ) = signedMultiply ( signExtend ( D ( JJJJJ.Da ) . L , 40 ) , signExtend ( D ( JJJJJ.Db ) . L , 40 ) ) ;
    L ( FFF ) = 0 ;
}
DATA
,
},
},
"parent" => "impy_family",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(JJJJJ.Da)[mask 0x000000ffff]/p",
"D(JJJJJ.Db)[mask 0x000000ffff]/p",
],
"outputs" => [
"D(FFF)",
"L(FFF)",
],
},
"inc" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "inc",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"aliases" => {
"ADD__u5_Dn" => {
"sources" => [
],
"destinations" => [
],
"miscs" => [
"misc" => {
"field" => "u5_imp_bits__4_4_x_3_3_x_2_2_x_1_1_x_0_0_",
"value" => 1,
},
],
"parent_action" => <<DATA
{
     bits < 40 > D_imm = zeroExtend ( u5 , 40 ) ;
     bits < 41 > sum = ( sbits < 41 >  ) D_imm + ( sbits < 41 >  ) D ( FFF ) ;
    overflow40 ( sum ) ;
     bits < 40 > Gcarry = Carry ( D_imm , D ( FFF ) , 0 ) ;
    SR . C = Gcarry ( 40 - 1 ) ;
    saturation32 ( sum , FFF ) ;
    L ( FFF ) = LnComputation ( sum ) ;
}
DATA
,
},
},
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"mac_family1" => {
"syntax" => "mac_family1 OPCD, Src1, Src2, Res",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( Src1 ) ( 31 , 16 ) , 41 ) , signExtend ( D ( Src2 ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( Res ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , Res ) ;
    L ( Res ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"inputs" => [
"D(Res)",
"D(Src1)[mask 0x00ffff0000]/p",
"D(Src2)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(Res)?",
"EMR[mask 0x00000004]/p?",
"L(Res)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_pDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac jj,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 344,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family1" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 344,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj.Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family1",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x00ffff0000]/p",
"D(jj.Db)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_pDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 32,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family1" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 32,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ.Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family1",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x00ffff0000]/p",
"D(JJJJJ.Db)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"mac_family2" => {
"syntax" => "mac_family2 OPCD, Src1, Src2, Res",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( Src1 ) ( 31 , 16 ) , 41 ) , signExtend ( D ( Src2 ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( Res ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , Res ) ;
    L ( Res ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Res)",
"D(Src1)[mask 0x00ffff0000]/p",
"D(Src2)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(Res)?",
"EMR[mask 0x00000004]/p?",
"L(Res)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_mDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac -jj,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 346,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family2" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 346,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj.Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family2",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x00ffff0000]/p",
"D(jj.Db)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_mDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac -JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 34,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family2" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 34,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ.Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family2",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x00ffff0000]/p",
"D(JJJJJ.Db)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"mac_family3" => {
"syntax" => "mac_family3 OPCD, Src1, Src2, Res",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( Src1 ) ( 31 , 16 ) , 41 ) , signExtend ( D ( Src2 ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( Res ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , Res ) ;
    L ( Res ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Res)",
"D(Src1)[mask 0x00ffff0000]/p",
"D(Src2)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(Res)?",
"EMR[mask 0x00000004]/p?",
"L(Res)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_pDa_H_Da_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac jj_DaH_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 1344,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family3" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 1344,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family3",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x00ffff0000]/p",
"D(jj.Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_pDa_H_Db_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac DaH_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 160,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family3" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 160,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family3",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x00ffff0000]/p",
"D(JJJJJ.Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_pDa_L_Da_H_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac jj_DaL_DbH,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 1346,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family3" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Db",
},
"source" => {
"field" => "Src2",
"value" => "jj.Da",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 1346,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Db ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj.Da ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family3",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x000000ffff]/p",
"D(jj.Db)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_pDa_L_Db_H_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac DaL_DbH,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 162,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family3" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 162,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Db ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ.Da ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family3",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x000000ffff]/p",
"D(JJJJJ.Db)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"mac_family4" => {
"syntax" => "mac_family4 OPCD, Src1, Src2, Res",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( Src1 ) ( 31 , 16 ) , 41 ) , signExtend ( D ( Src2 ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( Res ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , Res ) ;
    L ( Res ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Res)",
"D(Src1)[mask 0x00ffff0000]/p",
"D(Src2)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(Res)?",
"EMR[mask 0x00000004]/p?",
"L(Res)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_mDa_H_Da_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac -jj_DaH_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 1345,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family4" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 1345,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family4",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x00ffff0000]/p",
"D(jj.Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_mDa_H_Db_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac -DaH_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 161,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family4" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 161,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family4",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x00ffff0000]/p",
"D(JJJJJ.Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_mDa_L_Da_H_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac -jj_DaL_DbH,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 1347,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family4" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Db",
},
"source" => {
"field" => "Src2",
"value" => "jj.Da",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 1347,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Db ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj.Da ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family4",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x000000ffff]/p",
"D(jj.Db)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_mDa_L_Db_H_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac -DaL_DbH,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 163,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family4" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 163,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Db ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ.Da ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family4",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x000000ffff]/p",
"D(JJJJJ.Db)[mask 0x00ffff0000]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"mac_family5" => {
"syntax" => "mac_family5 OPCD, Src1, Src2, Res",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( Src1 ) ( 15 , 0 ) , 41 ) , signExtend ( D ( Src2 ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( Res ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , Res ) ;
    L ( Res ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Res)",
"D(Src1)[mask 0x000000ffff]/p",
"D(Src2)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(Res)?",
"EMR[mask 0x00000004]/p?",
"L(Res)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_pDa_L_Da_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac jj_DaL_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 1348,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family5" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 1348,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( jj.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family5",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x000000ffff]/p",
"D(jj.Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_pDa_L_Db_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac DaL_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 164,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family5" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 164,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( JJJJJ.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family5",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x000000ffff]/p",
"D(JJJJJ.Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"mac_family6" => {
"syntax" => "mac_family6 OPCD, Src1, Src2, Res",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( Src1 ) ( 15 , 0 ) , 41 ) , signExtend ( D ( Src2 ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( Res ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , Res ) ;
    L ( Res ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Res)",
"D(Src1)[mask 0x000000ffff]/p",
"D(Src2)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(Res)?",
"EMR[mask 0x00000004]/p?",
"L(Res)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_mDa_L_Da_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac -jj_DaL_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 1349,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family6" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 1349,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( jj.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family6",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x000000ffff]/p",
"D(jj.Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"MAC_mDa_L_Db_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "mac -DaL_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 165,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"mac_family6" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 165,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( JJJJJ.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    saturation32 ( tmp , FFF ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "mac_family6",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x000000ffff]/p",
"D(JJJJJ.Db)[mask 0x000000ffff]/p",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"macr_family1" => {
"syntax" => "macr Src1,Src2,Res",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( Src1 ) ( 31 , 16 ) , 41 ) , signExtend ( D ( Src2 ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( Res ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( Res ) = round ( tmp ( 39 , 0 ) ) ;
    L ( Res ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Res)",
"D(Src1)[mask 0x00ffff0000]/p",
"D(Src2)[mask 0x00ffff0000]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(Res)",
"EMR[mask 0x00000004]/p?",
"L(Res)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_pDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr jj,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 350,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family1" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 350,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj.Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family1",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x00ffff0000]/p",
"D(jj.Db)[mask 0x00ffff0000]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_pDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 36,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family1" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 36,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ.Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family1",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x00ffff0000]/p",
"D(JJJJJ.Db)[mask 0x00ffff0000]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"macr_family2" => {
"syntax" => "macr -Src1,Src2,Res",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( Src1 ) ( 31 , 16 ) , 41 ) , signExtend ( D ( Src2 ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( Res ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( Res ) = round ( tmp ( 39 , 0 ) ) ;
    L ( Res ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Res)",
"D(Src1)[mask 0x00ffff0000]/p",
"D(Src2)[mask 0x00ffff0000]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(Res)",
"EMR[mask 0x00000004]/p?",
"L(Res)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_mDa_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr -jj,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 351,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family2" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 351,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj.Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family2",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x00ffff0000]/p",
"D(jj.Db)[mask 0x00ffff0000]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_mDa_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr -JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 38,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family2" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 38,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ.Db ) ( 31 , 16 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family2",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x00ffff0000]/p",
"D(JJJJJ.Db)[mask 0x00ffff0000]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"macr_family3" => {
"syntax" => "macr_family3 OPCD, Src1, Src2, Res",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( Src1 ) ( 31 , 16 ) , 41 ) , signExtend ( D ( Src2 ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( Res ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( Res ) = round ( tmp ( 39 , 0 ) ) ;
    L ( Res ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Res)",
"D(Src1)[mask 0x00ffff0000]/p",
"D(Src2)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(Res)",
"EMR[mask 0x00000004]/p?",
"L(Res)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_pDa_H_Da_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr jj_DaH_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 1352,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family3" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 1352,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family3",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x00ffff0000]/p",
"D(jj.Db)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_pDa_H_Db_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr DaH_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 172,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family3" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 172,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family3",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x00ffff0000]/p",
"D(JJJJJ.Db)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_pDa_L_Da_H_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr jj_DaL_DbH,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 1354,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family3" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Db",
},
"source" => {
"field" => "Src2",
"value" => "jj.Da",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 1354,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Db ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj.Da ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family3",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x000000ffff]/p",
"D(jj.Db)[mask 0x00ffff0000]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_pDa_L_Db_H_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr DaL_DbH,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 174,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family3" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 174,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Db ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ.Da ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family3",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x000000ffff]/p",
"D(JJJJJ.Db)[mask 0x00ffff0000]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"macr_family4" => {
"syntax" => "macr_family4 OPCD, Src1, Src2, Res",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( Src1 ) ( 31 , 16 ) , 41 ) , signExtend ( D ( Src2 ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( Res ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( Res ) = round ( tmp ( 39 , 0 ) ) ;
    L ( Res ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Res)",
"D(Src1)[mask 0x00ffff0000]/p",
"D(Src2)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(Res)",
"EMR[mask 0x00000004]/p?",
"L(Res)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_mDa_H_Da_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr -jj_DaH_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 1353,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family4" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 1353,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family4",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x00ffff0000]/p",
"D(jj.Db)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_mDa_H_Db_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr -DaH_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 173,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family4" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 173,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Da ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family4",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x00ffff0000]/p",
"D(JJJJJ.Db)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_mDa_L_Da_H_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr -jj_DaL_DbHFFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 1355,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family4" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Db",
},
"source" => {
"field" => "Src2",
"value" => "jj.Da",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 1355,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Db ) ( 31 , 16 ) , 41 ) , signExtend ( D ( jj.Da ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family4",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x000000ffff]/p",
"D(jj.Db)[mask 0x00ffff0000]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_mDa_L_Db_H_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr -DaL_DbH,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 175,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family4" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 175,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Db ) ( 31 , 16 ) , 41 ) , signExtend ( D ( JJJJJ.Da ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family4",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x000000ffff]/p",
"D(JJJJJ.Db)[mask 0x00ffff0000]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"macr_family5" => {
"syntax" => "macr Src1.l,Src2.l,Res",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( Src1 ) ( 15 , 0 ) , 41 ) , signExtend ( D ( Src2 ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( Res ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( Res ) = round ( tmp ( 39 , 0 ) ) ;
    L ( Res ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Res)",
"D(Src1)[mask 0x000000ffff]/p",
"D(Src2)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(Res)",
"EMR[mask 0x00000004]/p?",
"L(Res)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_pDa_L_Da_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr jj_DaL_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 1350,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family5" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 1350,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( jj.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family5",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x000000ffff]/p",
"D(jj.Db)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_pDa_L_Db_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr DaL_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 166,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family5" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 166,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( JJJJJ.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) + ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family5",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x000000ffff]/p",
"D(JJJJJ.Db)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"macr_family6" => {
"syntax" => "macr -Src1.l,Src2.l,Res",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( Src1 ) ( 15 , 0 ) , 41 ) , signExtend ( D ( Src2 ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( Res ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( Res ) = round ( tmp ( 39 , 0 ) ) ;
    L ( Res ) = LnComputation ( tmp ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Res)",
"D(Src1)[mask 0x000000ffff]/p",
"D(Src2)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(Res)",
"EMR[mask 0x00000004]/p?",
"L(Res)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_mDa_L_Da_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr -jj_DaL_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_2_" => 1351,
"jj_imp_bits__18_17_x_1_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family6" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Da",
},
"source" => {
"field" => "Src2",
"value" => "jj.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 1351,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( jj.Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( jj.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family6",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(jj.Da)[mask 0x000000ffff]/p",
"D(jj.Db)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"MACR_mDa_L_Db_L_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "macr -DaL_DbL,FFF",
"block" => "dalu",
"attributes" => {
"Type11" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_10_x_6_5_" => 167,
"JJJJJ_imp_bits__18_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"macr_family6" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Da",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Res",
"value" => "FFF",
},
],
"destinations" => [
"destination" => {
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 167,
},
],
"parent_action" => <<DATA
{
     bits < 41 > mult = ( ( signedMultiply ( signExtend ( D ( JJJJJ.Da ) ( 15 , 0 ) , 41 ) , signExtend ( D ( JJJJJ.Db ) ( 15 , 0 ) , 41 ) ) ) << 1 ) ;
     bits < 41 > tmp = ( sbits < 41 >  ) D ( FFF ) - ( sbits < 41 >  ) mult ;
    overflow40 ( tmp ) ;
    D ( FFF ) = round ( tmp ( 39 , 0 ) ) ;
    L ( FFF ) = LnComputation ( tmp ) ;
}
DATA
,
},
},
"parent" => "macr_family6",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(FFF)",
"D(JJJJJ.Da)[mask 0x000000ffff]/p",
"D(JJJJJ.Db)[mask 0x000000ffff]/p",
"EMR/p",
"SR/p",
],
"outputs" => [
"D(FFF)",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
],
"helpers" => [
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"round",
"saturate",
],
"raises_exception" => 1,
},
"nop" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "nop",
"attributes" => {
"Type4" => "",
"one_word" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_0_" => 37056,
"reserved_imp_bits__18_16_" => "reserved",
},
"action" => <<DATA
{
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
},
"sub_family" => {
"syntax" => "sub_family OPCD, Src1, Src2, Res",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"OPCD" => "",
"Src1" => "",
"Src2" => "",
"Res" => "",
},
"action" => <<DATA
{
     bits < 41 > sum = ( sbits < 41 >  ) D ( Src1 ) - ( sbits < 41 >  ) D ( Src2 ) ;
    overflow40 ( sum ) ;
    Borrow ( sum , D ( Src2 ) , D ( Src1 ) ) ;
    saturation32 ( sum , Res ) ;
    L ( Res ) = LnComputation ( sum ) ;
}
DATA
,
"disassemble" => 1,
"type" => "OneWord",
"inputs" => [
"D(Src1)",
"D(Src2)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(Res)?",
"EMR[mask 0x00000004]/p?",
"L(Res)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"Borrow",
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"SUB_Da_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sub jj,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_x_4_4_x_3_3_x_2_2_" => 281,
"jj_imp_bits__18_18_x_17_17_x_1_1_x_0_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"sub_family" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "jj.Db",
},
"source" => {
"field" => "Src2",
"value" => "jj.Da",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 281,
},
],
"parent_action" => <<DATA
{
     bits < 41 > sum = ( sbits < 41 >  ) D ( jj.Db ) - ( sbits < 41 >  ) D ( jj.Da ) ;
    overflow40 ( sum ) ;
    Borrow ( sum , D ( jj.Da ) , D ( jj.Db ) ) ;
    saturation32 ( sum , FFF ) ;
    L ( FFF ) = LnComputation ( sum ) ;
}
DATA
,
},
},
"parent" => "sub_family",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(jj.Da)",
"D(jj.Db)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"Borrow",
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"SUB_Da_Db_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sub JJJJJ,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 44,
"JJJJJ_imp_bits__18_18_x_17_17_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"sub_family" => {
"sources" => [
"source" => {
"field" => "Src1",
"value" => "JJJJJ.Db",
},
"source" => {
"field" => "Src2",
"value" => "JJJJJ.Da",
},
],
"destinations" => [
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 44,
},
],
"parent_action" => <<DATA
{
     bits < 41 > sum = ( sbits < 41 >  ) D ( JJJJJ.Db ) - ( sbits < 41 >  ) D ( JJJJJ.Da ) ;
    overflow40 ( sum ) ;
    Borrow ( sum , D ( JJJJJ.Da ) , D ( JJJJJ.Db ) ) ;
    saturation32 ( sum , FFF ) ;
    L ( FFF ) = LnComputation ( sum ) ;
}
DATA
,
},
},
"parent" => "sub_family",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(JJJJJ.Da)",
"D(JJJJJ.Db)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"Borrow",
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"SUB_Db_Da_Dn" => {
"width" => 20,
"fetch_width" => 16,
"syntax" => "sub DbDa,FFF",
"block" => "dalu",
"attributes" => {
"Type1" => "",
"one_word" => "",
"instr_tbl_DAU" => "",
},
"fields" => {
"VarInstrOpcode_imp_bits__19_19_x_15_15_x_13_13_x_12_12_x_11_11_x_10_10_x_6_6_x_5_5_" => 45,
"JJJJJ_imp_bits__17_17_x_18_18_x_4_0_" => "",
"FFF_imp_bits__16_16_x_9_9_x_8_8_x_7_7_" => "",
"reserved_imp_bits__14_14_" => "reserved",
},
"aliases" => {
"sub_family" => {
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
"destination" => {
"conditional" => 1,
"field" => "Res",
"value" => "FFF",
},
],
"miscs" => [
"misc" => {
"field" => "OPCD",
"value" => 45,
},
],
"parent_action" => <<DATA
{
     bits < 41 > sum = ( sbits < 41 >  ) D ( JJJJJ.Da ) - ( sbits < 41 >  ) D ( JJJJJ.Db ) ;
    overflow40 ( sum ) ;
    Borrow ( sum , D ( JJJJJ.Db ) , D ( JJJJJ.Da ) ) ;
    saturation32 ( sum , FFF ) ;
    L ( FFF ) = LnComputation ( sum ) ;
}
DATA
,
},
},
"parent" => "sub_family",
"disassemble" => 0,
"type" => "OneWord",
"inputs" => [
"D(JJJJJ.Da)",
"D(JJJJJ.Db)",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF)?",
"EMR[mask 0x00000004]/p?",
"L(FFF)",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"Borrow",
"IsUserMode",
"LnComputation",
"LnComputation_nochk",
"overflow40",
"saturation32",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
},
"exceptions" => {
"IllegalAddress" => {
"priority" => 0,
"action" => <<DATA
{
}
DATA
,
},
"Overflow" => {
"priority" => 0,
"action" => <<DATA
{
}
DATA
,
},
"Privelege" => {
"priority" => 0,
"action" => <<DATA
{
}
DATA
,
},
},
"memories" => {
"DMEM" => {
"instr_mem" => 0,
"size" => 4026531840,
"addr_unit" => 1,
},
"IMEM" => {
"instr_mem" => 1,
"size" => 4026531840,
"addr_unit" => 1,
},
"PMEM" => {
"instr_mem" => 0,
"prefetch_mem" => 1,
"size" => 36,
"addr_unit" => 1,
},
},
"core-level-hooks" => {
"decode-miss" => <<DATA
{
    halt (  ) ;
}
DATA
,
"pre-cycle" => <<DATA

DATA
,
"post-cycle" => <<DATA

DATA
,
"pre-fetch" => <<DATA
{
     unsigned instr_size = calc_instr_size ( PC . uint32 (  ) ) ;
    if ( decode_mode == prefix_mode ) {
        switch ( instr_size ) {
            case 1 : setCurrentInstrTable ( prefix ) ;
            update_prefix_size ( + 1 ) ;
            break ;
            case 2 : setCurrentInstrTable ( tw_prefix ) ;
            update_prefix_size ( + 2 ) ;
            break ;
            case 3 : decode_mode = usual_mode ;
        }
    } else if ( decode_mode == esg_mode ) {
        if ( instr_size != 1 ) {
            decode_mode = usual_mode ;
        } else {
            setCurrentInstrTable ( esg_prefix ) ;
            update_prefix_size ( + 1 ) ;
        }
    }
    if ( decode_mode == usual_mode ) {
        switch ( instr_size ) {
            case 1 : setCurrentInstrTable ( one_word ) ;
            break ;
            case 2 : setCurrentInstrTable ( two_word ) ;
            break ;
            case 3 : setCurrentInstrTable ( three_word ) ;
            break ;
            default : error ( 1 , "Illegal instr_size" ) ;
        }
    }
    switch ( getCurrentInstrTable (  ) ) {
        case one_word :  ;
        break ;
        case two_word :  ;
        break ;
        case three_word :  ;
        break ;
        case prefix :  ;
        break ;
        case tw_prefix :  ;
        break ;
        case esg_prefix :  ;
        break ;
    }
    if ( cond_skip (  ) ) {
        NPC = PC + 2 * instr_size ;
        setPacketPosition ( getPacketPosition (  ) + instr_size ) ;
        skip_instruction (  ) ;
    }
}
DATA
,
"post-fetch" => <<DATA

DATA
,
"post-exec" => <<DATA
{
    if ( nop_prefix ) {
        setPacketPosition ( 0 ) ;
         ;
        nop_prefix = false ;
    } else if ( decode_mode == prefix_mode ) {
        decode_mode = esg_mode ;
        if ( ( SR . SLF == 1 ) && ( lpmarkA || lpmarkB ) && ( ignore_lpmarkA == a_not_ignore ) && ( ignore_lpmarkB == b_not_ignore ) ) {
            DSPC = PC ;
        }
    } else if ( decode_mode == esg_mode ) {
        decode_mode = usual_mode ;
    }
    if ( prefix_grouping ) {
         unsigned set_size = VlesSize == 0 ? 8 : VlesSize ;
        if ( getPacketPosition (  ) == set_size + 1 ) {
            setPacketPosition ( 0 ) ;
             ;
        } else {
            switch ( getCurrentInstrTable (  ) ) {
                case one_word : case two_word : case three_word : incr_instr_pos (  ) ;
                break ;
                default : break ;
            }
        }
    } else if ( decode_mode == usual_mode ) {
         int num_bytes = 0 ;
         uint32_t * instr = getCurrentInstr ( num_bytes ) ;
         bool end_of_set = false ;
        switch ( getCurrentInstrTable (  ) ) {
            case one_word : end_of_set = instr [ 0 ] & 0x0c000000 ;
            break ;
            case esg_prefix : break ;
            case two_word : end_of_set = true ;
            break ;
            case three_word : end_of_set = true ;
            break ;
            default : error ( 1 , "ERROR IN POST_EXEC SERIAL" ) ;
        }
        if ( end_of_set ) {
            setPacketPosition ( 0 ) ;
             ;
        } else {
            incr_instr_pos (  ) ;
        }
    }
}
DATA
,
"post-asm" => <<DATA
{
    InstrCnt += 1 ;
}
DATA
,
"post-packet-asm" => <<DATA
{
    InstrCnt = 0 ;
}
DATA
,
"post-packet" => <<DATA
{
    set_lpmarks (  ) ;
    set_ignore_lpmarks (  ) ;
    if ( LpmarkA || LpmarkB ) {
         uint32_t active_loop ;
        switch ( lpmarks_mode (  ) ) {
            case 1 : active_loop = ( SRUpdated ) ? FindNextActiveLoop (  ) : FindActiveLoop (  ) ;
            LC ( active_loop ) = LC ( active_loop ) - 1 ;
            Fetch = Fetch :: DoubleDelaySlot ;
            DSPC = SA ( active_loop ) ;
            ignore_lpmarkA = a_double_delay_slot ;
            ignore_lpmarkB = b_double_delay_slot ;
            break ;
            case 2 : active_loop = ( SRUpdated ) ? FindNextActiveLoop (  ) : FindActiveLoop (  ) ;
            LC ( active_loop ) = 0 ;
            clear_SR_LF ( active_loop ) ;
            ignore_lpmarkA = a_double_delay_slot ;
            break ;
            case 3 : active_loop = ( SRUpdated ) ? FindNextActiveLoop (  ) : FindActiveLoop (  ) ;
            LC ( active_loop ) = LC ( active_loop ) - 1 ;
            DSPC = SA ( active_loop ) ;
            Fetch = Fetch :: Jump ;
            break ;
            case 4 : active_loop = ( SRUpdated ) ? FindNextActiveLoop (  ) : FindActiveLoop (  ) ;
            LC ( active_loop ) = 0 ;
            clear_SR_LF ( active_loop ) ;
            SR . SLF = 0 ;
            break ;
            case 5 : active_loop = ( SRUpdated ) ? FindNextActiveLoop (  ) : FindActiveLoop (  ) ;
            LC ( active_loop ) = LC ( active_loop ) - 1 ;
            ignore_lpmarkA = a_delay_slot ;
            ignore_lpmarkB = b_delay_slot ;
            Fetch = Fetch :: DelaySlot ;
            if ( SR . SLF == 0 ) DSPC = SA ( active_loop ) ;
            break ;
            case 6 : active_loop = ( SRUpdated ) ? FindNextActiveLoop (  ) : FindActiveLoop (  ) ;
            LC ( active_loop ) = 0 ;
            clear_SR_LF ( active_loop ) ;
            SR . SLF = 0 ;
            break ;
            case 7 : active_loop = ( SRUpdated ) ? FindNextActiveLoop (  ) : FindActiveLoop (  ) ;
            LC ( active_loop ) = LC ( active_loop ) - 1 ;
            if ( SR . SLF == 0 ) DSPC = SA ( active_loop ) ;
            Fetch = Fetch :: Jump ;
            break ;
            case 8 : active_loop = ( SRUpdated ) ? FindNextActiveLoop (  ) : FindActiveLoop (  ) ;
            LC ( active_loop ) = 0 ;
            clear_SR_LF ( active_loop ) ;
            SR . SLF = 0 ;
            break ;
            default : break ;
        }
    }
    if ( subroutine_instr == regular ) {
        mem_write_xsr (  ) ;
        RAS ( 0 ) = NPC ;
        VALID_RAS ( 0 ) = 1 ;
        subroutine_instr = not_sub ;
    } else if ( subroutine_instr == delayed ) {
        subroutine_instr = regular ;
    }
    if ( trap_instr ) {
        DMEM ( ESP , 4 ) = NPC ;
        DMEM ( ESP + 4 , 4 ) = SR ;
        ESP = ESP + 8 ;
        trap_instr = false ;
    }
    if ( jump_instr ) {
        info ( 1 , "Entered post_packet, jump_instr, NVPC: " << hex << NVPC , " PC_ST_VLES: " << hex << PC_ST_VLES ) ;
        NPC = NVPC ;
        jump_instr = false ;
    }
    if ( Fetch == Fetch :: Normal ) {
    } else if ( Fetch == Fetch :: DoubleDelaySlot ) {
        Fetch = Fetch :: DelaySlot ;
    } else if ( Fetch == Fetch :: DelaySlot ) {
        Fetch = Fetch :: Jump ;
    } else if ( Fetch == Fetch :: Jump ) {
        NPC = DSPC ;
        Fetch = Fetch :: Normal ;
    }
    switch ( SP_update ) {
        case sp_update_p8 : SP = SP + 8 ;
        SP_update = no_update ;
        SP_32bit_instr_occured = false ;
        SP_64bit_instr_occured = false ;
        break ;
        case sp_update_p16 : SP = SP + 16 ;
        SP_update = no_update ;
        SP_32bit_instr_occured = false ;
        SP_64bit_instr_occured = false ;
        break ;
        case sp_update_m8 : SP = SP - 8 ;
        SP_update = no_update ;
        SP_32bit_instr_occured = false ;
        SP_64bit_instr_occured = false ;
        break ;
        case sp_update_m16 : SP = SP - 16 ;
        SP_update = no_update ;
        SP_32bit_instr_occured = false ;
        SP_64bit_instr_occured = false ;
        break ;
        case nsp_update_p8 : NSP = NSP + 8 ;
        SP_update = no_update ;
        SP_32bit_instr_occured = false ;
        SP_64bit_instr_occured = false ;
        break ;
        case nsp_update_p16 : NSP = NSP + 16 ;
        SP_update = no_update ;
        SP_32bit_instr_occured = false ;
        SP_64bit_instr_occured = false ;
        break ;
        case nsp_update_m8 : NSP = NSP - 8 ;
        SP_update = no_update ;
        SP_32bit_instr_occured = false ;
        SP_64bit_instr_occured = false ;
        break ;
        case nsp_update_m16 : NSP = NSP - 16 ;
        SP_update = no_update ;
        SP_32bit_instr_occured = false ;
        SP_64bit_instr_occured = false ;
        break ;
        case no_update : break ;
        default : break ;
    }
    PC_ST_VLES = NPC ;
    prefix_grouping = false ;
    reset_dispatch (  ) ;
    reset_prefix_size (  ) ;
    reset_cond_exec (  ) ;
    reset_instr_pos (  ) ;
    decode_mode = prefix_mode ;
    info ( 1 , "eop" ) ;
}
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
"EMR_LOW" => {
"value" => "false",
"options" => [
"false",
"true",
],
},
"EndianMode" => {
"value" => "Big",
"options" => [
"Big",
"Little",
],
},
"LegalPC" => {
"value" => "true",
"options" => [
"false",
"true",
],
},
"LpmarkA" => {
"value" => "false",
"options" => [
"false",
"true",
],
},
"LpmarkB" => {
"value" => "false",
"options" => [
"false",
"true",
],
},
"SP_32bit_instr_occured" => {
"value" => "false",
"options" => [
"false",
"true",
],
},
"SP_64bit_instr_occured" => {
"value" => "false",
"options" => [
"false",
"true",
],
},
"SP_update" => {
"value" => "no_update",
"options" => [
"no_update",
"nsp_update_m16",
"nsp_update_m8",
"nsp_update_p16",
"nsp_update_p8",
"sp_update_m16",
"sp_update_m8",
"sp_update_p16",
"sp_update_p8",
],
},
"SRUpdated" => {
"value" => "false",
"options" => [
"false",
"true",
],
},
"agu0" => {
"value" => "true",
"options" => [
"false",
"true",
],
},
"agu1" => {
"value" => "true",
"options" => [
"false",
"true",
],
},
"agu_dispatch" => {
"value" => "modulo2",
"options" => [
"modulo2",
"modulo4",
],
},
"clear_SR_PE" => {
"value" => "false",
"options" => [
"false",
"true",
],
},
"cond_exec" => {
"value" => "c0",
"options" => [
"c0",
"c1",
"c2",
"c3",
"c4",
"c5",
"c6",
"c7",
],
},
"dalu0" => {
"value" => "true",
"options" => [
"false",
"true",
],
},
"dalu1" => {
"value" => "true",
"options" => [
"false",
"true",
],
},
"dalu2" => {
"value" => "true",
"options" => [
"false",
"true",
],
},
"dalu3" => {
"value" => "true",
"options" => [
"false",
"true",
],
},
"decode_mode" => {
"value" => "prefix_mode",
"options" => [
"esg_mode",
"prefix_mode",
"usual_mode",
],
},
"exception_condition" => {
"value" => "false",
"options" => [
"false",
"true",
],
},
"exception_overflow" => {
"value" => "false",
"options" => [
"false",
"true",
],
},
"ignore_lpmarkA" => {
"value" => "a_not_ignore",
"options" => [
"a_delay_slot",
"a_double_delay_slot",
"a_not_ignore",
],
},
"ignore_lpmarkB" => {
"value" => "b_not_ignore",
"options" => [
"b_delay_slot",
"b_double_delay_slot",
"b_not_ignore",
],
},
"instr_pos" => {
"value" => "instr0",
"options" => [
"instr0",
"instr1",
"instr2",
"instr3",
"instr4",
"instr5",
"instr6",
"instr7",
"instr8",
],
},
"jump_instr" => {
"value" => "false",
"options" => [
"false",
"true",
],
},
"nop_prefix" => {
"value" => "false",
"options" => [
"false",
"true",
],
},
"prefix_grouping" => {
"value" => "false",
"options" => [
"false",
"true",
],
},
"prefix_size" => {
"value" => "pfx0",
"options" => [
"pfx0",
"pfx1",
"pfx2",
"pfx3",
],
},
"set_SR_PE" => {
"value" => "disable",
"options" => [
"disable",
"enable",
],
},
"subroutine_instr" => {
"value" => "not_sub",
"options" => [
"delayed",
"not_sub",
"regular",
],
},
"trap_instr" => {
"value" => "false",
"options" => [
"false",
"true",
],
},
"word4_dalu" => {
"value" => "non_dalu",
"options" => [
"is_adalu",
"non_dalu",
"word4_dalu1",
"word4_dalu2",
"word4_dalu3",
],
},
},
"asm_config" => {
"comments" => [
";",
],
"line_comments" => [
";",
],
"line_seps" => [
"\n",
],
"group_seps" => [
[
"[",
"]",
],
[
"\n",
"\n",
],
],
"instrtables" => [
"prefix",
"esg_prefix",
"tw_prefix",
"one_word",
"two_word",
"three_word",
],
"parms" => {
"InstrCnt" => {
"value" => 0,
"type" => "int",
},
},
},
"helpers" => {
"ABSbyte" => {
"action" => <<DATA
 bits < 9 > ABSbyte ( bits < 9 > val ) {
     bits < 9 > neg_val = ( ~ val  ) + 1 ;
    return ( val ( 8 ) == 1 ) ? neg_val : val ;
}
DATA
,
},
"ABSd" => {
"action" => <<DATA
 bits < 40 > ABSd ( bits < 40 > reg ) {
    return ( reg ( 39 ) == 1 ) ? ( ( ~ reg  ) + 1 ) : reg ;
}
DATA
,
},
"ABSword" => {
"action" => <<DATA
 bits < 16 > ABSword ( bits < 16 > val ) {
     bits < 16 > neg_val = ( ~ val  ) + 1 ;
    return ( val ( 15 ) == 1 ) ? neg_val : val ;
}
DATA
,
},
"Address_by_MCTL" => {
"action" => <<DATA
 bits < 32 > Address_by_MCTL ( bits < 5 > n , bits < 32 > value , bits < 1 > table , bits < 1 > decr , bits < 32 > access_width , bits < 32 > preshift ) {
     bits < 32 > rval = ( table != 0 ) ? RxTable ( n ) : rxTable ( n ) ;
     bits < 32 > offset = value ;
     bits < 4 > AM = MCTL . AM_R ( n ( 2 , 0 ) ) ;
     bits < 32 > address = 0 ;
     sbits < 32 > soffset = ( sbits < 32 >  ) offset ;
     bits < 4 > mode = AM ;
     bool off_sign = ( preshift ( 31 ) != 0 ) ? true : false ;
    if ( ( decr == 1 ) && ( mode . uint32 (  ) != 1 ) ) {
        off_sign = ! off_sign ;
        soffset = ( sbits < 32 >  ) 0x0 - ( sbits < 32 >  ) offset ;
        offset = 0 - offset ;
    }
    switch ( mode . uint32 (  ) ) {
        case 0x0 : address = rval + ( soffset ) ;
        break ;
        case 0x1 : {
             bits < 32 > rval_rev = 0 , offset_rev = 0 , addr_rev ;
            rval_rev = bitreverse ( rval ) ;
            if ( decr == 1 ) offset = ~ ( offset ) ;
            offset_rev = bitreverse ( offset ) ;
            addr_rev = rval_rev + offset_rev + decr ;
            address = bitreverse ( addr_rev ) ;
            address &= ( 0 - access_width ) ;
            break ;
        }
        case 0xc : case 0xd : case 0xe : case 0xf : {
             sbits < 32 > mrev ;
            address = ( soffset  ) + rval ;
            mrev = ~ ( M ( AM ( 1 , 0 ) ) ) ;
            address &= M ( AM ( 1 , 0 ) ) ;
            address |= ( rval & mrev ) ;
            break ;
        }
        case 0x8 : case 0x9 : case 0xa : case 0xb : {
             bits < 32 > modulus = M ( AM ( 1 , 0 ) ) ;
             bits < 32 > up_bound = B ( n ( 2 , 0 ) ) + modulus ;
             bits < 32 > adr1 = ( sbits < 32 >  ) soffset + rval ;
             bits < 32 > adr2 = adr1 ;
             bits < 32 > cond ;
            if ( ! off_sign ) {
                cond = adr1 - up_bound ;
                adr2 -= modulus ;
                address = ( ( cond ( 31 ) != 0 ) ? adr1 : adr2 ) ;
            } else {
                cond = adr1 - B ( n ( 2 , 0 ) ) ;
                adr2 += modulus ;
                address = ( ( cond ( 31 ) != 0 ) ? adr2 : adr1 ) ;
            }
            break ;
        }
        default : address = rval + ( soffset ) ;
    }
    return address ;
}
DATA
,
"inputs" => [
"B?",
"M?",
"MCTL/p",
"RxTable(n [parm #0])",
"rxTable(n [parm #0])",
],
"helpers" => [
"bitreverse",
"preshift",
],
},
"Address_by_mctl2" => {
"action" => <<DATA
 bits < 32 > Address_by_mctl2 ( bits < 3 > n , bits < 32 > value , bits < 1 > decr , bits < 32 > access_width , bits < 32 > preshift ) {
     bits < 32 > rval = R ( n ) ;
     bits < 32 > offset = value ;
     bits < 4 > AM = MCTL . AM_R ( n ( 2 , 0 ) ) ;
     bits < 32 > address = 0 ;
     sbits < 32 > soffset = ( sbits < 32 >  ) offset ;
     bits < 4 > mode = AM ;
     bool off_sign = ( preshift ( 31 ) != 0 ) ? true : false ;
    if ( ( decr == 1 ) && ( mode . uint32 (  ) != 1 ) ) {
        off_sign = ! off_sign ;
        soffset = ( sbits < 32 >  ) 0x0 - ( sbits < 32 >  ) offset ;
        offset = 0 - offset ;
    }
    switch ( mode . uint32 (  ) ) {
        case 0x0 : address = rval + ( soffset ) ;
        break ;
        case 0x1 : {
             bits < 32 > rval_rev = 0 , offset_rev = 0 , addr_rev ;
            rval_rev = bitreverse ( rval ) ;
            if ( decr == 1 ) offset = ~ ( offset ) ;
            offset_rev = bitreverse ( offset ) ;
            addr_rev = rval_rev + offset_rev + decr ;
            address = bitreverse ( addr_rev ) ;
            address &= ( 0 - access_width ) ;
            break ;
        }
        case 0xc : case 0xd : case 0xe : case 0xf : {
             sbits < 32 > mrev ;
            address = ( soffset  ) + rval ;
            mrev = ~ ( M ( AM ( 1 , 0 ) ) ) ;
            address &= M ( AM ( 1 , 0 ) ) ;
            address |= ( rval & mrev ) ;
            break ;
        }
        case 0x8 : case 0x9 : case 0xa : case 0xb : {
             bits < 32 > modulus = M ( AM ( 1 , 0 ) ) ;
             bits < 32 > up_bound = B ( n ( 2 , 0 ) ) + modulus ;
             bits < 32 > adr1 = ( sbits < 32 >  ) soffset + rval ;
             bits < 32 > adr2 = adr1 ;
             bits < 32 > cond ;
            if ( ! off_sign ) {
                cond = adr1 - up_bound ;
                adr2 -= modulus ;
                address = ( ( cond ( 31 ) != 0 ) ? adr1 : adr2 ) ;
            } else {
                cond = adr1 - B ( n ( 2 , 0 ) ) ;
                adr2 += modulus ;
                address = ( ( cond ( 31 ) != 0 ) ? adr2 : adr1 ) ;
            }
            break ;
        }
        default : address = rval + ( soffset ) ;
    }
    return address ;
}
DATA
,
"inputs" => [
"B?",
"M?",
"MCTL/p",
"R(n [parm #0])",
],
"helpers" => [
"bitreverse",
"preshift",
],
},
"AguSignedGreater" => {
"action" => <<DATA
 bits < 1 > AguSignedGreater ( bits < 32 > Rx , bits < 32 > rx ) {
     bits < 33 > subt = signExtend ( Rx , 33 ) + signExtend ( ( ~ rx + 1 ) , 33 ) ;
    if ( ( subt ( 32 + 1 ) == 0 ) && ( subt != 0 ) ) {
        return 1 ;
    } else {
        return 0 ;
    }
}
DATA
,
},
"Borrow" => {
"action" => <<DATA
 void Borrow ( bits < 41 > dest , bits < 40 > src0 , bits < 40 > src1 ) {
     bits < 1 > tmp = 0 ;
    if ( ( dest & ( 0x08000000000ull ) ) == 0 ) {
        if ( ( ( src0 & 0x8000000000ull ) != 0 ) && ( ( src1 & ( 0x8000000000ull ) ) == 0 ) ) {
            tmp = 1 ;
        }
    } else {
        if ( ( ( src0 & 0x8000000000ull ) != 0 ) || ( ( src1 & 0x8000000000ull ) == 0 ) ) {
            tmp = 1 ;
        }
    }
    SR . C = tmp ;
}
DATA
,
"inputs" => [
"SR/p",
],
"outputs" => [
"SR[mask 0x00000001]/p",
],
},
"DaluSignedGreater" => {
"action" => <<DATA
 bits < 1 > DaluSignedGreater ( bits < 40 > Dn , bits < 40 > Da ) {
     bits < 41 > subt = signExtend ( Dn , 41 ) + signExtend ( ( ~ Da + 1 ) , 41 ) ;
    if ( ( subt ( 40 + 1 ) == 0 ) && ( subt != 0 ) ) {
        return 1 ;
    } else {
        return 0 ;
    }
}
DATA
,
},
"EAtable" => {
"action" => <<DATA
 bits < 32 > EAtable ( bits < 3 > ea , bits < 4 > n , bits < 2 > shift ) {
     bits < 32 > Rn_initial = R ( n ) ;
     bits < 32 > R_tmp , Rn_new ;
     bits < 4 > AM = MCTL . AM_R ( n ( 2 , 0 ) . uint32 (  ) ) ;
     bits < 32 > force_lsb = 0xFFFFFFFF ;
    force_lsb <<= ( shift ) ;
     bits < 32 > offset = increase_size ( ea , shift ) ;
     sbits < 32 > soffset = ( sbits < 32 >  ) offset ;
     bits < 4 > mode = AM ;
    if ( n ( 3 ) == 1 ) {
        mode = 0 ;
    }
     bool off_sign = ( preshift ( ea ) ( 31 ) != 0 ) ? true : false ;
     bits < 1 > decr = ( ea . uint32 (  ) == 1 ) ? 1 : 0 ;
    switch ( mode . uint32 (  ) ) {
        case 0x0 : R_tmp = ( soffset  ) + R ( n ) ;
        break ;
        case 0x1 : {
             bits < 32 > Rn_reversed = bitreverse ( R ( n ) ) ;
            if ( decr == 1 ) soffset = soffset - 1 ;
             bits < 32 > rev_soffset = bitreverse ( soffset ) ;
            R_tmp = ( sbits < 32 >  ) Rn_reversed + ( sbits < 32 >  ) rev_soffset + ( bits < 32 >  ) decr ;
            R_tmp = ( bitreverse ( R_tmp ) ) ;
            R_tmp &= force_lsb ;
            break ;
        }
        case 0xc : case 0xd : case 0xe : case 0xf : {
            R_tmp = soffset + R ( n ) ;
             bits < 32 > mrev = ~ M ( AM ( 1 , 0 ) ) ;
            R_tmp &= M ( AM ( 1 , 0 ) ) ;
            R_tmp |= ( R ( n ) & mrev ) ;
            break ;
        }
        case 0x8 : case 0x9 : case 0xa : case 0xb : {
             bits < 32 > modulus = M ( AM ( 1 , 0 ) ) ;
             bits < 32 > up_bound = B ( n ( 2 , 0 ) ) + modulus ;
             bits < 32 > adr1 = ( sbits < 32 >  ) soffset + R ( n ) ;
             bits < 32 > adr2 = adr1 ;
             bits < 32 > cond ;
            if ( ! off_sign ) {
                cond = adr1 - up_bound ;
                adr2 -= modulus ;
                R_tmp = ( ( cond ( 31 ) != 0 ) ? adr1 : adr2 ) ;
            } else {
                cond = adr1 - B ( n ( 2 , 0 ) ) ;
                adr2 += modulus ;
                R_tmp = ( ( cond ( 31 ) != 0 ) ? adr2 : adr1 ) ;
            }
            break ;
        }
        default : R_tmp = ( soffset  ) + R ( n ) ;
    }
    if ( ea . uint32 (  ) != 0 ) {
        if ( ea . uint32 (  ) != 2 ) {
            R ( n ) = R_tmp ;
        }
        Rn_new = Rn_initial ;
    } else {
        Rn_new = R_tmp ;
    }
    return ( Rn_new ) ;
}
DATA
,
"inputs" => [
"B?",
"M?",
"MCTL/p",
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
"R(n [parm #1])?",
],
"outputs" => [
"R(n [parm #1])?",
],
"helpers" => [
"bitreverse",
"increase_size",
"preshift",
],
},
"FindActiveLoop" => {
"action" => <<DATA
 uint32_t FindActiveLoop (  ) {
    return ( SR . LF ( 3 ) != 0 ) ? 3 : ( SR . LF ( 2 ) != 0 ) ? 2 : ( SR . LF ( 1 ) != 0 ) ? 1 : 0 ;
}
DATA
,
"inputs" => [
"SR[mask 0x78000000]/p",
],
},
"FindNextActiveLoop" => {
"action" => <<DATA
 uint32_t FindNextActiveLoop (  ) {
    if ( SR . LF ( 3 ) == 1 ) {
        if ( SR . LF ( 2 ) == 1 ) {
            return 2 ;
        } else {
            if ( SR . LF ( 1 ) == 1 ) {
                return 1 ;
            } else {
                return 0 ;
            }
        }
    } else {
        if ( SR . LF ( 2 ) == 1 ) {
            if ( SR . LF ( 1 ) == 1 ) return 1 ; else return 0 ;
        } else return 0 ;
    }
}
DATA
,
"inputs" => [
"SR[mask 0x78000000]/p",
],
},
"IsNormalMode" => {
"action" => <<DATA
 bool IsNormalMode (  ) {
    if ( ( SR . EXP == 0 ) && ( SR . PE == 0 ) ) {
        return true ;
    } else {
        return false ;
    }
}
DATA
,
"inputs" => [
"SR[mask 0x00060000]/p",
],
},
"IsUserMode" => {
"action" => <<DATA
 bool IsUserMode (  ) {
    if ( ( SR . EXP == 0 ) && ( SR . PE == 1 ) ) {
        return true ;
    } else {
        return false ;
    }
}
DATA
,
"inputs" => [
"SR[mask 0x00060000]/p",
],
},
"LnComputation" => {
"action" => <<DATA
 bits < 1 > LnComputation ( bits < 40 > data ) {
    if ( SR . SM == 0 ) {
        return ( LnComputation_nochk ( data ) ) ;
    } else return 0 ;
}
DATA
,
"inputs" => [
"SR[mask 0x00000034]/p",
],
"helpers" => [
"LnComputation_nochk",
],
},
"LnComputation_nochk" => {
"action" => <<DATA
 bits < 1 > LnComputation_nochk ( bits < 40 > data ) {
     bits < 40 > mask ;
     bits < 2 > scale_mode = SR . SCM ;
    switch ( scale_mode . uint32 (  ) ) {
        case 0 : mask = 0xff80000000ull ;
        break ;
        case 1 : mask = 0xff00000000ull ;
        break ;
        case 2 : mask = 0xffc0000000ull ;
        break ;
        case 3 : mask = 0xff80000000ull ;
        default : break ;
    }
    if ( ( ( data & mask ) == mask ) || ( ( data & mask ) == 0 ) ) {
        return ( 0 ) ;
    }
    return ( 1 ) ;
}
DATA
,
"inputs" => [
"SR[mask 0x00000030]/p",
],
},
"LnComputation_special_instr" => {
"action" => <<DATA
 bits < 1 > LnComputation_special_instr ( bits < 40 > data ) {
    if ( SR . SM == 1 ) {
         bits < 40 > mask = 0xff80000000ull ;
        if ( ( ( data & mask ) == mask ) || ( ( data & mask ) == 0 ) ) {
            return ( 0 ) ;
        }
        return ( 1 ) ;
    } else {
        return ( LnComputation_nochk ( data ) ) ;
    }
}
DATA
,
"inputs" => [
"SR[mask 0x00000034]/p",
],
"helpers" => [
"LnComputation_nochk",
],
},
"NumberOfOnes" => {
"action" => <<DATA
 bits < 6 > NumberOfOnes ( bits < 40 > reg ) {
     bits < 6 > num_of_ones = 0 ;
     bits < 40 > mask = 0x8000000000ull ;
    while ( ( mask & reg ) != 0 ) {
        num_of_ones += 1 ;
        mask >>= 1 ;
    }
    return ( num_of_ones ) ;
}
DATA
,
},
"NumberOfOnes32" => {
"action" => <<DATA
 bits < 5 > NumberOfOnes32 ( bits < 32 > reg ) {
     bits < 5 > num_of_ones = 0 ;
     bits < 32 > mask = 0x80000000 ;
    while ( ( mask & reg ) != 0 ) {
        num_of_ones += 1 ;
        mask >>= 1 ;
    }
    return ( num_of_ones ) ;
}
DATA
,
},
"Shift_Left_Dalu" => {
"action" => <<DATA
 bits < 80 > Shift_Left_Dalu ( bits < 40 > src , int shift ) {
    if ( shift ) {
        shift = ( shift <= 40 ) ? shift : 40 ;
         var tmp = src << shift ;
         bits < 80 > extended_temp = signExtend ( tmp , 80 ) ;
        SR . C = extended_temp ( 40 , 40 ) ;
        return extended_temp ;
    } else {
        SR . C = 0 ;
        return signExtend ( src , 80 ) ;
    }
}
DATA
,
"inputs" => [
"SR/p",
],
"outputs" => [
"SR[mask 0x00000001]/p",
],
},
"Shift_Right_Dalu" => {
"action" => <<DATA
 bits < 40 > Shift_Right_Dalu ( bits < 40 > src , int shift ) {
    if ( shift ) {
        shift = ( shift <= 40 ) ? shift : 40 ;
         bits < 40 > tmp = src >> shift ;
        info ( 1 , "before=" << hex << tmp << " number=" << 40 - shift - 1 ) ;
        tmp = slice_signExtend ( tmp , 40 - shift - 1 ) ;
        info ( 1 , "after" << hex << tmp ) ;
        SR . C = ( shift ) ? ( src ( shift - 1 , shift - 1 ) ) : 0 ;
        return tmp ;
    }
    return src ;
}
DATA
,
"inputs" => [
"SR/p",
],
"outputs" => [
"SR[mask 0x00000001]/p?",
],
"helpers" => [
"slice_signExtend",
],
},
"WrExtWord" => {
"action" => <<DATA
 bits < 8 > WrExtWord ( bits < 40 > reg1 , bits < 40 > reg2 ) {
     bits < 40 > sum = signExtend ( reg1 ( 39 , 16 ) , 40 ) + signExtend ( reg2 ( 39 , 16 ) , 40 ) ;
    return ( sum ( 23 , 16 ) ) ;
}
DATA
,
},
"bitreverse" => {
"action" => <<DATA
 bits < 32 > bitreverse ( bits < 32 > src ) {
     bits < 32 > ret ( 0 ) ;
     bits < 32 > tmp = src . swap_bytes (  ) ;
    ret = ( ( tmp & 0x80808080 ) >> 7 ) | ( ( tmp & 0x40404040 ) >> 5 ) | ( ( tmp & 0x20202020 ) >> 3 ) | ( ( tmp & 0x10101010 ) >> 1 ) | ( ( tmp & 0x08080808 ) << 1 ) | ( ( tmp & 0x04040404 ) << 3 ) | ( ( tmp & 0x02020202 ) << 5 ) | ( ( tmp & 0x01010101 ) << 7 ) ;
    return ret ;
}
DATA
,
},
"calc_instr_size" => {
"action" => <<DATA
 unsigned calc_instr_size ( uint32_t pc ) {
     bits < 16 > a = PMEM ( pc , 2 ) ;
     bits < 16 > b = PMEM ( pc + 2 , 2 ) ;
     bits < 16 > c = PMEM ( pc + 4 , 2 ) ;
     int eos = end_of_set ( a , b , c ) ;
    if ( eos >= 0 ) {
        if ( type1_end_of_set ( a , b , c ) ) {
            return 1 ;
        } else {
            switch ( eos ) {
                case 0 : return 1 ;
                case 1 : if ( b ( 15 , 14 ) == 3 ) {
                    return 1 ;
                } else {
                    return 2 ;
                }
                case 2 : if ( c ( 15 , 14 ) == 3 ) {
                    return 1 ;
                } else if ( b ( 15 , 13 ) == 0 ) {
                    return 1 ;
                } else if ( b ( 15 , 13 ) == 1 ) {
                    return 3 ;
                } else {
                    error ( 1 , "Illegal instruction size" ) ;
                    halt (  ) ;
                    return 0 ;
                }
                break ;
                default : {
                    error ( 1 , "Illegal instruction size" ) ;
                    halt (  ) ;
                    return 0 ;
                }
            }
        }
    } else {
        return 1 ;
    }
}
DATA
,
"input_mems" => [
"PMEM",
],
"helpers" => [
"end_of_set",
"type1_end_of_set",
],
},
"check_exception_DxTable" => {
"action" => <<DATA
 void check_exception_DxTable ( bits < 6 > entry ) {
    if ( IsUserMode (  ) ) {
        if ( entry ( 4 , 0 ) == 2 ) {
            EMR_LOW = true ;
            EMR . ILPR = 1 ;
            raiseException ( Privelege ) ;
        }
    }
}
DATA
,
"inputs" => [
"EMR/p",
"SR[mask 0x00060000]/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p?",
],
"helpers" => [
"IsUserMode",
],
"raises_exception" => 1,
},
"clear_SR_LF" => {
"action" => <<DATA
 void clear_SR_LF ( uint32_t flag ) {
    switch ( flag ) {
        case 0 : SR . LF0 = 0 ;
        break ;
        case 1 : SR . LF1 = 0 ;
        break ;
        case 2 : SR . LF2 = 0 ;
        break ;
        case 3 : SR . LF3 = 0 ;
        break ;
        default : break ;
    }
}
DATA
,
"inputs" => [
"SR/p",
],
"outputs" => [
"SR[mask 0x78000000]/p?",
],
},
"clear_emr_nmid" => {
"action" => <<DATA
 void clear_emr_nmid (  ) {
    EMR_LOW = true ;
    EMR . NMID = 0 ;
}
DATA
,
"inputs" => [
"EMR/p",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000008]/p",
],
"helpers" => [
"IsUserMode",
],
},
"cond_skip" => {
"action" => <<DATA
 bool cond_skip (  ) {
    if ( decode_mode != usual_mode ) {
        return false ;
    }
     bool even = ( getPacketPosition (  ) % 2 == 0 ) ;
     bool t_bit = ( SR . T == 1 ) ;
    switch ( cond_exec ) {
        case c0 : return false ;
        case c1 : if ( t_bit ) {
            if ( even ) {
                return false ;
            }
        } else {
            if ( ! even ) {
                return false ;
            }
        }
        break ;
        case c2 : if ( t_bit ) {
            return false ;
        }
        break ;
        case c3 : if ( ! t_bit ) {
            return false ;
        }
        break ;
        case c4 : break ;
        case c5 : break ;
        case c6 : if ( ( t_bit && even ) || ( ! even ) ) {
            return false ;
        }
        break ;
        case c7 : if ( ( ( ! t_bit ) && even ) || ( ! even ) ) {
            return false ;
        }
        break ;
    }
    return true ;
}
DATA
,
"inputs" => [
"SR[mask 0x00000002]/p",
],
},
"csSAT" => {
"action" => <<DATA
 void csSAT ( bits < 40 > src , bits < 4 > FFF ) {
    if ( SR . SM != 0 ) {
        if ( D ( FFF ) . signedGT ( 0x7fffffff ) || D ( FFF ) . signedLT ( 0x80000000 ) ) {
            if ( src ( 39 ) != 0 ) {
                D ( FFF ) = ( 0xff80000000ull ) ;
                if ( ( EMR . DOVF == 0 && SR . OVE == 1 ) ) raiseException ( Overflow ) ;
                exception_overflow = true ;
                EMR . DOVF = 1 ;
            } else if ( src != 0 ) {
                D ( FFF ) = ( 0x007fffffffull ) ;
                if ( ( EMR . DOVF == 0 && SR . OVE == 1 ) ) raiseException ( Overflow ) ;
                exception_overflow = true ;
                EMR . DOVF = 1 ;
            }
        }
    }
}
DATA
,
"inputs" => [
"D(FFF [parm #1])?",
"EMR/p?",
"SR/p",
],
"outputs" => [
"D(FFF [parm #1])?",
"EMR[mask 0x00000004]/p?",
],
"helpers" => [
"IsUserMode",
],
"raises_exception" => 1,
},
"decr_prefix_size" => {
"action" => <<DATA
 void decr_prefix_size (  ) {
    switch ( prefix_size ) {
        case pfx3 : prefix_size = pfx2 ;
        break ;
        case pfx2 : prefix_size = pfx1 ;
        break ;
        case pfx1 : prefix_size = pfx0 ;
        break ;
        default : error ( 1 , "cannot decrement prefix_size" ) ;
        break ;
    }
}
DATA
,
},
"ea_2EA" => {
"action" => <<DATA
 bits < 2 > ea_2EA ( bits < 2 > ea_value ) {
     bits < 2 > EA_value = 0 ;
    switch ( ea_value . uint32 (  ) ) {
        case 0 : EA_value = 3 ;
        break ;
        case 1 : EA_value = 1 ;
        break ;
        case 2 : EA_value = 0 ;
        break ;
        case 3 : EA_value = 2 ;
        break ;
        default : break ;
    }
    return EA_value ;
}
DATA
,
},
"end_of_set" => {
"action" => <<DATA
 int end_of_set ( bits < 16 > w0 , bits < 16 > w1 , bits < 16 > w2 ) {
    if ( w0 ( 15 , 14 ) != 0 ) {
        return 0 ;
    } else if ( w1 ( 15 , 14 ) != 0 ) {
        return 1 ;
    } else if ( w2 ( 15 , 14 ) != 0 ) {
        return 2 ;
    } else {
        return - 1 ;
    }
}
DATA
,
},
"extension_update_add" => {
"action" => <<DATA
 bits < 8 > extension_update_add ( bits < 17 > valh , bits < 17 > vall ) {
    if ( valh ( 16 ) == 1 && vall ( 16 ) == 1 ) return ( 0x00 ) ; else if ( valh ( 16 ) == 1 ) return ( 0x0f ) ; else if ( vall ( 16 ) == 1 ) return ( 0xf0 ) ; else return ( 0xff ) ;
}
DATA
,
},
"extension_update_high" => {
"action" => <<DATA
 bits < 8 > extension_update_high ( bits < 17 > val ) {
    if ( val ( 16 ) == 1 ) return ( 0x0f ) ; else return ( 0xff ) ;
}
DATA
,
},
"extension_update_high_sub" => {
"action" => <<DATA
 bits < 8 > extension_update_high_sub ( bits < 17 > val ) {
    if ( val ( 16 ) == 0 ) return ( 0x0f ) ; else return ( 0xff ) ;
}
DATA
,
},
"extension_update_low" => {
"action" => <<DATA
 bits < 8 > extension_update_low ( bits < 17 > val ) {
    if ( val ( 16 ) == 1 ) return ( 0xf0 ) ; else return ( 0xff ) ;
}
DATA
,
},
"extension_update_low_sub" => {
"action" => <<DATA
 bits < 8 > extension_update_low_sub ( bits < 17 > val ) {
    if ( val ( 16 ) == 0 ) return ( 0xf0 ) ; else return ( 0xff ) ;
}
DATA
,
},
"extension_update_mixed" => {
"action" => <<DATA
 bits < 8 > extension_update_mixed ( bits < 17 > valh , bits < 17 > vall ) {
    if ( valh ( 16 ) == 1 && vall ( 16 ) == 0 ) return ( 0x00 ) ; else if ( valh ( 16 ) == 1 && vall ( 16 ) == 1 ) return ( 0x0f ) ; else if ( valh ( 16 ) == 0 && vall ( 16 ) == 0 ) return ( 0xf0 ) ; else return ( 0xff ) ;
}
DATA
,
},
"extension_update_mixed_xx" => {
"action" => <<DATA
 bits < 8 > extension_update_mixed_xx ( bits < 17 > valh , bits < 17 > vall ) {
    if ( valh ( 16 ) == 1 && vall ( 16 ) == 0 ) return ( 0x00 ) ; else if ( valh ( 16 ) == 0 && vall ( 16 ) == 0 ) return ( 0xf0 ) ; else if ( valh ( 16 ) == 1 && vall ( 16 ) == 1 ) return ( 0x0f ) ; else return ( 0xff ) ;
}
DATA
,
},
"extension_update_sub" => {
"action" => <<DATA
 bits < 8 > extension_update_sub ( bits < 17 > valh , bits < 17 > vall ) {
    if ( valh ( 16 ) == 0 && vall ( 16 ) == 0 ) return ( 0x00 ) ; else if ( valh ( 16 ) == 0 ) return ( 0x0f ) ; else if ( vall ( 16 ) == 0 ) return ( 0xf0 ) ; else return ( 0xff ) ;
}
DATA
,
},
"extract_f" => {
"action" => <<DATA
 void extract_f ( bits < 4 > FFF , bits < 4 > jjj , bits < 6 > width , bits < 6 > offset , bool sign_extension ) {
     bool modify = false ;
     bits < 6 > width_s = ( width > 40 ) ? 40 : width ;
     bits < 6 > offset_s = ( offset > 39 ) ? 39 : offset ;
    if ( width_s == 0 ) {
        width_s = 1 ;
    }
     bits < 40 > slice = modulo_slice ( D ( jjj ) , width_s , offset_s ) ;
    if ( sign_extension ) {
        if ( ( offset_s . uint32 (  ) == 39 ) && ( offset . uint32 (  ) != 39 ) ) {
            modify = true ;
            D ( FFF ) = 0 ;
        }
    } else {
        if ( ( width == 0 ) || ( offset_s == 39 ) ) {
            modify = true ;
            D ( FFF ) = 0 ;
        }
    }
    if ( ! modify ) {
        if ( sign_extension ) D ( FFF ) = slice_signExtend ( slice , width_s - 1 ) ; else D ( FFF ) = slice ;
    }
}
DATA
,
"inputs" => [
"D(jjj [parm #1])",
],
"outputs" => [
"D(FFF [parm #0])?",
],
"helpers" => [
"modulo_slice",
"slice_signExtend",
],
},
"first_available_dalu" => {
"action" => <<DATA
 unsigned first_available_dalu (  ) {
    if ( dalu0 ) {
        dalu0 = false ;
        return 0 ;
    } else if ( dalu1 ) {
        dalu1 = false ;
        return 1 ;
    } else if ( dalu2 ) {
        dalu2 = false ;
        return 2 ;
    } else if ( dalu3 ) {
        dalu3 = false ;
        return 3 ;
    } else {
        error ( 1 , "Too many dalu block requested in the VLES" ) ;
        return 0 ;
    }
}
DATA
,
},
"get_agu_dispatch" => {
"action" => <<DATA
 unsigned get_agu_dispatch (  ) {
     unsigned pos = getPacketPosition (  ) ;
    info ( 1 , "!!get_agu_dispatch,pos: " << pos ) ;
    if ( agu_dispatch == modulo2 ) {
        info ( 1 , "!! agu_dispatch==modulo2" ) ;
        return pos % 2 ;
    } else {
        if ( pos == 8 ) return 0 ;
        switch ( pos % 4 ) {
            case 0 : if ( agu0 ) {
                agu0 = false ;
                return 0 ;
            } else {
                agu1 = false ;
                return 1 ;
            }
            case 1 : agu1 = false ;
            return 1 ;
            case 2 : agu1 = false ;
            return 1 ;
            case 3 : agu0 = false ;
            return 0 ;
            default : return - 1 ;
        }
    }
}
DATA
,
},
"get_dalu_dispatch" => {
"action" => <<DATA
 unsigned get_dalu_dispatch ( unsigned index ) {
    switch ( index ) {
        case 0 : dalu0 = false ;
        return 0 ;
        case 1 : dalu1 = false ;
        return 1 ;
        case 2 : dalu2 = false ;
        return 2 ;
        case 3 : dalu3 = false ;
        return 3 ;
        case 4 : {
            if ( word4_dalu == word4_dalu1 ) {
                dalu1 = false ;
                return 1 ;
            } else if ( word4_dalu == word4_dalu2 ) {
                dalu2 = false ;
                return 2 ;
            } else if ( word4_dalu == word4_dalu3 ) {
                dalu3 = false ;
                return 3 ;
            }
            if ( dalu0 ) {
                dalu0 = false ;
                return 0 ;
            } else {
                return first_available_dalu (  ) ;
            }
        }
        case 5 : if ( dalu1 ) {
            dalu1 = false ;
            return 1 ;
        } else {
            return first_available_dalu (  ) ;
        }
        case 6 : if ( dalu2 ) {
            dalu2 = false ;
            return 2 ;
        } else {
            return first_available_dalu (  ) ;
        }
        case 7 : if ( dalu3 ) {
            dalu3 = false ;
            return 3 ;
        } else {
            return first_available_dalu (  ) ;
        }
        case 8 : return 0 ;
        default : {
            error ( 1 , "Illegal word position in dalu dispatch" ) ;
            return 0 ;
        }
    }
}
DATA
,
"helpers" => [
"first_available_dalu",
],
},
"get_esg_relevant_instruction_bit" => {
"action" => <<DATA
 unsigned get_esg_relevant_instruction_bit ( bits < 6 > esg_bits , int index , int pfx_size ) {
     unsigned pos = ( 10 - index + pfx_size ) % 6 ;
    return ( esg_bits ( pos ) ) . uint32 (  ) ;
}
DATA
,
},
"get_instr_pos" => {
"action" => <<DATA
 int get_instr_pos (  ) {
    return ( int  ) instr_pos ;
}
DATA
,
},
"get_prefix_size" => {
"action" => <<DATA
 unsigned get_prefix_size (  ) {
    switch ( prefix_size ) {
        case pfx0 : return 0 ;
        case pfx1 : return 1 ;
        case pfx2 : return 2 ;
        case pfx3 : return 3 ;
        default : error ( 1 , "Illegal prefix size" ) ;
        return 0 ;
    }
}
DATA
,
},
"incr_instr_pos" => {
"action" => <<DATA
 void incr_instr_pos (  ) {
     ;
    switch ( instr_pos ) {
        case instr0 : instr_pos = instr1 ;
        break ;
        case instr1 : instr_pos = instr2 ;
        break ;
        case instr2 : instr_pos = instr3 ;
        break ;
        case instr3 : instr_pos = instr4 ;
        break ;
        case instr4 : instr_pos = instr5 ;
        break ;
        case instr5 : instr_pos = instr6 ;
        break ;
        case instr6 : instr_pos = instr7 ;
        break ;
        case instr7 : instr_pos = instr8 ;
        break ;
        default : error ( 1 , "cannot increment instruction position" ) ;
        break ;
    }
}
DATA
,
},
"incr_prefix_size" => {
"action" => <<DATA
 void incr_prefix_size (  ) {
    switch ( prefix_size ) {
        case pfx0 : prefix_size = pfx1 ;
        break ;
        case pfx1 : prefix_size = pfx2 ;
        break ;
        case pfx2 : prefix_size = pfx3 ;
        break ;
        default : error ( 1 , "cannot increment prefix_size" ) ;
        break ;
    }
}
DATA
,
},
"increase_size" => {
"action" => <<DATA
 bits < 32 > increase_size ( bits < 3 > ea_increase , bits < 2 > shift ) {
     bits < 32 > increase ;
    switch ( ea_increase . uint32 (  ) ) {
        case 0 : increase = N ( 0 ) ;
        break ;
        case 1 : increase = 0xFFFFFFFF ;
        break ;
        case 2 : increase = 0x00000000 ;
        break ;
        case 3 : increase = 0x00000001 ;
        break ;
        case 4 : increase = N ( 0 ) ;
        break ;
        case 5 : increase = N ( 1 ) ;
        break ;
        case 6 : increase = N ( 2 ) ;
        break ;
        case 7 : increase = N ( 3 ) ;
        break ;
    }
    return ( increase << shift . uint32 (  ) ) ;
}
DATA
,
"inputs" => [
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
],
},
"init_cond_exec" => {
"action" => <<DATA
 void init_cond_exec ( unsigned x ) {
    switch ( x ) {
        case 0 : cond_exec = c0 ;
        break ;
        case 1 : cond_exec = c1 ;
        break ;
        case 2 : cond_exec = c2 ;
        break ;
        case 3 : cond_exec = c3 ;
        break ;
        case 4 : cond_exec = c4 ;
        break ;
        case 5 : cond_exec = c5 ;
        break ;
        case 6 : cond_exec = c6 ;
        break ;
        case 7 : cond_exec = c7 ;
        break ;
    }
}
DATA
,
},
"is_dalu" => {
"action" => <<DATA
 bool is_dalu ( uint32_t addr , int instr_size , uint32_t esg ) {
    if ( instr_size == 1 ) {
         bits < 16 > a = PMEM ( addr , 2 ) ;
        if ( a == 0x90c0 ) {
            return false ;
        }
         uint32_t msbits = ( a >> 14 ) . uint32 (  ) ;
        switch ( msbits ) {
            case 1 : if ( a ( 13 ) == 1 ) {
                return true ;
            } else {
                return false ;
            }
            case 2 : if ( esg == 1 ) {
                return true ;
            } else {
                return false ;
            }
            case 3 : if ( esg ) {
                if ( a ( 13 ) == 0 ) {
                    return true ;
                } else {
                    return false ;
                }
            } else {
                if ( a ( 13 ) == 0 && a ( 12 ) == 1 ) return true ; else return false ;
            }
        }
    }
    if ( instr_size == 2 ) {
         bits < 32 > a = PMEM ( addr , 4 ) ;
         bits < 4 > b = a ( 31 , 28 ) ;
        if ( b == 0x3 ) {
            return true ;
        } else {
            return false ;
        }
    }
    if ( instr_size == 3 ) {
        return false ;
    }
    return false ;
}
DATA
,
"input_mems" => [
"PMEM",
],
},
"is_esg" => {
"action" => <<DATA
 bool is_esg ( uint32_t addr ) {
     bits < 16 > a = PMEM ( addr , 2 ) ;
    if ( ( ( a & 0xffc0 ) == 0x7000 ) && ( ( a & 0x003f ) != 0x0000 ) ) {
        return true ;
    }
    return false ;
}
DATA
,
"input_mems" => [
"PMEM",
],
},
"lpmarks_mode" => {
"action" => <<DATA
 int lpmarks_mode (  ) {
    SRUpdated = false ;
     uint32_t active_loop = FindActiveLoop (  ) ;
     bool sr_slf = ( SR . SLF == 1 ) ? true : false ;
    if ( LpmarkA && LpmarkB ) {
        if ( LC ( active_loop ) . signedGT ( 1 ) ) {
            LpmarkB = false ;
        } else {
            LC ( active_loop ) = 0 ;
            clear_SR_LF ( active_loop ) ;
            SRUpdated = true ;
            SR . SLF = 0 ;
            sr_slf = false ;
            LpmarkA = false ;
        }
    }
    active_loop = ( SRUpdated ) ? FindNextActiveLoop (  ) : FindActiveLoop (  ) ;
    if ( ( ! LpmarkA ) && ( LpmarkB ) ) {
        if ( ( SR ( 27 + active_loop , 27 + active_loop ) ) == 1 && ( ! sr_slf ) ) {
            if ( LC ( active_loop ) . signedGT ( 1 ) ) {
                return 1 ;
            } else {
                return 2 ;
            }
        } else {
            if ( ( SR ( 27 + active_loop , 27 + active_loop ) == 1 ) && ( sr_slf ) ) {
                if ( LC ( active_loop ) . signedGT ( 1 ) ) {
                    return 5 ;
                } else {
                    return 6 ;
                }
            } else {
                error ( 1 , " Not supposed to get here - LPMARKS" ) ;
                return 0 ;
            }
        }
    } else {
        if ( ( LpmarkA ) && ( ! LpmarkB ) ) {
            if ( ( SR ( 27 + active_loop , 27 + active_loop ) == 1 ) && ( ! sr_slf ) ) {
                if ( LC ( active_loop ) . signedGT ( 1 ) ) {
                    return 3 ;
                } else {
                    return 4 ;
                }
            } else {
                if ( ( SR ( 27 + active_loop , 27 + active_loop ) == 1 ) && ( sr_slf ) ) {
                    if ( LC ( active_loop ) . signedGT ( 1 ) ) {
                        return 7 ;
                    } else {
                        return 8 ;
                    }
                } else {
                    error ( 1 , " Not supposed to get here - LPMARKS" ) ;
                    return 0 ;
                }
            }
        } else {
            error ( 1 , " Not supposed to get here - LPMARKS" ) ;
            return 0 ;
        }
    }
}
DATA
,
"inputs" => [
"LC?",
"SR/p",
],
"outputs" => [
"LC?",
"SR[mask 0xf8000000]/p?",
],
"helpers" => [
"FindActiveLoop",
"FindNextActiveLoop",
"clear_SR_LF",
],
},
"mem_write_pop2l_DeDoTable" => {
"action" => <<DATA
 void mem_write_pop2l_DeDoTable ( bits < 6 > entry ) {
    if ( SP_64bit_instr_occured || SP_32bit_instr_occured ) {
        DeDoTable ( entry ) = DMEM ( SP - 16 , 8 ) ;
        SP_update = sp_update_m16 ;
    } else {
        DeDoTable ( entry ) = DMEM ( SP - 8 , 8 ) ;
        SP_update = sp_update_m8 ;
    }
    SP_64bit_instr_occured = true ;
}
DATA
,
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"DeDoTable(entry [parm #0])",
],
"input_mems" => [
"DMEM",
],
},
"mem_write_pop_DeTable" => {
"action" => <<DATA
 void mem_write_pop_DeTable ( bits < 6 > entry ) {
    if ( SP_64bit_instr_occured ) {
        DeTable ( entry ) = DMEM ( SP - 16 , 4 ) ;
        SP_update = sp_update_m16 ;
    } else {
        DeTable ( entry ) = DMEM ( SP - 8 , 4 ) ;
        SP_update = sp_update_m8 ;
    }
    SP_32bit_instr_occured = true ;
}
DATA
,
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"DeTable(entry [parm #0])",
],
"input_mems" => [
"DMEM",
],
},
"mem_write_pop_DoTable" => {
"action" => <<DATA
 void mem_write_pop_DoTable ( bits < 6 > entry ) {
    if ( SP_64bit_instr_occured ) {
        DoTable ( entry ) = DMEM ( SP - 12 , 4 ) ;
        SP_update = sp_update_m16 ;
    } else {
        DoTable ( entry ) = DMEM ( SP - 4 , 4 ) ;
        SP_update = sp_update_m8 ;
    }
    SP_32bit_instr_occured = true ;
}
DATA
,
"inputs" => [
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"outputs" => [
"DoTable(entry [parm #0])",
],
"input_mems" => [
"DMEM",
],
},
"mem_write_popn2l_DeDoTable" => {
"action" => <<DATA
 void mem_write_popn2l_DeDoTable ( bits < 6 > entry ) {
    if ( SP_64bit_instr_occured || SP_32bit_instr_occured ) {
        DeDoTable ( entry ) = DMEM ( NSP - 16 , 8 ) ;
        SP_update = nsp_update_m16 ;
    } else {
        DeDoTable ( entry ) = DMEM ( NSP - 8 , 8 ) ;
        SP_update = nsp_update_m8 ;
    }
    SP_64bit_instr_occured = true ;
}
DATA
,
"inputs" => [
"NSP",
],
"outputs" => [
"DeDoTable(entry [parm #0])",
],
"input_mems" => [
"DMEM",
],
},
"mem_write_popn_DeTable" => {
"action" => <<DATA
 void mem_write_popn_DeTable ( bits < 6 > entry ) {
    if ( SP_64bit_instr_occured ) {
        DeTable ( entry ) = DMEM ( NSP - 16 , 4 ) ;
        SP_update = nsp_update_m16 ;
    } else {
        DeTable ( entry ) = DMEM ( NSP - 8 , 4 ) ;
        SP_update = nsp_update_m8 ;
    }
    SP_32bit_instr_occured = true ;
}
DATA
,
"inputs" => [
"NSP",
],
"outputs" => [
"DeTable(entry [parm #0])",
],
"input_mems" => [
"DMEM",
],
},
"mem_write_popn_DoTable" => {
"action" => <<DATA
 void mem_write_popn_DoTable ( bits < 6 > entry ) {
    if ( SP_64bit_instr_occured ) {
        DoTable ( entry ) = DMEM ( NSP - 12 , 4 ) ;
        SP_update = nsp_update_m16 ;
    } else {
        DoTable ( entry ) = DMEM ( NSP - 4 , 4 ) ;
        SP_update = nsp_update_m8 ;
    }
    SP_32bit_instr_occured = true ;
}
DATA
,
"inputs" => [
"NSP",
],
"outputs" => [
"DoTable(entry [parm #0])",
],
"input_mems" => [
"DMEM",
],
},
"mem_write_push2l_DeDoTable" => {
"action" => <<DATA
 void mem_write_push2l_DeDoTable ( bits < 6 > entry ) {
    if ( SP_64bit_instr_occured || SP_32bit_instr_occured ) {
        DMEM ( SP + 8 , 8 ) = DeDoTable ( entry ) ;
        SP_update = sp_update_p16 ;
    } else {
        DMEM ( SP , 8 ) = DeDoTable ( entry ) ;
        SP_update = sp_update_p8 ;
    }
    SP_64bit_instr_occured = true ;
}
DATA
,
"inputs" => [
"DeDoTable(entry [parm #0])",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"output_mems" => [
"DMEM",
],
},
"mem_write_push_DeTable" => {
"action" => <<DATA
 void mem_write_push_DeTable ( bits < 6 > entry ) {
    if ( SP_64bit_instr_occured ) {
        DMEM ( SP + 8 , 4 ) = DeTable ( entry ) ;
        SP_update = sp_update_p16 ;
    } else {
        DMEM ( SP , 4 ) = DeTable ( entry ) ;
        SP_update = sp_update_p8 ;
    }
    SP_32bit_instr_occured = true ;
}
DATA
,
"inputs" => [
"DeTable(entry [parm #0])",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"output_mems" => [
"DMEM",
],
},
"mem_write_push_DoTable" => {
"action" => <<DATA
 void mem_write_push_DoTable ( bits < 6 > entry ) {
    if ( SP_64bit_instr_occured ) {
        DMEM ( SP + 12 , 4 ) = DoTable ( entry ) ;
        SP_update = sp_update_p16 ;
    } else {
        DMEM ( SP + 4 , 4 ) = DoTable ( entry ) ;
        SP_update = sp_update_p8 ;
    }
    SP_32bit_instr_occured = true ;
}
DATA
,
"inputs" => [
"DoTable(entry [parm #0])",
"ESP?",
"NSP?",
"SP",
"SR/p",
],
"output_mems" => [
"DMEM",
],
},
"mem_write_pushn2l_DeDoTable" => {
"action" => <<DATA
 void mem_write_pushn2l_DeDoTable ( bits < 6 > entry ) {
    if ( SP_64bit_instr_occured || SP_32bit_instr_occured ) {
        DMEM ( NSP + 8 , 8 ) = DeDoTable ( entry ) ;
        SP_update = nsp_update_p16 ;
    } else {
        DMEM ( NSP , 8 ) = DeDoTable ( entry ) ;
        SP_update = nsp_update_p8 ;
    }
    SP_64bit_instr_occured = true ;
}
DATA
,
"inputs" => [
"DeDoTable(entry [parm #0])",
"NSP",
],
"output_mems" => [
"DMEM",
],
},
"mem_write_pushn_DeTable" => {
"action" => <<DATA
 void mem_write_pushn_DeTable ( bits < 6 > entry ) {
    if ( SP_64bit_instr_occured ) {
        DMEM ( NSP + 8 , 4 ) = DeTable ( entry ) ;
        SP_update = nsp_update_p16 ;
    } else {
        DMEM ( NSP , 4 ) = DeTable ( entry ) ;
        SP_update = nsp_update_p8 ;
    }
    SP_32bit_instr_occured = true ;
}
DATA
,
"inputs" => [
"DeTable(entry [parm #0])",
"NSP",
],
"output_mems" => [
"DMEM",
],
},
"mem_write_pushn_DoTable" => {
"action" => <<DATA
 void mem_write_pushn_DoTable ( bits < 6 > entry ) {
    if ( SP_64bit_instr_occured ) {
        DMEM ( NSP + 12 , 4 ) = DoTable ( entry ) ;
        SP_update = nsp_update_p16 ;
    } else {
        DMEM ( NSP + 4 , 4 ) = DoTable ( entry ) ;
        SP_update = nsp_update_p8 ;
    }
    SP_32bit_instr_occured = true ;
}
DATA
,
"inputs" => [
"DoTable(entry [parm #0])",
"NSP",
],
"output_mems" => [
"DMEM",
],
},
"mem_write_xsr" => {
"action" => <<DATA
 void mem_write_xsr (  ) {
    if ( SP_64bit_instr_occured || SP_32bit_instr_occured ) {
        DMEM ( SP + 8 , 4 ) = NPC ;
        DMEM ( SP + 12 , 4 ) = SR ;
        SP_update = sp_update_p16 ;
    } else {
        DMEM ( SP , 4 ) = NPC ;
        DMEM ( SP + 4 , 4 ) = SR ;
        SP_update = sp_update_p8 ;
    }
    SP_32bit_instr_occured = true ;
}
DATA
,
"inputs" => [
"ESP?",
"NPC",
"NSP?",
"SP",
"SR/p",
],
"output_mems" => [
"DMEM",
],
},
"modulo_slice" => {
"action" => <<DATA
 bits < 40 > modulo_slice ( bits < 40 > dsrc , bits < 6 > width , bits < 6 > offset ) {
     bits < 40 > mask1 = 0xffffffffffull ;
    mask1 &= dsrc ;
    mask1 >>= offset ;
     bits < 40 > mask2 = 0xffffffffffull ;
    mask2 &= dsrc ;
    mask2 <<= ( 40 - offset ) ;
     bits < 40 > slice = mask1 | mask2 ;
    slice <<= ( 40 - width ) ;
    slice >>= ( 40 - width ) ;
    return slice ;
}
DATA
,
},
"multiple_bit_shift" => {
"action" => <<DATA
 void multiple_bit_shift ( bits < 4 > FFF , bits < 7 > shift , bool shift_direction , bool saturation ) {
     bits < 40 > val ;
     bits < 7 > neg_val = ( ~ shift  ) + 1 ;
     bits < 7 > shift_s = ( shift ( 6 ) == 0 ) ? shift : ( neg_val ) ;
     bits < 6 > abs_shift = ( zeroExtend ( shift_s , 8 ) . signedGT ( 40 ) ) ? 40 : shift_s ;
     bool shift_left = ( ( shift_direction && ( shift ( 6 ) == 0 ) ) || ( ! shift_direction && ( shift ( 6 ) == 1 ) ) ) ;
    if ( ! shift_left ) {
        if ( shift_s . signedGT ( 40 ) ) {
            val = ( D ( FFF ) ( 39 ) == 0 ) ? 0x0 : 0xffffffffffull ;
            if ( saturation && ( val == 0xffffffffffull ) ) {
                D ( FFF ) = 0xff80000000ull ;
            } else {
                D ( FFF ) = val ;
            }
            SR . C = ( D ( FFF ) ( 39 ) == 0 ) ? 0 : 1 ;
        } else {
            val = D ( FFF ) >> abs_shift ;
            val = slice_signExtend ( val , ( 39 - abs_shift ) ) ;
            if ( saturation ) {
                D ( FFF ) = saturation32_shift ( false , D ( FFF ) ( 39 ) , val ) ;
            } else {
                D ( FFF ) = val ;
            }
            SR . C = ( abs_shift == 0 ) ? 0 : D ( FFF ) ( abs_shift - 1 ) ;
        }
    } else {
         bool ovf = overflow_shift_left ( D ( FFF ) , abs_shift ) ;
        val = D ( FFF ) << abs_shift ;
        if ( saturation ) {
            D ( FFF ) = saturation32_shift ( ovf , D ( FFF ) ( 39 ) , val ) ;
        } else {
            D ( FFF ) = val ;
            if ( ovf ) set_emr_dovf (  ) ;
        }
        SR . C = ( abs_shift == 0 ) ? 0 : D ( FFF ) ( 40 - abs_shift ) ;
    }
}
DATA
,
"inputs" => [
"D(FFF [parm #0])/p",
"EMR/p",
"SR[mask 0x00060004]/p?",
],
"outputs" => [
"D(FFF [parm #0])",
"EMR[mask 0x00000004]/p?",
"SR[mask 0x00000001]/p",
],
"helpers" => [
"IsUserMode",
"overflow_shift_left",
"saturation32_shift",
"set_emr_dovf",
"slice_signExtend",
],
},
"multiple_bit_shift2" => {
"action" => <<DATA
 void multiple_bit_shift2 ( bits < 4 > FFF , bits < 5 > shift , bool shift_direction ) {
     bits < 32 > val ;
     bits < 5 > neg_val = ( ~ shift  ) + 1 ;
     bits < 5 > shift_s = ( shift ( 4 ) == 0 ) ? shift : ( neg_val ) ;
     bits < 6 > shift_temp = zeroExtend ( shift_s , 6 ) ;
     bits < 6 > abs_shift = ( shift_temp . signedGT ( 16 ) ) ? 16 : shift_temp ;
     bool shift_left = ( ( shift_direction && ( shift ( 4 ) == 0 ) ) || ( ! shift_direction && ( shift ( 4 ) == 1 ) ) ) ;
    if ( ! shift_left ) {
         bits < 16 > val_low = D ( FFF ) . L >> abs_shift ;
         bits < 16 > val_high = D ( FFF ) . H >> abs_shift ;
        D ( FFF ) . L = slice_signExtend16 ( val_low , ( 15 - abs_shift ) ) ;
        D ( FFF ) . H = slice_signExtend16 ( val_high , ( 15 - abs_shift ) ) ;
    } else {
         bool ovf1 = overflow_shift_left ( concat ( D ( FFF ) . L , zero ( 24 ) ) , abs_shift ) ;
         bool ovf2 = overflow_shift_left ( concat ( D ( FFF ) . H , zero ( 24 ) ) , abs_shift ) ;
         bits < 32 > val_low = D ( FFF ) . L << abs_shift ;
         bits < 32 > val_high = D ( FFF ) . H << abs_shift ;
        D ( FFF ) . L = saturation16_shift ( ovf1 , D ( FFF ) ( 15 ) , val_low ) ;
        D ( FFF ) . H = saturation16_shift ( ovf2 , D ( FFF ) ( 31 ) , val_high ) ;
    }
}
DATA
,
"inputs" => [
"D(FFF [parm #0])[mask 0x00ffffffff]/p",
"EMR/p",
"SR[mask 0x00060080]/p?",
],
"outputs" => [
"D(FFF [parm #0])[mask 0x00ffffffff]/p",
"EMR[mask 0x00000004]/p?",
],
"helpers" => [
"IsUserMode",
"overflow_shift_left",
"saturation16_shift",
"set_emr_dovf",
"slice_signExtend16",
],
},
"multiple_bitwise_shift" => {
"action" => <<DATA
 void multiple_bitwise_shift ( bits < 4 > FFF , bits < 7 > shift , bool shift_direction ) {
     bits < 7 > neg_val = ( ~ shift  ) + 1 ;
     bits < 7 > shift_s = ( shift ( 6 ) == 0 ) ? shift : ( neg_val ) ;
     bits < 6 > abs_shift = ( zeroExtend ( shift_s , 8 ) . signedGT ( 40 ) ) ? 40 : shift_s ;
     bool shift_left = ( ( shift_direction && ( shift ( 6 ) == 0 ) ) || ( ! shift_direction && ( shift ( 6 ) == 1 ) ) ) ;
    if ( ! shift_left ) {
        D ( FFF ) = D ( FFF ) >> abs_shift ;
        SR . C = ( abs_shift == 0 ) ? 0 : D ( FFF ) ( abs_shift - 1 ) ;
    } else {
        D ( FFF ) = D ( FFF ) << abs_shift ;
        SR . C = ( abs_shift == 0 ) ? 0 : D ( FFF ) ( 40 - abs_shift ) ;
    }
}
DATA
,
"inputs" => [
"D(FFF [parm #0])",
"SR/p",
],
"outputs" => [
"D(FFF [parm #0])",
"SR[mask 0x00000001]/p",
],
},
"overflow40" => {
"action" => <<DATA
 void overflow40 ( bits < 41 > reg ) {
    if ( ( SR . SM == 0 ) && ( signExtend ( reg . get < 40 > ( 39 , 0 ) , 41 ) != reg ) ) {
        if ( EMR . DOVF == 0 && SR . OVE == 1 ) {
            raiseException ( Overflow ) ;
        }
        exception_condition = true ;
        EMR . DOVF = 1 ;
    }
}
DATA
,
"inputs" => [
"EMR/p?",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000004]/p?",
],
"helpers" => [
"IsUserMode",
],
"raises_exception" => 1,
},
"overflow_shift_left" => {
"action" => <<DATA
 bool overflow_shift_left ( bits < 40 > Dval , bits < 6 > shift ) {
    if ( shift == 0 ) return false ;
     bool overflow = false ;
     bits < 40 > mask = 0xffffffffffull ;
    mask >>= ( 40 - shift ) ;
    mask <<= ( 40 - shift ) ;
     bits < 40 > check_mask = mask & Dval ;
    if ( check_mask == 0 ) {
        if ( Dval ( 39 - shift ) == 1 ) overflow = true ;
    } else {
        if ( check_mask == mask ) {
            if ( Dval ( 39 - shift ) == 0 ) overflow = true ;
        } else overflow = true ;
    }
    return overflow ;
}
DATA
,
},
"preshift" => {
"action" => <<DATA
 bits < 32 > preshift ( bits < 3 > ea_increase ) {
     bits < 32 > increase ;
    switch ( ea_increase . uint32 (  ) ) {
        case 0 : increase = N ( 0 ) ;
        break ;
        case 1 : increase = 0xFFFFFFFF ;
        break ;
        case 2 : increase = 0x00000000 ;
        break ;
        case 3 : increase = 0x00000001 ;
        break ;
        case 4 : increase = N ( 0 ) ;
        break ;
        case 5 : increase = N ( 1 ) ;
        break ;
        case 6 : increase = N ( 2 ) ;
        break ;
        case 7 : increase = N ( 3 ) ;
        break ;
    }
    return ( increase ) ;
}
DATA
,
"inputs" => [
"N(0)?",
"N(1)?",
"N(2)?",
"N(3)?",
],
},
"reset_cond_exec" => {
"action" => <<DATA
 void reset_cond_exec (  ) {
    cond_exec = c0 ;
}
DATA
,
},
"reset_dalu" => {
"action" => <<DATA
 void reset_dalu (  ) {
    dalu0 = true ;
    dalu1 = true ;
    dalu2 = true ;
    dalu3 = true ;
}
DATA
,
},
"reset_dispatch" => {
"action" => <<DATA
 void reset_dispatch (  ) {
    dalu0 = true ;
    dalu1 = true ;
    dalu2 = true ;
    dalu3 = true ;
    agu0 = true ;
    agu1 = true ;
    agu_dispatch = modulo2 ;
}
DATA
,
},
"reset_instr_pos" => {
"action" => <<DATA
 void reset_instr_pos (  ) {
     ;
    instr_pos = instr0 ;
}
DATA
,
},
"reset_prefix_size" => {
"action" => <<DATA
 void reset_prefix_size (  ) {
    prefix_size = pfx0 ;
}
DATA
,
},
"round" => {
"action" => <<DATA
 bits < 40 > round ( bits < 40 > val ) {
     int a ;
     bits < 40 > rval = 0 , rmask , tmp ;
     bits < 40 > sum ;
    exception_overflow = false ;
    if ( ! SR . SM ) {
        a = SR . SCM . uint32 (  ) ;
    } else {
        a = 0 ;
    }
    switch ( a ) {
        case 0 : rval = ( 0x8000 ) ;
        break ;
        case 1 : rval = ( 0x10000 ) ;
        break ;
        case 2 : rval = ( 0x4000 ) ;
        break ;
        case 3 : rval = ( 0x8000 ) ;
        break ;
    }
    rmask = ( rval << 1 ) - 1 ;
    if ( SR . RM != 0 ) {
        tmp = val & rmask ;
        if ( tmp . signedGE ( rval ) ) {
            sum = saturate ( val + ( rval << 1 ) ) ;
        } else {
            sum = saturate ( val ) ;
        }
    } else {
        tmp = val & rmask ;
        if ( tmp . signedGT ( rval ) ) {
            sum = saturate ( val + ( rval << 1 ) ) ;
        } else if ( tmp . signedLT ( rval ) ) {
            sum = saturate ( val ) ;
        } else if ( ( val & ( rval << 1 ) ) != 0 ) {
            sum = saturate ( val + ( rval << 1 ) ) ;
        } else {
            sum = saturate ( val ) ;
        }
    }
    if ( ( sum & ~ rmask ) == ( 0x8000000000ull ) ) {
        exception_overflow = true ;
        EMR . DOVF = 1 ;
    }
    if ( ( exception_overflow == true ) && ( EMR . DOVF == 0 && SR . OVE == 1 ) ) {
        exception_overflow = false ;
        raiseException ( Overflow ) ;
    }
    return ( sum & ~ rmask ) ;
}
DATA
,
"inputs" => [
"EMR/p",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000004]?",
],
"helpers" => [
"IsUserMode",
"saturate",
],
"raises_exception" => 1,
},
"sat_moves" => {
"action" => <<DATA
 bits < 40 > sat_moves ( bits < 1 > Lbit , bits < 40 > val ) {
    if ( Lbit == 1 ) {
        if ( val . signedLT ( 0 ) ) {
            val = 0x0080000000ull ;
        } else if ( val . signedGT ( 0 ) ) {
            val = 0x007fffffffull ;
        }
    }
    return ( val ) ;
}
DATA
,
},
"saturate" => {
"action" => <<DATA
 bits < 40 > saturate ( bits < 40 > val ) {
     bits < 40 > temp = val ;
     bits < 40 > neg_val = ( 0xFF80000000ull ) ;
     bits < 40 > pos_val = ( 0x007fffffffull ) ;
    if ( SR . SM != 0 ) {
        if ( val . signedLT ( neg_val ) ) {
            temp = neg_val ;
            exception_overflow = true ;
            EMR . DOVF = 1 ;
        } else if ( val . signedGT ( pos_val ) ) {
            temp = pos_val ;
            exception_overflow = true ;
            EMR . DOVF = 1 ;
        }
    }
    return temp ;
}
DATA
,
"inputs" => [
"EMR/p",
"SR[mask 0x00060004]/p",
],
"outputs" => [
"EMR[mask 0x00000004]/p?",
],
"helpers" => [
"IsUserMode",
],
},
"saturation16" => {
"action" => <<DATA
 bits < 16 > saturation16 ( bits < 40 > data ) {
     bool exception_ovflow = false ;
     bits < 16 > reg = data ( 15 , 0 ) ;
     bits < 40 > neg_sat = 0xFFFFFF8000ull ;
     bits < 40 > pos_sat = 0x0000007FFFull ;
    if ( SR . SM2 != 0 ) {
        if ( data . signedLT ( neg_sat ) ) {
            reg = 0x8000 ;
            exception_ovflow = true ;
        } else if ( data . signedGT ( pos_sat ) ) {
            reg = 0x7FFF ;
            exception_ovflow = true ;
        }
        if ( exception_ovflow ) {
            if ( ( EMR . DOVF == 0 && SR . OVE == 1 ) ) raiseException ( Overflow ) ;
            exception_overflow = true ;
            EMR . DOVF = 1 ;
        }
    } else if ( ( data . signedLT ( neg_sat ) ) || ( data . signedGT ( pos_sat ) ) ) {
        if ( ( EMR . DOVF == 0 && SR . OVE == 1 ) ) raiseException ( Overflow ) ;
        set_emr_dovf (  ) ;
    }
    return reg ;
}
DATA
,
"inputs" => [
"EMR/p?",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000004]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"saturation16_nochk" => {
"action" => <<DATA
 bits < 16 > saturation16_nochk ( bits < 40 > data ) {
     bits < 16 > val = data ( 15 , 0 ) ;
     bits < 40 > neg_sat = 0xFFFFFF8000ull ;
     bits < 40 > pos_sat = 0x0000007FFFull ;
    if ( data . signedLT ( neg_sat ) ) {
        val = 0x8000 ;
        EMR_LOW = true ;
        EMR . DOVF = 1 ;
    } else if ( data . signedGT ( pos_sat ) ) {
        val = 0x7FFF ;
        EMR_LOW = true ;
        EMR . DOVF = 1 ;
    }
    return val ;
}
DATA
,
"inputs" => [
"EMR/p",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000004]/p?",
],
"helpers" => [
"IsUserMode",
],
},
"saturation16_noexception" => {
"action" => <<DATA
 bits < 16 > saturation16_noexception ( bits < 40 > data ) {
     bits < 16 > val = data ( 15 , 0 ) ;
     bits < 40 > neg_sat = 0xFFFFFF8000ull ;
     bits < 40 > pos_sat = 0x0000007FFFull ;
    if ( SR . SM2 != 0 ) {
        val = saturation16_nochk ( data ) ;
    }
    return val ;
}
DATA
,
"inputs" => [
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"EMR[mask 0x00000004]/p?",
],
"helpers" => [
"IsUserMode",
"saturation16_nochk",
],
},
"saturation16_shift" => {
"action" => <<DATA
 bits < 16 > saturation16_shift ( bool ovf , bits < 1 > sat_direction , bits < 32 > dval ) {
     bits < 32 > neg_val = ( 0xFFFF8000 ) ;
     bits < 32 > pos_val = ( 0x00007fff ) ;
     bool saturation = false ;
    if ( ovf ) {
        saturation = true ;
    }
    if ( ( SR . SM2 != 0 ) ) {
        if ( ! ovf ) {
             bits < 32 > shifted = ( 0xFFFF0000 ) ;
            shifted = dval & shifted ;
            if ( ( shifted != 0 ) && ( shifted != 0xFFFF0000 ) ) {
                saturation = true ;
            }
        }
        if ( saturation ) {
            dval = ( sat_direction == 1 ) ? neg_val : pos_val ;
        }
    }
    if ( saturation ) set_emr_dovf (  ) ;
    return dval ( 15 , 0 ) ;
}
DATA
,
"inputs" => [
"EMR/p",
"SR[mask 0x00060080]/p",
],
"outputs" => [
"EMR[mask 0x00000004]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_dovf",
],
},
"saturation20_nochk" => {
"action" => <<DATA
 bits < 20 > saturation20_nochk ( bits < 40 > data ) {
     bits < 20 > val = data ( 19 , 0 ) ;
     bits < 40 > neg_sat = 0xFFFFFF8000ull ;
     bits < 40 > pos_sat = 0x0000007FFFull ;
    if ( data . signedLT ( neg_sat ) ) {
        val = 0xf8000 ;
        exception_overflow = true ;
        EMR . DOVF = 1 ;
    } else if ( data . signedGT ( pos_sat ) ) {
        val = 0x07FFF ;
        exception_overflow = true ;
        EMR . DOVF = 1 ;
    }
    return val ;
}
DATA
,
"inputs" => [
"EMR/p",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000004]/p?",
],
"helpers" => [
"IsUserMode",
],
},
"saturation32" => {
"action" => <<DATA
 void saturation32 ( bits < 41 > data , bits < 4 > des ) {
    if ( SR . SM != 0 ) {
        saturation32_nochk ( data , des ) ;
    } else {
        D ( des ) = data ( 39 , 0 ) ;
    }
}
DATA
,
"inputs" => [
"EMR/p?",
"SR/p?",
],
"outputs" => [
"D(des [parm #1])?",
"EMR[mask 0x00000004]/p?",
],
"helpers" => [
"IsUserMode",
"saturation32_nochk",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"saturation32_nochk" => {
"action" => <<DATA
 void saturation32_nochk ( bits < 41 > data , bits < 4 > des ) {
     bool excep = false ;
    if ( ( data ( 40 ) == 0 ) && ( data ( 39 ) == 1 ) ) {
        D ( des ) = 0x007FFFFFFFull ;
        excep = true ;
    } else if ( ( data ( 40 ) == 1 ) && ( data ( 39 ) == 0 ) ) {
        D ( des ) = 0xFF80000000ull ;
        excep = true ;
    } else if ( ( data ( 39 ) == 1 ) && ( data ( 39 , 32 ) != 0xff ) ) {
        D ( des ) = 0xFF80000000ull ;
        excep = true ;
    } else if ( ( data ( 39 ) == 1 ) && ( data ( 31 ) == 0 ) ) {
        D ( des ) = 0xFF80000000ull ;
        excep = true ;
    } else if ( ( data ( 39 ) == 0 ) && ( data ( 31 ) == 1 ) ) {
        D ( des ) = 0x007FFFFFFFull ;
        excep = true ;
    } else if ( data . signedGT ( 0x0007FFFFFFFull ) ) {
        D ( des ) = 0x007FFFFFFFull ;
        excep = true ;
    }
    if ( excep ) {
         bool dovf = ( EMR . DOVF == 1 ) ? true : false ;
        set_emr_dovf (  ) ;
        if ( ( ! dovf ) && ( SR . OVE == 1 ) ) raiseException ( Overflow ) ;
    } else {
        D ( des ) = data ( 39 , 0 ) ;
    }
}
DATA
,
"inputs" => [
"EMR/p?",
"SR/p?",
],
"outputs" => [
"D(des [parm #1])?",
"EMR[mask 0x00000004]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_dovf",
],
"raises_exception" => 1,
},
"saturation32_nochk_noovf" => {
"action" => <<DATA
 void saturation32_nochk_noovf ( bits < 41 > data , bits < 4 > des ) {
     bits < 40 > tmp = data ( 39 , 0 ) ;
    if ( ( data ( 40 ) == 0 ) && ( data ( 39 ) == 1 ) ) {
        tmp = 0x007FFFFFFFull ;
    } else if ( ( data ( 40 ) == 1 ) && ( data ( 39 ) == 0 ) ) {
        tmp = 0xFF80000000ull ;
    } else if ( ( data ( 39 ) == 1 ) && ( data ( 39 , 32 ) != 0xff ) ) {
        tmp = 0xFF80000000ull ;
    } else if ( ( data ( 39 ) == 1 ) && ( data ( 31 ) == 0 ) ) {
        tmp = 0xFF80000000ull ;
    } else if ( ( data ( 39 ) == 0 ) && ( data ( 31 ) == 1 ) ) {
        tmp = 0x007FFFFFFFull ;
    }
    D ( des ) = tmp ;
}
DATA
,
"outputs" => [
"D(des [parm #1])",
],
},
"saturation32_noovf" => {
"action" => <<DATA
 void saturation32_noovf ( bits < 41 > data , bits < 4 > des ) {
    if ( SR . SM != 0 ) {
        saturation32_nochk_noovf ( data , des ) ;
    } else {
        D ( des ) = data ( 39 , 0 ) ;
    }
}
DATA
,
"inputs" => [
"SR[mask 0x00000004]/p",
],
"outputs" => [
"D(des [parm #1])",
],
"helpers" => [
"saturation32_nochk_noovf",
],
},
"saturation32_shift" => {
"action" => <<DATA
 bits < 40 > saturation32_shift ( bool ovf , bits < 1 > sat_direction , bits < 40 > dval ) {
     bits < 40 > neg_val = ( 0xFF80000000ull ) ;
     bits < 40 > pos_val = ( 0x007fffffffull ) ;
     bool saturation = false ;
    if ( ovf ) {
        saturation = true ;
    }
    if ( ( SR . SM != 0 ) ) {
        if ( ! ovf ) {
            info ( 1 , "saturation32_shift, dval: " << hex << dval ) ;
            if ( dval . signedGT ( pos_val ) || dval . signedLT ( neg_val ) ) saturation = true ;
        }
        if ( saturation ) {
            dval = ( sat_direction == 1 ) ? neg_val : pos_val ;
        }
    }
    if ( saturation ) set_emr_dovf (  ) ;
    return dval ;
}
DATA
,
"inputs" => [
"EMR/p",
"SR[mask 0x00060004]/p",
],
"outputs" => [
"EMR[mask 0x00000004]/p?",
],
"helpers" => [
"IsUserMode",
"set_emr_dovf",
],
},
"saturation8_nochk" => {
"action" => <<DATA
 bits < 40 > saturation8_nochk ( bits < 40 > data ) {
    if ( data . signedGT ( 0xff ) ) {
        exception_overflow = true ;
        EMR . DOVF = 1 ;
        return 0x00000000ffull ;
    } else return data ;
}
DATA
,
"inputs" => [
"EMR/p",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000004]/p?",
],
"helpers" => [
"IsUserMode",
],
},
"scale" => {
"action" => <<DATA
 bits < 40 > scale ( bits < 4 > dnum ) {
     bits < 40 > dval = D ( dnum ) ;
     int sm = ( ( SR . SM == 1 ) ? 0 : ( SR . SCM ) . uint32 (  ) ) ;
     bits < 32 > scaled = 0 ;
    switch ( sm ) {
        case 0 : scaled = ( ( ( dval & 0x40000000 ) >> 30 ) ^ ( ( dval & 0x20000000 ) >> 29 ) ) ;
        break ;
        case 1 : scaled = ( ( ( dval & 0x80000000 ) >> 31 ) ^ ( ( dval & 0x40000000 ) >> 30 ) ) ;
        dval = dval >> 1 ;
        break ;
        case 2 : scaled = ( ( ( dval & 0x20000000 ) >> 29 ) ^ ( ( dval & 0x10000000 ) >> 28 ) ) ;
        dval = dval << 1 ;
        break ;
        case 3 : scaled = ( ( ( dval & 0x40000000 ) >> 30 ) ^ ( ( dval & 0x20000000 ) >> 29 ) ) ;
        break ;
        default : break ;
    }
    if ( ( SR . SM == 1 ) ) {
        scaled = ( ( ( dval & 0x40000000 ) >> 30 ) ^ ( ( dval & 0x20000000 ) >> 29 ) ) ;
    }
    if ( ( scaled != 0 ) && ( L ( dnum ) == 0 ) ) {
        SR . S = 1 ;
    }
    return ( dval ) ;
}
DATA
,
"inputs" => [
"D(dnum [parm #0])",
"L(dnum [parm #0])",
"SR[mask 0x00020034]/p",
],
"outputs" => [
"SR[mask 0x00000040]/p?",
],
},
"set_SR_LF" => {
"action" => <<DATA
 void set_SR_LF ( bits < 2 > flag ) {
    switch ( flag . uint32 (  ) ) {
        case 0 : SR . LF0 = 1 ;
        break ;
        case 1 : SR . LF1 = 1 ;
        break ;
        case 2 : SR . LF2 = 1 ;
        break ;
        case 3 : SR . LF3 = 1 ;
        break ;
        default : break ;
    }
}
DATA
,
"inputs" => [
"SR/p",
],
"outputs" => [
"SR[mask 0x78000000]/p?",
],
},
"set_emr_dovf" => {
"action" => <<DATA
 void set_emr_dovf (  ) {
    EMR_LOW = true ;
    EMR . DOVF = 1 ;
}
DATA
,
"inputs" => [
"EMR/p",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000004]/p",
],
"helpers" => [
"IsUserMode",
],
},
"set_emr_ilpr" => {
"action" => <<DATA
 void set_emr_ilpr (  ) {
    EMR_LOW = true ;
    EMR . ILPR = 1 ;
}
DATA
,
"inputs" => [
"EMR/p",
"SR/p",
],
"outputs" => [
"EMR[mask 0x00000010]/p",
],
"helpers" => [
"IsUserMode",
],
},
"set_ignore_lpmarks" => {
"action" => <<DATA
 void set_ignore_lpmarks (  ) {
    if ( ignore_lpmarkA == a_double_delay_slot ) {
        ignore_lpmarkA = a_delay_slot ;
        LpmarkA = false ;
    } else {
        if ( ignore_lpmarkA == a_delay_slot ) {
            ignore_lpmarkA = a_not_ignore ;
            LpmarkA = false ;
        }
    }
    if ( ignore_lpmarkB == b_double_delay_slot ) {
        ignore_lpmarkB = b_delay_slot ;
        LpmarkB = false ;
    } else {
        if ( ignore_lpmarkB == b_delay_slot ) {
            ignore_lpmarkB = b_not_ignore ;
            LpmarkB = false ;
        }
    }
}
DATA
,
},
"set_lpmarks" => {
"action" => <<DATA
 void set_lpmarks (  ) {
    if ( lpmarkA == 1 ) {
        LpmarkA = true ;
    } else {
        LpmarkA = false ;
    }
    if ( lpmarkB == 1 ) {
        LpmarkB = true ;
    } else {
        LpmarkB = false ;
    }
}
DATA
,
},
"setup_dispatch" => {
"action" => <<DATA
 void setup_dispatch ( uint32_t pc ) {
     int i = get_prefix_size (  ) ;
     bits < 6 > esg_bits = 0 ;
     bits < 16 > esg_word = PMEM ( pc + ( 2 * i ) , 2 ) ;
    if ( is_esg ( pc + ( 2 * i ) ) ) {
        i ++ ;
        esg_bits = esg_word ( 5 , 0 ) ;
    }
     int prefix_size = i ;
    info ( 1 , "setup_dispatch, beginning, i= " << i ) ;
     uint32_t vles_size = ( VlesSize == 0 ) ? 9 : VlesSize + 1 ;
     int first_agu = - 1 ;
    word4_dalu = non_dalu ;
    while ( i < ( int  ) vles_size ) {
         unsigned instr_size = calc_instr_size ( pc + 2 * i ) ;
         unsigned instr_esg = get_esg_relevant_instruction_bit ( esg_bits , i , prefix_size ) ;
        info ( 1 , "while: i= " << i , ", instr_size: " << instr_size , ", instr_esg" << instr_esg ) ;
         bits < 16 > check_nop = PMEM ( pc + 2 * i , 2 ) ;
        if ( is_dalu ( pc + 2 * i , instr_size , instr_esg ) ) {
            info ( 1 , " if is_dalu=true , i= " << i ) ;
            if ( i == 8 ) {
                info ( 1 , "setup_dispatch, i==8 " ) ;
                dalu0 = false ;
                if ( word4_dalu == is_adalu ) {
                     int j = get_prefix_size (  ) ;
                     bits < 6 > esg_bits2 = 0 ;
                     bits < 16 > esg_word2 = PMEM ( pc + ( 2 * i ) , 2 ) ;
                    if ( is_esg ( pc + ( 2 * j ) ) ) {
                        j ++ ;
                        esg_bits2 = esg_word ( 5 , 0 ) ;
                    }
                    while ( ( j < ( int  ) vles_size ) && ( j < 8 ) ) {
                         unsigned instr_size_contention = calc_instr_size ( pc + 2 * j ) ;
                         unsigned instr_esg2 = get_esg_relevant_instruction_bit ( esg_bits2 , j , prefix_size ) ;
                        if ( ( j != 4 ) && ( is_dalu ( pc + 2 * j , instr_size_contention , instr_esg2 ) ) ) {
                            if ( j < 4 ) {
                                get_dalu_dispatch ( j ) ;
                            } else {
                                if ( j > 4 && j < 8 ) {
                                    switch ( j ) {
                                        case 5 : if ( dalu1 ) {
                                            dalu1 = false ;
                                        }
                                        break ;
                                        case 6 : if ( dalu2 ) {
                                            dalu2 = false ;
                                        }
                                        break ;
                                        case 7 : if ( dalu3 ) {
                                            dalu3 = false ;
                                        }
                                        break ;
                                        default : error ( 1 , "setup_dipatch function, not supposed to get here" ) ;
                                    }
                                } else {
                                }
                            }
                        }
                        j += instr_size_contention ;
                    }
                     unsigned available_dalu_word4 = first_available_dalu (  ) ;
                    switch ( available_dalu_word4 ) {
                        case 1 : word4_dalu = word4_dalu1 ;
                        break ;
                        case 2 : word4_dalu = word4_dalu2 ;
                        break ;
                        case 3 : word4_dalu = word4_dalu3 ;
                        break ;
                        default : {
                            error ( 1 , "!!!setup_dipatch function, dalu_word4 contention with word8 not supposed to get here" ) ;
                        }
                    }
                    info ( 1 , "setup_dispatch, word4_dalu= " << word4_dalu ) ;
                    reset_dalu (  ) ;
                    dalu0 = false ;
                }
            }
            if ( i == 4 ) {
                word4_dalu = is_adalu ;
                info ( 1 , "setup_dispatch, i==4, word4_dalu= " << word4_dalu ) ;
            }
        } else if ( ( instr_size == 1 ) && ( check_nop == 0x90c0 ) ) {
        } else {
            if ( first_agu > 0 ) {
                if ( i == 8 ) {
                    agu0 = false ;
                }
                if ( ( first_agu % 2 ) == ( i % 2 ) ) {
                    agu_dispatch = modulo4 ;
                } else {
                    agu_dispatch = modulo2 ;
                }
            } else {
                first_agu = i ;
            }
        }
        i += instr_size ;
    }
}
DATA
,
"input_mems" => [
"PMEM",
],
"helpers" => [
"calc_instr_size",
"end_of_set",
"first_available_dalu",
"get_dalu_dispatch",
"get_esg_relevant_instruction_bit",
"get_prefix_size",
"is_dalu",
"is_esg",
"reset_dalu",
"type1_end_of_set",
],
},
"slice_signExtend" => {
"action" => <<DATA
 bits < 40 > slice_signExtend ( bits < 40 > b , bits < 6 > start ) {
     bits < 40 > mask ( 0xffffffffffull ) ;
    mask >>= start ;
    mask <<= start ;
    if ( b ( start ) == 1 ) {
        return b | mask ;
    } else {
        return b & ( ~ mask ) ;
    }
}
DATA
,
},
"slice_signExtend16" => {
"action" => <<DATA
 bits < 16 > slice_signExtend16 ( bits < 16 > b , bits < 4 > start ) {
     bits < 16 > mask ( 0xffff ) ;
    mask >>= start ;
    mask <<= start ;
    if ( b ( start ) == 1 ) {
        return b | mask ;
    } else {
        return b & ( ~ mask ) ;
    }
}
DATA
,
},
"slice_write_dalu" => {
"action" => <<DATA
 bits < 40 > slice_write_dalu ( bits < 40 > des , bits < 40 > dsrc , bits < 6 > end , bits < 6 > start ) {
     bits < 40 > mask = 0xffffffffffull ;
    mask <<= 40 - ( end - start ) ;
    mask >>= 40 - ( end ) ;
    mask = ~ mask ;
    dsrc <<= 40 - ( end - start ) ;
    dsrc >>= 40 - ( end ) ;
    return ( ( des & mask ) | dsrc ) ;
}
DATA
,
},
"trap_f" => {
"action" => <<DATA
 void trap_f (  ) {
    SR . EXP = 1 ;
    SR . PE = 0 ;
    SR . C = 0 ;
    SR . T = 0 ;
    SR . S = 0 ;
    SR . SLF = 0 ;
    SR . LF = 0 ;
    SR . I = 0x7 ;
}
DATA
,
"inputs" => [
"SR/p",
],
"outputs" => [
"SR[mask 0xfbe60043]/p",
],
},
"type1_end_of_set" => {
"action" => <<DATA
 bool type1_end_of_set ( bits < 16 > w0 , bits < 16 > w1 , bits < 16 > w2 ) {
     int eos = end_of_set ( w0 , w1 , w2 ) ;
    if ( eos < 0 ) {
        error ( 1 , "Problem in normal_end_of_set" ) ;
        return 0 ;
    } else {
        switch ( eos ) {
            case 0 : return ( w0 ( 15 ) == 0 ) ;
            case 1 : return ( w1 ( 15 ) == 0 ) ;
            case 2 : return ( w2 ( 15 ) == 0 ) ;
            default : error ( 1 , "Problem in normal_end_of_set" ) ;
            return 0 ;
        }
    }
}
DATA
,
"helpers" => [
"end_of_set",
],
},
"update_prefix_size" => {
"action" => <<DATA
 void update_prefix_size ( int i ) {
    switch ( i ) {
        case 2 : incr_prefix_size (  ) ;
        incr_prefix_size (  ) ;
        break ;
        case 1 : incr_prefix_size (  ) ;
        break ;
        case - 1 : decr_prefix_size (  ) ;
        break ;
        case - 2 : decr_prefix_size (  ) ;
        decr_prefix_size (  ) ;
        break ;
        default : error ( 1 , "cannot update prefix_size" ) ;
        break ;
    }
}
DATA
,
"helpers" => [
"decr_prefix_size",
"incr_prefix_size",
],
},
"vtrace_index" => {
"action" => <<DATA
 bits < 5 > vtrace_index ( bits < 32 > dval , int m , int n ) {
     int shift = 31 - ( m + n ) ;
    dval >>= shift ;
    return ( dval ( 4 , 0 ) ) ;
}
DATA
,
},
"write_C4Table_2mem" => {
"action" => <<DATA
 void write_C4Table_2mem ( bits < 32 > addr , bits < 6 > src , int width ) {
    switch ( width ) {
        case 1 : DMEM ( addr , 1 ) = C4Table ( src ) ( 7 , 0 ) ;
        break ;
        case 2 : DMEM ( addr , 2 ) = C4Table ( src ) ( 15 , 0 ) ;
        break ;
        case 4 : DMEM ( addr , 4 ) = C4Table ( src ) ( 31 , 0 ) ;
        break ;
        default : break ;
    }
}
DATA
,
"inputs" => [
"C4Table(src [parm #1])[mask 0x00ffffffff]/p?",
],
"output_mems" => [
"DMEM",
],
},
"write_DRTable_2mem" => {
"action" => <<DATA
 void write_DRTable_2mem ( bits < 32 > addr , bits < 5 > src , int width ) {
    switch ( width ) {
        case 1 : DMEM ( addr , 1 ) = DRTable ( src ) ( 7 , 0 ) ;
        break ;
        case 2 : DMEM ( addr , 2 ) = DRTable ( src ) ( 15 , 0 ) ;
        break ;
        case 4 : DMEM ( addr , 4 ) = DRTable ( src ) ( 31 , 0 ) ;
        break ;
        default : break ;
    }
}
DATA
,
"inputs" => [
"DRTable(src [parm #1])[mask 0x00ffffffff]/p?",
],
"output_mems" => [
"DMEM",
],
},
"write_mem_2C4Table" => {
"action" => <<DATA
 void write_mem_2C4Table ( bits < 32 > addr , bits < 6 > des , int width ) {
    switch ( width ) {
        case 1 : if ( ( des ( 0 , 0 ) == 0 ) && ( des ( 4 , 4 ) == 0 ) ) {
            C4Table ( des ) = signExtend ( DMEM ( addr , 1 ) , 40 ) ;
            L ( concat ( des ( 5 , 5 ) , des ( 3 , 1 ) ) ) = 0 ;
        } else {
            C4Table ( des ) = signExtend ( DMEM ( addr , 1 ) , 32 ) ;
        }
        break ;
        case 2 : if ( ( des ( 0 , 0 ) == 0 ) && ( des ( 4 , 4 ) == 0 ) ) {
            C4Table ( des ) = signExtend ( DMEM ( addr , 2 ) , 40 ) ;
            L ( concat ( des ( 5 , 5 ) , des ( 3 , 1 ) ) ) = 0 ;
        } else {
            C4Table ( des ) = signExtend ( DMEM ( addr , 2 ) , 32 ) ;
        }
        break ;
        case 4 : if ( ( des ( 0 , 0 ) == 0 ) && ( des ( 4 , 4 ) == 0 ) ) {
            C4Table ( des ) = signExtend ( DMEM ( addr , 4 ) , 40 ) ;
            L ( concat ( des ( 5 , 5 ) , des ( 3 , 1 ) ) ) = 0 ;
        } else {
            C4Table ( des ) = signExtend ( DMEM ( addr , 4 ) , 32 ) ;
        }
        break ;
        default : break ;
    }
}
DATA
,
"outputs" => [
"C4Table(des [parm #1])?",
"L?",
],
"input_mems" => [
"DMEM",
],
},
"write_mem_2C4Table_zeroextend" => {
"action" => <<DATA
 void write_mem_2C4Table_zeroextend ( bits < 32 > addr , bits < 6 > des , int width ) {
    switch ( width ) {
        case 1 : if ( ( des ( 0 , 0 ) == 0 ) && ( des ( 4 , 4 ) == 0 ) ) {
            C4Table ( des ) = zeroExtend ( DMEM ( addr , 1 ) , 40 ) ;
            L ( concat ( des ( 5 , 5 ) , des ( 3 , 1 ) ) ) = 0 ;
        } else {
            C4Table ( des ) = zeroExtend ( DMEM ( addr , 1 ) , 32 ) ;
        }
        break ;
        case 2 : if ( ( des ( 0 , 0 ) == 0 ) && ( des ( 4 , 4 ) == 0 ) ) {
            C4Table ( des ) = zeroExtend ( DMEM ( addr , 2 ) , 40 ) ;
            L ( concat ( des ( 5 , 5 ) , des ( 3 , 1 ) ) ) = 0 ;
        } else {
            C4Table ( des ) = zeroExtend ( DMEM ( addr , 2 ) , 32 ) ;
        }
        break ;
        case 4 : if ( ( des ( 0 , 0 ) == 0 ) && ( des ( 4 , 4 ) == 0 ) ) {
            C4Table ( des ) = zeroExtend ( DMEM ( addr , 4 ) , 40 ) ;
            L ( concat ( des ( 5 , 5 ) , des ( 3 , 1 ) ) ) = 0 ;
        } else {
            C4Table ( des ) = zeroExtend ( DMEM ( addr , 4 ) , 32 ) ;
        }
        break ;
        default : break ;
    }
}
DATA
,
"outputs" => [
"C4Table(des [parm #1])?",
"L?",
],
"input_mems" => [
"DMEM",
],
},
"write_mem_2DRTable" => {
"action" => <<DATA
 void write_mem_2DRTable ( bits < 32 > addr , bits < 5 > des , int width ) {
    switch ( width ) {
        case 1 : if ( des ( 3 ) == 0 ) {
            DRTable ( des ) = signExtend ( DMEM ( addr , 1 ) , 40 ) ;
            L ( concat ( des ( 4 , 4 ) , des ( 2 , 0 ) ) ) = 0 ;
        } else {
            DRTable ( des ) = signExtend ( DMEM ( addr , 1 ) , 32 ) ;
        }
        break ;
        case 2 : if ( des ( 3 ) == 0 ) {
            DRTable ( des ) = signExtend ( DMEM ( addr , 2 ) , 40 ) ;
            L ( concat ( des ( 4 , 4 ) , des ( 2 , 0 ) ) ) = 0 ;
        } else {
            DRTable ( des ) = signExtend ( DMEM ( addr , 2 ) , 32 ) ;
        }
        break ;
        case 4 : if ( des ( 3 ) == 0 ) {
            DRTable ( des ) = signExtend ( DMEM ( addr , 4 ) , 40 ) ;
            L ( concat ( des ( 4 , 4 ) , des ( 2 , 0 ) ) ) = 0 ;
        } else {
            DRTable ( des ) = signExtend ( DMEM ( addr , 4 ) , 32 ) ;
        }
        break ;
        default : break ;
    }
}
DATA
,
"outputs" => [
"DRTable(des [parm #1])?",
"L?",
],
"input_mems" => [
"DMEM",
],
},
"write_mem_2DRTable_zeroextend" => {
"action" => <<DATA
 void write_mem_2DRTable_zeroextend ( bits < 32 > addr , bits < 5 > des , int width ) {
    switch ( width ) {
        case 1 : if ( des ( 3 ) == 0 ) {
            DRTable ( des ) = zeroExtend ( DMEM ( addr , 1 ) , 40 ) ;
            L ( concat ( des ( 4 , 4 ) , des ( 2 , 0 ) ) ) = 0 ;
        } else {
            DRTable ( des ) = zeroExtend ( DMEM ( addr , 1 ) , 32 ) ;
        }
        break ;
        case 2 : if ( des ( 3 ) == 0 ) {
            DRTable ( des ) = zeroExtend ( DMEM ( addr , 2 ) , 40 ) ;
            L ( concat ( des ( 4 , 4 ) , des ( 2 , 0 ) ) ) = 0 ;
        } else {
            DRTable ( des ) = zeroExtend ( DMEM ( addr , 2 ) , 32 ) ;
        }
        break ;
        case 4 : if ( des ( 3 ) == 0 ) {
            DRTable ( des ) = zeroExtend ( DMEM ( addr , 4 ) , 40 ) ;
            L ( concat ( des ( 4 , 4 ) , des ( 2 , 0 ) ) ) = 0 ;
        } else {
            DRTable ( des ) = zeroExtend ( DMEM ( addr , 4 ) , 32 ) ;
        }
        break ;
        default : break ;
    }
}
DATA
,
"outputs" => [
"DRTable(des [parm #1])?",
"L?",
],
"input_mems" => [
"DMEM",
],
},
},
},
},
"systems" => {
},
};

