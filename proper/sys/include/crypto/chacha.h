#ifndef _CRYPTO_CHACHA_H_
#define _CRYPTO_CHACHA_H_

#include <array>
#include <cstddef>
#include <cstdint>


namespace ilias {
namespace crypto {


/*
 * Chacha streaming cypher.
 */
class chacha {
 public:
  chacha() noexcept = default;
  chacha(const uint8_t*, size_t, size_t) noexcept;
  ~chacha() noexcept;
  void encrypt_bytes(const uint8_t*, uint8_t*, size_t) noexcept;

  void keysetup(const uint8_t*, size_t, size_t) noexcept;
  void ivsetup(const uint8_t*) noexcept;

 private:
  std::array<uint32_t, 16> input_;
};


}} /* namespace ilias::crypto */


#endif /* _CRYPTO_CHACHA_H_ */
