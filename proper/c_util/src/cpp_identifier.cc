#include <util/cpp/identifier.h>

namespace util {
namespace cpp {


cpp_identifier::~cpp_identifier() noexcept {
  return;
}

std::unique_ptr<cpp_identifier> cpp_identifier::clone() const {
  return std::unique_ptr<cpp_identifier>{ clone_() };
}

c_chord printable_chord(const cpp_identifier& i) {
  return i.get_printable();
}

c_string_ptr printable(const cpp_identifier& i) {
  return printable_chord(i);
}


}} /* namespace util::cpp */
