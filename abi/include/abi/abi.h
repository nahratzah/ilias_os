#ifndef _ABI_ABI_H_
#define _ABI_ABI_H_


#ifdef __cplusplus
#define _TYPES(name)		::abi::name
#define _VALUE(name)		::abi::name
#define _FN(name)		::abi::name
#define _ABI_TYPES(name)	name
#define _ABI_VALUE(name)	name
#else
#define _TYPES(name)		_##name
#define _VALUE(name)		_##name
#define _FN(name)		_##name
#define _ABI_TYPES(name)	_##name
#define _ABI_VALUE(name)	_##name
#endif


#ifdef __cplusplus
namespace __cxxabiv1 {
#endif

const unsigned int _ABI_VALUE(bits_per_byte) = 8;


typedef unsigned char		_ABI_TYPES(uint8_t);
typedef unsigned short		_ABI_TYPES(uint16_t);
typedef unsigned int		_ABI_TYPES(uint32_t);
typedef unsigned long long	_ABI_TYPES(uint64_t);

typedef signed int		_ABI_TYPES(int8_t);
typedef signed short		_ABI_TYPES(int16_t);
typedef signed int		_ABI_TYPES(int32_t);
typedef signed long long	_ABI_TYPES(int64_t);

/* Pointer magic. */
#ifdef __LP64__
typedef unsigned long		_ABI_TYPES(uintptr_t);
typedef signed long		_ABI_TYPES(intptr_t);
#else
typedef unsigned int		_ABI_TYPES(uintptr_t);
typedef signed int		_ABI_TYPES(intptr_t);
#endif
typedef _ABI_TYPES(uintptr_t)	_ABI_TYPES(size_t);
typedef _ABI_TYPES(intptr_t)	_ABI_TYPES(ptrdiff_t);

/* Wide characters. */
#ifndef __cplusplus
typedef _ABI_TYPES(int32_t)	_ABI_TYPES(wchar_t);
#else
static_assert(sizeof(wchar_t) == sizeof(_ABI_TYPES(int32_t)), "compiler and abi disagree on wchar_t");
#endif
typedef _ABI_TYPES(int64_t)	_ABI_TYPES(wint_t);
typedef _ABI_TYPES(int32_t)	_ABI_TYPES(wctype_t);
typedef _ABI_TYPES(int32_t)	_ABI_TYPES(wctrans_t);
#ifndef __cplusplus
typedef _ABI_TYPES(uint16_t)	_ABI_TYPES(char16_t);
typedef _ABI_TYPES(uint32_t)	_ABI_TYPES(char32_t);
#endif

/* File offset/size types. */
typedef _ABI_TYPES(int64_t)	_ABI_TYPES(off_t);
typedef _ABI_TYPES(int64_t)	_ABI_TYPES(fpos_t);

/* Widest integrals. */
typedef _ABI_TYPES(uint64_t)	_ABI_TYPES(uintmax_t);
typedef _ABI_TYPES(int64_t)	_ABI_TYPES(intmax_t);

/* Type with greatest alignment restriction. */
typedef long double		_ABI_TYPES(max_align_t);


/* Verify pointer types. */
#ifdef __cplusplus
static_assert(sizeof(_ABI_TYPES(uintptr_t)) == sizeof(void*), "incorrect definition for uintptr_t");
static_assert(sizeof(_ABI_TYPES(intptr_t)) == sizeof(void*), "incorrect definition for uintptr_t");
#endif


/* Endianness specification. */
enum _ABI_TYPES(endian) {
	_ABI_VALUE(big_endian),
	_ABI_VALUE(little_endian)
};

/* File seek flag. */
enum _ABI_TYPES(seek_flag) {
	_ABI_VALUE(seek_cur),
	_ABI_VALUE(seek_end),
	_ABI_VALUE(seek_set)
};

enum _ABI_TYPES(buffer_style) {
	_ABI_VALUE(buffer_none),
	_ABI_VALUE(buffer_line),
	_ABI_VALUE(buffer_full)
};

struct _ABI_TYPES(mbstate_t)
{
	char no_clue[8];	/* XXX figure out what mbstate is supposed to do and implement whatever. */
};

struct _ABI_TYPES(stack32_t)	/* 32-bit register stack. */
{
	_ABI_TYPES(uint32_t)	ss_sp;
	_ABI_TYPES(uint32_t)	ss_size;
	_ABI_TYPES(int32_t)	ss_flags;
};
struct _ABI_TYPES(stack64_t)	/* 64-bit register stack. */
{
	_ABI_TYPES(uint64_t)	ss_sp;
	_ABI_TYPES(uint64_t)	ss_size;
	_ABI_TYPES(int32_t)	ss_flags;
};
typedef _ABI_TYPES(uint32_t)	_ABI_TYPES(sigset_t);

#ifndef __cplusplus
typedef enum _ABI_TYPES(endian)		_ABI_TYPES(endian);
typedef enum _ABI_TYPES(seek_flag)	_ABI_TYPES(seek_flag);
typedef enum _ABI_TYPES(buffer_style)	_ABI_TYPES(buffer_style);
typedef struct _ABI_TYPES(mbstate_t)	_ABI_TYPES(mbstate_t);
typedef struct _ABI_TYPES(stack32_t)	_ABI_TYPES(stack32_t);
typedef struct _ABI_TYPES(stack64_t)	_ABI_TYPES(stack64_t);
#endif

typedef _ABI_TYPES(int64_t)		_ABI_TYPES(time_t);


#ifdef __cplusplus
} /* namespace __cxxabiv1 */

/* Namespace ::abi is an alias for ::__cxxabiv1. */
namespace abi = ::__cxxabiv1;
#endif

#endif /* _ABI_ABI_H_ */