#ifndef _CRYPTO_SHA1_H_
#define _CRYPTO_SHA1_H_

#include <array>
#include <cstdint>

namespace ilias {
namespace crypto {


class sha1 {
 public:
  static constexpr size_t SHA1_BLOCK_LENGTH = 64;
  static constexpr size_t SHA1_DIGEST_LENGTH = 20;

  using digest_type = std::array<uint8_t, SHA1_DIGEST_LENGTH>;

  sha1() noexcept = default;
  ~sha1() noexcept;

  void update(const uint8_t*, size_t) noexcept
      __attribute__((__bounded__(__string__, 1, 2)));
  digest_type finalize() noexcept;
  void finalize(uint8_t[SHA1_DIGEST_LENGTH]) noexcept
      __attribute__((__bounded__(__minbytes__, 1, SHA1_DIGEST_LENGTH)));

  static digest_type calculate(const uint8_t*, size_t) noexcept
      __attribute__((__bounded__(__string__, 1, 2)));
  static void calculate(uint8_t[SHA1_DIGEST_LENGTH],
                        const uint8_t*, size_t) noexcept
      __attribute__((__bounded__(__minbytes__, 1, SHA1_DIGEST_LENGTH)))
      __attribute__((__bounded__(__string__, 2, 3)));

 private:
  std::array<uint32_t, 5> state_{{
    0x67452301U,
    0xefcdab89U,
    0x98badcfeU,
    0x10325476U,
    0xc3d2e1f0U
  }};
  uint64_t count_ = 0;
  std::array<uint8_t, SHA1_BLOCK_LENGTH> buffer_;
};


}} /* namespace ilias::crypto */

#endif /* _CRYPTO_SHA1_H_ */
