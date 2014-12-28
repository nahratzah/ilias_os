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
		*(.gnu.linkonce.t*)
	}

	.rodata ALIGN(0x1000):
	{
		*(.rodata*)

		. = ALIGN(0x8);
		start_ctors = .;
		*(SORT(.ctors*))
		end_ctors = .;

		start_dtors = .;
		*(SORT(.dtors*))
		end_dtors = .;

		*(.gnu.linkonce.r*)
	}

	.data ALIGN(0x1000):
	{
		*(.data)
		*(.gnu.linkonce.d*)
	}

	.bss ALIGN(0x1000):
	{
		sbss = .;

		*(COMMON)
		*(.bss)
		*(.gnu.linkonce.b*)

		ebss = .;
	}

	.eh_frame ALIGN(0x1000):
	{
		__eh_frame_begin = .;
		*(.eh_frame)
		*(.eh_frame*)
		__eh_frame_end = .;
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
