				;;
				;; Test to make sure that we can trigger prefix creation and then
				;; set a prefix field only if a prefix exists.
				;;
				add r1,r2,r3 						; no prefix
				add r1,r2,r3						; no prefix
				pr3 add r1,r2,r3				; prefix, but vles size = 1, so Foo = 0
				add r1,r2,r3 add r4,r5,r6 ; prefix and size = 2, so Foo = 1
				add r1,r2,r3 add r4,r5,r6 mv r2,15,r4 ; prefix, but size = 3, so Foo = 0
				