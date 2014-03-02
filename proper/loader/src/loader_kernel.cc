#include <loader/kernel.h>

/* Provided by objcopy. */
extern "C" char _binary_64_kgz_start;
extern "C" std::uint32_t _binary_64_kgz_size;
extern "C" char _binary_32_kgz_start;
extern "C" std::uint32_t _binary_32_kgz_size;

namespace loader {

const kernel kernel::amd64 = { &_binary_64_kgz_start, _binary_64_kgz_size };
const kernel kernel::i386 = { &_binary_32_kgz_start, _binary_32_kgz_size };

} /* namespace loader */
