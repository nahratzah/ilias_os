#ifndef _STDIMPL_CONVERTIBLE_ENUM_H_
#define _STDIMPL_CONVERTIBLE_ENUM_H_

#include <cdecl.h>
#include <type_traits>

_namespace_begin(std)
namespace impl {


template<typename Enum>
class convertible_enum {
 public:
  constexpr convertible_enum() = default;
  constexpr convertible_enum(const convertible_enum&) = default;
  convertible_enum& operator=(const convertible_enum&) = default;
  explicit constexpr convertible_enum(Enum e) noexcept : enum_(e) {}
  explicit constexpr convertible_enum(typename underlying_type<Enum>::type)
      noexcept;

  explicit constexpr operator bool() const noexcept;
  constexpr operator typename underlying_type<Enum>::type() const noexcept;

  constexpr bool operator==(const convertible_enum&) const noexcept;
  constexpr bool operator!=(const convertible_enum&) const noexcept;

  constexpr convertible_enum operator&(const convertible_enum&) const noexcept;
  constexpr convertible_enum operator|(const convertible_enum&) const noexcept;
  constexpr convertible_enum operator^(const convertible_enum&) const noexcept;
  constexpr convertible_enum operator~() const noexcept;

  convertible_enum& operator&=(const convertible_enum&) noexcept;
  convertible_enum& operator|=(const convertible_enum&) noexcept;
  convertible_enum& operator^=(const convertible_enum&) noexcept;

 private:
  Enum enum_;
};

template<typename Enum>
constexpr convertible_enum<Enum>::convertible_enum(
    typename underlying_type<Enum>::type e) noexcept
: convertible_enum(static_cast<Enum>(e))
{}

template<typename Enum>
constexpr convertible_enum<Enum>::operator bool() const noexcept {
  return *this != convertible_enum(static_cast<Enum>(0));
}

template<typename Enum>
constexpr convertible_enum<Enum>::
    operator typename underlying_type<Enum>::type() const noexcept {
  using int_type = typename underlying_type<Enum>::type;

  return static_cast<int_type>(enum_);
}

template<typename Enum>
constexpr auto convertible_enum<Enum>::operator==(
    const convertible_enum& other) const noexcept -> bool {
  return enum_ == other.enum_;
}

template<typename Enum>
constexpr auto convertible_enum<Enum>::operator!=(
    const convertible_enum& other) const noexcept -> bool {
  return !(*this == other);
}

template<typename Enum>
constexpr auto convertible_enum<Enum>::operator&(const convertible_enum& other)
    const noexcept -> convertible_enum {
  using int_type = typename underlying_type<Enum>::type;
  using self_t = convertible_enum;

  return self_t(static_cast<Enum>(static_cast<int_type>(enum_) &
                                  static_cast<int_type>(other.enum_)));
}

template<typename Enum>
constexpr auto convertible_enum<Enum>::operator|(const convertible_enum& other)
    const noexcept -> convertible_enum {
  using int_type = typename underlying_type<Enum>::type;
  using self_t = convertible_enum;

  return self_t(static_cast<Enum>(static_cast<int_type>(enum_) |
                                  static_cast<int_type>(other.enum_)));
}

template<typename Enum>
constexpr auto convertible_enum<Enum>::operator^(const convertible_enum& other)
    const noexcept -> convertible_enum {
  using int_type = typename underlying_type<Enum>::type;
  using self_t = convertible_enum;

  return self_t(static_cast<Enum>(static_cast<int_type>(enum_) ^
                                  static_cast<int_type>(other.enum_)));
}

template<typename Enum>
constexpr auto convertible_enum<Enum>::operator~()
    const noexcept -> convertible_enum {
  using int_type = typename underlying_type<Enum>::type;
  using self_t = convertible_enum;

  return self_t(static_cast<Enum>(~static_cast<int_type>(enum_)));
}

template<typename Enum>
auto convertible_enum<Enum>::operator&=(const convertible_enum& other)
    noexcept -> convertible_enum& {
  return *this = (*this & other);
}

template<typename Enum>
auto convertible_enum<Enum>::operator|=(const convertible_enum& other)
    noexcept -> convertible_enum& {
  return *this = (*this | other);
}

template<typename Enum>
auto convertible_enum<Enum>::operator^=(const convertible_enum& other)
    noexcept -> convertible_enum& {
  return *this = (*this ^ other);
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_CONVERTIBLE_ENUM_H_ */
