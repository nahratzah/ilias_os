#ifndef _SYS_ARCHITECTURES_H_
#define _SYS_ARCHITECTURES_H_

#include <abi/abi.h>

namespace abi {


struct mcontext_i386_t
{
	uint32_t			 eax, ebx, ecx, edx, ebp, esp, esi, edi;
	uint32_t			 cs, ds, es, fs, gs, ss;
	uint32_t			 isp;
	uint32_t			 eip, err;
	uint32_t			 trapno, eflags;
	uint32_t			 fsbase, gsbase;

	/* XXX FPU/MMX/SSE/SSE2/SSSE/SSE3 etc etc etc */
	double				 st[8];		/* FPU */
};
struct mcontext_amd64_t
{
	uint64_t			 rax, rbx, rcx, rdx, rbp, rsp, rsi, rdi,
					 r8, r9, r10, r11, r12, r13, r14, r15;
	uint64_t			 cs, ds, es, fs, gs, ss;
	uint64_t			 rip, err;
	uint32_t			 trapno, flags;
	uint64_t			 rflags;
	uint64_t			 fsbase, gsbase;

	/* XXX FPU/MMX/SSE/SSE2/SSSE/SSE3 etc etc etc */
};


/*
 * List of architectures the OS supports.
 * Each architecture is assigned a number.
 * When an architecture is removed, other architectures may not be renumbered, numbers may not be re-used.
 */
enum architecture {
	/* 0: reserved. */
	arch_amd64 = 1,
	arch_i386 = 2,
};


/* List of pointer sizes per architecture. */
template<architecture Arch> struct arch_traits;

template<unsigned int PtrSize> struct arch_type_traits;

template<>
struct arch_type_traits<4>
{
	static const unsigned int pointer_size = 4;

	typedef uint32_t		uintptr_t;
	typedef int32_t			intptr_t;
	typedef uint32_t		gpr_t;	/* General purpose register type. */
};

template<>
struct arch_type_traits<8>
{
	static const unsigned int pointer_size = 8;

	typedef uint64_t		uintptr_t;
	typedef int64_t			intptr_t;
	typedef uint64_t		gpr_t;	/* General purpose register type. */
};

#define _ARCH_TRAITS(arch_name, ptrsz, endianness)			\
	template<> struct arch_traits< arch_##arch_name >		\
	:	public arch_type_traits<ptrsz>				\
	{								\
		static const architecture type = arch_##arch_name;	\
		static const endian endian = endianness;		\
		static const unsigned int pointer_bits =		\
		    bits_per_byte *					\
		    arch_type_traits<ptrsz>::pointer_size;		\
									\
		static const char* name() noexcept {			\
			return #arch_name;				\
		}							\
									\
		typedef uintptr_t		size_t;			\
		typedef intptr_t		ptrdiff_t;		\
	};
_ARCH_TRAITS(amd64, 8, little_endian)
_ARCH_TRAITS(i386, 4, little_endian)

#undef _ARCH_TRAITS


#ifdef __amd64__
typedef arch_traits<arch_amd64> arch_current;
#elif __i386__
typedef arch_traits<arch_i386> arch_current;
#else
#error "Unable to detect current architecture.  Please update sys/architectures.h."
#endif

/* Validate pointer size. */
static_assert(arch_current::pointer_size == sizeof(void*),
    "current platform definition defines the wrong pointer size");
/* Validate type sizes. */
static_assert(sizeof(arch_current::uintptr_t) == sizeof(uintptr_t),
    "current platform definition defines the wrong uintptr_t");
static_assert(sizeof(arch_current::intptr_t) == sizeof(intptr_t),
    "current platform definition defines the wrong intptr_t");
static_assert(sizeof(arch_current::size_t) == sizeof(size_t),
    "current platform definition defines the wrong size_t");
static_assert(sizeof(arch_current::ptrdiff_t) == sizeof(ptrdiff_t),
    "current platform definition defines the wrong ptrdiff_t");

} /* namespace abi */

#endif /* _SYS_ARCHITECTURES_H_ */
