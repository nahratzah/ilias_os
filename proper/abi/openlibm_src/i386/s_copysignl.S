/*
 * Based on code written by J.T. Conklin <jtc@NetBSD.org>.
 * Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/s_copysignl.S,v 1.3 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl copysignl
.type copysignl,@function
copysignl:
	movl	24(%esp),%edx
	andl	$0x8000,%edx
	movl	12(%esp),%eax
	andl	$0x7fff,%eax
	orl	%edx,%eax
	movl	%eax,12(%esp)
	fldt	4(%esp)
	ret
.size fmod, . - fmod
