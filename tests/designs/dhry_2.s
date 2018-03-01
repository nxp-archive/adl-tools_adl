	.file	"dhry_2.c"
	.section	".text"
	.align 2
	.globl Proc_6
	.type	Proc_6, @function
Proc_6:
	stwu 1,-32(1)
	mflr 0
	stw 30,24(1)
	stw 31,28(1)
	stw 0,36(1)
	mr 31,3
	mr 30,4
	stw 3,0(30)
	bl Func_3
	cmpwi 0,3,0
	bne- 0,.L2
	li 0,3
	stw 0,0(30)
.L2:
	cmplwi 0,31,4
	bgt- 0,.L3
	lis 9,.L11@ha
	la 9,.L11@l(9)
	slwi 0,31,2
	lwzx 0,9,0
	add 0,0,9
	mtctr 0
	bctr
	.section	.rodata
	.align 2
	.align 2
.L11:
	.long .L14-.L11
	.long .L5-.L11
	.long .L8-.L11
	.long .L3-.L11
	.long .L10-.L11
	.section	".text"
.L5:
	lis 9,Int_Glob@ha
	lwz 0,Int_Glob@l(9)
	cmpwi 0,0,100
	ble- 0,.L6
.L14:
	li 0,0
	b .L13
.L6:
	li 0,3
	b .L13
.L8:
	li 0,1
	b .L13
.L10:
	li 0,2
.L13:
	stw 0,0(30)
.L3:
	lwz 0,36(1)
	mtlr 0
	lwz 30,24(1)
	lwz 31,28(1)
	addi 1,1,32
	blr
	.size	Proc_6, .-Proc_6
	.align 2
	.globl Proc_7
	.type	Proc_7, @function
Proc_7:
	addi 3,3,2
	add 4,4,3
	stw 4,0(5)
	blr
	.size	Proc_7, .-Proc_7
	.align 2
	.globl Proc_8
	.type	Proc_8, @function
Proc_8:
	addi 10,5,5
	slwi 0,10,2
	add 9,0,3
	stwx 6,3,0
	stw 6,4(9)
	stw 10,120(9)
	mr 9,10
	addi 5,5,6
	cmpw 0,10,5
	bgt- 0,.L23
	mulli 8,10,50
	addi 11,10,1
.L21:
	add 0,8,9
	slwi 0,0,2
	stwx 10,4,0
	addi 9,9,1
	cmpw 0,9,11
	ble+ 0,.L21
.L23:
	mulli 11,10,204
	add 11,11,4
	lwz 9,-4(11)
	addi 9,9,1
	stw 9,-4(11)
	slwi 0,10,2
	lwzx 0,3,0
	stw 0,4000(11)
	li 0,5
	lis 9,Int_Glob@ha
	stw 0,Int_Glob@l(9)
	blr
	.size	Proc_8, .-Proc_8
	.align 2
	.globl Func_1
	.type	Func_1, @function
Func_1:
	rlwinm 4,4,0,0xff
	rlwinm 3,3,0,0xff
	li 0,0
	cmpw 0,3,4
	bne+ 0,.L24
	lis 9,Ch_1_Glob@ha
	stb 3,Ch_1_Glob@l(9)
	li 0,1
.L24:
	mr 3,0
	blr
	.size	Func_1, .-Func_1
	.align 2
	.globl Func_2
	.type	Func_2, @function
Func_2:
	stwu 1,-32(1)
	mflr 0
	stw 28,16(1)
	stw 29,20(1)
	stw 30,24(1)
	stw 31,28(1)
	stw 0,36(1)
	mr 30,3
	mr 29,4
	li 31,2
.L32:
	lbzx 3,30,31
	add 9,29,31
	lbz 4,1(9)
	bl Func_1
	cmpwi 0,3,0
	bne- 0,.L28
	li 28,65
	addi 31,31,1
.L28:
	cmpwi 0,31,2
	ble+ 0,.L32
	addi 0,28,-87
	rlwinm 0,0,0,0xff
	cmplwi 0,0,2
	bgt- 0,.L33
	li 31,7
.L33:
	li 0,1
	cmpwi 0,28,82
	beq- 0,.L27
	mr 3,30
	mr 4,29
	bl strcmp
	li 0,0
	cmpwi 0,3,0
	ble- 0,.L27
	addi 31,31,7
	lis 9,Int_Glob@ha
	stw 31,Int_Glob@l(9)
	li 0,1
.L27:
	mr 3,0
	lwz 0,36(1)
	mtlr 0
	lwz 28,16(1)
	lwz 29,20(1)
	lwz 30,24(1)
	lwz 31,28(1)
	addi 1,1,32
	blr
	.size	Func_2, .-Func_2
	.align 2
	.globl Func_3
	.type	Func_3, @function
Func_3:
	xori 3,3,2
	subfic 0,3,0
	adde 3,0,3
	blr
	.size	Func_3, .-Func_3
	.ident	"GCC: (GNU) 3.3.2"
