/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/e_remainder.S,v 1.11 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl remainder
.type remainder,@function
remainder:
	fldl	12(%esp)
	fldl	4(%esp)
1:	fprem1
	fstsw	%ax
	sahf
	jp	1b
	fstp	%st(1)
	ret
.size remainder, . - remainder
