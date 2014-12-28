OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)

KERNEL_VMA = 1M;

ENTRY(loader)

SECTIONS
{
	. = KERNEL_VMA;

	kernel_start = .;

	.text ALIGN(0x1000):
	{
		loader/src/loader.ol(*)
		*(.text*)
		*(.gnu.linkonce.t.*)
	}

	.rodata ALIGN(0x1000):
	{
		*(.rodata*)

		. = ALIGN(0x4);
		start_ctors = .;
		/* *(SORT(.init_array*)) */
		*(SORT(.ctors*))
		end_ctors = .;

		start_dtors = .;
		*(SORT(.dtors*))
		/* *(SORT(.fini_array*)) */
		end_dtors = .;

		*(.gnu.linkonce.r.*)
	}

	.eh_frame ALIGN(0x1000):
	{
		__eh_frame_begin = .;
		*(.eh_frame)
		*(.eh_frame*)
		__eh_frame_end = .;
	}

	.gcc_except_table ALIGN(0x1000):
	{
		*(.gcc_except_table*)
	}

	.data ALIGN(0x1000):
	{
		*(.data*)
		*(.gnu.linkonce.d.*)
	}

	.tdata ALIGN(0x1000):
	{
		*(.tdata*)
		*(.gnu.linkonce.td.*)
	}

	.dynamic ALIGN(0x1000):
	{
		*(.dynamic)
		*(.dynamic*)
	}

	.bss ALIGN(0x1000):
	{
		/* Stack used during loading of external kernels. */
		boot_stack_begin = .;
		. += 0x10000;		/* Allocate 64k for stack. */
		boot_stack_end = .;
		boot_stack_init = .;	/* Initialize stack pointer at this address (stack grows down). */

		. = ALIGN(0x1000);
		sbss = .;

		*(.bss*)
		*(COMMON*)
		*(.gnu.linkonce.b.*)

		ebss = .;

		. = ALIGN(0x8);
		heap_start = .;
		. += 0x40000;		/* Allocate 256k for heap. */
		heap_end = .;
	}

	.tbss ALIGN(0x1000):
	{
		*(.tbss*)
		*(.gnu.linkonce.tb.*)
	}

	kernel_end = .;

	/DISCARD/ :
	{
		*(.comment)
	}
}
