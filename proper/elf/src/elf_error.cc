#include <ilias/elf/elf_error.h>
#include <string>

namespace ilias {
namespace elf {


elf_error::elf_error()
: elf_error("elf_error")
{}

elf_error::elf_error(const std::string& msg)
: elf_error(string_ref(msg))
{}

elf_error::elf_error(string_ref msg)
: runtime_error(msg)
{}

elf_error::elf_error(const char* msg)
: elf_error(string_ref(msg))
{}

elf_error::~elf_error() noexcept {}

auto elf_error::__throw() -> void {
  throw elf_error();
}

auto elf_error::__throw(const string& msg) -> void {
  throw elf_error(msg);
}

auto elf_error::__throw(string_ref msg) -> void {
  throw elf_error(msg);
}

auto elf_error::__throw(const char* msg) -> void {
  throw elf_error(msg);
}


}} /* namespace ilias::elf */
