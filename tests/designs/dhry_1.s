	.file	"dhry_1.c"
	.globl Reg
	.section	".sbss","aw",@nobits
	.align 2
Reg:
	.zero	4
	.size	Reg, 4
	.type	Reg, @object
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align 2
.LC0:
	.string	"DHRYSTONE PROGRAM, SOME STRING"
	.align 2
.LC1:
	.string	"DHRYSTONE PROGRAM, 1'ST STRING"
	.align 2
.LC2:
	.string	"DHRYSTONE PROGRAM, 2'ND STRING"
	.align 2
.LC3:
	.string	"DHRYSTONE PROGRAM, 3'RD STRING"
	.section	".text"
	.align 2
	.globl main
	.type	main, @function
main:
	stwu 1,-240(1)
	mflr 0
	stw 23,204(1)
	stw 24,208(1)
	stw 25,212(1)
	stw 26,216(1)
	stw 27,220(1)
	stw 28,224(1)
	stw 29,228(1)
	stw 30,232(1)
	stw 31,236(1)
	stw 0,244(1)
	addi 11,1,72
	lis 9,Next_ptr_Glob@ha
	stw 11,Next_Ptr_Glob@l(9)
	addi 0,1,120
	lis 9,Ptr_Glob@ha
	stw 0,Ptr_Glob@l(9)
	stw 11,120(1)
	li 0,0
	stw 0,124(1)
	li 0,2
	stw 0,128(1)
	li 0,40
	stw 0,132(1)
	lis 11,.LC0@ha
	la 9,.LC0@l(11)
	lwz 0,.LC0@l(11)
	lwz 11,4(9)
	lwz 10,8(9)
	lwz 8,12(9)
	stw 0,136(1)
	stw 11,140(1)
	stw 10,144(1)
	stw 8,148(1)
	lwz 0,16(9)
	lwz 11,20(9)
	lwz 10,24(9)
	lhz 8,28(9)
	stw 0,152(1)
	stw 11,156(1)
	stw 10,160(1)
	sth 8,164(1)
	lbz 0,30(9)
	stb 0,166(1)
	lis 11,.LC1@ha
	la 9,.LC1@l(11)
	lwz 0,.LC1@l(11)
	lwz 11,4(9)
	lwz 10,8(9)
	lwz 8,12(9)
	stw 0,8(1)
	stw 11,12(1)
	stw 10,16(1)
	stw 8,20(1)
	lwz 0,16(9)
	lwz 11,20(9)
	lwz 10,24(9)
	lhz 8,28(9)
	stw 0,24(1)
	stw 11,28(1)
	stw 10,32(1)
	sth 8,36(1)
	lbz 0,30(9)
	stb 0,38(1)
	lis 9,Arr_2_Glob+1628@ha
	li 0,10
	stw 0,Arr_2_Glob+1628@l(9)
	li 23,1000
	li 26,1
	lis 9,.LC2@ha
	la 28,.LC2@l(9)
	lwz 25,.LC2@l(9)
	lis 24,Bool_Glob@ha
.L16:
	bl Proc_5
	bl Proc_4
	li 0,2
	stw 0,176(1)
	li 29,3
	lwz 0,4(28)
	lwz 9,8(28)
	lwz 11,12(28)
	stw 25,40(1)
	stw 0,44(1)
	stw 9,48(1)
	stw 11,52(1)
	lwz 0,16(28)
	lwz 9,20(28)
	lwz 11,24(28)
	lhz 10,28(28)
	stw 0,56(1)
	stw 9,60(1)
	stw 11,64(1)
	sth 10,68(1)
	lbz 0,30(28)
	stb 0,70(1)
	li 0,1
	stw 0,172(1)
	addi 3,1,8
	addi 4,1,40
	bl Func_2
	subfic 0,3,0
	adde 3,0,3
	stw 3,Bool_Glob@l(24)
	lwz 0,176(1)
	mr 3,0
	cmpwi 0,0,3
	bge+ 0,.L20
.L9:
	slwi 0,3,2
	add 0,0,3
	subf 0,29,0
	stw 0,168(1)
	mr 4,29
	addi 5,1,168
	bl Proc_7
	lwz 9,176(1)
	addi 9,9,1
	stw 9,176(1)
	mr 3,9
	cmpw 0,9,29
	blt+ 0,.L9
.L20:
	lis 3,Arr_1_Glob@ha
	la 3,Arr_1_Glob@l(3)
	lis 4,Arr_2_Glob@ha
	la 4,Arr_2_Glob@l(4)
	lwz 5,176(1)
	lwz 6,168(1)
	bl Proc_8
	lis 9,Ptr_Glob@ha
	lwz 3,Ptr_Glob@l(9)
	bl Proc_1
	li 31,65
	lis 9,Ch_2_Glob@ha
	lbz 0,Ch_2_Glob@l(9)
	cmplwi 0,0,65
	blt- 0,.L22
	lis 9,.LC3@ha
	la 30,.LC3@l(9)
	lwz 27,.LC3@l(9)
.L15:
	mr 3,31
	li 4,67
	bl Func_1
	lwz 0,172(1)
	cmpw 0,0,3
	bne+ 0,.L12
	li 3,0
	addi 4,1,172
	bl Proc_6
	lwz 0,4(30)
	lwz 9,8(30)
	lwz 11,12(30)
	stw 27,40(1)
	stw 0,44(1)
	stw 9,48(1)
	stw 11,52(1)
	lwz 0,16(30)
	lwz 9,20(30)
	lwz 11,24(30)
	lhz 10,28(30)
	stw 0,56(1)
	stw 9,60(1)
	stw 11,64(1)
	sth 10,68(1)
	lbz 0,30(30)
	stb 0,70(1)
	mr 29,26
	lis 9,Int_Glob@ha
	stw 26,Int_Glob@l(9)
.L12:
	addi 0,31,1
	rlwinm 31,0,0,0xff
	lis 9,Ch_2_Glob@ha
	lbz 0,Ch_2_Glob@l(9)
	cmplw 0,0,31
	bge+ 0,.L15
.L22:
	lwz 0,176(1)
	mullw 29,29,0
	lwz 0,168(1)
	divw 0,29,0
	stw 0,176(1)
	addi 3,1,176
	bl Proc_2
	addi 26,26,1
	cmpw 0,26,23
	ble+ 0,.L16
	lwz 0,244(1)
	mtlr 0
	lwz 23,204(1)
	lwz 24,208(1)
	lwz 25,212(1)
	lwz 26,216(1)
	lwz 27,220(1)
	lwz 28,224(1)
	lwz 29,228(1)
	lwz 30,232(1)
	lwz 31,236(1)
	addi 1,1,240
	blr
	.size	main, .-main
	.align 2
	.globl Proc_1
	.type	Proc_1, @function
Proc_1:
	stwu 1,-32(1)
	mflr 0
	stw 29,20(1)
	stw 30,24(1)
	stw 31,28(1)
	stw 0,36(1)
	mr 30,3
	lwz 31,0(3)
	lis 29,Ptr_Glob@ha
	lwz 4,Ptr_Glob@l(29)
	mr 3,31
	li 5,48
	bl memcpy
	li 0,5
	stw 0,12(30)
	stw 0,12(31)
	lwz 0,0(30)
	stw 0,0(31)
	mr 3,31
	bl Proc_3
	lwz 0,4(31)
	cmpwi 0,0,0
	bne- 0,.L24
	li 0,6
	stw 0,12(31)
	addi 4,31,8
	lwz 3,8(30)
	bl Proc_6
	lwz 9,Ptr_Glob@l(29)
	lwz 0,0(9)
	stw 0,0(31)
	addi 5,31,12
	lwz 3,12(31)
	li 4,10
	bl Proc_7
	b .L25
.L24:
	lwz 4,0(30)
	mr 3,30
	li 5,48
	bl memcpy
.L25:
	lwz 0,36(1)
	mtlr 0
	lwz 29,20(1)
	lwz 30,24(1)
	lwz 31,28(1)
	addi 1,1,32
	blr
	.size	Proc_1, .-Proc_1
	.align 2
	.globl Proc_2
	.type	Proc_2, @function
Proc_2:
	lwz 9,0(3)
	addi 9,9,10
	lis 10,Ch_1_Glob@ha
	lis 8,Int_Glob@ha
.L27:
	lbz 0,Ch_1_Glob@l(10)
	cmpwi 0,0,65
	bne+ 0,.L29
	addi 9,9,-1
	lwz 0,Int_Glob@l(8)
	subf 0,0,9
	stw 0,0(3)
	li 11,0
.L29:
	cmpwi 0,11,0
	bne+ 0,.L27
	blr
	.size	Proc_2, .-Proc_2
	.align 2
	.globl Proc_3
	.type	Proc_3, @function
Proc_3:
	stwu 1,-16(1)
	mflr 0
	stw 0,20(1)
	lis 9,Ptr_Glob@ha
	lwz 9,Ptr_Glob@l(9)
	cmpwi 0,9,0
	beq- 0,.L33
	lwz 0,0(9)
	stw 0,0(3)
.L33:
	lis 9,Ptr_Glob@ha
	lwz 5,Ptr_Glob@l(9)
	addi 5,5,12
	li 3,10
	lis 9,Int_Glob@ha
	lwz 4,Int_Glob@l(9)
	bl Proc_7
	lwz 0,20(1)
	mtlr 0
	addi 1,1,16
	blr
	.size	Proc_3, .-Proc_3
	.align 2
	.globl Proc_4
	.type	Proc_4, @function
Proc_4:
	lis 9,Ch_1_Glob@ha
	lbz 0,Ch_1_Glob@l(9)
	xori 0,0,65
	subfic 9,0,0
	adde 0,9,0
	lis 11,Bool_Glob@ha
	lwz 9,Bool_Glob@l(11)
	or 0,0,9
	stw 0,Bool_Glob@l(11)
	li 0,66
	lis 9,Ch_2_Glob@ha
	stb 0,Ch_2_Glob@l(9)
	blr
	.size	Proc_4, .-Proc_4
	.align 2
	.globl Proc_5
	.type	Proc_5, @function
Proc_5:
	li 0,65
	lis 9,Ch_1_Glob@ha
	stb 0,Ch_1_Glob@l(9)
	li 0,0
	lis 9,Bool_Glob@ha
	stw 0,Bool_Glob@l(9)
	blr
	.size	Proc_5, .-Proc_5
	.align 2
	.globl exit
	.type	exit, @function
exit:
	stwu 1,-16(1)
	mflr 0
	stw 0,20(1)
	.long 0x0
	bl abort
	.size	exit, .-exit
	.comm	Ptr_Glob,4,4
	.comm	Next_Ptr_Glob,4,4
	.comm	Int_Glob,4,4
	.comm	Bool_Glob,4,4
	.comm	Ch_1_Glob,1,1
	.comm	Ch_2_Glob,1,1
	.comm	Arr_1_Glob,200,4
	.comm	Arr_2_Glob,10000,4
	.comm	Begin_Time,4,4
	.comm	End_Time,4,4
	.comm	User_Time,4,4
	.comm	Microseconds,4,4
	.comm	Dhrystones_Per_Second,4,4
	.ident	"GCC: (GNU) 3.3.2"
