/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/e_log10f.S,v 1.4 2011/01/07 16:13:12 kib Exp $");
/* RCSID("$NetBSD: e_log10f.S,v 1.1 1996/07/03 16:50:22 jtc Exp $") */

.p2align 2,0x90
.globl log10f
.type log10f,@function
log10f:
	fldlg2
	flds	4(%esp)
	fyl2x
	ret
.size log10f, . - log10f
