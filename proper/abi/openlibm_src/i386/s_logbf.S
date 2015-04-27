/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/s_logbf.S,v 1.3 2011/01/07 16:13:12 kib Exp $");
/* RCSID("$NetBSD: s_logbf.S,v 1.3 1995/05/09 00:15:12 jtc Exp $") */

.p2align 2,0x90
.globl logbf
.type logbf,@function
logbf:
	flds	4(%esp)
	fxtract
	fstp	%st
	ret
.size logbf, . - logbf
