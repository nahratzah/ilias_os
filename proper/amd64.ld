OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

KERNEL_VMA = 0xffffffff80100000;

ENTRY(loader)

SECTIONS
{
	. = KERNEL_VMA;

	kernel_start = .;

	.text ALIGN(0x1000):
	{
		*(.text)
	}

	.rodata ALIGN(0x1000):
	{
		*(.rodata*)
	}

	.data ALIGN(0x1000):
	{
		*(.data)
	}

	.bss ALIGN(0x1000):
	{
		*(COMMON)
		*(.bss)
	}

	.eh_frame ALIGN(0x1000):
	{
		*(.eh_frame)
	}

	.boot_stack ALIGN(0x1000):
	{
		boot_stack_begin = .;
		. += 0x10000;		/* Allocate 64k for stack. */
		boot_stack_end = .;
		boot_stack_init = .;	/* Initialize stack pointer at this address (stack grows down). */
	}

	kernel_end = .;

	/DISCARD/ :
	{
		*(.comment)
	}
}
