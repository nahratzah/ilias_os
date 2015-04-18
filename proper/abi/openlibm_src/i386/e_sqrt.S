/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/e_sqrt.S,v 1.10 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl sqrt
.type sqrt,@function
sqrt:
	fldl	4(%esp)
	fsqrt
	ret
.size sqrt, . - sqrt
