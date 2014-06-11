#include <abi/ext/c_string_ptr.h>
#include <stdexcept>
#include <string>

namespace __cxxabiv1 {
namespace ext {


c_string_ptr::c_string_ptr(const std::string_ref& s) : c_string_ptr() {
  reserve(s.length());
  len_ = s.length();
  std::memcpy(data(), s.data(), len_);
}

c_string_ptr::c_string_ptr(const c_string_ptr& other) {
  reserve(other.size());
  len_ = other.size();
  std::memcpy(data(), other.c_str(), len_);
}

void c_string_ptr::reserve(size_type len) {
  if (capacity_ >= len + 1U) return;

  size_type alloc_capacity = (16 * capacity_ + 15) / 10;
  if (alloc_capacity <= len) alloc_capacity = len + 1U;

  char* d = static_cast<char*>(std::realloc(data_, alloc_capacity));
  if (!d) throw std::bad_alloc();
  capacity_ = alloc_capacity;
}

void c_string_ptr::shrink_to_fit() noexcept {
  if (capacity_ > len_ + 1U) {
    char* d = static_cast<char*>(std::realloc(data_, len_ + 1U));
    if (d) {
      data_ = d;
      capacity_ = len_ + 1U;
    }
  }
}

c_string_ptr& c_string_ptr::operator=(const c_string_ptr& other) {
  reserve(other.size());
  std::memcpy(data(), other.data(), other.size());
  len_ = other.size();
  return *this;
}

c_string_ptr& c_string_ptr::operator+=(_namespace(std)::string_ref sp) {
  append(sp.data(), sp.size());
  return *this;
}

void c_string_ptr::append(const char* s, size_type len) {
  if (!s) throw std::invalid_argument("cannot append nul string");
  reserve(size() + len);
  std::memcpy(&data()[size()], s, len);
  len_ += len;
}

c_string_ptr::operator _namespace(std)::string_ref() const noexcept {
  return std::string_ref(data(), size());
}

}} /* namespace __cxxabiv1::ext */
