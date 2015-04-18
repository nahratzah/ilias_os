/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/s_logbl.S,v 1.3 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl logbl
.type logbl,@function
logbl:
	fldt	4(%esp)
	fxtract
	fstp	%st
	ret
.size logbl, . - logbl
