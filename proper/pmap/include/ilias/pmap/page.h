#ifndef _ILIAS_PMAP_PAGE_H_
#define _ILIAS_PMAP_PAGE_H_

#include <cstddef>
#include <ilias/arch.h>
#include <ilias/pmap/consts.h>
#include <algorithm>
#include <functional>

namespace ilias {
namespace pmap {

template<arch> class phys_addr;
template<arch> class vaddr;
template<arch> class page_no;
template<arch> class vpage_no;
template<arch> class page_count;

/* Physical address. */
template<arch Arch>
class phys_addr {
 public:
  using type = std::make_unsigned_t<char[page_addr_size(Arch)]>;

  constexpr phys_addr() noexcept = default;
  explicit constexpr phys_addr(type v) noexcept : v_(v) {}
  constexpr phys_addr(const page_no<Arch>&) noexcept;
  constexpr phys_addr(const phys_addr&) noexcept = default;
  phys_addr& operator=(const phys_addr&) noexcept = default;

  constexpr type get() const noexcept { return v_; }

 private:
  type v_ = 0;
};

/* Virtual address. */
template<arch Arch>
class vaddr {
 public:
  using type = std::make_unsigned_t<char[pointer_size(Arch)]>;

  constexpr vaddr() noexcept = default;
  explicit constexpr vaddr(type v) noexcept : v_(v) {}
  constexpr vaddr(const vpage_no<Arch>&) noexcept;
  constexpr vaddr(const vaddr&) noexcept = default;
  vaddr& operator=(const vaddr&) noexcept = default;

  constexpr type get() const noexcept { return v_; }

 private:
  type v_ = 0;
};

/* Physical page number. */
template<arch Arch>
class page_no {
 public:
  using type = std::make_unsigned_t<char[page_no_size(Arch)]>;

  constexpr page_no() noexcept = default;
  explicit constexpr page_no(type v) noexcept : v_(v) {}
  constexpr page_no(const phys_addr<Arch>&);
  constexpr page_no(const page_no&) noexcept = default;
  page_no& operator=(const page_no&) noexcept = default;

  constexpr type get() const noexcept { return v_; }

  page_no& operator++() noexcept;
  page_no operator++(int) noexcept;
  page_no& operator--() noexcept;
  page_no operator--(int) noexcept;

  page_no& operator+=(page_count<Arch>) noexcept;
  page_no& operator-=(page_count<Arch>) noexcept;

 private:
  type v_ = 0;
};

/* Virtual page number. */
template<arch Arch>
class vpage_no {
 public:
  using type = std::make_unsigned_t<char[pointer_size(Arch)]>;

  constexpr vpage_no() noexcept = default;
  explicit constexpr vpage_no(type v) noexcept : v_(v) {}
  constexpr vpage_no(const vaddr<Arch>&);
  constexpr vpage_no(const vpage_no&) noexcept = default;
  vpage_no& operator=(const vpage_no&) noexcept = default;

  constexpr type get() const noexcept { return v_; }

  vpage_no& operator++() noexcept;
  vpage_no operator++(int) noexcept;
  vpage_no& operator--() noexcept;
  vpage_no operator--(int) noexcept;

  vpage_no& operator+=(page_count<Arch>) noexcept;
  vpage_no& operator-=(page_count<Arch>) noexcept;

 private:
  type v_ = 0;
};

template<arch Arch>
class page_count {
 private:
  /* Ensure we are large enough to hold the virtual and physical page count. */
  static constexpr auto _bytes_ = (pointer_size(Arch) >= page_no_size(Arch) ?
                                   pointer_size(Arch) :
                                   page_no_size(Arch));

 public:
  using type = std::make_signed_t<char[_bytes_]>;

  constexpr page_count() noexcept = default;
  explicit constexpr page_count(type v) noexcept : v_(v) {}
  constexpr page_count(const page_count&) noexcept = default;
  page_count& operator=(const page_count&) noexcept = default;

  constexpr type get() const noexcept { return v_; }

  page_count& operator++() noexcept;
  page_count operator++(int) noexcept;
  page_count& operator--() noexcept;
  page_count operator--(int) noexcept;

  page_count& operator+=(page_count) noexcept;
  page_count& operator-=(page_count) noexcept;

  explicit operator bool() const noexcept { return v_ != 0; }

 private:
  type v_ = 0;
};

template<arch A>
constexpr bool operator==(const phys_addr<A>&, const phys_addr<A>&) noexcept;
template<arch A>
constexpr bool operator!=(const phys_addr<A>&, const phys_addr<A>&) noexcept;
template<arch A>
constexpr bool operator<(const phys_addr<A>&, const phys_addr<A>&) noexcept;
template<arch A>
constexpr bool operator>(const phys_addr<A>&, const phys_addr<A>&) noexcept;
template<arch A>
constexpr bool operator<=(const phys_addr<A>&, const phys_addr<A>&) noexcept;
template<arch A>
constexpr bool operator>=(const phys_addr<A>&, const phys_addr<A>&) noexcept;

template<arch A>
constexpr bool operator==(const vaddr<A>&, const vaddr<A>&) noexcept;
template<arch A>
constexpr bool operator!=(const vaddr<A>&, const vaddr<A>&) noexcept;
template<arch A>
constexpr bool operator<(const vaddr<A>&, const vaddr<A>&) noexcept;
template<arch A>
constexpr bool operator>(const vaddr<A>&, const vaddr<A>&) noexcept;
template<arch A>
constexpr bool operator<=(const vaddr<A>&, const vaddr<A>&) noexcept;
template<arch A>
constexpr bool operator>=(const vaddr<A>&, const vaddr<A>&) noexcept;

template<arch A>
constexpr bool operator==(const page_no<A>&, const page_no<A>&) noexcept;
template<arch A>
constexpr bool operator!=(const page_no<A>&, const page_no<A>&) noexcept;
template<arch A>
constexpr bool operator<(const page_no<A>&, const page_no<A>&) noexcept;
template<arch A>
constexpr bool operator>(const page_no<A>&, const page_no<A>&) noexcept;
template<arch A>
constexpr bool operator<=(const page_no<A>&, const page_no<A>&) noexcept;
template<arch A>
constexpr bool operator>=(const page_no<A>&, const page_no<A>&) noexcept;

template<arch A>
constexpr bool operator==(const vpage_no<A>&, const vpage_no<A>&) noexcept;
template<arch A>
constexpr bool operator!=(const vpage_no<A>&, const vpage_no<A>&) noexcept;
template<arch A>
constexpr bool operator<(const vpage_no<A>&, const vpage_no<A>&) noexcept;
template<arch A>
constexpr bool operator>(const vpage_no<A>&, const vpage_no<A>&) noexcept;
template<arch A>
constexpr bool operator<=(const vpage_no<A>&, const vpage_no<A>&) noexcept;
template<arch A>
constexpr bool operator>=(const vpage_no<A>&, const vpage_no<A>&) noexcept;

template<arch A>
constexpr bool operator==(const phys_addr<A>&, const page_no<A>&) noexcept;
template<arch A>
constexpr bool operator==(const page_no<A>&, const phys_addr<A>&) noexcept;
template<arch A>
constexpr bool operator!=(const phys_addr<A>&, const page_no<A>&) noexcept;
template<arch A>
constexpr bool operator!=(const page_no<A>&, const phys_addr<A>&) noexcept;
template<arch A>
constexpr bool operator<(const phys_addr<A>&, const page_no<A>&) noexcept;
template<arch A>
constexpr bool operator<(const page_no<A>&, const phys_addr<A>&) noexcept;
template<arch A>
constexpr bool operator>(const phys_addr<A>&, const page_no<A>&) noexcept;
template<arch A>
constexpr bool operator>(const page_no<A>&, const phys_addr<A>&) noexcept;
template<arch A>
constexpr bool operator<=(const phys_addr<A>&, const page_no<A>&) noexcept;
template<arch A>
constexpr bool operator<=(const page_no<A>&, const phys_addr<A>&) noexcept;
template<arch A>
constexpr bool operator>=(const phys_addr<A>&, const page_no<A>&) noexcept;
template<arch A>
constexpr bool operator>=(const page_no<A>&, const phys_addr<A>&) noexcept;

template<arch A>
constexpr bool operator==(const vaddr<A>&, const vpage_no<A>&) noexcept;
template<arch A>
constexpr bool operator==(const vpage_no<A>&, const vaddr<A>&) noexcept;
template<arch A>
constexpr bool operator!=(const vaddr<A>&, const vpage_no<A>&) noexcept;
template<arch A>
constexpr bool operator!=(const vpage_no<A>&, const vaddr<A>&) noexcept;
template<arch A>
constexpr bool operator<(const vaddr<A>&, const vpage_no<A>&) noexcept;
template<arch A>
constexpr bool operator<(const vpage_no<A>&, const vaddr<A>&) noexcept;
template<arch A>
constexpr bool operator>(const vaddr<A>&, const vpage_no<A>&) noexcept;
template<arch A>
constexpr bool operator>(const vpage_no<A>&, const vaddr<A>&) noexcept;
template<arch A>
constexpr bool operator<=(const vaddr<A>&, const vpage_no<A>&) noexcept;
template<arch A>
constexpr bool operator<=(const vpage_no<A>&, const vaddr<A>&) noexcept;
template<arch A>
constexpr bool operator>=(const vaddr<A>&, const vpage_no<A>&) noexcept;
template<arch A>
constexpr bool operator>=(const vpage_no<A>&, const vaddr<A>&) noexcept;

template<arch A>
constexpr bool operator==(const page_count<A>&, const page_count<A>&) noexcept;
template<arch A>
constexpr bool operator!=(const page_count<A>&, const page_count<A>&) noexcept;
template<arch A>
constexpr bool operator<(const page_count<A>&, const page_count<A>&) noexcept;
template<arch A>
constexpr bool operator>(const page_count<A>&, const page_count<A>&) noexcept;
template<arch A>
constexpr bool operator<=(const page_count<A>&, const page_count<A>&) noexcept;
template<arch A>
constexpr bool operator>=(const page_count<A>&, const page_count<A>&) noexcept;

template<arch A>
constexpr page_count<A> operator-(const page_no<A>&, const page_no<A>&)
    noexcept;
template<arch A>
constexpr page_no<A> operator+(const page_no<A>&, const page_count<A>&)
    noexcept;
template<arch A>
constexpr page_no<A> operator+(const page_count<A>&, const page_no<A>&)
    noexcept;
template<arch A>
constexpr page_no<A> operator-(const page_no<A>&, const page_count<A>&)
    noexcept;

template<arch A>
constexpr page_count<A> operator-(const vpage_no<A>&, const vpage_no<A>&)
    noexcept;
template<arch A>
constexpr vpage_no<A> operator+(const vpage_no<A>&, const page_count<A>&)
    noexcept;
template<arch A>
constexpr vpage_no<A> operator+(const page_count<A>&, const vpage_no<A>&)
    noexcept;
template<arch A>
constexpr vpage_no<A> operator-(const vpage_no<A>&, const page_count<A>&)
    noexcept;

template<arch A>
constexpr page_count<A> operator+(const page_count<A>&, const page_count<A>&)
    noexcept;
template<arch A>
constexpr page_count<A> operator-(const page_count<A>&, const page_count<A>&)
    noexcept;

template<arch A>
constexpr page_count<A> operator*(const page_count<A>&, int) noexcept;
template<arch A>
constexpr page_count<A> operator*(int, const page_count<A>&) noexcept;
template<arch A>
constexpr page_count<A> operator/(const page_count<A>&, int) noexcept;
template<arch A>
constexpr auto operator/(const page_count<A>&, const page_count<A>&)
    noexcept -> typename page_count<A>::type;


}} /* namespace ilias::pmap */

/*
 * Phys_addr and page_no will hash to the same value, if they are equal.
 * Vaddr and vpage_no will hash to the same value, if they are equal.
 */
namespace std {

template<ilias::arch A>
struct hash<ilias::pmap::phys_addr<A>> {
  using result_type = size_t;
  using argument_type = ilias::pmap::phys_addr<A>;
  size_t operator()(const ilias::pmap::phys_addr<A>&) const noexcept;
};

template<ilias::arch A>
struct hash<ilias::pmap::vaddr<A>> {
  using result_type = size_t;
  using argument_type = ilias::pmap::vaddr<A>;
  size_t operator()(const ilias::pmap::vaddr<A>&) const noexcept;
};

template<ilias::arch A>
struct hash<ilias::pmap::page_no<A>> {
  using result_type = size_t;
  using argument_type = ilias::pmap::page_no<A>;
  size_t operator()(const ilias::pmap::page_no<A>&) const noexcept;
};

template<ilias::arch A>
struct hash<ilias::pmap::vpage_no<A>> {
  using result_type = size_t;
  using argument_type = ilias::pmap::vpage_no<A>;
  size_t operator()(const ilias::pmap::vpage_no<A>&) const noexcept;
};

template<ilias::arch A>
struct hash<ilias::pmap::page_count<A>> {
  using result_type = size_t;
  using argument_type = ilias::pmap::page_count<A>;
  size_t operator()(const ilias::pmap::page_count<A>&) const noexcept;
};

} /* namespace std */

#include <ilias/pmap/page-inl.h>

#endif /* _ILIAS_PMAP_PAGE_H_ */
