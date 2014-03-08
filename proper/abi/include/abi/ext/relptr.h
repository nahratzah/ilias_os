#ifndef _ABI_EXT_RELPTR_H_
#define _ABI_EXT_RELPTR_H_

#include <abi/abi.h>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <memory>

namespace __cxxabiv1 {
namespace ext {


/* 32 bit relative pointer. */
template<typename T>
class relptr {
 public:
  using element_type = T;
  using pointer = _namespace(std)::add_pointer_t<element_type>;
  using reference = _namespace(std)::add_lvalue_reference_t<T>;
  using difference_type = ptrdiff_t;

  constexpr relptr(decltype(nullptr) = nullptr) noexcept {}
  relptr(const relptr&);
  template<typename U> relptr(const relptr<U>&);
  explicit relptr(pointer);

  relptr& operator=(decltype(nullptr)) noexcept;
  relptr& operator=(const relptr&);
  template<typename U> relptr& operator=(const relptr<U>&);
  relptr& operator=(pointer);

  explicit operator bool() const noexcept;

  pointer operator->() const noexcept;
  reference operator*() const noexcept;

  relptr& operator+=(ptrdiff_t);
  relptr& operator-=(ptrdiff_t);
  pointer operator+(ptrdiff_t) const;
  pointer operator-(ptrdiff_t) const;

  pointer get() const noexcept;

 private:
  static constexpr int32_t invalid = INT32_MIN;

  void assign_(pointer);

  int32_t rel_ = invalid;
};

template<typename T, typename U>
bool operator==(const relptr<T>& a, const relptr<U>& b)
    noexcept(noexcept(&*a == &*b));

template<typename T, typename U>
bool operator!=(const relptr<T>& a, const relptr<U>& b)
    noexcept(noexcept(!(a == b)));

template<typename T, typename U>
bool operator<(const relptr<T>& a, const relptr<U>& b)
    noexcept(noexcept(&*a < &*b));

template<typename T, typename U>
bool operator>(const relptr<T>& a, const relptr<U>& b)
    noexcept(noexcept(b < a));

template<typename T, typename U>
bool operator<=(const relptr<T>& a, const relptr<U>& b)
    noexcept(noexcept(!(b < a)));

template<typename T, typename U>
bool operator>=(const relptr<T>& a, const relptr<U>& b)
    noexcept(noexcept(!(a < b)));


template<typename T> struct relptr_allocator {
  using value_type = T;
  using pointer = relptr<value_type>;
  using const_pointer = relptr<const value_type>;
  using size_type = uint32_t;
  using difference_type = int32_t;

  pointer allocate(size_type);
  void deallocate(pointer, size_type);
  constexpr size_type max_size() const noexcept;
};

template<> struct relptr_allocator<void> {
  using value_type = void;
  using pointer = relptr<value_type>;
  using const_pointer = relptr<const value_type>;
  using size_type = uint32_t;
  using difference_type = int32_t;
};

template<typename T, typename U>
bool operator==(const relptr_allocator<T>&, const relptr_allocator<U>&)
    noexcept;

template<typename T, typename U>
bool operator!=(const relptr_allocator<T>&, const relptr_allocator<U>&)
    noexcept;


}} /* namespace __cxxabiv1::ext */

#include <abi/ext/relptr-inl.h>

#endif /* _ABI_EXT_RELPTR_H_ */
