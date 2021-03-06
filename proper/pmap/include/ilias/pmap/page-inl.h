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
constexpr vaddr<Arch>::vaddr(const vpage_no<Arch>& pgno) noexcept
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

template<arch Arch>
auto page_no<Arch>::operator++() noexcept -> page_no& {
  ++v_;
  return *this;
}

template<arch Arch>
auto page_no<Arch>::operator++(int) noexcept -> page_no {
  page_no tmp = *this;
  ++*this;
  return tmp;
}

template<arch Arch>
auto page_no<Arch>::operator--() noexcept -> page_no& {
  --v_;
  return *this;
}

template<arch Arch>
auto page_no<Arch>::operator--(int) noexcept -> page_no {
  page_no tmp = *this;
  --*this;
  return tmp;
}

template<arch Arch>
auto page_no<Arch>::operator+=(page_count<Arch> c) noexcept -> page_no& {
  v_ += c.get();
  return *this;
}

template<arch Arch>
auto page_no<Arch>::operator-=(page_count<Arch> c) noexcept -> page_no& {
  v_ -= c.get();
  return *this;
}


template<arch Arch>
constexpr vpage_no<Arch>::vpage_no(const vaddr<Arch>& va)
: v_(va.get() >> page_shift(Arch))
{
  if ((va.get() & page_mask(Arch)) != 0)
    throw std::invalid_argument("Virtual address is not page aligned.");
  if (va.get() > std::numeric_limits<type>::max())
    throw std::out_of_range("Virtual address too large.");
}

template<arch Arch>
auto vpage_no<Arch>::operator++() noexcept -> vpage_no& {
  ++v_;
  return *this;
}

template<arch Arch>
auto vpage_no<Arch>::operator++(int) noexcept -> vpage_no {
  vpage_no tmp = *this;
  ++*this;
  return tmp;
}

template<arch Arch>
auto vpage_no<Arch>::operator--() noexcept -> vpage_no& {
  --v_;
  return *this;
}

template<arch Arch>
auto vpage_no<Arch>::operator--(int) noexcept -> vpage_no {
  vpage_no tmp = *this;
  --*this;
  return tmp;
}

template<arch Arch>
auto vpage_no<Arch>::operator+=(page_count<Arch> c) noexcept -> vpage_no& {
  v_ += c.get();
  return *this;
}

template<arch Arch>
auto vpage_no<Arch>::operator-=(page_count<Arch> c) noexcept -> vpage_no& {
  v_ -= c.get();
  return *this;
}


template<arch Arch>
auto page_count<Arch>::operator++() noexcept -> page_count& {
  ++v_;
  return *this;
}

template<arch Arch>
auto page_count<Arch>::operator++(int) noexcept -> page_count {
  page_count tmp = *this;
  ++*this;
  return tmp;
}

template<arch Arch>
auto page_count<Arch>::operator--() noexcept -> page_count& {
  --v_;
  return *this;
}

template<arch Arch>
auto page_count<Arch>::operator--(int) noexcept -> page_count {
  page_count tmp = *this;
  --*this;
  return tmp;
}

template<arch Arch>
auto page_count<Arch>::operator+=(page_count c) noexcept -> page_count& {
  v_ += c.get();
  return *this;
}

template<arch Arch>
auto page_count<Arch>::operator-=(page_count c) noexcept -> page_count& {
  v_ -= c.get();
  return *this;
}


template<arch A>
constexpr bool operator==(const phys_addr<A>& a, const phys_addr<A>& b)
    noexcept {
  return a.get() == b.get();
}
template<arch A>
constexpr bool operator!=(const phys_addr<A>& a, const phys_addr<A>& b)
    noexcept {
  return !(a == b);
}
template<arch A>
constexpr bool operator<(const phys_addr<A>& a, const phys_addr<A>& b)
    noexcept {
  return a.get() < b.get();
}
template<arch A>
constexpr bool operator>(const phys_addr<A>& a, const phys_addr<A>& b)
    noexcept {
  return b < a;
}
template<arch A>
constexpr bool operator<=(const phys_addr<A>& a, const phys_addr<A>& b)
    noexcept {
  return !(b < a);
}
template<arch A>
constexpr bool operator>=(const phys_addr<A>& a, const phys_addr<A>& b)
    noexcept {
  return !(a < b);
}

template<arch A>
constexpr bool operator==(const vaddr<A>& a, const vaddr<A>& b) noexcept {
  return a.get() == b.get();
}
template<arch A>
constexpr bool operator!=(const vaddr<A>& a, const vaddr<A>& b) noexcept {
  return !(a == b);
}
template<arch A>
constexpr bool operator<(const vaddr<A>& a, const vaddr<A>& b) noexcept {
  return a.get() < b.get();
}
template<arch A>
constexpr bool operator>(const vaddr<A>& a, const vaddr<A>& b) noexcept {
  return b < a;
}
template<arch A>
constexpr bool operator<=(const vaddr<A>& a, const vaddr<A>& b) noexcept {
  return !(b < a);
}
template<arch A>
constexpr bool operator>=(const vaddr<A>& a, const vaddr<A>& b) noexcept {
  return !(a < b);
}

template<arch A>
constexpr bool operator==(const page_no<A>& a, const page_no<A>& b) noexcept {
  return a.get() == b.get();
}
template<arch A>
constexpr bool operator!=(const page_no<A>& a, const page_no<A>& b) noexcept {
  return !(a == b);
}
template<arch A>
constexpr bool operator<(const page_no<A>& a, const page_no<A>& b) noexcept {
  return a.get() < b.get();
}
template<arch A>
constexpr bool operator>(const page_no<A>& a, const page_no<A>& b) noexcept {
  return b < a;
}
template<arch A>
constexpr bool operator<=(const page_no<A>& a, const page_no<A>& b) noexcept {
  return !(b < a);
}
template<arch A>
constexpr bool operator>=(const page_no<A>& a, const page_no<A>& b) noexcept {
  return !(a < b);
}

template<arch A>
constexpr bool operator==(const vpage_no<A>& a, const vpage_no<A>& b)
    noexcept {
  return a.get() == b.get();
}
template<arch A>
constexpr bool operator!=(const vpage_no<A>& a, const vpage_no<A>& b)
    noexcept {
  return !(a == b);
}
template<arch A>
constexpr bool operator<(const vpage_no<A>& a, const vpage_no<A>& b)
    noexcept {
  return a.get() < b.get();
}
template<arch A>
constexpr bool operator>(const vpage_no<A>& a, const vpage_no<A>& b)
    noexcept {
  return b < a;
}
template<arch A>
constexpr bool operator<=(const vpage_no<A>& a, const vpage_no<A>& b)
    noexcept {
  return !(b < a);
}
template<arch A>
constexpr bool operator>=(const vpage_no<A>& a, const vpage_no<A>& b)
    noexcept {
  return !(a < b);
}

template<arch A>
constexpr bool operator==(const phys_addr<A>& a, const page_no<A>& b)
    noexcept {
  return a == phys_addr<A>(b);
}
template<arch A>
constexpr bool operator==(const page_no<A>& a, const phys_addr<A>& b)
    noexcept {
  return phys_addr<A>(a) == b;
}
template<arch A>
constexpr bool operator!=(const phys_addr<A>& a, const page_no<A>& b)
    noexcept {
  return a != phys_addr<A>(b);
}
template<arch A>
constexpr bool operator!=(const page_no<A>& a, const phys_addr<A>& b)
    noexcept {
  return phys_addr<A>(a) != b;
}
template<arch A>
constexpr bool operator<(const phys_addr<A>& a, const page_no<A>& b)
    noexcept {
  return a < phys_addr<A>(b);
}
template<arch A>
constexpr bool operator<(const page_no<A>& a, const phys_addr<A>& b)
    noexcept {
  return phys_addr<A>(a) < b;
}
template<arch A>
constexpr bool operator>(const phys_addr<A>& a, const page_no<A>& b)
    noexcept {
  return a > phys_addr<A>(b);
}
template<arch A>
constexpr bool operator>(const page_no<A>& a, const phys_addr<A>& b)
    noexcept {
  return phys_addr<A>(a) > b;
}
template<arch A>
constexpr bool operator<=(const phys_addr<A>& a, const page_no<A>& b)
    noexcept {
  return a <= phys_addr<A>(b);
}
template<arch A>
constexpr bool operator<=(const page_no<A>& a, const phys_addr<A>& b)
    noexcept {
  return phys_addr<A>(a) <= b;
}
template<arch A>
constexpr bool operator>=(const phys_addr<A>& a, const page_no<A>& b)
    noexcept {
  return a >= phys_addr<A>(b);
}
template<arch A>
constexpr bool operator>=(const page_no<A>& a, const phys_addr<A>& b)
    noexcept {
  return phys_addr<A>(a) >= b;
}

template<arch A>
constexpr bool operator==(const vaddr<A>& a, const vpage_no<A>& b) noexcept {
  return a == vaddr<A>(b);
}
template<arch A>
constexpr bool operator==(const vpage_no<A>& a, const vaddr<A>& b) noexcept {
  return vaddr<A>(a) == b;
}
template<arch A>
constexpr bool operator!=(const vaddr<A>& a, const vpage_no<A>& b) noexcept {
  return a != vaddr<A>(b);
}
template<arch A>
constexpr bool operator!=(const vpage_no<A>& a, const vaddr<A>& b) noexcept {
  return vaddr<A>(a) != b;
}
template<arch A>
constexpr bool operator<(const vaddr<A>& a, const vpage_no<A>& b) noexcept {
  return a < vaddr<A>(b);
}
template<arch A>
constexpr bool operator<(const vpage_no<A>& a, const vaddr<A>& b) noexcept {
  return vaddr<A>(a) < b;
}
template<arch A>
constexpr bool operator>(const vaddr<A>& a, const vpage_no<A>& b) noexcept {
  return a > vaddr<A>(b);
}
template<arch A>
constexpr bool operator>(const vpage_no<A>& a, const vaddr<A>& b) noexcept {
  return vaddr<A>(a) > b;
}
template<arch A>
constexpr bool operator<=(const vaddr<A>& a, const vpage_no<A>& b) noexcept {
  return a <= vaddr<A>(b);
}
template<arch A>
constexpr bool operator<=(const vpage_no<A>& a, const vaddr<A>& b) noexcept {
  return vaddr<A>(a) <= b;
}
template<arch A>
constexpr bool operator>=(const vaddr<A>& a, const vpage_no<A>& b) noexcept {
  return a >= vaddr<A>(b);
}
template<arch A>
constexpr bool operator>=(const vpage_no<A>& a, const vaddr<A>& b) noexcept {
  return vaddr<A>(a) >= b;
}

template<arch A>
constexpr bool operator==(const page_count<A>& a, const page_count<A>& b)
    noexcept {
  return a.get() == b.get();
}
template<arch A>
constexpr bool operator!=(const page_count<A>& a, const page_count<A>& b)
    noexcept {
  return !(a == b);
}
template<arch A>
constexpr bool operator<(const page_count<A>& a, const page_count<A>& b)
    noexcept {
  return a.get() < b.get();
}
template<arch A>
constexpr bool operator>(const page_count<A>& a, const page_count<A>& b)
    noexcept {
  return b < a;
}
template<arch A>
constexpr bool operator<=(const page_count<A>& a, const page_count<A>& b)
    noexcept {
  return !(b < a);
}
template<arch A>
constexpr bool operator>=(const page_count<A>& a, const page_count<A>& b)
    noexcept {
  return !(a < b);
}

template<arch A>
constexpr page_count<A> operator-(const page_no<A>& a, const page_no<A>& b)
    noexcept {
  return page_count<A>(b.get() - a.get());
}
template<arch A>
constexpr page_no<A> operator+(const page_no<A>& a, const page_count<A>& b)
    noexcept {
  return page_no<A>(a.get() + b.get());
}
template<arch A>
constexpr page_no<A> operator+(const page_count<A>& a, const page_no<A>& b)
    noexcept {
  return b + a;
}
template<arch A>
constexpr page_no<A> operator-(const page_no<A>& a, const page_count<A>& b)
    noexcept {
  return page_no<A>(a.get() - b.get());
}

template<arch A>
constexpr page_count<A> operator-(const vpage_no<A>& a, const vpage_no<A>& b)
    noexcept {
  return page_count<A>(b.get() - a.get());
}
template<arch A>
constexpr vpage_no<A> operator+(const vpage_no<A>& a, const page_count<A>& b)
    noexcept {
  return vpage_no<A>(a.get() + b.get());
}
template<arch A>
constexpr vpage_no<A> operator+(const page_count<A>& a, const vpage_no<A>& b)
    noexcept {
  return b + a;
}
template<arch A>
constexpr vpage_no<A> operator-(const vpage_no<A>& a, const page_count<A>& b)
    noexcept {
  return vpage_no<A>(a.get() - b.get());
}

template<arch A>
constexpr page_count<A> operator+(const page_count<A>& a,
                                  const page_count<A>& b) noexcept {
  return page_count<A>(a.get() + b.get());
}
template<arch A>
constexpr page_count<A> operator-(const page_count<A>& a,
                                  const page_count<A>& b) noexcept {
  return page_count<A>(a.get() - b.get());
}

template<arch A>
constexpr page_count<A> operator*(const page_count<A>& a, int b) noexcept {
  return page_count<A>(a.get() * b);
}
template<arch A>
constexpr page_count<A> operator*(int a, const page_count<A>& b) noexcept {
  return b * a;
}
template<arch A>
constexpr page_count<A> operator/(const page_count<A>& a, int b) noexcept {
  return page_count<A>(a.get() / b);
}
template<arch A>
constexpr auto operator/(const page_count<A>& a, const page_count<A>& b)
    noexcept -> typename page_count<A>::type {
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
auto hash<ilias::pmap::vaddr<A>>::operator()(
    const ilias::pmap::vaddr<A>& va) const noexcept -> size_t {
  return hash<typename ilias::pmap::vaddr<A>::type>()(va.get());
}

template<ilias::arch A>
auto hash<ilias::pmap::page_no<A>>::operator()(
    const ilias::pmap::page_no<A>& pg) const noexcept -> size_t {
  return hash<ilias::pmap::phys_addr<A>>()(pg);
}

template<ilias::arch A>
auto hash<ilias::pmap::vpage_no<A>>::operator()(
    const ilias::pmap::vpage_no<A>& pg) const noexcept -> size_t {
  return hash<ilias::pmap::vaddr<A>>()(pg);
}

template<ilias::arch A>
auto hash<ilias::pmap::page_count<A>>::operator()(
    const ilias::pmap::page_count<A>& c) const noexcept -> size_t {
  return hash<typename ilias::pmap::page_count<A>::type>()(c.get());
}

} /* namespace std */

#endif /* _ILIAS_PMAP_PAGE_INL_H_ */
