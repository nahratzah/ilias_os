#ifndef _CRYPTO_MD5_H_
#define _CRYPTO_MD5_H_

#include <array>
#include <cstddef>
#include <cstdint>

namespace ilias {
namespace crypto {


class md5 {
 public:
  static constexpr size_t BLOCK_LENGTH = 64;
  static constexpr size_t DIGEST_LENGTH = 16;

  using digest_type = std::array<uint8_t, DIGEST_LENGTH>;

  ~md5() noexcept;

  void update(const uint8_t*, size_t) noexcept
      __attribute__((__bounded__(__string__, 1, 2)));
  digest_type finalize() noexcept;
  void finalize(uint8_t[DIGEST_LENGTH]) noexcept
      __attribute__((__bounded__(__minbytes__, 1, DIGEST_LENGTH)));

  static digest_type calculate(const uint8_t*, size_t) noexcept
      __attribute__((__bounded__(__string__, 1, 2)));
  static void calculate(uint8_t[DIGEST_LENGTH],
                        const uint8_t*, size_t) noexcept
      __attribute__((__bounded__(__minbytes__, 1, DIGEST_LENGTH)))
      __attribute__((__bounded__(__string__, 2, 3)));

 private:
  std::array<uint32_t, 4> state_{{
    0x67452301U, 0xefcdab89U, 0x98badcfeU, 0x10325476U
  }};

  uint64_t count_ = 0;
  std::array<uint8_t, BLOCK_LENGTH> buffer_;
};


}} /* namespace ilias::crypto */

#endif /* _CRYPTO_MD5_H_ */
