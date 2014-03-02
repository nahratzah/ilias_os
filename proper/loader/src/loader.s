.global loader

# Multiboot header.
.set MAGIC,	0x1BADB002			# Magic for multiboot header.
.set FLAGS,	0x3				# Multiboot flags.
.align 4
.long MAGIC					# Multiboot header starts here.
.long FLAGS
.long -(MAGIC + FLAGS)				# Multiboot checksum.

loader:
	push	%ebx
	push	%eax
	call	bss_zero
	pop	%eax
	pop	%ebx

	mov	%eax, mb_magic			# Save magic.
	mov	%ebx, mb_data			# Save multiboot data pointer.

	mov	boot_stack_init, %esp		# Initialize stack pointer.

	call	loader_constructors

	# BSS is initialized, multiboot data saved, next stage can commence.
	call	loader_setup

	call	__cxa_finalize_0
	call	loader_destructors

	# Ensure we don't do anything if loader_setup returns for some reason.
halt:
	hlt
	jmp halt
