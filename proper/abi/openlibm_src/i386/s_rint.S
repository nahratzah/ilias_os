/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/s_rint.S,v 1.9 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl rint
.type rint,@function
rint:
	fldl	4(%esp)
	frndint
	ret
.size rint, . - rint
