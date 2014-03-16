#ifndef _ABI_EXT_DTOA_H_
#define _ABI_EXT_DTOA_H_

#include <cdecl.h>
#include <abi/abi.h>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace __cxxabiv1 {
namespace ext {


class bigint {
 private:
  using int_t = uintptr_t;

 public:
  bigint() = default;
  bigint(const bigint&) = default;
  bigint(bigint&&) noexcept;
  bigint& operator=(bigint) noexcept;

  template<typename Char, typename Traits>
  explicit bigint(
      _namespace(std)::basic_string_ref<Char, Traits>,
      typename _namespace(std)::basic_string_ref<Char, Traits>::size_type* =
        nullptr);
  bigint(uintmax_t);
  bigint(intmax_t);

  template<typename Int, typename = _namespace(std)::enable_if_t<
      _namespace(std)::is_integral<Int>::value,
      _namespace(std)::conditional_t<_namespace(std)::is_unsigned<Int>::value,
                                     uintmax_t, intmax_t>>>
  bigint(Int);

  void swap(bigint&) noexcept;
  void shrink_to_fit()
      noexcept(noexcept(_namespace(std)::declval<
          _namespace(std)::vector<int_t>>().shrink_to_fit()));

  friend int compare(const bigint&, const bigint&) noexcept;

  static bigint multadd(bigint, uintmax_t, uintmax_t = 0);
  bigint& multadd(uintmax_t, uintmax_t = 0);
  static bigint multsub(bigint, uintmax_t, uintmax_t = 0);
  bigint& multsub(uintmax_t, uintmax_t = 0);
  static bigint powmult(bigint, uintmax_t, uintmax_t);
  bigint& powmult(uintmax_t, uintmax_t);
  static _namespace(std)::tuple<bigint, bigint> divmod(bigint, bigint);
  static _namespace(std)::tuple<bigint, uintmax_t> divmod(bigint, uintmax_t);

  friend bigint operator+(bigint, uintmax_t);
  friend bigint operator+(uintmax_t, bigint);
  friend bigint operator+(bigint, const bigint&);
  friend bigint operator+(const bigint&, bigint&&);
  friend bigint operator+(bigint&&, bigint&&);
  bigint& operator+=(uintmax_t);
  bigint& operator+=(const bigint&);
  bigint& operator+=(bigint&&);

  friend bigint operator-(bigint, uintmax_t);
  friend bigint operator-(uintmax_t, bigint);
  friend bigint operator-(const bigint&, bigint);
  friend bigint operator-(bigint&&, bigint);
  bigint& operator-=(uintmax_t);
  bigint& operator-=(const bigint&);
  bigint& operator-=(bigint&&);

  friend bigint operator*(bigint, uintmax_t);
  friend bigint operator*(uintmax_t, bigint);
  friend bigint operator*(bigint, const bigint&);
  friend bigint operator*(const bigint&, bigint&&);
  friend bigint operator*(bigint&&, bigint&&);
  bigint& operator*=(uintmax_t);
  bigint& operator*=(const bigint&);
  bigint& operator*=(bigint&&);

  friend uintmax_t operator&(const bigint&, uintmax_t) noexcept;
  friend uintmax_t operator&(uintmax_t, const bigint&) noexcept;
  friend bigint operator&(bigint, const bigint&) noexcept;
  bigint& operator&=(uintmax_t) noexcept;
  bigint& operator&=(const bigint&) noexcept;

  friend bigint operator|(bigint, uintmax_t);
  friend bigint operator|(uintmax_t, bigint);
  friend bigint operator|(bigint, const bigint&);
  bigint& operator|=(uintmax_t);
  bigint& operator|=(const bigint&);

  friend bigint operator^(bigint, uintmax_t);
  friend bigint operator^(uintmax_t, bigint);
  friend bigint operator^(bigint, const bigint&);
  bigint& operator^=(uintmax_t);
  bigint& operator^=(const bigint&);

  friend bigint operator>>(bigint, uintmax_t) noexcept;
  bigint& operator>>=(uintmax_t) noexcept;
  friend bigint operator<<(bigint, uintmax_t);
  bigint& operator<<=(uintmax_t);

  friend bigint operator/(bigint, uintmax_t);
  friend bigint operator/(uintmax_t, bigint);
  friend bigint operator/(bigint, bigint);
  bigint& operator/=(uintmax_t);
  bigint& operator/=(bigint);

  friend uintmax_t operator%(bigint, uintmax_t);
  friend uintmax_t operator%(uintmax_t, bigint);
  friend bigint operator%(bigint, bigint);
  bigint& operator%=(uintmax_t);
  bigint& operator%=(bigint);

  friend bigint operator-(bigint) noexcept;
  friend bigint abs(bigint) noexcept;

  explicit operator bool() const noexcept;
  bool operator!() const noexcept;

  friend _namespace(std)::string to_string(bigint);
  bool is_pow2() const noexcept;

 private:
  enum sign { POS, NEG };

  _namespace(std)::vector<int_t> x_;
  sign sign_ = POS;
};

bool operator==(const bigint&, const bigint&) noexcept;
bool operator!=(const bigint&, const bigint&) noexcept;
bool operator<(const bigint&, const bigint&) noexcept;
bool operator>(const bigint&, const bigint&) noexcept;
bool operator<=(const bigint&, const bigint&) noexcept;
bool operator>=(const bigint&, const bigint&) noexcept;

template<typename Int>
auto operator+(bigint, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint>;
template<typename Int>
auto operator+(Int, bigint) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint>;
template<typename Int>
auto operator+(bigint, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint>;
template<typename Int>
auto operator+(Int, bigint) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint>;

template<typename Int>
auto operator-(bigint, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint>;
template<typename Int>
auto operator-(Int, bigint) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint>;
template<typename Int>
auto operator-(bigint, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint>;
template<typename Int>
auto operator-(Int, bigint) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint>;

template<typename Int>
auto operator*(bigint, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint>;
template<typename Int>
auto operator*(Int, bigint) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint>;
template<typename Int>
auto operator*(bigint, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint>;
template<typename Int>
auto operator*(Int, bigint) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint>;

template<typename Int>
auto operator>>(bigint, Int) noexcept ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint>;
template<typename Int>
auto operator>>(bigint, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint>;

template<typename Int>
auto operator<<(bigint, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint>;
template<typename Int>
auto operator<<(bigint, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint>;

template<typename Int>
auto operator>>=(bigint&, Int) noexcept ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint&>;
template<typename Int>
auto operator>>=(bigint&, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint&>;

template<typename Int>
auto operator<<=(bigint&, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint&>;
template<typename Int>
auto operator<<=(bigint&, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint&>;

template<typename Int>
auto operator/(bigint, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint>;
template<typename Int>
auto operator/(Int, bigint) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint>;
template<typename Int>
auto operator/(bigint, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint>;
template<typename Int>
auto operator/(Int, bigint) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint>;

template<typename Int>
auto operator%(bigint, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint>;
template<typename Int>
auto operator%(Int, bigint) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint>;
template<typename Int>
auto operator%(bigint, Int) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint>;
template<typename Int>
auto operator%(Int, bigint) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint>;


}} /* namespace __cxxabiv1::ext */

#include <abi/ext/bigint-inl.h>

#endif /* _ABI_EXT_DTOA_H_ */
