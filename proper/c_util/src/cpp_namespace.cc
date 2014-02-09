#include <util/cpp/namespace.h>
#include <utility>

namespace util {
namespace cpp {


cpp_namespace::~cpp_namespace() noexcept {
  return;
}

c_chord cpp_namespace::get_printable() const {
  c_chord rv;
  for (const auto& path_elem : path_) {
    rv += "::";
    rv += path_elem;
  }
  return rv;
}

std::unique_ptr<cpp_namespace> cpp_namespace::clone() const {
  return std::unique_ptr<cpp_namespace>{ clone_() };
}

cpp_namespace* cpp_namespace::clone_() const {
  std::unique_ptr<cpp_namespace> copy(new cpp_namespace);
  copy->path_ = path_;
  return copy.release();
}

cpp_namespace cpp_namespace::operator+(c_string_ptr&& elem) const {
  cpp_namespace clone = *this;
  clone.path_.emplace_back(std::move(elem));
  return clone;
}

cpp_namespace cpp_namespace::operator+(const c_string_ptr& elem) const {
  cpp_namespace clone = *this;
  clone.path_.emplace_back(elem);
  return clone;
}

cpp_namespace cpp_namespace::operator+(c_string_piece elem) const {
  return *this + c_string_ptr(elem);
}


}} /* namespace util::cpp */
