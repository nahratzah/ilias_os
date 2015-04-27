/*
 * Based on code written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/amd64/s_scalbnl.S,v 1.3 2011/01/07 16:13:12 kib Exp $")
/* //RCSID("$NetBSD: s_scalbnf.S,v 1.4 1999/01/02 05:15:40 kristerw Exp $") */

.text
.p2align 4,0x90
.globl scalbnl
.type scalbnl,@function
scalbnl:
	movl	%edi,-4(%rsp)
	fildl	-4(%rsp)
	fldt	8(%rsp)
	fscale
	fstp	%st(1)
	ret
.size scalbnl, . - scalbnl

.globl ldexpl
.set   ldexpl,scalbnl
