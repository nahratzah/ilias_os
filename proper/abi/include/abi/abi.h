#ifndef _ABI_ABI_H_
#define _ABI_ABI_H_

#if defined(__cplusplus) && __has_include(<ilias/stats.h>)
# include <ilias/stats-fwd.h>
#endif

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

#if __SIZEOF_INT128__
# define _USE_INT128	1
#endif

typedef unsigned char		_ABI_TYPES(uint8_t);
typedef unsigned short		_ABI_TYPES(uint16_t);
typedef unsigned int		_ABI_TYPES(uint32_t);
typedef unsigned long long	_ABI_TYPES(uint64_t);
#if _USE_INT128
typedef unsigned __int128	_ABI_TYPES(uint128_t);
#endif

typedef signed char		_ABI_TYPES(int8_t);
typedef signed short		_ABI_TYPES(int16_t);
typedef signed int		_ABI_TYPES(int32_t);
typedef signed long long	_ABI_TYPES(int64_t);
#if _USE_INT128
typedef signed __int128		_ABI_TYPES(int128_t);
#endif

/* Pointer magic. */
#ifdef __LP64__
typedef unsigned long		_ABI_TYPES(uintptr_t);
typedef signed long		_ABI_TYPES(intptr_t);
#else
typedef unsigned int		_ABI_TYPES(uintptr_t);
typedef signed int		_ABI_TYPES(intptr_t);
#endif
typedef __SIZE_TYPE__		_ABI_TYPES(size_t);
typedef _ABI_TYPES(intptr_t)	_ABI_TYPES(ssize_t);
typedef __PTRDIFF_TYPE__	_ABI_TYPES(ptrdiff_t);
typedef _ABI_TYPES(uintptr_t)	_ABI_TYPES(uptrdiff_t);

/* Wide characters. */
#ifndef __cplusplus
typedef __WCHAR_TYPE__		_ABI_TYPES(wchar_t);
#else
static_assert(sizeof(wchar_t) == sizeof(int),
    "compiler and abi disagree on wchar_t");
#endif
typedef __WINT_TYPE__		_ABI_TYPES(wint_t);
typedef _ABI_TYPES(int32_t)	_ABI_TYPES(wctype_t);
typedef _ABI_TYPES(int32_t)	_ABI_TYPES(wctrans_t);
#ifndef __cplusplus
typedef _ABI_TYPES(uint16_t)	_ABI_TYPES(char16_t);
typedef _ABI_TYPES(uint32_t)	_ABI_TYPES(char32_t);
#else
static_assert(sizeof(uint16_t) == sizeof(char16_t),
    "compiler and abi disagree on char16_t");
static_assert(sizeof(uint32_t) == sizeof(char32_t),
    "compiler and abi disagree on char16_t");
#endif

/* File offset/size types. */
#if _USE_INT128
typedef _ABI_TYPES(int128_t)	_ABI_TYPES(off_t);
typedef _ABI_TYPES(int128_t)	_ABI_TYPES(fpos_t);
#else
typedef _ABI_TYPES(int64_t)	_ABI_TYPES(off_t);
typedef _ABI_TYPES(int64_t)	_ABI_TYPES(fpos_t);
#endif

/* Widest integrals. */
#if _USE_INT128
typedef _ABI_TYPES(int128_t)	_ABI_TYPES(intmax_t);
typedef _ABI_TYPES(uint128_t)	_ABI_TYPES(uintmax_t);
#else
typedef _ABI_TYPES(int64_t)	_ABI_TYPES(intmax_t);
typedef _ABI_TYPES(uint64_t)	_ABI_TYPES(uintmax_t);
#endif

/* Atomic (wrt signals) type. */
typedef int			_ABI_TYPES(sig_atomic_t);

/* Type with greatest alignment restriction. */
typedef long double		_ABI_TYPES(max_align_t);


/* Verify pointer types. */
#ifdef __cplusplus
static_assert(sizeof(_ABI_TYPES(uintptr_t)) == sizeof(void*),
    "incorrect definition for uintptr_t");
static_assert(sizeof(_ABI_TYPES(intptr_t)) == sizeof(void*),
    "incorrect definition for uintptr_t");
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
	_ABI_TYPES(uint64_t) _data;  // 8 byte of state data

#ifdef __cplusplus
	bool operator==(const _ABI_TYPES(mbstate_t)& o) const
	{ return _data == o._data; }
	bool operator!=(const _ABI_TYPES(mbstate_t)& o) const
	{ return !(*this == o); }
#endif
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

/* Statistics for abi. */
#if defined(__cplusplus) && __has_include(<ilias/stats.h>)
extern _namespace(ilias)::global_stats_group abi_group;
extern _namespace(ilias)::global_stats_group abi_ext_group;
#endif

typedef _ABI_TYPES(int64_t)		_ABI_TYPES(time_t);


#ifdef __cplusplus
} /* namespace __cxxabiv1 */

/* Namespace ::abi is an alias for ::__cxxabiv1. */
namespace abi = ::__cxxabiv1;
#endif

#endif /* _ABI_ABI_H_ */
