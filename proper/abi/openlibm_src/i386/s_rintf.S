/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/s_rintf.S,v 1.3 2011/01/07 16:13:12 kib Exp $");
/* RCSID("$NetBSD: s_rintf.S,v 1.3 1995/05/09 00:17:22 jtc Exp $") */

.p2align 2,0x90
.globl rintf
.type rintf,@function
rintf:
	flds	4(%esp)
	frndint
	ret
.size rintf, . - rintf
