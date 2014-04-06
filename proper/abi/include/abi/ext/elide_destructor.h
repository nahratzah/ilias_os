#ifndef _ABI_EXT_ELIDE_DESTRUCTOR_H_
#define _ABI_EXT_ELIDE_DESTRUCTOR_H_

#include <cdecl.h>
#include <abi/abi.h>
#include <memory>
#include <type_traits>
#include <utility>

namespace __cxxabiv1 {
namespace ext {


/*
 * Block the destructor from being called.
 * The destructor will be silently ignored.
 */
template<typename T>
class elide_destructor {
 public:
  using element_type = T;
  using pointer = element_type*;
  using reference = element_type&;
  using const_pointer = const element_type*;
  using const_reference = const element_type&;
  using difference_type = ptrdiff_t;

 private:
  union data_t {
    template<typename... Args>
    data_t(Args&&... args)
      noexcept(_namespace(std)::is_nothrow_default_constructible<
                   element_type>::value);

    ~data_t() noexcept {}

    T v;
  };

 public:
  elide_destructor()
    noexcept(_namespace(std)::is_nothrow_default_constructible<data_t>::value);
  elide_destructor(const elide_destructor&) = delete;
  elide_destructor& operator=(const elide_destructor&) = delete;
  template<typename... Args> elide_destructor(Args&&... args)
    noexcept(_namespace(std)::is_nothrow_constructible<
                 data_t, Args...>::value);

  pointer operator->() noexcept;
  const_pointer operator->() const noexcept;
  reference operator*() noexcept;
  const_reference operator*() const noexcept;

  explicit operator bool() const noexcept;
  bool operator!() const noexcept;

 private:
  data_t data_;
};


}} /* namespace __cxxabiv1::ext */

#include <abi/ext/elide_destructor-inl.h>

#endif /* _ABI_EXT_ELIDE_DESTRUCTOR_H_ */
