#ifndef _STDIMPL_GCD_H_
#define _STDIMPL_GCD_H_

_namespace_begin(std)
namespace impl {


template<typename T>
constexpr T sign(T a) noexcept {
  return (a < 0 ? -1 : (a > 0 ? 1 : 0));
}

template<typename T>
constexpr T abs(T a) noexcept {
  return (a < 0 ? -a : a);
}

template<typename T>
constexpr T gcd(T a, T b) noexcept {
  return (a == 0 ?
          abs(b) :
          (b == 0 ?
           abs(a) :
           gcd(abs(b), abs(a) % abs(b))));
}

template<typename T>
constexpr T lcm(T a, T b) noexcept {
  return abs(a) / gcd(a, b) * abs(b);
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_GCD_H_ */
