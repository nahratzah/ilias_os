#ifndef _STDIMPL_HEAP_ARRAY_H_
#define _STDIMPL_HEAP_ARRAY_H_

#include <cdecl.h>
#include <cstddef>
#include <memory>

_namespace_begin(std)
namespace impl {


/* Tiny class that provides temporary array storage. */
template<typename T>
class heap_array {
 public:
  using value_type = T;
  using pointer = value_type*;
  using iterator = pointer;

  heap_array(size_t n) noexcept
  : max_size_(n),
    ptr_(get<0>(get_temporary_buffer<value_type>(n)))
  {}

  heap_array() = delete;
  heap_array(const heap_array&) = delete;
  heap_array& operator=(const heap_array&) = delete;

  heap_array(heap_array&& other) noexcept
  : max_size_(other.max_size_),
    size_(other.size_),
    ptr_(exchange(other.ptr_, nullptr))
  {}

  ~heap_array() noexcept {
    if (*this) {
      while (size_-- > 0)
        ptr_[size_].~value_type();
      return_temporary_buffer(ptr_);
    }
  }

  explicit operator bool() const noexcept { return ptr_; }

  iterator begin() const noexcept { return ptr_; }
  iterator end() const noexcept { return begin() + size_; }

  void push_back(value_type&& v) {
    assert(size_ < max_size_);
    new (&ptr_[size_]) value_type(forward<value_type>(v));
    ++size_;
  }

  void emplace_back(value_type&& v) {
    emplace_back(move(v));
  }

 private:
  const size_t max_size_;
  size_t size_ = 0;
  pointer ptr_;
};


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_HEAP_ARRAY_H_ */
