/*
 * Written by:
 *	J.T. Conklin (jtc@netbsd.org)
 *	Public domain.
 */

.text
.p2align 4,0x90
.globl rintl
.type rintl,@function
rintl:
	fldt	8(%rsp)
	frndint
	ret
.size rintl, . - rintl
