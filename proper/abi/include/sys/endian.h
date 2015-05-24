#ifndef _SYS_ENDIAN_H_
#define _SYS_ENDIAN_H_

#include <abi/abi.h>
#ifdef __cplusplus
# include <type_traits>
#endif


#define __LITTLE_ENDIAN  __LITTLE_ENDIAN__
#define __BIG_ENDIAN     __BIG_ENDIAN__

#define LITTLE_ENDIAN    __ORDER_LITTLE_ENDIAN__
#define BIG_ENDIAN       __ORDER_BIG_ENDIAN__
#define BYTE_ORDER       __BYTE_ORDER__

#if __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
# error "PDP endianness currently not supported"
#endif

inline _TYPES(uint16_t) bswap16(_TYPES(uint16_t) v) noexcept {
  return
      ((v >> 8) & 0x00ff) |
      ((v << 8) & 0xff00);
}

inline _TYPES(uint32_t) bswap32(_TYPES(uint32_t) v) noexcept {
  return
      ((v >> 24) & 0x000000ff) |
      ((v >>  8) & 0x0000ff00) |
      ((v <<  8) & 0x00ff0000) |
      ((v << 24) & 0xff000000);
}

inline _TYPES(uint64_t) bswap64(_TYPES(uint64_t) v) noexcept {
  return
      ((v >> 56) & 0x00000000000000ff) |
      ((v >> 40) & 0x000000000000ff00) |
      ((v >> 24) & 0x0000000000ff0000) |
      ((v >>  8) & 0x00000000ff000000) |
      ((v <<  8) & 0x000000ff00000000) |
      ((v << 24) & 0x0000ff0000000000) |
      ((v << 40) & 0x00ff000000000000) |
      ((v << 56) & 0xff00000000000000);
}

#if _USE_INT128
inline _TYPES(uint128_t) bswap128(_TYPES(uint128_t) v) noexcept {
  return
      (uint128_t)bswap64(v >> 64) |
      (uint128_t)bswap64(v) << 64;
}
#endif

#if BYTE_ORDER == LITTLE_ENDIAN

inline _TYPES(uint16_t) letoh16(_TYPES(uint16_t) v) noexcept {
  return v;
}
inline _TYPES(uint16_t) htole16(_TYPES(uint16_t) v) noexcept {
  return v;
}
inline _TYPES(uint32_t) letoh32(_TYPES(uint32_t) v) noexcept {
  return v;
}
inline _TYPES(uint32_t) htole32(_TYPES(uint32_t) v) noexcept {
  return v;
}
inline _TYPES(uint64_t) letoh64(_TYPES(uint64_t) v) noexcept {
  return v;
}
inline _TYPES(uint64_t) htole64(_TYPES(uint64_t) v) noexcept {
  return v;
}
#if _USE_INT128
inline _TYPES(uint128_t) letoh128(_TYPES(uint128_t) v) noexcept {
  return v;
}
inline _TYPES(uint128_t) htole128(_TYPES(uint128_t) v) noexcept {
  return v;
}
#endif

inline _TYPES(uint16_t) betoh16(_TYPES(uint16_t) v) noexcept {
  return bswap16(v);
}
inline _TYPES(uint16_t) htobe16(_TYPES(uint16_t) v) noexcept {
  return bswap16(v);
}
inline _TYPES(uint32_t) betoh32(_TYPES(uint32_t) v) noexcept {
  return bswap32(v);
}
inline _TYPES(uint32_t) htobe32(_TYPES(uint32_t) v) noexcept {
  return bswap32(v);
}
inline _TYPES(uint64_t) betoh64(_TYPES(uint64_t) v) noexcept {
  return bswap64(v);
}
inline _TYPES(uint64_t) htobe64(_TYPES(uint64_t) v) noexcept {
  return bswap64(v);
}
#if _USE_INT128
inline _TYPES(uint128_t) betoh128(_TYPES(uint128_t) v) noexcept {
  return bswap128(v);
}
inline _TYPES(uint128_t) htobe128(_TYPES(uint128_t) v) noexcept {
  return bswap128(v);
}
#endif

#elif BYTE_ORDER == BIG_ENDIAN

inline _TYPES(uint16_t) betoh16(_TYPES(uint16_t) v) noexcept {
  return v;
}
inline _TYPES(uint16_t) htobe16(_TYPES(uint16_t) v) noexcept {
  return v;
}
inline _TYPES(uint32_t) betoh32(_TYPES(uint32_t) v) noexcept {
  return v;
}
inline _TYPES(uint32_t) htobe32(_TYPES(uint32_t) v) noexcept {
  return v;
}
inline _TYPES(uint64_t) betoh64(_TYPES(uint64_t) v) noexcept {
  return v;
}
inline _TYPES(uint64_t) htobe64(_TYPES(uint64_t) v) noexcept {
  return v;
}
#if _USE_INT128
inline _TYPES(uint128_t) betoh128(_TYPES(uint128_t) v) noexcept {
  return v;
}
inline _TYPES(uint128_t) htobe128(_TYPES(uint128_t) v) noexcept {
  return v;
}
#endif

inline _TYPES(uint16_t) letoh16(_TYPES(uint16_t) v) noexcept {
  return bswap16(v);
}
inline _TYPES(uint16_t) htole16(_TYPES(uint16_t) v) noexcept {
  return bswap16(v);
}
inline _TYPES(uint32_t) letoh32(_TYPES(uint32_t) v) noexcept {
  return bswap32(v);
}
inline _TYPES(uint32_t) htole32(_TYPES(uint32_t) v) noexcept {
  return bswap32(v);
}
inline _TYPES(uint64_t) letoh64(_TYPES(uint64_t) v) noexcept {
  return bswap64(v);
}
inline _TYPES(uint64_t) htole64(_TYPES(uint64_t) v) noexcept {
  return bswap64(v);
}
#if _USE_INT128
inline _TYPES(uint128_t) letoh128(_TYPES(uint128_t) v) noexcept {
  return bswap128(v);
}
inline _TYPES(uint128_t) htole128(_TYPES(uint128_t) v) noexcept {
  return bswap128(v);
}
#endif

#endif


inline _TYPES(uint16_t) htons(_TYPES(uint16_t) v) noexcept {
  return htobe16(v);
}
inline _TYPES(uint16_t) ntohs(_TYPES(uint16_t) v) noexcept {
  return betoh16(v);
}

inline _TYPES(uint32_t) htonl(_TYPES(uint32_t) v) noexcept {
  return htobe32(v);
}
inline _TYPES(uint32_t) ntohl(_TYPES(uint32_t) v) noexcept {
  return betoh32(v);
}

inline _TYPES(uint64_t) hton64(_TYPES(uint64_t) v) noexcept {
  return htobe64(v);
}
inline _TYPES(uint64_t) ntoh64(_TYPES(uint64_t) v) noexcept {
  return betoh64(v);
}

#if _USE_INT128
inline _TYPES(uint128_t) hton128(_TYPES(uint128_t) v) noexcept {
  return htobe128(v);
}
inline _TYPES(uint128_t) ntoh128(_TYPES(uint128_t) v) noexcept {
  return betoh128(v);
}
#endif


#ifdef __cplusplus

_namespace_begin(std)
namespace impl {


template<typename T, size_t = sizeof(T)> struct endian_converter;  // Undefined

template<typename T>
struct endian_converter<T, 1> {
  static T htobe(T v) { return v; }
  static T betoh(T v) { return v; }

  static T htole(T v) { return v; }
  static T letoh(T v) { return v; }
};

template<typename T>
struct endian_converter<T, 2> {
  static T htobe(T v) { return ::htobe16(v); }
  static T betoh(T v) { return ::betoh16(v); }

  static T htole(T v) { return ::htole16(v); }
  static T letoh(T v) { return ::letoh16(v); }
};

template<typename T>
struct endian_converter<T, 4> {
  static T htobe(T v) { return ::htobe32(v); }
  static T betoh(T v) { return ::betoh32(v); }

  static T htole(T v) { return ::htole32(v); }
  static T letoh(T v) { return ::letoh32(v); }
};

template<typename T>
struct endian_converter<T, 8> {
  static T htobe(T v) { return ::htobe64(v); }
  static T betoh(T v) { return ::betoh64(v); }

  static T htole(T v) { return ::htole64(v); }
  static T letoh(T v) { return ::letoh64(v); }
};

#if _USE_INT128
template<typename T>
struct endian_converter<T, 16> {
  static T htobe(T v) { return ::htobe128(v); }
  static T betoh(T v) { return ::betoh128(v); }

  static T htole(T v) { return ::htole128(v); }
  static T letoh(T v) { return ::letoh128(v); }
};
#endif


} /* namespace std::impl */
_namespace_end(std)

template<typename T>
inline auto htobe(T v) -> std::enable_if_t<std::is_integral<T>::value, T> {
  return ::_namespace(std)::impl::endian_converter<T>::htobe(v);
}

template<typename T>
inline auto betoh(T v) -> std::enable_if_t<std::is_integral<T>::value, T> {
  return ::_namespace(std)::impl::endian_converter<T>::betoh(v);
}

template<typename T>
inline auto htole(T v) -> std::enable_if_t<std::is_integral<T>::value, T> {
  return ::_namespace(std)::impl::endian_converter<T>::htole(v);
}

template<typename T>
inline auto letoh(T v) -> std::enable_if_t<std::is_integral<T>::value, T> {
  return ::_namespace(std)::impl::endian_converter<T>::letoh(v);
}

#endif  // __cplusplus


#endif /* _SYS_ENDIAN_H_ */
