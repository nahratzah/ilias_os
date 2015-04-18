/*
 * Based on code written by J.T. Conklin <jtc@NetBSD.org>.
 * Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/s_trunc.S,v 1.3 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl trunc
.type trunc,@function
trunc:
	pushl	%ebp
	movl	%esp,%ebp
	subl	$8,%esp

	fstcw	-4(%ebp)		/* store fpu control word */
	movw	-4(%ebp),%dx
	orw	$0x0c00,%dx		/* round towards -oo */
	movw	%dx,-8(%ebp)
	fldcw	-8(%ebp)		/* load modfied control word */

	fldl	8(%ebp)			/* round */
	frndint

	fldcw	-4(%ebp)		/* restore original control word */

	leave
	ret
.size trunc, . - trunc
