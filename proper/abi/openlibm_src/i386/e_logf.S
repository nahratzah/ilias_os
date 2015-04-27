/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/e_logf.S,v 1.3 2011/01/07 16:13:12 kib Exp $");
/* RCSID("$NetBSD: e_logf.S,v 1.2 1996/07/06 00:15:45 jtc Exp $") */

.p2align 2,0x90
.globl logf
.type logf,@function
logf:
	fldln2
	flds	4(%esp)
	fyl2x
	ret
.size logf, . - logf
