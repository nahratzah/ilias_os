#ifndef _LOADER_MULTIBOOT_H_
#define _LOADER_MULTIBOOT_H_

#include <vector>
#include <iosfwd>

namespace loader {
namespace multiboot {


/* An entry in the memory map. */
struct memory_line {
  uint64_t base;
  uint64_t len;
  bool usable;

  bool is_free() const noexcept { return usable; }
};

/* Test if the provided multiboot data is valid. */
bool valid() noexcept;

std::vector<memory_line> memory_map();
std::string_ref command_line();


}} /* namespace loader::multiboot */

#endif /* _LOADER_MULTIBOOT_H_ */
