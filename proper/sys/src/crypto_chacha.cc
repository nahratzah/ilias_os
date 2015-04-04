/*
 * chacha-merged.c version 20080118
 * D. J. Bernstein
 * Public domain.
 *
 * Ported to C++ by Ariane
 */
#include <crypto/chacha.h>
#include <abi/memory.h>

namespace ilias {
namespace crypto {


using namespace std;


namespace {


constexpr uint8_t u8v(unsigned int x) noexcept {
  return x & 0xffU;
}

constexpr uint32_t u32v(unsigned int x) noexcept {
  return x & 0xffffffffU;
}

constexpr uint32_t rotl32(uint32_t v, unsigned int n) noexcept {
  return u32v((v << n) | (v >> (32 - n)));
}

inline uint32_t u8to32_little(const uint8_t* p) noexcept {
  return u32v(p[0]      ) |
         u32v(p[1] <<  8) |
         u32v(p[2] << 16) |
         u32v(p[3] << 24);
}

inline void u32to8_little(uint8_t* p, uint32_t v) noexcept {
  p[0] = u8v(v      );
  p[1] = u8v(v >>  8);
  p[2] = u8v(v >> 16);
  p[3] = u8v(v >> 24);
}

constexpr uint32_t rotate(uint32_t v, unsigned int n) noexcept {
  return rotl32(v, n);
}

constexpr uint32_t xor_(uint32_t v, uint32_t w) noexcept {
  return (v ^ w);
}

constexpr uint32_t plus(uint32_t v, uint32_t w) noexcept {
  return u32v(v + w);
}

constexpr uint32_t plusone(uint32_t v) noexcept {
  return plus(v, 1U);
}

inline void quarterround(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d)
    noexcept {
  a = plus(a, b); d = rotate(xor_(d, a), 16);
  c = plus(c, d); b = rotate(xor_(b, c), 12);
  a = plus(a, b); d = rotate(xor_(d, a),  8);
  c = plus(c, d); b = rotate(xor_(b, c),  7);
}

constexpr string_ref sigma = string_ref("expand 32-byte k", 16);
constexpr string_ref tau = string_ref("expand 16-byte k", 16);


} /* namespace ilias::crypto::<unnamed> */


chacha::chacha(const uint8_t* buf, size_t kbits, size_t ivbits) noexcept {
  keysetup(buf, kbits, ivbits);
  ivsetup(buf + kbits / CHAR_BIT);
}

chacha::~chacha() noexcept {
  abi::safe_memzero(input_.data(), sizeof(input_));
}

auto chacha::encrypt_bytes(const uint8_t* m, uint8_t* c, size_t bytes)
    noexcept -> void {
  array<uint32_t, 16> x, j;
  uint8_t* ctarget = nullptr;
  array<uint8_t, 64> tmp;
  const bool keystream_only = (m == nullptr);

  if (bytes == 0) return;

  j = input_;

  for (;;) {
    if (bytes < 64) {
      copy_n(m, bytes, tmp.begin());
      m = tmp.data();
      ctarget = c;
      c = tmp.data();
    }

    x = j;
    for (int i = 20; i > 0; i -= 2) {
      quarterround( x[0],  x[4],  x[8], x[12]);
      quarterround( x[1],  x[5],  x[9], x[13]);
      quarterround( x[2],  x[6], x[10], x[14]);
      quarterround( x[3],  x[7], x[11], x[15]);
      quarterround( x[0],  x[5], x[10], x[15]);
      quarterround( x[1],  x[6], x[11], x[12]);
      quarterround( x[2],  x[7],  x[8], x[13]);
      quarterround( x[3],  x[4],  x[9], x[14]);
    }
    transform(x.begin(), x.end(), j.begin(), x.begin(), &plus);

    if (!keystream_only) {
      const uint8_t* m_iter = m;
      for (auto& x_val : x) {
        x_val = xor_(x_val, u8to32_little(m_iter));
        m_iter += 4;
      }
    }

    j[12] = plusone(j[12]);
    if (!j[12]) {
      j[13] = plusone(j[13]);
      /* stopping at 2^70 bytes per nonce is user's responsibility */
    }

    {
      uint8_t* c_iter = c;
      for (auto x_val : x) {
        u32to8_little(c_iter, x_val);
        c_iter += 4;
      }
    }

    if (bytes <= 64) {
      if (bytes < 64) copy_n(c, bytes, ctarget);
      input_[12] = j[12];
      input_[13] = j[13];
      return;
    }

    bytes -= 64;
    c += 64;
    if (!keystream_only) m += 64;
  }
}

auto chacha::keysetup(const uint8_t* k, size_t kbits, size_t /*ivbits*/)
    noexcept -> void {
  string_ref constants;

  input_[4] = u8to32_little(k + 0);
  input_[5] = u8to32_little(k + 4);
  input_[6] = u8to32_little(k + 8);
  input_[7] = u8to32_little(k + 12);
  if (kbits == 256) {  // recommended
    k += 16;
    constants = sigma;
  } else {  // kbits == 128
    constants = tau;
  }
  input_[8] = u8to32_little(k + 0);
  input_[9] = u8to32_little(k + 4);
  input_[10] = u8to32_little(k + 8);
  input_[11] = u8to32_little(k + 12);
  input_[0] =
      u8to32_little(reinterpret_cast<const uint8_t*>(constants.begin() + 0));
  input_[1] =
      u8to32_little(reinterpret_cast<const uint8_t*>(constants.begin() + 4));
  input_[2] =
      u8to32_little(reinterpret_cast<const uint8_t*>(constants.begin() + 8));
  input_[3] =
      u8to32_little(reinterpret_cast<const uint8_t*>(constants.begin() + 12));
}

auto chacha::ivsetup(const uint8_t* iv) noexcept -> void {
  input_[12] = 0;
  input_[13] = 0;
  input_[14] = u8to32_little(iv + 0);
  input_[15] = u8to32_little(iv + 4);
}


}} /* namespace ilias::crypto */
