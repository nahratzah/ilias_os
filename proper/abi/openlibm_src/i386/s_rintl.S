/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/s_rintl.S,v 1.3 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl rintl
.type rintl,@function
rintl:
	fldt	4(%esp)
	frndint
	ret
.size rintl, . - rintl
