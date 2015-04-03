#ifndef _ILIAS_OPTIONAL_H_
#define _ILIAS_OPTIONAL_H_

#include <cdecl.h>
#include <type_traits>
#include <stdexcept>
#include <string>
#include <tuple>

_namespace_begin(ilias)


class optional_error
: public _namespace(std)::runtime_error
{
 public:
  explicit optional_error(_namespace(std)::string_ref);
  explicit optional_error(const _namespace(std)::string&);
  explicit optional_error(const char*);

  ~optional_error() noexcept override;

  static void __throw(_namespace(std)::string_ref)
      __attribute__((__noreturn__));
  static void __throw(const _namespace(std)::string&)
      __attribute__((__noreturn__));
  static void __throw(const char*)
      __attribute__((__noreturn__));
};


template<typename T>
class optional {
 private:
  using storage_t = _namespace(std)::aligned_union_t<0, T>;

  static_assert(!_namespace(std)::is_reference<T>::value,
                "Optional may not be a reference type.");

  static constexpr bool nothrow_copy_() { return _namespace(std)::is_nothrow_copy_constructible<T>::value; }
  static constexpr bool nothrow_move_() { return _namespace(std)::is_nothrow_copy_constructible<T>::value; }
  static constexpr bool nothrow_assign_() { return _namespace(std)::is_nothrow_assignable<T, const T&>::value; }
  static constexpr bool nothrow_move_assign_() { return _namespace(std)::is_nothrow_assignable<T, T&&>::value; }

 public:
  using value_type = T;
  using const_value_type = const T;
  using reference = value_type&;
  using const_reference = const_value_type&;
  using pointer = value_type*;
  using const_pointer = const_value_type*;

  constexpr optional() noexcept;
  optional(const optional&) noexcept(nothrow_copy_());
  optional(optional&&) noexcept(nothrow_move_());
  optional(const value_type&) noexcept(nothrow_copy_());
  optional(value_type&&) noexcept(nothrow_move_());
  ~optional() noexcept;

  optional& operator=(const optional&)
      noexcept(nothrow_copy_() && nothrow_assign_());
  optional& operator=(optional&&)
      noexcept(nothrow_move_() && nothrow_move_assign_());

  explicit operator bool() const noexcept { return is_present(); }
  bool is_present() const noexcept;

  reference operator*();
  const_reference operator*() const;
  pointer operator->();
  const_pointer operator->() const;

  void assign(const value_type&)
      noexcept(nothrow_copy_() && nothrow_assign_());
  void assign(value_type&&)
      noexcept(nothrow_move_() && nothrow_move_assign_());
  value_type release();
  const value_type& get() const;
  value_type& get();

 private:
  void ensure_present_() const;

  _namespace(std)::tuple<bool, storage_t> data_;
};


_namespace_end(ilias)

#include "optional-inl.h"

#endif /* _ILIAS_OPTIONAL_H_ */
