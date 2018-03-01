	.file	"addr1.c"
	.gnu_attribute 4, 1
	.gnu_attribute 8, 1
	.gnu_attribute 12, 1
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	".text"
.Ltext0:
	.cfi_sections	.debug_frame
	.globl a
	.section	".sbss","aw",@nobits
	.align 2
a:
	.zero	4
	.size	a, 4
	.type	a, @object
	.globl b
	.align 2
b:
	.zero	4
	.size	b, 4
	.type	b, @object
	.section	".text"
	.align 2
	.globl main
	.type	main, @function
main:
.LFB0:
	.file 1 "../../tests/designs/addr1.c"
	.loc 1 7 0
	.cfi_startproc
	stwu 1,-16(1)
.LCFI0:
	.cfi_def_cfa_offset 16
	stw 31,12(1)
	mr 31,1
	.cfi_offset 31, -4
.LCFI1:
	.cfi_def_cfa_register 31
	.loc 1 8 0
	lis 0,a@ha
	li 9,1
	mr 11,0
	stw 9,a@l(11)
	.loc 1 9 0
	lis 0,b@ha
	li 9,2
	mr 11,0
	stw 9,b@l(11)
	.loc 1 10 0
	li 0,0
	.loc 1 11 0
	mr 3,0
	addi 11,31,16
	lwz 31,-4(11)
.LCFI2:
	.cfi_def_cfa 11, 0
	mr 1,11
.LCFI3:
	.cfi_def_cfa_register 1
	.cfi_restore 31
	blr
	.cfi_endproc
.LFE0:
	.size	main, .-main
.Letext0:
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST0:
	.4byte	.LFB0-.Ltext0
	.4byte	.LCFI0-.Ltext0
	.2byte	0x2
	.byte	0x71
	.sleb128 0
	.4byte	.LCFI0-.Ltext0
	.4byte	.LCFI1-.Ltext0
	.2byte	0x2
	.byte	0x71
	.sleb128 16
	.4byte	.LCFI1-.Ltext0
	.4byte	.LCFI2-.Ltext0
	.2byte	0x2
	.byte	0x8f
	.sleb128 16
	.4byte	.LCFI2-.Ltext0
	.4byte	.LCFI3-.Ltext0
	.2byte	0x2
	.byte	0x7b
	.sleb128 0
	.4byte	.LCFI3-.Ltext0
	.4byte	.LFE0-.Ltext0
	.2byte	0x2
	.byte	0x71
	.sleb128 0
	.4byte	0x0
	.4byte	0x0
	.section	.debug_info
	.4byte	0x77
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF0
	.byte	0x1
	.4byte	.LASF1
	.4byte	.LASF2
	.4byte	.Ltext0
	.4byte	.Letext0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.4byte	.LASF3
	.byte	0x1
	.byte	0x7
	.4byte	0x3d
	.4byte	.LFB0
	.4byte	.LFE0
	.4byte	.LLST0
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x4
	.string	"a"
	.byte	0x1
	.byte	0x5
	.4byte	0x3d
	.byte	0x1
	.byte	0x1
	.uleb128 0x4
	.string	"b"
	.byte	0x1
	.byte	0x5
	.4byte	0x3d
	.byte	0x1
	.byte	0x1
	.uleb128 0x5
	.string	"a"
	.byte	0x1
	.byte	0x5
	.4byte	0x3d
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	a
	.uleb128 0x5
	.string	"b"
	.byte	0x1
	.byte	0x5
	.4byte	0x3d
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	b
	.byte	0x0
	.section	.debug_abbrev
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
	.uleb128 0x2e
	.byte	0x0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",@progbits
	.4byte	0x23
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x7b
	.4byte	0x25
	.string	"main"
	.4byte	0x5a
	.string	"a"
	.4byte	0x6a
	.string	"b"
	.4byte	0x0
	.section	.debug_aranges,"",@progbits
	.4byte	0x1c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0x0
	.2byte	0x0
	.2byte	0x0
	.4byte	.Ltext0
	.4byte	.Letext0-.Ltext0
	.4byte	0x0
	.4byte	0x0
	.section	.debug_str,"MS",@progbits,1
.LASF2:
	.string	"/proj/ppctools/adl/a3/adl/tests/designs"
.LASF1:
	.string	"../../tests/designs/addr1.c"
.LASF0:
	.string	"GNU C 4.5.1"
.LASF3:
	.string	"main"
	.ident	"GCC: (Sourcery CodeBench Lite 2011.09-105) 4.5.1"
