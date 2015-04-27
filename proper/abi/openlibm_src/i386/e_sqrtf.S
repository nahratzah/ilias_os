/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/e_sqrtf.S,v 1.4 2011/01/07 16:13:12 kib Exp $");
/* RCSID("$NetBSD: e_sqrtf.S,v 1.2 1995/05/08 23:50:14 jtc Exp $") */

.p2align 2,0x90
.globl sqrtf
.type sqrtf,@function
sqrtf:
	flds	4(%esp)
	fsqrt
	ret
.size sqrtf, . - sqrtf
