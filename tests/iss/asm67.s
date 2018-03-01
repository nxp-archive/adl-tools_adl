
				set.mem 2,100,1
				set.imm 5,200,4
				set 7,300,6
				add.mem 2,200,1
				add 5,300,4
				get 20,10
				stop
				mult.ss.qext.ind 0x1234deadbeef,16,4,6,7
				mult.modM.twram.srcscale.noss.qext.imm 1,2,3,4,5,6,7
				