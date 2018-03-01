; Test for '#' in syntax, conditional execution,
; Allow ";" as comment, explicit register names allow to distinguish between
; move for 'd' and move for 'r'.

.align 8

	move.l ($FFFF),r1 ; Comment
	move.l ($2),r2
	
	cmpeq r1,r1
        ift add r1,r2,r13  iff add r1,r2,r3

        
	cmpeq r1,r2
        ift add r1,r2,r13  iff add r1,r2,r3
	
	move.l ($33),d0 ; should choose d-version
	move.l ($44),r0 ; here long version of move
	move.l ($4),r15 ; here also long version of move
	move.l ($4),r0  ; should choose short version of move.
	move.l (r1+a),S  ; should not choose immediat version.
