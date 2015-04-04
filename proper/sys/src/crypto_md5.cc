#include <crypto/md5.h>
#include <algorithm>
#include <cstring>
#include <tuple>
#include <abi/memory.h>

/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 */

namespace ilias {
namespace crypto {


using namespace std;


constexpr size_t md5::MD5_BLOCK_LENGTH;
constexpr size_t md5::MD5_DIGEST_LENGTH;


namespace {


inline auto put_64bit_le(uint8_t* cp, uint64_t value) noexcept -> void {
  cp[7] = value >> 56;
  cp[6] = value >> 48;
  cp[5] = value >> 40;
  cp[4] = value >> 32;
  cp[3] = value >> 24;
  cp[2] = value >> 16;
  cp[1] = value >>  8;
  cp[0] = value >>  0;
}

inline auto put_32bit_le(uint8_t* cp, uint32_t value) noexcept -> void {
  cp[3] = value >> 24;
  cp[2] = value >> 16;
  cp[1] = value >>  8;
  cp[0] = value >>  0;
}

constexpr array<uint8_t, md5::MD5_BLOCK_LENGTH> PADDING{{
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
}};

/* The four core functions - F1 is optimized somewhat */

constexpr auto F1(uint32_t x, uint32_t y, uint32_t z) noexcept -> uint32_t {
  return z ^ (x & (y ^ z));
}

constexpr auto F2(uint32_t x, uint32_t y, uint32_t z) noexcept -> uint32_t {
  return F1(z, x, y);
}

constexpr auto F3(uint32_t x, uint32_t y, uint32_t z) noexcept -> uint32_t {
  return x ^ y ^ z;
}

constexpr auto F4(uint32_t x, uint32_t y, uint32_t z) noexcept -> uint32_t {
  return y ^ (x | ~z);
}

/* This is the central step in the MD5 algorithm. */
constexpr auto MD5STEP(uint32_t (*f)(uint32_t, uint32_t, uint32_t),
                       uint32_t& w, uint32_t& x, uint32_t& y, uint32_t& z,
                       uint32_t data, unsigned int s) noexcept -> uint32_t {
  w += (*f)(x, y, z) + data;
  w = (w << s) | (w >> (32 - s));
  return w += x;
}

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  md5::update blocks
 * the data and convers bytes into longwords for this routine.
 */
inline auto md5_transform(uint32_t state[4],
                          const uint8_t block[md5::MD5_BLOCK_LENGTH])
    noexcept -> void {
  uint32_t a, b, c, d;
  array<uint32_t, md5::MD5_BLOCK_LENGTH / 4> in;

#if defined(__BIG_ENDIAN__)
  bcopy(block, in, sizeof(in));
#else
  {
    const uint8_t* block_iter = block;
    for (uint32_t& in_iter : in) {
      in_iter = (static_cast<uint32_t>(block_iter[0]) <<  0) |
                (static_cast<uint32_t>(block_iter[1]) <<  8) |
                (static_cast<uint32_t>(block_iter[2]) << 16) |
                (static_cast<uint32_t>(block_iter[3]) << 24);
    }
  }
#endif

  tie(a, b, c, d) = make_tuple(state[0], state[1], state[2], state[3]);

  MD5STEP(F1, a, b, c, d, in[ 0] + 0xd76aa478U,  7);
  MD5STEP(F1, d, a, b, c, in[ 1] + 0xe8c7b756U, 12);
  MD5STEP(F1, c, d, a, b, in[ 2] + 0x242070dbU, 17);
  MD5STEP(F1, b, c, d, a, in[ 3] + 0xc1bdceeeU, 22);
  MD5STEP(F1, a, b, c, d, in[ 4] + 0xf57c0fafU,  7);
  MD5STEP(F1, d, a, b, c, in[ 5] + 0x4787c62aU, 12);
  MD5STEP(F1, c, d, a, b, in[ 6] + 0xa8304613U, 17);
  MD5STEP(F1, b, c, d, a, in[ 7] + 0xfd469501U, 22);
  MD5STEP(F1, a, b, c, d, in[ 8] + 0x698098d8U,  7);
  MD5STEP(F1, d, a, b, c, in[ 9] + 0x8b44f7afU, 12);
  MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1U, 17);
  MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7beU, 22);
  MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122U,  7);
  MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193U, 12);
  MD5STEP(F1, c, d, a, b, in[14] + 0xa679438eU, 17);
  MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821U, 22);

  MD5STEP(F2, a, b, c, d, in[ 1] + 0xf61e2562U,  5);
  MD5STEP(F2, d, a, b, c, in[ 6] + 0xc040b340U,  9);
  MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51U, 14);
  MD5STEP(F2, b, c, d, a, in[ 0] + 0xe9b6c7aaU, 20);
  MD5STEP(F2, a, b, c, d, in[ 5] + 0xd62f105dU,  5);
  MD5STEP(F2, d, a, b, c, in[10] + 0x02441453U,  9);
  MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681U, 14);
  MD5STEP(F2, b, c, d, a, in[ 4] + 0xe7d3fbc8U, 20);
  MD5STEP(F2, a, b, c, d, in[ 9] + 0x21e1cde6U,  5);
  MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6U,  9);
  MD5STEP(F2, c, d, a, b, in[ 3] + 0xf4d50d87U, 14);
  MD5STEP(F2, b, c, d, a, in[ 8] + 0x455a14edU, 20);
  MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905U,  5);
  MD5STEP(F2, d, a, b, c, in[ 2] + 0xfcefa3f8U,  9);
  MD5STEP(F2, c, d, a, b, in[ 7] + 0x676f02d9U, 14);
  MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8aU, 20);

  MD5STEP(F3, a, b, c, d, in[ 5] + 0xfffa3942U,  4);
  MD5STEP(F3, d, a, b, c, in[ 8] + 0x8771f681U, 11);
  MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122U, 16);
  MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380cU, 23);
  MD5STEP(F3, a, b, c, d, in[ 1] + 0xa4beea44U,  4);
  MD5STEP(F3, d, a, b, c, in[ 4] + 0x4bdecfa9U, 11);
  MD5STEP(F3, c, d, a, b, in[ 7] + 0xf6bb4b60U, 16);
  MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70U, 23);
  MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6U,  4);
  MD5STEP(F3, d, a, b, c, in[ 0] + 0xeaa127faU, 11);
  MD5STEP(F3, c, d, a, b, in[ 3] + 0xd4ef3085U, 16);
  MD5STEP(F3, b, c, d, a, in[ 6] + 0x04881d05U, 23);
  MD5STEP(F3, a, b, c, d, in[ 9] + 0xd9d4d039U,  4);
  MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5U, 11);
  MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8U, 16);
  MD5STEP(F3, b, c, d, a, in[ 2] + 0xc4ac5665U, 23);

  MD5STEP(F4, a, b, c, d, in[ 0] + 0xf4292244,  6);
  MD5STEP(F4, d, a, b, c, in[ 7] + 0x432aff97, 10);
  MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
  MD5STEP(F4, b, c, d, a, in[ 5] + 0xfc93a039, 21);
  MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3,  6);
  MD5STEP(F4, d, a, b, c, in[ 3] + 0x8f0ccc92, 10);
  MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
  MD5STEP(F4, b, c, d, a, in[ 1] + 0x85845dd1, 21);
  MD5STEP(F4, a, b, c, d, in[ 8] + 0x6fa87e4f,  6);
  MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
  MD5STEP(F4, c, d, a, b, in[ 6] + 0xa3014314, 15);
  MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
  MD5STEP(F4, a, b, c, d, in[ 4] + 0xf7537e82,  6);
  MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
  MD5STEP(F4, c, d, a, b, in[ 2] + 0x2ad7d2bb, 15);
  MD5STEP(F4, b, c, d, a, in[ 9] + 0xeb86d391, 21);

  tie(state[0], state[1], state[2], state[3]) = make_tuple(a, b, c, d);
}


} /* namespace ilias::crypto::<unnamed> */


md5::~md5() noexcept {
  /* Zero memory in case it's sensitive. */
  abi::safe_memzero(state_.data(), sizeof(state_));
  abi::safe_memzero(&count_, sizeof(count_));
  abi::safe_memzero(buffer_.data(), sizeof(buffer_));
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
auto md5::update(const uint8_t* input, size_t len) noexcept -> void {
  /* Check how many bytes we already have and how many more we need. */
  size_t have = static_cast<size_t>(count_ >> 3) % MD5_BLOCK_LENGTH;
  size_t need = MD5_BLOCK_LENGTH - have;

  /* Update bitcount */
  count_ += static_cast<uint64_t>(len) << 3;

  if (len >= need) {
    if (have != 0) {
      copy_n(input, need, buffer_.begin() + have);
      md5_transform(state_.data(), buffer_.data());
      input += need;
      len -= need;
      have = 0;
    }

    /* Process data in MD5_BLOCK_LENGTH-byte chunks. */
    while (len >= MD5_BLOCK_LENGTH) {
      md5_transform(state_.data(), input);
      input += MD5_BLOCK_LENGTH;
      len -= MD5_BLOCK_LENGTH;
    }
  }

  /* Handle any remaining bytes of data. */
  copy_n(input, len, buffer_.begin() + have);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
auto md5::finalize(uint8_t rv[MD5_DIGEST_LENGTH]) noexcept -> void {
  /* Convert count to 8 bytes in little endian order. */
  array<uint8_t, 8> count;
  put_64bit_le(count.begin(), count_);

  /* Pad out to 56 mod 64. */
  size_t padlen = MD5_BLOCK_LENGTH - ((count_ >> 3) % MD5_BLOCK_LENGTH);
  if (padlen < 1 + 8) padlen += MD5_BLOCK_LENGTH;
  update(PADDING.data(), padlen - 8);  // padlen - 8 <= 64 */
  update(count.data(), count.size());

  uint8_t* rv_iter = rv;
  for (auto s : state_) {
    put_32bit_le(rv_iter, s);
    rv_iter += 4;
  }
}

/*
 * Variant of finalize that returns a buffer.
 */
auto md5::finalize() noexcept -> digest_type {
  digest_type rv;
  finalize(rv.data());
  return rv;
}

/*
 * Convenience function, for if the entire byte stream is available.
 */
auto md5::calculate(const uint8_t* input, size_t len) noexcept -> digest_type {
  md5 ctx;
  ctx.update(input, len);
  return ctx.finalize();
}

/*
 * Convenience function, for if the entire byte stream is available.
 */
auto md5::calculate(uint8_t rv[MD5_DIGEST_LENGTH],
                    const uint8_t* input, size_t len) noexcept -> void {
  md5 ctx;
  ctx.update(input, len);
  return ctx.finalize(rv);
}


}} /* namespace ilias::crypto */
