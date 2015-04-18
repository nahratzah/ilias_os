/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

.p2align 2,0x90
.globl ceil
.type ceil,@function
ceil:
    pushl   %ebp
	movl	%esp,%ebp
	subl	$8,%esp

	fstcw	-4(%ebp)		/* store fpu control word */
	movw	-4(%ebp),%dx
	orw	$0x0800,%dx		/* round towards +oo */
	andw	$0xfbff,%dx
	movw	%dx,-8(%ebp)
	fldcw	-8(%ebp)		/* load modfied control word */

	fldl	8(%ebp);		/* round */
	frndint

	fldcw	-4(%ebp)		/* restore original control word */

	leave
	ret
.size ceil, . - ceil
