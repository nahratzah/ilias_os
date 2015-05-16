# Multiboot header.
.set ALIGN, 1<<0		# Align loaded modules on page boundaries.
.set MEMINFO, 1<<1		# Provide memory map.
.set FLAGS, ALIGN | MEMINFO	# Multiboot flags.
.set MAGIC, 0x1BADB002		# Magic for multiboot header.
.set CHECKSUM, -(MAGIC + FLAGS)	# Checksum of the above.

.section .multiboot
.align 4
.long MAGIC			# Multiboot header starts here.
.long FLAGS
.long CHECKSUM			# Multiboot checksum.

.section .text
.global loader
.type loader, @function
.global halt
.type halt, @function
loader:
	mov	$boot_stack_init, %esp		# Initialize stack pointer.
	# Reset eflags.
	push	$0
	popf

	# To prevent losing the mb_magic and mb_data, we borrow some of the
	# stack of the loader to save them.
	push	%eax
	push	%ebx

	# Allow SSE instructions (otherwise we'll get a Protection Failure)
	# Note that eax has been saved by now.
	mov %cr0, %eax
	and $0xfffb, %ax			# Clear CR0.EM
	or $0x2, %ax				# Set CR0.MP
	mov %eax, %cr0
	mov %cr4, %eax
	or $0x600, %ax				# Set CR4.OSFXSR,
						#     CR4.OSXMMEXCPT
	mov %eax, %cr4

	# Zero bss area.
	call	bss_zero

	# mb_magic and mb_data live in BSS and cannot be assigned earlier.
	pop	%ebx
	pop	%eax
	mov	%eax, mb_magic			# Save magic.
	mov	%ebx, mb_data			# Save multiboot data pointer.

	mov	$boot_stack_init, %ebp		# Initialize stack base.
	push	%ebp
	push	$0

	call	loader_constructors

	# BSS is initialized, multiboot data saved, next stage can commence.
	call	loader_setup

	call	__cxa_finalize_0
	call	loader_destructors
	call	loader_print_endmsg

	# Ensure we don't do anything if loader_setup returns for some reason.
halt:
	hlt
	jmp halt

.size loader, . - loader
.size halt, . - halt
