/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/e_sqrtl.S,v 1.3 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl sqrtl
.type sqrtl,@function
sqrtl:
	fldt	4(%esp)
	fsqrt
	ret
.size sqrtl, . - sqrtl
