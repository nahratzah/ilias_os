#include <loader/ldexport.h>
#include <string>

namespace loader {


std::string ldexport::to_string() const {
  std::string rv;
  rv += "=== Physical memory map ===\n";
  rv += physmem.to_string();
  return rv;
}


} /* namespace loader */
