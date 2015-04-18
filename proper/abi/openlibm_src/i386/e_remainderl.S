/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/e_remainderl.S,v 1.2 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl remainderl
.type remainderl,@function
remainderl:
	fldt	16(%esp)
	fldt	4(%esp)
1:	fprem1
	fstsw	%ax
	sahf
	jp	1b
	fstp	%st(1)
	ret
.size remainder, . - remainder
