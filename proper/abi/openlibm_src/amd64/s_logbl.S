/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/amd64/s_logbl.S,v 1.4 2011/01/07 16:13:12 kib Exp $")

.text
.p2align 4,0x90
.globl logbl
.type logbl,@function
logbl:
	fldt	8(%rsp)
	fxtract
	fstp	%st
	ret
.size logbl, . - logbl
