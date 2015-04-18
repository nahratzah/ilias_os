/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/s_scalbn.S,v 1.10 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl scalbn
.type scalbn,@function
scalbn:
	fildl	12(%esp)
	fldl	4(%esp)
	fscale
	fstp	%st(1)
	ret
.size scalbn, . - scalbn

.globl ldexp
.set   ldexp, scalbn
