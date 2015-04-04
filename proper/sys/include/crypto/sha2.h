#ifndef _CRYPTO_SHA2_H_
#define _CRYPTO_SHA2_H_

#include <array>
#include <cstdint>

namespace ilias {
namespace crypto {


class sha256 {
 public:
  static constexpr size_t BLOCK_LENGTH = 64;
  static constexpr size_t DIGEST_LENGTH = 32;

  using digest_type = std::array<uint8_t, DIGEST_LENGTH>;

  sha256() noexcept = default;
  ~sha256() noexcept;

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
  void transform_(const uint8_t*) noexcept;

  std::array<uint32_t, 8> state_{{
    0x6a09e667U,
    0xbb67ae85U,
    0x3c6ef372U,
    0xa54ff53aU,
    0x510e527fU,
    0x9b05688cU,
    0x1f83d9abU,
    0x5be0cd19U
  }};

  uint64_t bitcount_ = 0;
  std::array<uint8_t, BLOCK_LENGTH> buffer_;
};

class sha384 {
 public:
  static constexpr size_t BLOCK_LENGTH = 128;
  static constexpr size_t DIGEST_LENGTH = 48;

  using digest_type = std::array<uint8_t, DIGEST_LENGTH>;

  sha384() noexcept = default;
  ~sha384() noexcept;

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
  void transform_(const uint8_t*) noexcept;

  std::array<uint64_t, 8> state_{{
    0xcbbb9d5dc1059ed8ULL,
    0x629a292a367cd507ULL,
    0x9159015a3070dd17ULL,
    0x152fecd8f70e5939ULL,
    0x67332667ffc00b31ULL,
    0x8eb44a8768581511ULL,
    0xdb0c2e0d64f98fa7ULL,
    0x47b5481dbefa4fa4ULL
  }};

  std::array<uint64_t, 2> bitcount_{{ 0ULL, 0ULL }};
  std::array<uint8_t, BLOCK_LENGTH> buffer_;
};

class sha512 {
 public:
  static constexpr size_t BLOCK_LENGTH = 128;
  static constexpr size_t DIGEST_LENGTH = 64;

  using digest_type = std::array<uint8_t, DIGEST_LENGTH>;

  sha512() noexcept = default;
  ~sha512() noexcept;

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
  void transform_(const uint8_t*) noexcept;

  std::array<uint64_t, 8> state_{{
    0x6a09e667f3bcc908ULL,
    0xbb67ae8584caa73bULL,
    0x3c6ef372fe94f82bULL,
    0xa54ff53a5f1d36f1ULL,
    0x510e527fade682d1ULL,
    0x9b05688c2b3e6c1fULL,
    0x1f83d9abfb41bd6bULL,
    0x5be0cd19137e2179ULL
  }};
  std::array<uint64_t, 2> bitcount_{{ 0ULL, 0ULL }};
  std::array<uint8_t, BLOCK_LENGTH> buffer_;
};


}} /* namespace ilias::crypto */

#endif /* _CRYPTO_SHA2_H_ */
