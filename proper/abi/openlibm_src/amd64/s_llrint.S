//__FBSDID("$FreeBSD: src/lib/msun/amd64/s_llrint.S,v 1.3 2011/02/04 21:54:06 kib Exp $")

.text
.p2align 4,0x90
.globl llrint
.type llrint,@function
llrint:
	cvtsd2si %xmm0, %rax
	ret
.size llrint, . - llrint
