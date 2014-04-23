#include <loader/memorymap.h>
#include <string>
#include <cstdio>

namespace loader {
namespace {


const std::string_ref units[] = {
    { "B", 1 },  // bytes
    { "kB", 2 },  // kilobytes
    { "MB", 2 },  // megabytes
    { "GB", 2 },  // gigabytes
    { "TB", 2 },  // terabytes
    { "PB", 2 },  // petabytes
    { "EB", 2 },  // exabytes
    { "ZB", 2 },  // zetabytes
    { "YB", 2 },  // yottabytes
  };

constexpr unsigned int N_UNITS = sizeof(units) / sizeof(units[0]);


}


std::string memorymap::to_string() const {
  std::string rv;
  for (const auto& line : *this) {
    unsigned long long base = line.addr;
    unsigned long long top = base + line.len;
    rv += std::format("%#18llx - %#18llx  ", base, top);

    decltype(line.len) values[N_UNITS];
    auto sz = line.len;
    for (unsigned int i = 0; i < N_UNITS - 1U; ++i) {
      values[i] = sz % 1024U;
      sz /= 1024U;
    }
    values[N_UNITS - 1] = sz;

    unsigned int i = N_UNITS;
    bool printed_nothing = true;
    while (i-- > 0) {
      if ((printed_nothing && i == 0) || values[i] != 0) {
        rv += std::format(" %4jd ", intmax_t(values[i]));
        rv += units[i];
        printed_nothing = false;
      }
    }
    rv += "\n";
  }
  return rv;
}


} /* namespace loader */
