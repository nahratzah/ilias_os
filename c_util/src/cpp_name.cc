#include <util/cpp/name.h>

namespace util {
namespace cpp {


c_chord cpp_name::get_printable() const {
  c_chord rv;
  if (parent_) rv += printable(*parent_);
  rv += "::";
  rv += name_;
  return rv;
}

std::unique_ptr<cpp_name> cpp_name::clone() const {
  return std::unique_ptr<cpp_name>{ clone_() };
}

cpp_name* cpp_name::clone_() const {
  return new cpp_name(*this);
}


}} /* namespace util::cpp */
