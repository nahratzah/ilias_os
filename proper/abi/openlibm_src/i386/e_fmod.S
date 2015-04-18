/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/e_fmod.S,v 1.11 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl fmod
.type fmod,@function
fmod:
	fldl	12(%esp)
	fldl	4(%esp)
1:	fprem
	fstsw	%ax
	sahf
	jp	1b
	fstp	%st(1)
	ret
.size fmod, . - fmod
