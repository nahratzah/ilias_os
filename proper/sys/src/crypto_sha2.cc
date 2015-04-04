/*
 * FILE:	sha2.c
 * AUTHOR:	Aaron D. Gifford <me@aarongifford.com>
 *
 * Copyright (c) 2015, Ariane van der Steldt
 * Copyright (c) 2000-2001, Aaron D. Gifford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTOR(S) ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTOR(S) BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $From: sha2.c,v 1.1 2001/11/08 00:01:51 adg Exp adg $
 */

#include <crypto/sha2.h>
#include <algorithm>
#include <cstring>
#include <abi/memory.h>

/* Ported to C++ by Ariane. */

namespace ilias {
namespace crypto {


using namespace std;


namespace {


/* Endian reversal macros */
#if defined(__LITTLE_ENDIAN__)
constexpr auto reverse32(uint32_t w) noexcept -> uint32_t {
  uint32_t tmp = (w >> 16) | (w << 16);
  return ((tmp & 0xff00ff00U) >> 8) | ((tmp & 0x00ff00ffU) << 8);
}
constexpr auto reverse64(uint64_t w) noexcept -> uint64_t {
  uint64_t tmp0 = (w >> 32) | (w << 32);
  uint64_t tmp1 = ((tmp0 & 0xff00ff00ff00ff00ULL) >> 8) |
                  ((tmp0 & 0x00ff00ff00ff00ffULL) << 8);
  return ((tmp1 & 0xffff0000ffff0000ULL) >> 16) |
         ((tmp1 & 0x0000ffff0000ffffULL) << 16);
}
#endif

/*
 * Function for incrementally adding the unsigned 64-bit integer n to the
 * unsigned 128-bit integer (represented using a two-element array of
 * 64-bit words).
 */
inline auto addinc128(uint64_t w[2], uint64_t n) noexcept -> void {
  uint64_t carry;

  w[0] = addc(w[0], n, 0, &carry);
  if (carry) ++w[1];
}

/* The six logical functions.
 *
 * Bit shifting and rotation (used by the six SHA-XYZ logical functions:
 *
 *   NOTE:  The naming of R and S appears backwards here (R is a SHIFT and
 *   S is a ROTATION) because the SHA-256/384/512 description document
 *   (see http://csrc.nist.gov/cryptval/shs/sha256-384-512.pdf) uses this
 *   same "backwards" definition.
 */

/* Shift-right (used in SHA-256, SHA-384, and SHA-512) */
constexpr auto R(unsigned int b, uint32_t x) noexcept -> uint32_t {
  return x >> b;
}
constexpr auto R(unsigned int b, uint64_t x) noexcept -> uint64_t {
  return x >> b;
}

/* 32-bit Rotate-right (used in SHA-256) */
constexpr auto S32(unsigned int b, uint32_t x) noexcept -> uint32_t {
  return (x >> b) | (x << (32 - b));
}
constexpr auto S64(unsigned int b, uint32_t x) noexcept -> uint32_t {
  return (x >> b) | (x << (64 - b));
}

/* Two of six logical functions used in SHA-256, SHA-384, and SHA-512 */
constexpr auto Ch(uint32_t x, uint32_t y, uint32_t z) noexcept -> uint32_t {
  return (x & y) ^ (~x & z);
}
constexpr auto Ch(uint64_t x, uint64_t y, uint64_t z) noexcept -> uint64_t {
  return (x & y) ^ (~x & z);
}

constexpr auto Maj(uint32_t x, uint32_t y, uint32_t z) noexcept -> uint32_t {
  return (x & y) ^ (x & z) ^ (y & z);
}
constexpr auto Maj(uint64_t x, uint64_t y, uint64_t z) noexcept -> uint64_t {
  return (x & y) ^ (x & z) ^ (y & z);
}

/* Four of six logical functions used in SHA-256 */
constexpr auto Sigma0_256(uint32_t x) noexcept -> uint32_t {
  return S32(2,  x) ^ S32(13, x) ^ S32(22, x);
}
constexpr auto Sigma1_256(uint32_t x) noexcept -> uint32_t {
  return S32(6,  x) ^ S32(11, x) ^ S32(25, x);
}
constexpr auto sigma0_256(uint32_t x) noexcept -> uint32_t {
  return S32(7,  x) ^ S32(18, x) ^ R(3 ,   x);
}
constexpr auto sigma1_256(uint32_t x) noexcept -> uint32_t {
  return S32(17, x) ^ S32(19, x) ^ R(10,   x);
}

/* Four of six logical functions used in SHA-384 and SHA-512 */
constexpr auto Sigma0_512(uint64_t x) noexcept -> uint64_t {
  return S64(28, x) ^ S64(34, x) ^ S64(39, x);
}
constexpr auto Sigma1_512(uint64_t x) noexcept -> uint64_t {
  return S64(14, x) ^ S64(18, x) ^ S64(41, x);
}
constexpr auto sigma0_512(uint64_t x) noexcept -> uint64_t {
  return S64( 1, x) ^ S64( 8, x) ^ R( 7,   x);
}
constexpr auto sigma1_512(uint64_t x) noexcept -> uint64_t {
  return S64(19, x) ^ S64(61, x) ^ R( 6,   x);
}

/* Hash constant words K for SHA-256 */
constexpr array<uint32_t, 64> K256{{
  0x428a2f98U, 0x71374491U, 0xb5c0fbcfU, 0xe9b5dba5U,
  0x3956c25bU, 0x59f111f1U, 0x923f82a4U, 0xab1c5ed5U,
  0xd807aa98U, 0x12835b01U, 0x243185beU, 0x550c7dc3U,
  0x72be5d74U, 0x80deb1feU, 0x9bdc06a7U, 0xc19bf174U,
  0xe49b69c1U, 0xefbe4786U, 0x0fc19dc6U, 0x240ca1ccU,
  0x2de92c6fU, 0x4a7484aaU, 0x5cb0a9dcU, 0x76f988daU,
  0x983e5152U, 0xa831c66dU, 0xb00327c8U, 0xbf597fc7U,
  0xc6e00bf3U, 0xd5a79147U, 0x06ca6351U, 0x14292967U,
  0x27b70a85U, 0x2e1b2138U, 0x4d2c6dfcU, 0x53380d13U,
  0x650a7354U, 0x766a0abbU, 0x81c2c92eU, 0x92722c85U,
  0xa2bfe8a1U, 0xa81a664bU, 0xc24b8b70U, 0xc76c51a3U,
  0xd192e819U, 0xd6990624U, 0xf40e3585U, 0x106aa070U,
  0x19a4c116U, 0x1e376c08U, 0x2748774cU, 0x34b0bcb5U,
  0x391c0cb3U, 0x4ed8aa4aU, 0x5b9cca4fU, 0x682e6ff3U,
  0x748f82eeU, 0x78a5636fU, 0x84c87814U, 0x8cc70208U,
  0x90befffaU, 0xa4506cebU, 0xbef9a3f7U, 0xc67178f2U
}};

/* Hash constant words K for SHA-384 and SHA-512 */
constexpr array<uint64_t, 80> K512{{
  0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL,
  0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
  0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
  0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
  0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
  0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
  0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL,
  0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
  0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
  0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
  0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL,
  0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
  0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL,
  0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
  0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
  0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
  0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL,
  0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
  0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL,
  0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
  0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
  0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
  0xd192e819d6ef5218ULL, 0xd69906245565a910ULL,
  0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
  0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
  0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
  0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
  0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
  0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL,
  0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
  0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL,
  0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
  0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
  0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
  0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
  0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
  0x28db77f523047d84ULL, 0x32caab7b40c72493ULL,
  0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
  0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
  0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
}};


} /* namespace ilias::crypto::<unnamed> */


sha256::~sha256() noexcept {
  abi::safe_memzero(state_.data(), sizeof(state_));
  abi::safe_memzero(&bitcount_, sizeof(bitcount_));
  abi::safe_memzero(buffer_.data(), sizeof(buffer_));
}

auto sha256::update(const uint8_t* data, size_t len) noexcept -> void {
  if (len == 0) return;

  size_t usedspace = (bitcount_ >> 3) % BLOCK_LENGTH;
  if (usedspace > 0) {
    /* Calculate how much free space is available in the buffer */
    size_t freespace = BLOCK_LENGTH - usedspace;

    if (len >= freespace) {
      /* Fill the buffer completely and process it */
      bcopy(data, buffer_.data() + usedspace, freespace);
      bitcount_ += freespace << 3;
      len -= freespace;
      data += freespace;
      transform_(buffer_.data());
    } else {
      /* The buffer is not yet full */
      bcopy(data, buffer_.data() + usedspace, len);
      bitcount_ += len << 3;
      /* Clean up */
      usedspace = freespace = 0;
      return;
    }

    freespace = 0;
  }

  while (len >= BLOCK_LENGTH) {
    /* Process as many complete blocks as we can */
    transform_(data);
    bitcount_ += BLOCK_LENGTH << 3;
    len -= BLOCK_LENGTH;
    data += BLOCK_LENGTH;
  }

  if (len > 0) {
    /* There's left-overs, so save 'em */
    bcopy(data, buffer_.data(), len);
    bitcount_ += len << 3;
  }
  /* Clean up */
  usedspace = 0;
}

auto sha256::finalize() noexcept -> digest_type {
  digest_type rv;
  finalize(rv.data());
  return rv;
}

auto sha256::finalize(uint8_t rv[DIGEST_LENGTH]) noexcept -> void {
  constexpr size_t SHORT_BLOCK_LENGTH = BLOCK_LENGTH - 8;
  array<uint32_t, DIGEST_LENGTH / 4> d;

  size_t usedspace = (bitcount_ >> 3) % BLOCK_LENGTH;
#if defined(__LITTLE_ENDIAN__)
  /* Convert from host order */
  bitcount_ = reverse64(bitcount_);
#endif
  if (usedspace > 0) {
    /* Begin padding with a 1 bit */
    buffer_[usedspace++] = 0x80U;

    if (usedspace <= SHORT_BLOCK_LENGTH) {
      /* Set-up for the last transform */
      bzero(buffer_.data() + usedspace, SHORT_BLOCK_LENGTH - usedspace);
    } else {
      if (usedspace < BLOCK_LENGTH) {
        bzero(buffer_.data() + usedspace, BLOCK_LENGTH - usedspace);
      }
      /* Do second-to-last transform */
      transform_(buffer_.data());
      /* And set-up for the last transform */
      bzero(buffer_.data(), SHORT_BLOCK_LENGTH);
    }
  } else {
    /* Set-up for the last transform */
    bzero(buffer_.data(), SHORT_BLOCK_LENGTH);
    /* Begin padding with a 1 bit */
    buffer_[0] = 0x80U;
  }

  /* Set the bit count */
  bcopy(&bitcount_, buffer_.data() + SHORT_BLOCK_LENGTH,
        sizeof(bitcount_));

  /* Final transform */
  transform_(buffer_.data());

#if defined(__LITTLE_ENDIAN__)
  /* Convert TO host byte order */
  transform(state_.begin(), state_.end(), d.begin(), &reverse32);
#else
  d = state_;
#endif

  if (rv) bcopy(d.data(), rv, DIGEST_LENGTH);
  abi::safe_memzero(state_.data(), sizeof(state_));
}

auto sha256::calculate(const uint8_t* input, size_t len) noexcept ->
    digest_type {
  sha256 ctx;
  ctx.update(input, len);
  return ctx.finalize();
}

auto sha256::calculate(uint8_t rv[DIGEST_LENGTH],
                       const uint8_t* input, size_t len) noexcept -> void {
  sha256 ctx;
  ctx.update(input, len);
  ctx.finalize(rv);
}

auto sha256::transform_(const uint8_t* data) noexcept -> void {
  uint32_t a, b, c, d, e, f, g, h;
  uint32_t* W256 = reinterpret_cast<uint32_t*>(buffer_.data());

  /* Initialize registers with the prev. intermediate value */
  tie(a, b, c, d, e, f, g, h) =
      make_tuple(state_[0], state_[1], state_[2], state_[3],
                 state_[4], state_[5], state_[6], state_[7]);

  for (int j = 0; j != 16; ++j) {
    W256[j] = (static_cast<uint32_t>(data[3])      ) |
              (static_cast<uint32_t>(data[2]) <<  8) |
              (static_cast<uint32_t>(data[1]) << 16) |
              (static_cast<uint32_t>(data[0]) << 24);
    data += 4;
    /* Apply the SHA-256 compression function to update a..h */
    const uint32_t T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + W256[j];
    const uint32_t T2 = Sigma0_256(a) + Maj(a, b, c);
    tie(a, b, c, d, e, f, g, h) =
        make_tuple(T1 + T2, a, b, c, d + T1, e, f, g);
  }

  for (int j = 16; j != 64; ++j) {
    /* Part of the message block expansion */
    const uint32_t s0 = sigma0_256(W256[(j +  1) & 0xf]);
    const uint32_t s1 = sigma1_256(W256[(j + 14) & 0xf]);

    /* Apply the SHA-256 compression function to update a..h */
    const uint32_t T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] +
         (W256[j & 0xf] += s1 + W256[(j + 9) & 0xf] + s0);
    const uint32_t T2 = Sigma0_256(a) + Maj(a, b, c);

    tie(a, b, c, d, e, f, g, h) =
        make_tuple(T1 + T2, a, b, c, d + T1, e, f, g);
  }

  /* Compute the current intermediate hash value */
  state_[0] += a;
  state_[1] += b;
  state_[2] += c;
  state_[3] += d;
  state_[4] += e;
  state_[5] += f;
  state_[6] += g;
  state_[7] += h;

  /* Clean up */
  a = b = c = d = e = f = g = h = 0;
}


sha512::~sha512() noexcept {
  abi::safe_memzero(state_.data(), sizeof(state_));
  abi::safe_memzero(bitcount_.data(), sizeof(bitcount_));
  abi::safe_memzero(buffer_.data(), sizeof(buffer_));
}

auto sha512::update(const uint8_t* data, size_t len) noexcept -> void {
  if (len == 0) return;

  size_t usedspace = (bitcount_[0] >> 3) % BLOCK_LENGTH;
  if (usedspace > 0) {
    /* Calculate how much free space is available in the buffer */
    size_t freespace = BLOCK_LENGTH - usedspace;

    if (len >= freespace) {
      /* Fill the buffer completely and process it */
      bcopy(data, buffer_.data() + usedspace, freespace);
      addinc128(bitcount_.data(), freespace << 3);
      len -= freespace;
      data += freespace;
      transform_(buffer_.data());
    } else {
      /* The buffer is not yet full */
      bcopy(data, buffer_.data() + usedspace, len);
      addinc128(bitcount_.data(), len << 3);
      /* Clean up */
      usedspace = freespace = 0;
      return;
    }

    freespace = 0;
  }

  while (len >= BLOCK_LENGTH) {
    /* Process as many complete blocks as we can */
    transform_(data);
    addinc128(bitcount_.data(), BLOCK_LENGTH << 3);
    len -= BLOCK_LENGTH;
    data += BLOCK_LENGTH;
  }

  if (len > 0) {
    /* There's left-overs, so save 'em */
    bcopy(data, buffer_.data(), len);
    addinc128(bitcount_.data(), len << 3);
  }
  /* Clean up */
  usedspace = 0;
}

auto sha512::finalize() noexcept -> digest_type {
  digest_type rv;
  finalize(rv.data());
  return rv;
}

auto sha512::finalize(uint8_t rv[DIGEST_LENGTH]) noexcept -> void {
  constexpr size_t SHORT_BLOCK_LENGTH = BLOCK_LENGTH - 16;
  array<uint64_t, DIGEST_LENGTH / 8> d;

  size_t usedspace = (bitcount_[0] >> 3) % BLOCK_LENGTH;
#if defined(__LITTLE_ENDIAN__)
  /* Convert from host order */
  transform(bitcount_.begin(), bitcount_.end(), bitcount_.begin(), &reverse64);
#endif
  if (usedspace > 0) {
    /* Begin padding with a 1 bit */
    buffer_[usedspace++] = 0x80U;

    if (usedspace <= SHORT_BLOCK_LENGTH) {
      /* Set-up for the last transform */
      bzero(buffer_.data() + usedspace, SHORT_BLOCK_LENGTH - usedspace);
    } else {
      if (usedspace < BLOCK_LENGTH) {
        bzero(buffer_.data() + usedspace, BLOCK_LENGTH - usedspace);
      }
      /* Do second-to-last transform */
      transform_(buffer_.data());
      /* And set-up for the last transform */
      bzero(buffer_.data(), BLOCK_LENGTH - 2);
    }
  } else {
    /* Set-up for the last transform */
    bzero(buffer_.data(), SHORT_BLOCK_LENGTH);
    /* Begin padding with a 1 bit */
    buffer_[0] = 0x80U;
  }

  /* Store the length of input data (in bits) */
  bcopy(&bitcount_[1], buffer_.data() + SHORT_BLOCK_LENGTH,
        sizeof(bitcount_[1]));
  bcopy(&bitcount_[0], buffer_.data() + SHORT_BLOCK_LENGTH + 8,
        sizeof(bitcount_[0]));

  /* Final transform */
  transform_(buffer_.data());

#if defined(__LITTLE_ENDIAN__)
  /* Convert TO host byte order */
  transform(state_.begin(), state_.end(), d.begin(), &reverse64);
#else
  d = state_;
#endif

  if (rv) bcopy(d.data(), rv, DIGEST_LENGTH);
  abi::safe_memzero(state_.data(), sizeof(state_));
}

auto sha512::calculate(const uint8_t* input, size_t len) noexcept ->
    digest_type {
  sha512 ctx;
  ctx.update(input, len);
  return ctx.finalize();
}

auto sha512::calculate(uint8_t rv[DIGEST_LENGTH],
                       const uint8_t* input, size_t len) noexcept -> void {
  sha512 ctx;
  ctx.update(input, len);
  ctx.finalize(rv);
}

auto sha512::transform_(const uint8_t* data) noexcept -> void {
  uint64_t a, b, c, d, e, f, g, h;
  uint64_t* W512 = reinterpret_cast<uint64_t*>(buffer_.data());

  tie(a, b, c, d, e, f, g, h) =
      make_tuple(state_[0], state_[1], state_[2], state_[3],
                 state_[4], state_[5], state_[6], state_[7]);

  for (int j = 0; j != 16; ++j) {
    W512[j] = (static_cast<uint64_t>(data[7])      ) |
              (static_cast<uint64_t>(data[6]) <<  8) |
              (static_cast<uint64_t>(data[5]) << 16) |
              (static_cast<uint64_t>(data[4]) << 24) |
              (static_cast<uint64_t>(data[3]) << 32) |
              (static_cast<uint64_t>(data[2]) << 40) |
              (static_cast<uint64_t>(data[1]) << 48) |
              (static_cast<uint64_t>(data[0]) << 56);
    data += 8;
    /* Apply the SHA-512 compression function to update a..h */
    const uint64_t T1 = h + Sigma1_512(e) + Ch(e, f, g) + K512[j] + W512[j];
    const uint64_t T2 = Sigma0_512(a) + Maj(a, b, c);
    tie(a, b, c, d, e, f, g, h) =
        make_tuple(T1 + T2, a, b, c, d + T1, e, f, g);
  }

  for (int j = 16; j != 80; ++j) {
    /* Part of the message block expansion */
    const uint64_t s0 = sigma0_512(W512[(j +  1) & 0xf]);
    const uint64_t s1 = sigma1_512(W512[(j + 14) & 0xf]);

    /* Apply the SHA-512 compression function to update a..h */
    const uint64_t T1 = h + Sigma1_512(e) + Ch(e, f, g) + K512[j] +
        (W512[j & 0xf] += s1 + W512[(j + 9) & 0xf] + s0);
    const uint64_t T2 = Sigma0_512(a) + Maj(a, b, c);
    tie(a, b, c, d, e, f, g, h) =
        make_tuple(T1 + T2, a, b, c, d + T1, e, f, g);
  }

  /* Compute the current intermediate hash value */
  state_[0] += a;
  state_[1] += b;
  state_[2] += c;
  state_[3] += d;
  state_[4] += e;
  state_[5] += f;
  state_[6] += g;
  state_[7] += h;

  /* Clean up */
  a = b = c = d = e = f = g = h = 0;
}


sha384::~sha384() noexcept {
  abi::safe_memzero(state_.data(), sizeof(state_));
  abi::safe_memzero(bitcount_.data(), sizeof(bitcount_));
  abi::safe_memzero(buffer_.data(), sizeof(buffer_));
}

auto sha384::update(const uint8_t* data, size_t len) noexcept -> void {
  if (len == 0) return;

  size_t usedspace = (bitcount_[0] >> 3) % BLOCK_LENGTH;
  if (usedspace > 0) {
    /* Calculate how much free space is available in the buffer */
    size_t freespace = BLOCK_LENGTH - usedspace;

    if (len >= freespace) {
      /* Fill the buffer completely and process it */
      bcopy(data, buffer_.data() + usedspace, freespace);
      addinc128(bitcount_.data(), freespace << 3);
      len -= freespace;
      data += freespace;
      transform_(buffer_.data());
    } else {
      /* The buffer is not yet full */
      bcopy(data, buffer_.data() + usedspace, len);
      addinc128(bitcount_.data(), len << 3);
      /* Clean up */
      usedspace = freespace = 0;
      return;
    }

    freespace = 0;
  }

  while (len >= BLOCK_LENGTH) {
    /* Process as many complete blocks as we can */
    transform_(data);
    addinc128(bitcount_.data(), BLOCK_LENGTH << 3);
    len -= BLOCK_LENGTH;
    data += BLOCK_LENGTH;
  }

  if (len > 0) {
    /* There's left-overs, so save 'em */
    bcopy(data, buffer_.data(), len);
    addinc128(bitcount_.data(), len << 3);
  }
  /* Clean up */
  usedspace = 0;
}

auto sha384::finalize() noexcept -> digest_type {
  digest_type rv;
  finalize(rv.data());
  return rv;
}

auto sha384::finalize(uint8_t rv[DIGEST_LENGTH]) noexcept -> void {
  constexpr size_t SHORT_BLOCK_LENGTH = BLOCK_LENGTH - 16;
  array<uint64_t, DIGEST_LENGTH / 8> d;

  size_t usedspace = (bitcount_[0] >> 3) % BLOCK_LENGTH;
#if defined(__LITTLE_ENDIAN__)
  /* Convert from host order */
  transform(bitcount_.begin(), bitcount_.end(), bitcount_.begin(), &reverse64);
#endif
  if (usedspace > 0) {
    /* Begin padding with a 1 bit */
    buffer_[usedspace++] = 0x80U;

    if (usedspace <= SHORT_BLOCK_LENGTH) {
      /* Set-up for the last transform */
      bzero(buffer_.data() + usedspace, SHORT_BLOCK_LENGTH - usedspace);
    } else {
      if (usedspace < BLOCK_LENGTH) {
        bzero(buffer_.data() + usedspace, BLOCK_LENGTH - usedspace);
      }
      /* Do second-to-last transform */
      transform_(buffer_.data());
      /* And set-up for the last transform */
      bzero(buffer_.data(), BLOCK_LENGTH - 2);
    }
  } else {
    /* Set-up for the last transform */
    bzero(buffer_.data(), SHORT_BLOCK_LENGTH);
    /* Begin padding with a 1 bit */
    buffer_[0] = 0x80U;
  }

  /* Store the length of input data (in bits) */
  bcopy(&bitcount_[1], buffer_.data() + SHORT_BLOCK_LENGTH,
        sizeof(bitcount_[1]));
  bcopy(&bitcount_[0], buffer_.data() + SHORT_BLOCK_LENGTH + 8,
        sizeof(bitcount_[0]));

  /* Final transform */
  transform_(buffer_.data());

#if defined(__LITTLE_ENDIAN__)
  /* Convert TO host byte order */
  transform(state_.begin(), state_.begin() + d.size(), d.begin(), &reverse64);
#else
  copy_n(state_.begin(), d.size(), d.begin());
#endif

  if (rv) bcopy(d.data(), rv, DIGEST_LENGTH);
  abi::safe_memzero(state_.data(), sizeof(state_));
}

auto sha384::calculate(const uint8_t* input, size_t len) noexcept ->
    digest_type {
  sha384 ctx;
  ctx.update(input, len);
  return ctx.finalize();
}

auto sha384::calculate(uint8_t rv[DIGEST_LENGTH],
                       const uint8_t* input, size_t len) noexcept -> void {
  sha384 ctx;
  ctx.update(input, len);
  ctx.finalize(rv);
}

auto sha384::transform_(const uint8_t* data) noexcept -> void {
  uint64_t a, b, c, d, e, f, g, h;
  uint64_t* W512 = reinterpret_cast<uint64_t*>(buffer_.data());

  tie(a, b, c, d, e, f, g, h) =
      make_tuple(state_[0], state_[1], state_[2], state_[3],
                 state_[4], state_[5], state_[6], state_[7]);

  for (int j = 0; j != 16; ++j) {
    W512[j] = (static_cast<uint64_t>(data[7])      ) |
              (static_cast<uint64_t>(data[6]) <<  8) |
              (static_cast<uint64_t>(data[5]) << 16) |
              (static_cast<uint64_t>(data[4]) << 24) |
              (static_cast<uint64_t>(data[3]) << 32) |
              (static_cast<uint64_t>(data[2]) << 40) |
              (static_cast<uint64_t>(data[1]) << 48) |
              (static_cast<uint64_t>(data[0]) << 56);
    data += 8;
    /* Apply the SHA-512 compression function to update a..h */
    const uint64_t T1 = h + Sigma1_512(e) + Ch(e, f, g) + K512[j] + W512[j];
    const uint64_t T2 = Sigma0_512(a) + Maj(a, b, c);
    tie(a, b, c, d, e, f, g, h) =
        make_tuple(T1 + T2, a, b, c, d + T1, e, f, g);
  }

  for (int j = 16; j != 80; ++j) {
    /* Part of the message block expansion */
    const uint64_t s0 = sigma0_512(W512[(j +  1) & 0xf]);
    const uint64_t s1 = sigma1_512(W512[(j + 14) & 0xf]);

    /* Apply the SHA-512 compression function to update a..h */
    const uint64_t T1 = h + Sigma1_512(e) + Ch(e, f, g) + K512[j] +
        (W512[j & 0xf] += s1 + W512[(j + 9) & 0xf] + s0);
    const uint64_t T2 = Sigma0_512(a) + Maj(a, b, c);
    tie(a, b, c, d, e, f, g, h) =
        make_tuple(T1 + T2, a, b, c, d + T1, e, f, g);
  }

  /* Compute the current intermediate hash value */
  state_[0] += a;
  state_[1] += b;
  state_[2] += c;
  state_[3] += d;
  state_[4] += e;
  state_[5] += f;
  state_[6] += g;
  state_[7] += h;

  /* Clean up */
  a = b = c = d = e = f = g = h = 0;
}


}} /* namespace ilias::crypto */
