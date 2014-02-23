#include <abi/ext/reader.h>

namespace __cxxabiv1 {
namespace ext {


const uint8_t* memfind(const uint8_t* s_haystack, size_t n_haystack,
                       const uint8_t* s_needle, size_t n_needle) noexcept {
  constexpr size_t histogram_sz = UINT8_MAX + 1U;
  ssize_t histogram[histogram_sz];
  unsigned int nz = 0;

  if (n_needle == 0) return s_haystack;
  if (n_haystack < n_needle) return nullptr;

  /* Fill histogram, by subtracting needle. */
  {
    auto r = reader<DIR_FORWARD, uint8_t>(s_needle, n_needle);
    for (size_t n = 0; n < n_needle; ++n)
      if (histogram[r.read8(n_needle - n)]-- == 0) ++nz;
  }

  /* Pre-read n_needle bytes from haystack. */
  auto head = reader<DIR_FORWARD, uint8_t>(s_haystack, n_haystack);
  for (size_t n = 0; n < n_needle; ++n) {
    switch (histogram[head.read8(n_haystack - n)]++) {
    default:
      break;
    case 0:
      ++nz;
      break;
    case -1:
      --nz;
      break;
    }
  }

  /* Walk window, described by [tail - head) through histogram. */
  auto tail = reader<DIR_FORWARD, uint8_t>(s_haystack, n_haystack - n_needle);
  for (size_t n = n_needle; n < n_haystack; ++n) {
    if (nz == 0 &&
        memcmp(tail.addr<uint8_t>(), s_needle, n_needle) == 0)
      return tail.addr<uint8_t>();

    const auto head_c = head.read8(n_haystack - n);
    const auto tail_c = tail.read8(n_haystack - n);

    /* Add head character to histogram. */
    switch (histogram[head_c]++) {
      default:
        break;
      case 0:
        ++nz;
        break;
      case -1:
        --nz;
        break;
    }
    /* Remove tail character from histogram. */
    switch (histogram[tail_c]--) {
      default:
        break;
      case 0:
        ++nz;
        break;
      case 1:
        --nz;
        break;
    }
  }

  /* Check last input from loop above. */
  if (nz == 0 &&
      memcmp(tail.addr<uint8_t>(), s_needle, n_needle) == 0)
    return tail.addr<uint8_t>();

  return nullptr;
}

template int memcmp<uint8_t>(const uint8_t*, const uint8_t*, size_t) noexcept;


}} /* namespace __cxxabiv1::ext */
