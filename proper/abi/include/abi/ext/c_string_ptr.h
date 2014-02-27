#ifndef _ABI_EXT_C_STRING_PTR_H_
#define _ABI_EXT_C_STRING_PTR_H_

#include <cdecl.h>
#include <abi/abi.h>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iosfwd>
#include <new>
#include <utility>

namespace __cxxabiv1 {
namespace ext {


class c_string_ptr {
 public:
  using size_type = std::size_t;
  using iterator = char*;
  using const_iterator = const char*;

  constexpr c_string_ptr() = default;
  inline ~c_string_ptr() noexcept;
  c_string_ptr(const c_string_ptr&);
  inline c_string_ptr(c_string_ptr&&) noexcept;
  c_string_ptr(const std::string&);

  void reserve(size_type);
  void shrink_to_fit() noexcept;
  inline size_type size() const noexcept;
  inline size_type length() const noexcept;
  inline bool empty() const noexcept;
  inline char* release() noexcept;
  inline char* c_str() const noexcept;
  inline char* data() const noexcept;
  inline void swap(c_string_ptr&) noexcept;
  friend inline void swap(c_string_ptr&, c_string_ptr&) noexcept;
  friend inline c_string_ptr make_c_string_ptr_buffer(char*, size_type) noexcept;

  inline bool operator==(const c_string_ptr&) const noexcept;
  inline bool operator==(const char*) const noexcept;
  inline bool operator!=(const c_string_ptr&) const noexcept;
  inline bool operator!=(const char*) const noexcept;

  c_string_ptr& operator=(const c_string_ptr&);
  inline c_string_ptr& operator=(c_string_ptr&&) noexcept;

  inline c_string_ptr& operator+=(const c_string_ptr&);
  c_string_ptr& operator+=(_namespace(std)::string_ref);
  inline c_string_ptr& operator+=(const char*);
  inline void append(const c_string_ptr&);
  inline void append(const char*);
  void append(const char*, size_type);
  void clear() noexcept;

  inline iterator begin();
  inline iterator end();
  inline const_iterator begin() const;
  inline const_iterator end() const;
  inline const_iterator cbegin() const;
  inline const_iterator cend() const;
  inline char& operator[](size_type idx);
  inline const char& operator[](size_type idx) const;

  operator std::string() const;

 private:
  char* data_ = nullptr;
  size_type len_ = 0;
  size_type capacity_ = 0;
};

inline c_string_ptr make_c_string_ptr(const char*);
inline c_string_ptr make_c_string_ptr(const char*, c_string_ptr::size_type);
inline c_string_ptr make_c_string_ptr_buffer(c_string_ptr::size_type);
inline c_string_ptr make_c_string_ptr_buffer(const char*, c_string_ptr::size_type);

inline bool operator==(const char* a, const c_string_ptr& b) noexcept;
inline bool operator!=(const char* a, const c_string_ptr& b) noexcept;


}} /* namespace __cxxabiv1::ext */

#include <abi/ext/c_string_ptr-inl.h>

#endif /* _ABI_EXT_C_STRING_PTR_H_ */
