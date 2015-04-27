/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/s_copysign.S,v 1.9 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl copysign
.type copysign,@function
copysign:
	movl	16(%esp),%edx
	andl	$0x80000000,%edx
	movl	8(%esp),%eax
	andl	$0x7fffffff,%eax
	orl	%edx,%eax
	movl	%eax,8(%esp)
	fldl	4(%esp)
	ret
.size fmod, . - fmod
