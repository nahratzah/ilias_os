#ifndef _SYS_ENDIAN_H_
#define _SYS_ENDIAN_H_

#include <abi/abi.h>
#include <abi/architectures.h>
#include <cdecl.h>


/*
 * Store machine endianness in abi.
 */
namespace abi {

#if defined(__BIG_ENDIAN__)
const endian machine_endian = big_endian;
#elif defined(__LITTLE_ENDIAN__)
const endian machine_endian = little_endian;
#endif

} /* namespace abi */

/* C defines. */
#define _BIG_ENDIAN	4321
#define _LITTLE_ENDIAN	1234

#if defined(__BIG_ENDIAN__)
#define _ENDIAN		_BIG_ENDIAN
#elif defined(__LITTLE_ENDIAN__)
#define _ENDIAN		_LITTLE_ENDIAN
#else
#error "unable to detect operating system endianness"
#endif


namespace std {
namespace ilias {


using abi::machine_endian;
using abi::bits_per_byte;

/* Ensure architecture defines the right endianness. */
static_assert(abi::arch_current::endian == machine_endian,
    "platform endianness and detected endianness differ");


namespace endian_detail {


template<abi::endian Endian = machine_endian> struct endian_ops;


/*
 * Repeat a bit pattern of delta bits.
 */
template<typename UInt>
constexpr UInt
repeat(UInt base, unsigned int delta)
{
	return (delta >= bits_per_byte * sizeof(UInt) ? base : repeat((base << delta) | base, 2U * delta));
}

template<typename UInt>
constexpr UInt
log2_mask(unsigned int delta)
{
	return repeat((UInt(1U) << (2U * delta)) - (UInt(1U) << delta), 2U * delta);
}

template<typename UInt>
constexpr unsigned int
log2_support(UInt v, unsigned int delta)
{
	return (delta >= bits_per_byte * sizeof(UInt) ?
	    0U :
	    ((v & log2_mask<UInt>(delta)) ? delta : 0U) + log2_support(v, 2U * delta));
}

/*
 * Returns the largest N for which (1 << N) <= v.
 */
template<typename UInt>
constexpr unsigned int
log2(UInt v)
{
	return log2_support(v, 1);
}

/*
 * Test if the given value is a power of 2.
 */
template<typename UInt>
constexpr bool
is_pow2(UInt v)
{
	return (v & (v - 1)) == 0;
}


template<typename UInt>
constexpr unsigned int
bits_modulo(unsigned int bits)
{
	return bits % (bits_per_byte * sizeof(UInt));
}


template<typename UInt>
constexpr UInt
byte_swap_low_mask(unsigned int delta)
{
	return (UInt(0xff) << delta);
}
template<typename UInt>
constexpr UInt
byte_swap_high_mask(unsigned int delta)
{
	return (UInt(0xff) << (bits_per_byte * (sizeof(UInt) - 1) - delta));
}
template<typename UInt>
constexpr unsigned int
byte_swap_shift(unsigned int delta)
{
	return bits_per_byte * (sizeof(UInt) - 1) - 2 * delta;
}

template<typename UInt>
constexpr UInt
byte_swap_specific(UInt v, unsigned int delta)
{
	return (v & ~(byte_swap_low_mask<UInt>(delta) | byte_swap_high_mask<UInt>(delta))) |
	    (v & byte_swap_low_mask<UInt>(delta)) << byte_swap_shift<UInt>(delta) |
	    (v & byte_swap_high_mask<UInt>(delta)) >> byte_swap_shift<UInt>(delta);
}

template<typename UInt>
constexpr UInt
byte_swap_support(UInt v, unsigned int delta)
{
	return (delta >= bits_per_byte * sizeof(v) / 2 ? v :
	    byte_swap_support(byte_swap_specific(v, delta), delta + bits_per_byte));
}

/*
 * Invert the order of bytes.
 *
 * Clang correctly optimizes this to a builtin byte-swap operation
 * if the platform supports it.
 */
template<typename UInt>
constexpr UInt
byte_swap(UInt v)
{
	static_assert(machine_endian == abi::big_endian || machine_endian == abi::little_endian,
	    "byte_swap is only implemented for little endian and big endian machines");
	return byte_swap_support(v, 0);
}


template<>
struct endian_ops<abi::big_endian>
{
	template<typename UInt>
	static constexpr UInt
	to_big_endian(UInt v)
	{
		return v;
	}

	template<typename UInt>
	static constexpr UInt
	to_little_endian(UInt v)
	{
		return byte_swap(v);
	}

	template<typename UInt>
	static constexpr UInt from_big_endian(UInt v)
	{
		return v;
	}

	template<typename UInt>
	static constexpr UInt
	from_little_endian(UInt v)
	{
		return byte_swap(v);
	}


	template<typename UInt>
	static constexpr UInt
	endian_mask_low_addr(unsigned int bits)
	{
		return (bits >= bits_per_byte * sizeof(UInt) ? UInt(0) : ~((UInt(1) << bits) - 1));
	}

	template<typename UInt>
	static constexpr UInt
	endian_mask_high_addr(unsigned int bits)
	{
		return (bits >= bits_per_byte * sizeof(UInt) ? ~UInt(0) : ((UInt(1) << bits) - 1));
	}

	template<typename UInt>
	static constexpr UInt
	rotate_down_addr(UInt v, unsigned int bits)
	{
		return (bits_modulo<UInt>(bits) == 0 ? v :
		    v << bits_modulo<UInt>(bits) | v >> (bits_per_byte * sizeof(UInt) - bits_modulo<UInt>(bits)));
	}

	template<typename UInt>
	static constexpr UInt
	rotate_up_addr(UInt v, unsigned int bits)
	{
		return (bits_modulo<UInt>(bits) == 0 ? v :
		    v >> bits_modulo<UInt>(bits) | v << (bits_per_byte * sizeof(UInt) - bits_modulo<UInt>(bits)));
	}
};

template<>
struct endian_ops<abi::little_endian>
{
	template<typename UInt>
	static constexpr UInt
	to_big_endian(UInt v)
	{
		return byte_swap(v);
	}

	template<typename UInt>
	static constexpr UInt
	to_little_endian(UInt v)
	{
		return v;
	}

	template<typename UInt>
	static constexpr UInt
	from_big_endian(UInt v)
	{
		return byte_swap(v);
	}

	template<typename UInt>
	static constexpr UInt
	from_little_endian(UInt v)
	{
		return v;
	}


	template<typename UInt>
	static constexpr UInt
	endian_mask_low_addr(unsigned int bits)
	{
		return (bits >= bits_per_byte * sizeof(UInt) ? ~UInt(0) : ((UInt(1) << bits) - 1));
	}

	template<typename UInt>
	static constexpr UInt
	endian_mask_high_addr(unsigned int bits)
	{
		return (bits >= bits_per_byte * sizeof(UInt) ? UInt(0) : ~((UInt(1) << bits) - 1));
	}

	template<typename UInt>
	static constexpr UInt
	rotate_down_addr(UInt v, unsigned int bits)
	{
		return (bits_modulo<UInt>(bits) == 0 ? v :
		    v >> bits_modulo<UInt>(bits) | v << (bits_per_byte * sizeof(UInt) - bits_modulo<UInt>(bits)));
	}

	template<typename UInt>
	static constexpr UInt
	rotate_up_addr(UInt v, unsigned int bits)
	{
		return (bits_modulo<UInt>(bits) == 0 ? v :
		    v << bits_modulo<UInt>(bits) | v >> (bits_per_byte * sizeof(UInt) - bits_modulo<UInt>(bits)));
	}
};


template<typename UInt, abi::endian Endian = machine_endian>
static constexpr UInt
to_big_endian(UInt v)
{
	return endian_ops<Endian>::to_big_endian(v);
}

template<typename UInt, abi::endian Endian = machine_endian>
static constexpr UInt
to_little_endian(UInt v)
{
	return endian_ops<Endian>::to_little_endian(v);
}

template<typename UInt, abi::endian Endian = machine_endian>
static constexpr UInt
from_big_endian(UInt v)
{
	return endian_ops<Endian>::from_big_endian(v);
}

template<typename UInt, abi::endian Endian = machine_endian>
static constexpr UInt
from_little_endian(UInt v)
{
	return endian_ops<Endian>::from_little_endian(v);
}


template<typename UInt, abi::endian Endian = machine_endian>
static constexpr UInt
endian_mask_low_addr(unsigned int bits)
{
	return endian_ops<Endian>::template endian_mask_low_addr<UInt>(bits);
}

template<typename UInt, abi::endian Endian = machine_endian>
static constexpr UInt
endian_mask_high_addr(unsigned int bits)
{
	return endian_ops<Endian>::template endian_mask_high_addr<UInt>(bits);
}

template<typename UInt, abi::endian Endian = machine_endian>
static constexpr UInt
rotate_down_addr(UInt v, unsigned int bits)
{
	return endian_ops<Endian>::rotate_down_addr(v, bits);
}

template<typename UInt, abi::endian Endian = machine_endian>
static constexpr UInt
rotate_up_addr(UInt v, unsigned int bits)
{
	return endian_ops<Endian>::rotate_up_addr(v, bits);
}


/*
 * Shift v0 and v1 down and return the resulting uint.
 *
 * I.e. addr_shift(uint16_t v0, uint16_t v1, 8) would yield
 * the second byte of v0 and the first byte of v1,
 * concatenated in the return value.
 */
template<typename UInt, abi::endian Endian = machine_endian>
constexpr UInt
addr_shift(UInt v0, UInt v1, unsigned int bits)
{
	return (rotate_down_addr<UInt, Endian>(v0, bits) & endian_mask_low_addr<UInt, Endian>(bits)) |
	    (rotate_up_addr<UInt, Endian>(v1, bits) & endian_mask_high_addr<UInt, Endian>(bits));
}


} /* namespace ilias::endian_detail */


using endian_detail::repeat;
using endian_detail::log2;
using endian_detail::is_pow2;
using endian_detail::endian_mask_low_addr;
using endian_detail::endian_mask_high_addr;
using endian_detail::addr_shift;
using endian_detail::byte_swap;

template<typename UInt>
inline constexpr UInt
htobe(UInt v)
{
	return endian_detail::to_big_endian(v);
}

template<typename UInt>
inline constexpr UInt
htole(UInt v)
{
	return endian_detail::to_little_endian(v);
}

template<typename UInt>
inline constexpr UInt
betoh(UInt v)
{
	return endian_detail::from_big_endian(v);
}

template<typename UInt>
inline constexpr UInt
letoh(UInt v)
{
	return endian_detail::from_little_endian(v);
}


}} /* namespace std::ilias */


#ifndef _ENDIAN_INLINE
#define _ENDIAN_INLINE	constexpr
#endif

extern "C" {

#define _ENDIAN_CONVERT(x)						\
_ENDIAN_INLINE abi::uint##x##_t						\
htobe##x(abi::uint##x##_t v)						\
{									\
	return std::ilias::htobe(v);					\
}									\
_ENDIAN_INLINE abi::uint##x##_t						\
htole##x(abi::uint##x##_t v)						\
{									\
	return std::ilias::htole(v);					\
}									\
_ENDIAN_INLINE abi::uint##x##_t						\
betoh##x(abi::uint##x##_t v)						\
{									\
	return std::ilias::betoh(v);					\
}									\
_ENDIAN_INLINE abi::uint##x##_t						\
letoh##x(abi::uint##x##_t v)						\
{									\
	return std::ilias::letoh(v);					\
}

_ENDIAN_CONVERT(8)
_ENDIAN_CONVERT(16)
_ENDIAN_CONVERT(32)
_ENDIAN_CONVERT(64)

#undef _ENDIAN_CONVERT

_ENDIAN_INLINE abi::uint16_t
htons(abi::uint16_t v)
{
	return std::ilias::htobe(v);
}

_ENDIAN_INLINE abi::uint16_t
ntohs(abi::uint16_t v)
{
	return std::ilias::betoh(v);
}

_ENDIAN_INLINE abi::uint32_t
htonl(abi::uint32_t v)
{
	return std::ilias::htobe(v);
}

_ENDIAN_INLINE abi::uint32_t
ntohl(abi::uint32_t v)
{
	return std::ilias::betoh(v);
}

} /* extern C */

#undef _ENDIAN_INLINE


#endif /* _SYS_ENDIAN_H_ */
