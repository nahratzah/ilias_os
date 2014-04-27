#ifndef _ILIAS_PMAP_PAGE_INL_H_
#define _ILIAS_PMAP_PAGE_INL_H_

#include <ilias/pmap/page.h>
#include <functional>
#include <stdexcept>
#include <limits>

namespace ilias {
namespace pmap {


template<arch Arch>
constexpr phys_addr<Arch>::phys_addr(const page_no<Arch>& pgno) noexcept
: v_(pgno.get() << page_shift(Arch))
{}


template<arch Arch>
constexpr page_no<Arch>::page_no(const phys_addr<Arch>& pa)
: v_(pa.get() >> page_shift(Arch))
{
  if ((pa.get() & page_mask(Arch)) != 0)
    throw std::invalid_argument("Physical address is not page aligned.");
  if (pa.get() > std::numeric_limits<type>::max())
    throw std::out_of_range("Physical address too large.");
}


template<arch A>
bool operator==(const phys_addr<A>& a, const phys_addr<A>& b) noexcept {
  return a.get() == b.get();
}
template<arch A>
bool operator!=(const phys_addr<A>& a, const phys_addr<A>& b) noexcept {
  return !(a == b);
}
template<arch A>
bool operator<(const phys_addr<A>& a, const phys_addr<A>& b) noexcept {
  return a.get() < b.get();
}
template<arch A>
bool operator>(const phys_addr<A>& a, const phys_addr<A>& b) noexcept {
  return b < a;
}
template<arch A>
bool operator<=(const phys_addr<A>& a, const phys_addr<A>& b) noexcept {
  return !(b < a);
}
template<arch A>
bool operator>=(const phys_addr<A>& a, const phys_addr<A>& b) noexcept {
  return !(a < b);
}

template<arch A>
bool operator==(const page_no<A>& a, const page_no<A>& b) noexcept {
  return a.get() == b.get();
}
template<arch A>
bool operator!=(const page_no<A>& a, const page_no<A>& b) noexcept {
  return !(a == b);
}
template<arch A>
bool operator<(const page_no<A>& a, const page_no<A>& b) noexcept {
  return a.get() < b.get();
}
template<arch A>
bool operator>(const page_no<A>& a, const page_no<A>& b) noexcept {
  return b < a;
}
template<arch A>
bool operator<=(const page_no<A>& a, const page_no<A>& b) noexcept {
  return !(b < a);
}
template<arch A>
bool operator>=(const page_no<A>& a, const page_no<A>& b) noexcept {
  return !(a < b);
}

template<arch A>
bool operator==(const phys_addr<A>& a, const page_no<A>& b) noexcept {
  return a == phys_addr<A>(b);
}
template<arch A>
bool operator==(const page_no<A>& a, const phys_addr<A>& b) noexcept {
  return phys_addr<A>(a) == b;
}
template<arch A>
bool operator!=(const phys_addr<A>& a, const page_no<A>& b) noexcept {
  return a != phys_addr<A>(b);
}
template<arch A>
bool operator!=(const page_no<A>& a, const phys_addr<A>& b) noexcept {
  return phys_addr<A>(a) != b;
}
template<arch A>
bool operator<(const phys_addr<A>& a, const page_no<A>& b) noexcept {
  return a < phys_addr<A>(b);
}
template<arch A>
bool operator<(const page_no<A>& a, const phys_addr<A>& b) noexcept {
  return phys_addr<A>(a) < b;
}
template<arch A>
bool operator>(const phys_addr<A>& a, const page_no<A>& b) noexcept {
  return a > phys_addr<A>(b);
}
template<arch A>
bool operator>(const page_no<A>& a, const phys_addr<A>& b) noexcept {
  return phys_addr<A>(a) > b;
}
template<arch A>
bool operator<=(const phys_addr<A>& a, const page_no<A>& b) noexcept {
  return a <= phys_addr<A>(b);
}
template<arch A>
bool operator<=(const page_no<A>& a, const phys_addr<A>& b) noexcept {
  return phys_addr<A>(a) <= b;
}
template<arch A>
bool operator>=(const phys_addr<A>& a, const page_no<A>& b) noexcept {
  return a >= phys_addr<A>(b);
}
template<arch A>
bool operator>=(const page_no<A>& a, const phys_addr<A>& b) noexcept {
  return phys_addr<A>(a) >= b;
}

template<arch A>
bool operator==(const page_count<A>& a, const page_count<A>& b) noexcept {
  return a.get() == b.get();
}
template<arch A>
bool operator!=(const page_count<A>& a, const page_count<A>& b) noexcept {
  return !(a == b);
}
template<arch A>
bool operator<(const page_count<A>& a, const page_count<A>& b) noexcept {
  return a.get() < b.get();
}
template<arch A>
bool operator>(const page_count<A>& a, const page_count<A>& b) noexcept {
  return b < a;
}
template<arch A>
bool operator<=(const page_count<A>& a, const page_count<A>& b) noexcept {
  return !(b < a);
}
template<arch A>
bool operator>=(const page_count<A>& a, const page_count<A>& b) noexcept {
  return !(a < b);
}

template<arch A>
page_count<A> operator-(const page_no<A>& a, const page_no<A>& b) noexcept {
  return page_count<A>(b.get() - a.get());
}
template<arch A>
page_no<A> operator+(const page_no<A>& a, const page_count<A>& b) noexcept {
  return page_no<A>(a.get() + b.get());
}
template<arch A>
page_no<A> operator+(const page_count<A>& a, const page_no<A>& b) noexcept {
  return b + a;
}
template<arch A>
page_no<A> operator-(const page_no<A>& a, const page_count<A>& b) noexcept {
  return page_no<A>(a.get() - b.get());
}
template<arch A>
page_count<A> operator+(const page_count<A>& a, const page_count<A>& b)
    noexcept {
  return page_count<A>(a.get() + b.get());
}
template<arch A>
page_count<A> operator-(const page_count<A>& a, const page_count<A>& b)
    noexcept {
  return page_count<A>(a.get() - b.get());
}

template<arch A>
page_count<A> operator*(const page_count<A>& a, int b) noexcept {
  return page_count<A>(a.get() * b);
}
template<arch A>
page_count<A> operator*(int a, const page_count<A>& b) noexcept {
  return b * a;
}
template<arch A>
page_count<A> operator/(const page_count<A>& a, int b) noexcept {
  return page_count<A>(a.get() / b);
}
template<arch A>
auto operator/(const page_count<A>& a, const page_count<A>& b) noexcept ->
    typename page_count<A>::type {
  return a.get() / b.get();
}


}} /* namespace ilias::pmap */


namespace std {

template<ilias::arch A>
auto hash<ilias::pmap::phys_addr<A>>::operator()(
    const ilias::pmap::phys_addr<A>& paddr) const noexcept -> size_t {
  return hash<typename ilias::pmap::phys_addr<A>::type>()(paddr.get());
}

template<ilias::arch A>
auto hash<ilias::pmap::page_no<A>>::operator()(
    const ilias::pmap::page_no<A>& pg) const noexcept -> size_t {
  return hash<ilias::pmap::phys_addr<A>>()(pg.get());
}

} /* namespace std */

#endif /* _ILIAS_PMAP_PAGE_INL_H_ */
