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
loader:
	mov	boot_stack_init, %ebp		# Initialize stack base.
	mov	boot_stack_init, %esp		# Initialize stack pointer.

	push	%ebx
	push	%eax
	call	bss_zero
	pop	%eax
	pop	%ebx

	mov	%eax, mb_magic			# Save magic.
	mov	%ebx, mb_data			# Save multiboot data pointer.

	call	loader_constructors

	# BSS is initialized, multiboot data saved, next stage can commence.
	call	loader_setup

	call	__cxa_finalize_0
	call	loader_destructors
	call	loader_print_endmsg

	# Ensure we don't do anything if loader_setup returns for some reason.
halt:
	# hlt
	jmp halt

.size loader, . - loader
