#include <loader/memorymap.h>
#include <string>
#include <cstdio>

namespace loader {
namespace {


const std::string_ref units[] = {
    "B",  // bytes
    "kB",  // kilobytes
    "MB",  // megabytes
    "GB",  // gigabytes
    "TB",  // terabytes
  };

constexpr unsigned int N_UNITS = sizeof(units) / sizeof(units[0]);


}


std::string memorymap::to_string() const {
  std::string rv;
  for (const auto line : *this) {
    unsigned long long base = line.addr;
    unsigned long long top = base + line.len;
    rv += std::format("%#20llx - %#20llx  ", base, top);

    decltype(line.len) values[N_UNITS];
    auto sz = line.len;
    for (unsigned int i = 0; i < N_UNITS - 1U; ++i) {
      values[i] = sz % 1024U;
      sz /= 1024U;
    }
    values[N_UNITS - 1] = sz;

    unsigned int i = N_UNITS;
    while (i-- > 0) rv += std::format(" %4d %-2s ", values[i], units[i]);
    rv += "\n";
  }
  return rv;
}


} /* namespace loader */
