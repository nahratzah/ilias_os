#ifndef _BITSET_INL_H_
#define _BITSET_INL_H_

#include <bitset>
#include <abi/misc_int.h>
#include <utility>

_namespace_begin(std)


template<size_t N>
constexpr auto operator&(const bitset<N>& x, const bitset<N>& y) noexcept ->
    bitset<N> {
  return bitset<N>(typename bitset<N>::and_tag(), x, y);
}

template<size_t N>
constexpr auto operator|(const bitset<N>& x, const bitset<N>& y) noexcept ->
    bitset<N> {
  return bitset<N>(typename bitset<N>::or_tag(), x, y);
}

template<size_t N>
constexpr auto operator^(const bitset<N>& x, const bitset<N>& y) noexcept ->
    bitset<N> {
  return bitset<N>(typename bitset<N>::xor_tag(), x, y);
}


template<size_t N>
constexpr auto bitset<N>::and_tag::op(const int_type& x, const int_type& y)
    noexcept -> int_type {
  return x & y;
}

template<size_t N>
constexpr auto bitset<N>::or_tag::op(const int_type& x, const int_type& y)
    noexcept -> int_type {
  return x | y;
}

template<size_t N>
constexpr auto bitset<N>::xor_tag::op(const int_type& x, const int_type& y)
    noexcept -> int_type {
  return x ^ y;
}


template<size_t N>
auto bitset<N>::reference::operator=(bool v) noexcept -> reference& {
  assert(ptr_ != nullptr);
  if (v)
    *ptr_ |= msk_;
  else
    *ptr_ &= ~msk_;
  return *this;
}

template<size_t N>
auto bitset<N>::reference::operator=(const reference& v) noexcept ->
    reference& {
  return *this = bool(v);
}

template<size_t N>
auto bitset<N>::reference::operator~() const noexcept -> bool {
  return ~(bool(*this));
}

template<size_t N>
bitset<N>::reference::operator bool() const noexcept {
  assert(ptr_ != nullptr);
  return (*ptr_ & msk_) != 0;
}

template<size_t N>
auto bitset<N>::reference::flip() noexcept -> reference& {
  assert(ptr_ != nullptr);
  *ptr_ ^= msk_;
  return *this;
}


template<size_t N>
template<typename... T>
constexpr bitset<N>::bitset(enable_if_t<sizeof...(T) == N_int_type, init_tag>,
                            T&&... v)
    noexcept
: data_{ forward<T>(v)... }
{}

template<size_t N>
template<typename... T>
constexpr bitset<N>::bitset(enable_if_t<sizeof...(T) < N_int_type, init_tag>,
                            T&&... v)
    noexcept
: bitset(init_tag(), forward<T>(v)..., int_type(0))
{}

template<size_t N>
template<size_t... Indices>
constexpr bitset<N>::bitset(invert_tag, int_type arr[N_int_type],
                            index_sequence<Indices...>) noexcept
: data_(invert_tag::op(arr[Indices])...)
{}

template<size_t N>
template<size_t... Indices>
constexpr bitset<N>::bitset(and_tag, const bitset& x, const bitset& y,
                            index_sequence<Indices...>) noexcept
: data_(and_tag::op(x.data_[Indices], y.data_[Indices])...)
{}

template<size_t N>
template<size_t... Indices>
constexpr bitset<N>::bitset(or_tag, const bitset& x, const bitset& y,
                            index_sequence<Indices...>) noexcept
: data_(or_tag::op(x.data_[Indices], y.data_[Indices])...)
{}

template<size_t N>
template<size_t... Indices>
constexpr bitset<N>::bitset(xor_tag, const bitset& x, const bitset& y,
                            index_sequence<Indices...>) noexcept
: data_(xor_tag::op(x.data_[Indices], y.data_[Indices])...)
{}

template<size_t N>
constexpr bitset<N>::bitset() noexcept
: bitset(init_tag())
{}

template<size_t N>
constexpr bitset<N>::bitset(unsigned long long v) noexcept
: bitset(init_tag(),
         int_type(v) & (N >= numeric_limits<int_type>::digits ?
                        ~int_type(0) :
                        (int_type(1) << N) - 1U))
{
  static_assert(sizeof(int_type) >= sizeof(v),
                "bitset::int_type must be at least unsigned long long");
}

template<size_t N>
template<typename Char, typename Traits, typename Alloc>
bitset<N>::bitset(const basic_string<Char, Traits, Alloc>& s,
                  typename basic_string<Char, Traits, Alloc>::size_type pos,
                  typename basic_string<Char, Traits, Alloc>::size_type n,
                  Char zero, Char one)
: bitset(basic_string_ref<Char, Traits>(s).substr(pos, n), zero, one)
{}

template<size_t N>
template<typename Char>
bitset<N>::bitset(const Char* s,
                  typename basic_string<Char>::size_type n,
                  Char zero, Char one)
: bitset((n == basic_string<Char>::npos ?
          basic_string_ref<Char>(s) :
          basic_string_ref<Char>(s, n)),
         zero, one)
{}

template<size_t N>
template<typename Char, typename Traits>
bitset<N>::bitset(basic_string_ref<Char, Traits> s,
                  Char zero, Char one)
: bitset()
{
  using traits = typename basic_string_ref<Char, Traits>::traits_type;

  for_each(s.begin(), s.end(),
           [&zero, &one](const Char& c) {
             if (!traits::eq(c, zero) && !traits::eq(c, one))
               throw invalid_argument("bitset from string");
           });

  for (size_t i = 0; i < N && i < s.length(); ++i)
    (*this)[i] = !traits::eq(s[i], zero);
}

template<size_t N>
auto bitset<N>::operator&=(const bitset& o) noexcept -> bitset& {
  return *this = (*this & o);
}

template<size_t N>
auto bitset<N>::operator|=(const bitset& o) noexcept -> bitset& {
  return *this = (*this | o);
}

template<size_t N>
auto bitset<N>::operator^=(const bitset& o) noexcept -> bitset& {
  return *this = (*this ^ o);
}

template<size_t N>
auto bitset<N>::operator<<=(size_t n) noexcept -> bitset& {
  if (_predict_false(n >= N)) return *this = bitset();

  constexpr unsigned int digits = numeric_limits<int_type>::digits;
  if (n > digits) {
    move_backward(begin(data_), end(data_) - n / digits, end(data_));
    fill(begin(data_), begin(data_) + n / digits, int_type(0));
    n %= digits;
  }
  if (n == 0) return *this;

  int_type* dst = &data_[N_int_type - 1U];
  while (dst != &data_[0]) {
    int_type* src = dst - 1U;
    *dst = (*dst << n) |
           (*src >> (digits - n));
  }
  *dst <<= n;

  return *this;
}

template<size_t N>
auto bitset<N>::operator>>=(size_t n) noexcept -> bitset& {
  if (_predict_false(n >= N)) return *this = bitset();

  constexpr unsigned int digits = numeric_limits<int_type>::digits;
  if (N % digits != 0)
    data_[N_int_type - 1U] &= (int_type(1) << (N % digits)) - 1U;

  if (n > digits) {
    move(begin(data_) + n / digits, end(data_), begin(data_));
    fill(end(data_) - n / digits, end(data_), int_type(0));
    n %= digits;
  }
  if (n == 0) return *this;

  int_type* dst = &data_[0];
  while (dst != &data_[N_int_type - 1U]) {
    int_type* src = dst + 1U;
    *dst = (*dst >> n) |
           (*src << (digits - n));
  }
  *dst >>= n;

  return *this;
}

template<size_t N>
auto bitset<N>::set() noexcept -> bitset& {
  fill(begin(data_), end(data_), ~int_type(0));
  return *this;
}

template<size_t N>
auto bitset<N>::set(size_t i, bool v) -> bitset& {
  constexpr unsigned int digits = numeric_limits<int_type>::digits;

  if (i <= 0 || i >= N) throw out_of_range("bitset::set");
  if (v)
    data_[i / digits] |=  (int_type(1) << (i % digits));
  else
    data_[i / digits] &= ~(int_type(1) << (i % digits));
  return *this;
}

template<size_t N>
auto bitset<N>::reset() noexcept -> bitset& {
  fill(begin(data_), end(data_), int_type(0));
  return *this;
}

template<size_t N>
auto bitset<N>::reset(size_t i) -> bitset& {
  constexpr unsigned int digits = numeric_limits<int_type>::digits;

  if (i <= 0 || i >= N) throw out_of_range("bitset::set");
  data_[i / digits] &= ~(int_type(1) << (i % digits));
  return *this;
}

template<size_t N>
constexpr auto bitset<N>::operator~() const noexcept -> bitset {
  return bitset(invert_tag(), *this);
}

template<size_t N>
auto bitset<N>::flip() noexcept -> bitset& {
  for (auto& i : data_) i = ~i;
  return *this;
}

template<size_t N>
auto bitset<N>::flip(size_t i) -> bitset& {
  constexpr unsigned int digits = numeric_limits<int_type>::digits;

  if (i <= 0 || i >= N) throw out_of_range("bitset::set");
  data_[i / digits] ^= (int_type(1) << (i % digits));
  return *this;
}

template<size_t N>
constexpr auto bitset<N>::operator[](size_t i) const -> bool {
  constexpr unsigned int digits = numeric_limits<int_type>::digits;

  if (i <= 0 || i >= N) throw out_of_range("bitset::set");
  return data_[i / digits] & (int_type(1) << (i % digits));
}

template<size_t N>
auto bitset<N>::operator[](size_t i) -> reference {
  constexpr unsigned int digits = numeric_limits<int_type>::digits;

  if (i <= 0 || i >= N) throw out_of_range("bitset::set");
  reference rv;
  rv.ptr_ = &data_[i / digits];
  rv.msk_ = (int_type(1) << (i % digits));
  return rv;
}

template<size_t N>
auto bitset<N>::to_ulong() const -> unsigned long {
  constexpr unsigned int digits = numeric_limits<int_type>::digits;

  int_type msk;
  if (N < digits)
    msk = (int_type(1) << N) - 1U;
  else
    msk = ~int_type(0);
  return data_[0] & msk;
}

template<size_t N>
auto bitset<N>::to_ullong() const -> unsigned long long {
  constexpr unsigned int digits = numeric_limits<int_type>::digits;

  int_type msk;
  if (N < digits)
    msk = (int_type(1) << N) - 1U;
  else
    msk = ~int_type(0);
  return data_[0] & msk;
}

template<size_t N>
template<typename Char, typename Traits, typename Alloc>
auto bitset<N>::to_string(Char zero, Char one) const ->
    basic_string<Char, Traits, Alloc> {
  basic_string<Char, Traits, Alloc> rv;
  rv.resize(N);

  for (size_t i = 0; i < N; ++i)
    rv[i] = ((*this)[i] ? one : zero);
  return rv;
}

template<size_t N>
auto bitset<N>::count() const noexcept -> size_t {
  constexpr unsigned int digits = numeric_limits<int_type>::digits;
  using abi::popcount;

  size_t rv = 0;

  int_type msk;
  if (N % digits != 0)
    msk = (int_type(1) << (N % digits)) - 1U;
  else
    msk = ~int_type(0);

  int_type* i;
  for (i = begin(data_); i != end(data_) - 1U; ++i)
    rv += popcount(*i);
  rv += popcount(*i & msk);

  return rv;
}

template<size_t N>
constexpr auto bitset<N>::size() const noexcept -> size_t {
  return N;
}

template<size_t N>
auto bitset<N>::operator==(const bitset<N>& o) const noexcept -> bool {
  constexpr unsigned int digits = numeric_limits<int_type>::digits;

  int_type msk;
  if (N % digits != 0)
    msk = (int_type(1) << (N % digits)) - 1U;
  else
    msk = ~int_type(0);

  const int_type* i;
  const int_type* j;
  for (i = begin(data_), j = begin(o.data_); i != end(data_) - 1U; ++i, ++j)
    if (*i != *j) return false;
  return (*i & msk) == (*j & msk);
}

template<size_t N>
auto bitset<N>::operator!=(const bitset<N>& o) const noexcept -> bool {
  return !(*this == o);
}

template<size_t N>
auto bitset<N>::test(size_t i) const -> bool {
  if (i <= 0 || i >= N) throw out_of_range("bitset::set");

  return (*this)[i];
}

template<size_t N>
auto bitset<N>::all() const noexcept -> bool {
  constexpr unsigned int digits = numeric_limits<int_type>::digits;

  int_type msk;
  if (N % digits != 0)
    msk = (int_type(1) << (N % digits)) - 1U;
  else
    msk = ~int_type(0);

  const int_type* i;
  for (i = begin(data_); i != end(data_) - 1U; ++i)
    if (~(*i) != 0U) return false;
  return (~(*i) & msk) == 0U;
}

template<size_t N>
auto bitset<N>::any() const noexcept -> bool {
  return !none();
}

template<size_t N>
auto bitset<N>::none() const noexcept -> bool {
  constexpr unsigned int digits = numeric_limits<int_type>::digits;

  int_type msk;
  if (N % digits != 0)
    msk = (int_type(1) << (N % digits)) - 1U;
  else
    msk = ~int_type(0);

  const int_type* i;
  for (i = begin(data_); i != end(data_) - 1U; ++i)
    if ((*i) != 0U) return false;
  return (*i & msk) == 0U;
}

template<size_t N>
auto bitset<N>::operator<<(size_t n) const noexcept -> bitset {
  bitset tmp = *this;
  tmp <<= n;
  return tmp;
}

template<size_t N>
auto bitset<N>::operator>>(size_t n) const noexcept -> bitset {
  bitset tmp = *this;
  tmp >>= n;
  return tmp;
}


template<typename Char, typename Traits, typename Alloc>
bitset<0>::bitset(const basic_string<Char, Traits, Alloc>& s,
                  typename basic_string<Char, Traits, Alloc>::size_type pos,
                  typename basic_string<Char, Traits, Alloc>::size_type n,
                  Char zero, Char one)
: bitset(basic_string_ref<Char, Traits>(s).substr(pos, n), zero, one)
{}

template<typename Char>
bitset<0>::bitset(const Char* s,
                  typename basic_string<Char>::size_type n,
                  Char zero, Char one)
: bitset((n == basic_string<Char>::npos ?
          basic_string_ref<Char>(s) :
          basic_string_ref<Char>(s, n)),
         zero, one)
{}

template<typename Char, typename Traits>
bitset<0>::bitset(basic_string_ref<Char, Traits> s,
                  Char zero, Char one)
: bitset()
{
  using traits = typename basic_string_ref<Char, Traits>::traits_type;

  for_each(s.begin(), s.end(),
           [&zero, &one](const Char& c) {
             if (!traits::eq(c, zero) && !traits::eq(c, one))
               throw invalid_argument("bitset from string");
           });
}

template<typename Char, typename Traits, typename Alloc>
auto bitset<0>::to_string(Char, Char) const ->
    basic_string<Char, Traits, Alloc> {
  return basic_string<Char, Traits, Alloc>();
}


_namespace_end(std)

#endif /* _BITSET_INL_H_ */
