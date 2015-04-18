//__FBSDID("$FreeBSD: src/lib/msun/amd64/s_llrintf.S,v 1.3 2011/02/04 21:54:06 kib Exp $")

.text
.p2align 4,0x90
.globl llrintf
.type llrintf,@function
llrintf:
	cvtss2si %xmm0, %rax
	ret
.size llrintf, . - llrintf
