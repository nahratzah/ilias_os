#include <util/cpp/name.h>

namespace impl {
namespace cpp {


c_chord cpp_name::get_printable() const {
  c_chord rv;
  if (parent_) rv += printable(*parent);
  rv += "::" += name_;
  return rv;
}

std::unique_ptr<cpp_name> cpp_name::clone() const {
  return clone_();
}

cpp_name* cpp_name::clone_() const {
  return new cpp_name(*this);
}


}} /* namespace impl::cpp */
