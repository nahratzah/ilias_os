#ifndef _SYSTEM_ERROR_INL_H_
#define _SYSTEM_ERROR_INL_H_

#include <system_error>

_namespace_begin(std)


constexpr error_category::error_category() noexcept {}

inline bool error_category::operator==(const error_category& rhs)
    const noexcept {
  return this == &rhs;
}

inline bool error_category::operator!=(const error_category& rhs)
    const noexcept {
  return !(*this == rhs);
}

inline bool error_category::operator<(const error_category& rhs)
    const noexcept {
  return this < &rhs;
}


inline error_code::error_code() noexcept
: error_code(0, system_category())
{}

inline error_code::error_code(int val, const error_category& cat) noexcept
: val_(val),
  cat_(&cat)
{}

template<class ErrorCodeEnum>
error_code::error_code(ErrorCodeEnum e,
           enable_if_t<is_error_code_enum<ErrorCodeEnum>::value, _enable_tag>)
    noexcept
: error_code(static_cast<int>(e), generic_category())
{}

inline void error_code::assign(int val, const error_category& cat) noexcept {
  val_ = val;
  cat_ = &cat;
}

template<typename ErrorCodeEnum>
auto error_code::operator=(
    ErrorCodeEnum e) noexcept ->
    enable_if_t<is_error_code_enum<ErrorCodeEnum>::value, error_code&> {
  val_ = static_cast<int>(e);
  cat_ = &generic_category();
}

inline void error_code::clear() noexcept {
  assign(0, system_category());
}

inline int error_code::value() const noexcept {
  return val_;
}

inline const error_category& error_code::category() const noexcept {
  return *cat_;
}

inline error_code::operator bool() const noexcept {
  return value() != 0;
}

inline error_code make_error_code(errc e) noexcept {
  return error_code(static_cast<int>(e), generic_category());
}

inline bool operator<(const error_code& lhs, const error_code& rhs) noexcept {
  return lhs.category() < rhs.category() ||
         (lhs.category() == rhs.category() && lhs.value() == rhs.value());
}

#if __has_include(<ostream>)
template<typename Char, typename Traits>
basic_ostream<Char, Traits>& operator<<(basic_ostream<Char, Traits>& os,
                                        const error_code& code) {
  return os << ec.category().name() << ':' << ec.value();
}
#endif


inline error_condition::error_condition() noexcept
: error_condition(0, generic_category())
{}

inline error_condition::error_condition(int val, const error_category& cat)
    noexcept
: val_(val),
  cat_(&cat)
{}

template<class ErrorCodeEnum>
error_condition::error_condition(ErrorCodeEnum e,
       enable_if_t<is_error_condition_enum<ErrorCodeEnum>::value, _enable_tag>)
    noexcept
: error_condition(static_cast<int>(e), generic_category())
{}

inline void error_condition::assign(int val, const error_category& cat)
    noexcept {
  val_ = val;
  cat_ = &cat;
}

template<typename ErrorCodeEnum>
auto error_condition::operator=(ErrorCodeEnum e) noexcept ->
    enable_if_t<is_error_condition_enum<ErrorCodeEnum>::value,
              error_condition&> {
  assign(static_cast<int>(e), generic_category());
}

inline void error_condition::clear() noexcept {
  assign(0, generic_category());
}

inline int error_condition::value() const noexcept {
  return val_;
}

inline const error_category& error_condition::category() const noexcept {
  return *cat_;
}

inline error_condition::operator bool() const noexcept {
  return value() != 0;
}

inline error_condition make_error_condition(errc e) noexcept {
  return error_condition(static_cast<int>(e), generic_category());
}

inline bool operator<(const error_condition& lhs, const error_condition& rhs)
    noexcept {
  return lhs.category() < rhs.category() ||
         (lhs.category() == rhs.category() && lhs.value() < rhs.value());
}


inline bool operator==(const error_code& lhs, const error_code& rhs) noexcept {
  return lhs.category() == rhs.category() && lhs.value() == rhs.value();
}

inline bool operator==(const error_code& lhs, const error_condition& rhs)
    noexcept {
  return lhs.category().equivalent(lhs.value(), rhs) ||
         rhs.category().equivalent(lhs, rhs.value());
}

inline bool operator==(const error_condition& lhs, const error_code& rhs)
    noexcept {
  return rhs.category().equivalent(rhs.value(), lhs) ||
         lhs.category().equivalent(rhs, lhs.value());
}

inline bool operator==(const error_condition& lhs, const error_condition& rhs)
    noexcept {
  return lhs.category() == rhs.category() && lhs.value() == rhs.value();
}

inline bool operator!=(const error_code& lhs, const error_code& rhs)
    noexcept {
  return !(lhs == rhs);
}
inline bool operator!=(const error_code& lhs, const error_condition& rhs)
    noexcept {
  return !(lhs == rhs);
}
inline bool operator!=(const error_condition& lhs, const error_code& rhs)
    noexcept {
  return !(lhs == rhs);
}
inline bool operator!=(const error_condition& lhs, const error_condition& rhs)
    noexcept {
  return !(lhs == rhs);
}


_namespace_end(std)

#endif /* _SYSTEM_ERROR_INL_H_ */
