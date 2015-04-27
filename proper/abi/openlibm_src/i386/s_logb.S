/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/s_logb.S,v 1.10 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl logb
.type logb,@function
logb:
	fldl	4(%esp)
	fxtract
	fstp	%st
	ret
.size logb, . - logb
