#ifndef _ABI_ABI_H_
#define _ABI_ABI_H_


#ifdef __cplusplus
#define _TYPES(name)		::abi::name
#define _VALUE(name)		::abi::name
#define _FN(name)		::abi::name
#define _TYPES_HERE(name)	name
#define _VALUE_HERE(name)	name
#else
#define _TYPES(name)		_##name
#define _VALUE(name)		_##name
#define _FN(name)		_##name
#define _TYPES_HERE(name)	_##name
#define _VALUE_HERE(name)	_##name
#endif


#ifdef __cplusplus
namespace abi {
#endif

const unsigned int _VALUE_HERE(bits_per_byte) = 8;


typedef unsigned char		_TYPES_HERE(uint8_t);
typedef unsigned short		_TYPES_HERE(uint16_t);
typedef unsigned int		_TYPES_HERE(uint32_t);
typedef unsigned long long	_TYPES_HERE(uint64_t);

typedef signed int		_TYPES_HERE(int8_t);
typedef signed short		_TYPES_HERE(int16_t);
typedef signed int		_TYPES_HERE(int32_t);
typedef signed long long	_TYPES_HERE(int64_t);

/* Pointer magic. */
#ifdef __LP64__
typedef unsigned long		_TYPES_HERE(uintptr_t);
typedef signed long		_TYPES_HERE(intptr_t);
#else
typedef unsigned int		_TYPES_HERE(uintptr_t);
typedef signed int		_TYPES_HERE(intptr_t);
#endif
typedef _TYPES_HERE(uintptr_t)	_TYPES_HERE(size_t);
typedef _TYPES_HERE(intptr_t)	_TYPES_HERE(ptrdiff_t);

/* Wide characters. */
#ifndef __cplusplus
typedef _TYPES_HERE(int32_t)	_TYPES_HERE(wchar_t);
#else
static_assert(sizeof(wchar_t) == sizeof(_TYPES_HERE(int32_t)), "compiler and abi disagree on wchar_t");
#endif
typedef _TYPES_HERE(int64_t)	_TYPES_HERE(wint_t);
typedef _TYPES_HERE(int32_t)	_TYPES_HERE(wctype_t);
typedef _TYPES_HERE(int32_t)	_TYPES_HERE(wctrans_t);
#ifndef __cplusplus
typedef _TYPES_HERE(uint16_t)	_TYPES_HERE(char16_t);
typedef _TYPES_HERE(uint32_t)	_TYPES_HERE(char32_t);
#endif

/* File offset/size types. */
typedef _TYPES_HERE(int64_t)	_TYPES_HERE(off_t);
typedef _TYPES_HERE(int64_t)	_TYPES_HERE(fpos_t);

/* Widest integrals. */
typedef _TYPES_HERE(uint64_t)	_TYPES_HERE(uintmax_t);
typedef _TYPES_HERE(int64_t)	_TYPES_HERE(intmax_t);


/* Verify pointer types. */
#ifdef __cplusplus
static_assert(sizeof(_TYPES_HERE(uintptr_t)) == sizeof(void*), "incorrect definition for uintptr_t");
static_assert(sizeof(_TYPES_HERE(intptr_t)) == sizeof(void*), "incorrect definition for uintptr_t");
#endif


/* Endianness specification. */
enum _TYPES_HERE(endian) {
	_VALUE_HERE(big_endian),
	_VALUE_HERE(little_endian)
};

/* File seek flag. */
enum _TYPES_HERE(seek_flag) {
	_VALUE_HERE(seek_cur),
	_VALUE_HERE(seek_end),
	_VALUE_HERE(seek_set)
};

enum _TYPES_HERE(buffer_style) {
	_VALUE_HERE(buffer_none),
	_VALUE_HERE(buffer_line),
	_VALUE_HERE(buffer_full)
};

struct _TYPES_HERE(mbstate_t)
{
	char no_clue[8];	/* XXX figure out what mbstate is supposed to do and implement whatever. */
};

struct _TYPES_HERE(stack32_t)	/* 32-bit register stack. */
{
	_TYPES(uint32_t)	ss_sp;
	_TYPES(uint32_t)	ss_size;
	_TYPES(int32_t)		ss_flags;
};
struct _TYPES_HERE(stack64_t)	/* 64-bit register stack. */
{
	_TYPES(uint64_t)	ss_sp;
	_TYPES(uint64_t)	ss_size;
	_TYPES(int32_t)		ss_flags;
};
typedef _TYPES_HERE(uint32_t)	_TYPES_HERE(sigset_t);

#ifndef __cplusplus
typedef enum _TYPES_HERE(endian)	_TYPES_HERE(endian);
typedef enum _TYPES_HERE(seek_flag)	_TYPES_HERE(seek_flag);
typedef enum _TYPES_HERE(buffer_style)	_TYPES_HERE(buffer_style);
typedef struct _TYPES_HERE(mbstate_t)	_TYPES_HERE(mbstate_t);
typedef struct _TYPES_HERE(stack32_t)	_TYPES_HERE(stack32_t);
typedef struct _TYPES_HERE(stack64_t)	_TYPES_HERE(stack64_t);
#endif

typedef _TYPES_HERE(int64_t)		_TYPES_HERE(time_t);


#ifdef __cplusplus
} /* namespace abi */
#endif


#undef _TYPES_HERE
#undef _VALUE_HERE

#endif /* _ABI_ABI_H_ */
