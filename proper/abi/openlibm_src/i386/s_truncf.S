/*
 * Based on code written by J.T. Conklin <jtc@NetBSD.org>.
 * Public domain.
 */

//__FBSDID("$FreeBSD: src/lib/msun/i387/s_truncf.S,v 1.4 2011/01/07 16:13:12 kib Exp $")

.p2align 2,0x90
.globl truncf
.type truncf,@function
truncf:
	pushl	%ebp
	movl	%esp,%ebp
	subl	$8,%esp

	fstcw	-4(%ebp)		/* store fpu control word */
	movw	-4(%ebp),%dx
	orw	$0x0c00,%dx		/* round towards -oo */
	movw	%dx,-8(%ebp)
	fldcw	-8(%ebp)		/* load modfied control word */

	flds	8(%ebp)			/* round */
	frndint

	fldcw	-4(%ebp)		/* restore original control word */

	leave
	ret
.size truncf, . - truncf
