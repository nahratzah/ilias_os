#include <util/c_string_ptr.h>
#include <util/string_piece.h>

namespace util {


c_string_ptr::c_string_ptr(const c_string_ptr& other) {
  reserve(other.size());
  len_ = other.size();
  memcpy(data_, other.c_str(), len_);
}

void c_string_ptr::reserve(size_type len) {
  if (capacity_ >= len + 1U) return;

  size_type alloc_capacity = (16 * capacity_ + 15) / 10;
  if (alloc_capacity <= len) alloc_capacity = len + 1U;

  char* d = static_cast<char*>(realloc(data_, alloc_capacity));
  if (!d) throw std::bad_alloc();
  capacity_ = alloc_capacity;
}

void c_string_ptr::shrink_to_fit() noexcept {
  if (capacity_ > len_ + 1U) {
    char* d = static_cast<char*>(realloc(data_, len_ + 1U));
    if (d) {
      data_ = d;
      capacity_ = len_ + 1U;
    }
  }
}

c_string_ptr& c_string_ptr::operator=(const c_string_ptr& other) {
  reserve(other.size());
  memcpy(data_, other.data_, other.len_);
  len_ = other.len_;
  return *this;
}

c_string_ptr& c_string_ptr::operator+=(string_piece sp) {
  append(sp.data(), sp.size());
  return *this;
}

void c_string_ptr::append(const char* s, size_type len) {
  if (!s) throw std::invalid_argument("cannot append nul string");
  reserve(len_ + len);
  memcpy(&data_[len_], s, len);
  len_ += len;
}

} /* namespace util */
