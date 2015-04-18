/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/e_remainderf.S,v 1.4 2011/01/07 16:13:12 kib Exp $");
/* RCSID("$NetBSD: e_remainderf.S,v 1.2 1995/05/08 23:49:47 jtc Exp $") */

.p2align 2,0x90
.globl remainderf
.type remainderf,@function
remainderf:
	flds	8(%esp)
	flds	4(%esp)
1:	fprem1
	fstsw	%ax
	sahf
	jp	1b
	fstp	%st(1)
	ret
.size remainderf, . - remainderf
