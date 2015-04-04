#include <crypto/sha1.h>
#include <abi/memory.h>

/*
 * SHA-1 in C
 * By Steve Reid <steve@edmweb.com>
 * 100% Public Domain
 *
 * Test Vectors (from FIPS PUB 180-1)
 * "abc"
 *   A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
 * "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
 *   84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
 * A million repetitions of "a"
 *   34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
 *
 * Ported to C++ by Ariane
 */

namespace ilias {
namespace crypto {


using namespace std;

constexpr size_t sha1::BLOCK_LENGTH;
constexpr size_t sha1::DIGEST_LENGTH;


namespace {


class block {
 public:
  block(const uint8_t buffer[sha1::BLOCK_LENGTH]) noexcept;
  ~block() noexcept;

  /* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
  void R0(uint32_t&, uint32_t&, uint32_t&, uint32_t&, uint32_t&, unsigned int)
      noexcept;
  void R1(uint32_t&, uint32_t&, uint32_t&, uint32_t&, uint32_t&, unsigned int)
      noexcept;
  void R2(uint32_t&, uint32_t&, uint32_t&, uint32_t&, uint32_t&, unsigned int)
      noexcept;
  void R3(uint32_t&, uint32_t&, uint32_t&, uint32_t&, uint32_t&, unsigned int)
      noexcept;
  void R4(uint32_t&, uint32_t&, uint32_t&, uint32_t&, uint32_t&, unsigned int)
      noexcept;

 private:
  static constexpr auto rol(uint32_t value, unsigned int bits) noexcept ->
      uint32_t {
    return (value << bits) | (value >> (32 - bits));
  }

  uint32_t blk(unsigned int i) noexcept;
  uint32_t blk0(unsigned int i) noexcept;

  array<uint32_t, 16> l_;
};


inline block::block(const uint8_t buffer[sha1::BLOCK_LENGTH]) noexcept {
  bcopy(buffer, l_.data(), sha1::BLOCK_LENGTH);
}

inline block::~block() noexcept {
  abi::safe_memzero(l_.data(), sizeof(l_));
}

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
inline auto block::blk0(unsigned int i) noexcept -> uint32_t {
#if defined(__LITTLE_ENDIAN__)
  l_[i] = (rol(l_[i], 24) & 0xff00ff00U) |
          (rol(l_[i],  8) & 0x00ff00ffU);
#endif
  return l_[i];
}

inline auto block::blk(unsigned int i) noexcept -> uint32_t {
  return l_[i & 15] = rol(l_[(i + 13) & 15] ^
                          l_[(i +  8) & 15] ^
                          l_[(i +  2) & 15] ^
                          l_[(i     ) & 15], 1);
}

inline auto block::R0(uint32_t& v, uint32_t& w, uint32_t& x,
                      uint32_t& y, uint32_t& z,
                      unsigned int i) noexcept -> void {
  z += ((w & (x ^ y)) ^ y) + blk0(i) + 0x5a827999U + rol(v, 5);
  w = rol(w, 30);
}

inline auto block::R1(uint32_t& v, uint32_t& w, uint32_t& x,
                      uint32_t& y, uint32_t& z,
                      unsigned int i) noexcept -> void {
  z += ((w & (x ^ y)) ^ y) + blk(i) + 0x5a827999U + rol(v, 5);
  w = rol(w, 30);
}

inline auto block::R2(uint32_t& v, uint32_t& w, uint32_t& x,
                      uint32_t& y, uint32_t& z,
                      unsigned int i) noexcept -> void {
  z += (w ^ x ^ y) + blk(i) + 0x6ed9eba1U + rol(v, 5);
  w = rol(w, 30);
}

inline auto block::R3(uint32_t& v, uint32_t& w, uint32_t& x,
                      uint32_t& y, uint32_t& z,
                      unsigned int i) noexcept -> void {
  z += (((w | x) & y) | (w & x)) + blk(i) + 0x8f1bbcdcU + rol(v, 5);
  w = rol(w, 30);
}

inline auto block::R4(uint32_t& v, uint32_t& w, uint32_t& x,
                      uint32_t& y, uint32_t& z,
                      unsigned int i) noexcept -> void {
  z += (w ^ x ^ y) + blk(i) + 0xca62c1d6U + rol(v, 5);
  w = rol(w, 30);
}

/* Hash a single 512-bit block.  This is the core of the algorithm. */
auto sha1_transform(uint32_t state[5],
                    const uint8_t buffer[sha1::BLOCK_LENGTH])
    noexcept -> void {
  block workspace{ buffer };

  /* Copy context state to working vars. */
  uint32_t a, b, c, d, e;
  tie(a, b, c, d, e) =
      make_tuple(state[0], state[1], state[2], state[3], state[4]);

  /* 4 rounds of 20 operations each. Loop unrolled. */
  workspace.R0(a,b,c,d,e, 0);
  workspace.R0(e,a,b,c,d, 1);
  workspace.R0(d,e,a,b,c, 2);
  workspace.R0(c,d,e,a,b, 3);
  workspace.R0(b,c,d,e,a, 4);
  workspace.R0(a,b,c,d,e, 5);
  workspace.R0(e,a,b,c,d, 6);
  workspace.R0(d,e,a,b,c, 7);
  workspace.R0(c,d,e,a,b, 8);
  workspace.R0(b,c,d,e,a, 9);
  workspace.R0(a,b,c,d,e,10);
  workspace.R0(e,a,b,c,d,11);
  workspace.R0(d,e,a,b,c,12);
  workspace.R0(c,d,e,a,b,13);
  workspace.R0(b,c,d,e,a,14);
  workspace.R0(a,b,c,d,e,15);
  workspace.R1(e,a,b,c,d,16);
  workspace.R1(d,e,a,b,c,17);
  workspace.R1(c,d,e,a,b,18);
  workspace.R1(b,c,d,e,a,19);
  workspace.R2(a,b,c,d,e,20);
  workspace.R2(e,a,b,c,d,21);
  workspace.R2(d,e,a,b,c,22);
  workspace.R2(c,d,e,a,b,23);
  workspace.R2(b,c,d,e,a,24);
  workspace.R2(a,b,c,d,e,25);
  workspace.R2(e,a,b,c,d,26);
  workspace.R2(d,e,a,b,c,27);
  workspace.R2(c,d,e,a,b,28);
  workspace.R2(b,c,d,e,a,29);
  workspace.R2(a,b,c,d,e,30);
  workspace.R2(e,a,b,c,d,31);
  workspace.R2(d,e,a,b,c,32);
  workspace.R2(c,d,e,a,b,33);
  workspace.R2(b,c,d,e,a,34);
  workspace.R2(a,b,c,d,e,35);
  workspace.R2(e,a,b,c,d,36);
  workspace.R2(d,e,a,b,c,37);
  workspace.R2(c,d,e,a,b,38);
  workspace.R2(b,c,d,e,a,39);
  workspace.R3(a,b,c,d,e,40);
  workspace.R3(e,a,b,c,d,41);
  workspace.R3(d,e,a,b,c,42);
  workspace.R3(c,d,e,a,b,43);
  workspace.R3(b,c,d,e,a,44);
  workspace.R3(a,b,c,d,e,45);
  workspace.R3(e,a,b,c,d,46);
  workspace.R3(d,e,a,b,c,47);
  workspace.R3(c,d,e,a,b,48);
  workspace.R3(b,c,d,e,a,49);
  workspace.R3(a,b,c,d,e,50);
  workspace.R3(e,a,b,c,d,51);
  workspace.R3(d,e,a,b,c,52);
  workspace.R3(c,d,e,a,b,53);
  workspace.R3(b,c,d,e,a,54);
  workspace.R3(a,b,c,d,e,55);
  workspace.R3(e,a,b,c,d,56);
  workspace.R3(d,e,a,b,c,57);
  workspace.R3(c,d,e,a,b,58);
  workspace.R3(b,c,d,e,a,59);
  workspace.R4(a,b,c,d,e,60);
  workspace.R4(e,a,b,c,d,61);
  workspace.R4(d,e,a,b,c,62);
  workspace.R4(c,d,e,a,b,63);
  workspace.R4(b,c,d,e,a,64);
  workspace.R4(a,b,c,d,e,65);
  workspace.R4(e,a,b,c,d,66);
  workspace.R4(d,e,a,b,c,67);
  workspace.R4(c,d,e,a,b,68);
  workspace.R4(b,c,d,e,a,69);
  workspace.R4(a,b,c,d,e,70);
  workspace.R4(e,a,b,c,d,71);
  workspace.R4(d,e,a,b,c,72);
  workspace.R4(c,d,e,a,b,73);
  workspace.R4(b,c,d,e,a,74);
  workspace.R4(a,b,c,d,e,75);
  workspace.R4(e,a,b,c,d,76);
  workspace.R4(d,e,a,b,c,77);
  workspace.R4(c,d,e,a,b,78);
  workspace.R4(b,c,d,e,a,79);

  tie(state[0], state[1], state[2], state[3], state[4]) =
      make_tuple(a, b, c, d, e);
  /* Wipe variables */
  a = b = c = d = e = 0;
}



} /* namespace ilias::crypto::<unnamed> */


sha1::~sha1() noexcept {
  abi::safe_memzero(buffer_.data(), sizeof(buffer_));
  abi::safe_memzero(state_.data(), sizeof(state_));
}

/* Run your data through this. */
auto sha1::update(const uint8_t* data, size_t len) noexcept -> void {
  uint32_t j = (count_ >> 3) & 63;
  count_ += (len << 3);

  size_t i = 0;
  if (j + len > 63) {
    i = 64 - j;
    bcopy(data, buffer_.begin() + j, i);
    sha1_transform(state_.data(), buffer_.data());
    for (; i + 63 < len; i += 64)
      sha1_transform(state_.data(), data + i);
    j = 0;
  }

  bcopy(data + i, buffer_.data() + j, len - i);
}

/* Add padding and return message digest. */
auto sha1::finalize(uint8_t rv[DIGEST_LENGTH]) noexcept -> void {
  array<uint8_t, 8> finalcount;

  for (size_t i = 0; i < 8; ++i) {
    finalcount[i] =
        ((count_ >> ((7 - (i & 7)) * 8)) & 0xff);  // Endian dependent
  }
  update(reinterpret_cast<const uint8_t*>("\200"), 1);
  while ((count_ & 504) != 448)
    update(reinterpret_cast<const uint8_t*>("\0"), 1);
  update(finalcount.data(),
         finalcount.size());  // Should cause a sha1_transform

  if (rv) {
    for (size_t i = 0; i < DIGEST_LENGTH; ++i)
      rv[i] = state_[i >> 2] >> ((3 - (i & 3)) & 0xffU);
  }
  abi::safe_memzero(finalcount.data(), finalcount.size());
}

auto sha1::finalize() noexcept -> digest_type {
  digest_type rv;
  finalize(rv.data());
  return rv;
}

/*
 * Convenience function, for if the entire byte stream is available.
 */
auto sha1::calculate(const uint8_t* input, size_t len) noexcept ->
    digest_type {
  sha1 ctx;
  ctx.update(input, len);
  return ctx.finalize();
}

/*
 * Convenience function, for if the entire byte stream is available.
 */
auto sha1::calculate(uint8_t rv[DIGEST_LENGTH],
                     const uint8_t* input, size_t len) noexcept -> void {
  sha1 ctx;
  ctx.update(input, len);
  ctx.finalize(rv);
}


}} /* namespace ilias::crypto */
