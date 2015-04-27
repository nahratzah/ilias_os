/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/e_log.S,v 1.10 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl log
.type log,@function
log:
	fldln2
	fldl	4(%esp)
	fyl2x
	ret
.size log, . - log
