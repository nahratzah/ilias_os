#ifndef _STDIMPL_HEAP_ARRAY_H_
#define _STDIMPL_HEAP_ARRAY_H_

#include <cdecl.h>
#include <cstddef>
#include <cassert>
#include <memory>
#include <type_traits>

_namespace_begin(std)
namespace impl {


/*
 * Tiny class that provides temporary array storage.
 *
 * This class only allocates at construction time and cannot be resized.
 *
 * Note: heap_array will not throw on allocation failure.
 *       Use operator bool() to check for success instead.
 */
template<typename T>
class heap_array {
 public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = pointer;
  using const_iterator = const_pointer;

  heap_array(size_t n) noexcept
  : max_size_(n),
    ptr_(get<0>(get_temporary_buffer<value_type>(n)))
  {}

  heap_array() = delete;
  heap_array(const heap_array&) = delete;
  heap_array& operator=(const heap_array&) = delete;

  heap_array(heap_array&& other) noexcept
  : max_size_(exchange(other.max_size_, 0)),
    size_(exchange(other.size_, 0)),
    ptr_(exchange(other.ptr_, nullptr))
  {}

  ~heap_array() noexcept {
    if (*this) {
      if (!is_trivially_destructible<value_type>::value) {
        while (size_-- > 0)
          ptr_[size_].~value_type();
      }
      return_temporary_buffer(ptr_);
    }
  }

  explicit operator bool() const noexcept { return ptr_; }

  iterator begin() noexcept { return ptr_; }
  iterator end() noexcept { return begin() + size_; }
  const_iterator begin() const noexcept { return ptr_; }
  const_iterator end() const noexcept { return begin() + size_; }
  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }

  bool empty() const noexcept { return size_ == 0; }
  size_t size() const noexcept { return size_; }
  size_t max_size() const noexcept { return max_size_; }

  reference operator[](size_t i) noexcept {
    assert(i < size_);
    return begin()[size_];
  }
  const_reference operator[](size_t i) const noexcept {
    assert(i < size_);
    return begin()[size_];
  }

  void push_back(const value_type& v) { emplace_back(v); }
  void push_back(value_type&& v) { emplace_back(move(v)); }

  template<typename... Args>
  void emplace_back(Args&&... args) {
    assert(size_ < max_size_);
    new (&ptr_[size_]) value_type(forward<Args>(args)...);
    ++size_;
  }

 private:
  size_t max_size_;
  size_t size_ = 0;
  pointer ptr_;
};


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_HEAP_ARRAY_H_ */
