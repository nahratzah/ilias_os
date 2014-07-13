#include <abi/ext/bigint.h>
#include <abi/ext/log2.h>
#include <abi/misc_int.h>
#include <limits>
#include <tuple>

namespace __cxxabiv1 {
namespace ext {


bigint::bigint(uintmax_t i) {
  constexpr int_t mask = int_t(-1);

  while (i > 0) {
    x_.push_back(i & mask);
    for (unsigned int _ = 0; _ < sizeof(int_t); ++_) i >>= CHAR_BIT;
  }
}

bigint::bigint(intmax_t i)
: bigint(i < 0 ? uintmax_t(-(i + 1)) + 1U : uintmax_t(i))
{
  if (i < 0) sign_ = NEG;
}

int compare(const bigint& lhs, const bigint& rhs)
    noexcept {
  if (lhs.sign_ != rhs.sign_) return (lhs.sign_ == bigint::NEG ? -1 : 1);

  const int sign_mul = (lhs.sign_ == rhs.sign_ ? 1 : -1);
  if (lhs.x_.size() != rhs.x_.size())
    return (lhs.x_.size() < rhs.x_.size() ? -sign_mul : sign_mul);

  auto li = lhs.x_.rbegin();
  auto ri = rhs.x_.rbegin();

  while (li != lhs.x_.rend() && ri != rhs.x_.rend()) {
    if (*li != *ri) return (*li < *ri ? -sign_mul : sign_mul);
    ++li;
    ++ri;
  }

  assert(li == lhs.x_.rend());
  assert(ri == rhs.x_.rend());
  return 0;
}

bool bigint::is_pow2() const noexcept {
  if (x_.empty() || sign_ != POS) return false;

  const auto xback = x_.back();
  assert(xback != 0U);
  if ((xback & (xback - 1U)) != 0U) return false;

  for (auto i = x_.begin(), i_end = x_.end() - 1; i != i_end; ++i)
    if (*i != 0U) return false;
  return true;
}

bigint bigint::multadd(bigint v, uintmax_t m, uintmax_t a) {
  v.multadd(m, a);
  return v;
}

bigint& bigint::multadd(uintmax_t m, uintmax_t a) {
  if (_predict_false(m > _namespace(std)::numeric_limits<int_t>::max() ||
                     a > _namespace(std)::numeric_limits<int_t>::max()))
    return *this = *this * bigint(m) + bigint(a);

  x_.reserve(x_.size() + 1U);
  int_t carry = (sign_ == POS ? a : 0U);
  int_t carry_in = 0;

  for (auto& xi : x_) {
    int_t next_carry;
    _namespace(std)::tie(xi, next_carry) = umul_extend(xi, int_t(m));
    int_t carry_out;
    xi = addc(xi, carry, carry_in, &carry_out);
    carry = next_carry;
    carry_in = carry_out;
  }
  carry += _namespace(std)::exchange(carry_in, 0);
  if (carry) x_.push_back(carry);

  if (sign_ == NEG && a != 0) *this -= a;
  return *this;
}

bigint bigint::multsub(bigint v, uintmax_t m, uintmax_t a) {
  v.multsub(m, a);
  return v;
}

bigint& bigint::multsub(uintmax_t m, uintmax_t a) {
  if (_predict_false(m > _namespace(std)::numeric_limits<int_t>::max() ||
                     a > _namespace(std)::numeric_limits<int_t>::max()))
    return *this = *this * bigint(m) - bigint(a);

  x_.reserve(x_.size() + 1U);
  int_t carry = (sign_ == NEG ? a : 0U);
  int_t carry_in = 0;

  for (auto& xi : x_) {
    int_t next_carry;
    _namespace(std)::tie(xi, next_carry) = umul_extend(xi, int_t(m));
    int_t carry_out;
    xi = addc(xi, carry, carry_in, &carry_out);
    carry = next_carry;
    carry_in = carry_out;
  }
  carry += _namespace(std)::exchange(carry_in, 0);
  if (carry) x_.push_back(carry);

  if (sign_ == POS && a != 0) *this += a;
  return *this;
}

bigint bigint::powmult(bigint v, uintmax_t base, uintmax_t exp) {
  using abi::ext::is_pow2;

  /*
   * Special cases:
   * v * (base^0)  -->  v
   * v * (0^exp)  -->  0  iff exp != 0
   */
  if (exp == 0) return v;
  if (base == 0) {
    v = bigint();
    return v;
  }

  /*
   * Powers of 2 are done quicker by shifting.
   */
  if (is_pow2(base)) {
    uintmax_t sh;
    if (!umul_overflow(uintmax_t(log2_down(base)), exp, &sh)) {
      v <<= sh;
      return v;
    }
  }

  /*
   * All other powers are done using squaring and multiplication.
   */
  bigint bi_base = base;
  while (exp) {
    if (exp % 2 == 1) {
      exp -= 1;
      v *= bi_base;
    }
    exp /= 2;
    if (exp) bi_base *= bi_base;
  }

  return v;
}

bigint& bigint::powmult(uintmax_t base, uintmax_t exp) {
  return *this = powmult(*this, base, exp);
}

_namespace(std)::tuple<bigint, bigint> bigint::divmod(bigint x, bigint y) {
  if (y.x_.empty())
    throw _namespace(std)::invalid_argument("bigint: division by zero");
  if (x.x_.empty()) return _namespace(std)::make_tuple(bigint(), bigint());

  bigint div;
  if (x.sign_ != y.sign_) div.sign_ = NEG;

  x.sign_ = POS;
  y.sign_ = POS;
  const size_t x_digits =
      x.x_.size() * _namespace(std)::numeric_limits<int_t>::digits -
      clz(x.x_.back());
  const size_t y_digits =
      y.x_.size() * _namespace(std)::numeric_limits<int_t>::digits -
      clz(y.x_.back());

  bigint mask = 1U;
  if (x_digits > y_digits) {
    y <<= (x_digits - y_digits + 1U);
    mask <<= (x_digits - y_digits + 1U);
  }
  for (ptrdiff_t sh = x_digits - y_digits;
       sh >= 0;
       --sh, y >>= 1U, mask >>= 1U) {
    if (y <= x) {
      x -= y;
      div |= mask;
    }
  }

  if (div.x_.empty()) div.sign_ = POS;
  assert(x < y);
  return _namespace(std)::forward_as_tuple(_namespace(std)::move(div),
                                           _namespace(std)::move(x));
}

_namespace(std)::tuple<bigint, uintmax_t> bigint::divmod(bigint x,
                                                         uintmax_t y) {
  bigint div, bigmod;
  _namespace(std)::tie(div, bigmod) =
      divmod(_namespace(std)::move(x), bigint(y));

  uintmax_t mod = 0;
  for (auto mi = bigmod.x_.rbegin(); mi != bigmod.x_.rend(); ++mi) {
    for (unsigned int i = 0; i < sizeof(int_t); ++i) mod <<= CHAR_BIT;
    mod |= *mi;
  }
  return _namespace(std)::forward_as_tuple(div, mod);
}


bigint operator+(bigint x, uintmax_t y) {
  bigint::int_t carry = 0;
  constexpr bigint::int_t mask = bigint::int_t(-1);

  if (x.sign_ == bigint::POS) {
    for (auto& xi : x.x_) {
      if (carry == 0 && y == 0) break;
      xi = addc(xi, bigint::int_t(y & mask), carry, &carry);
      for (unsigned int i = 0; i < sizeof(bigint::int_t); ++i) y >>= CHAR_BIT;
    }

    while (y || carry) {
      x.x_.push_back(addc(bigint::int_t(0), bigint::int_t(y & mask),
                          carry, &carry));
      for (unsigned int i = 0; i < sizeof(bigint::int_t); ++i) y >>= CHAR_BIT;
    }
  } else {
    for (auto& xi : x.x_) {
      if (carry == 0 && y == 0) break;
      xi = subc(xi, bigint::int_t(y & mask), carry, &carry);
      for (unsigned int i = 0; i < sizeof(bigint::int_t); ++i) y >>= CHAR_BIT;
    }

    const bool flip_sign = y || carry;
    while (y || carry) {
      x.x_.push_back(addc(bigint::int_t(0), bigint::int_t(y & mask),
                          carry, &carry));
      for (unsigned int i = 0; i < sizeof(bigint::int_t); ++i) y >>= CHAR_BIT;
    }

    if (flip_sign) {
      for (auto& xi : x.x_) xi = ~xi;
      while (x.x_.back() == 0) x.x_.pop_back();
      x.sign_ = bigint::POS;
    }
  }
  return x;
}

bigint operator+(uintmax_t x, bigint y) {
  return _namespace(std)::move(y) + x;
}

bigint operator+(bigint x, const bigint& y) {
  using int_t = bigint::int_t;

  int_t carry = 0;
  auto xi = x.x_.begin();
  auto yi = y.x_.begin();

  if (x.sign_ == y.sign_) {
    while (xi != x.x_.end() && yi != y.x_.end()) {
      *xi = addc(*xi, *yi, carry, &carry);
      ++xi;
      ++yi;
    }

    if (yi != y.x_.end()) {
      do {
        x.x_.push_back(addc(int_t(0), *yi, carry, &carry));
        ++yi;
      } while (yi != y.x_.end());
    } else if (xi != x.x_.end() && carry != 0) {
      do {
        *xi = addc(*xi, int_t(0), carry, &carry);
        ++xi;
      } while (xi != x.x_.end() && carry != 0);
    }

    if (carry != 0) x.x_.push_back(carry);
  } else {
    while (xi != x.x_.end() && yi != y.x_.end()) {
      *xi = subc(*xi, *yi, carry, &carry);
      ++xi;
      ++yi;
    }

    if (yi != y.x_.end()) {
      do {
        x.x_.push_back(subc(int_t(0), *yi, carry, &carry));
        ++yi;
      } while (yi != y.x_.end());
    } else if (xi != x.x_.end() && carry != 0) {
      do {
        *xi = subc(*xi, int_t(0), carry, &carry);
        ++xi;
      } while (xi != x.x_.end() && carry != 0);
    }

    if (carry != 0) {
      for (auto& ival : x.x_) ival = ~ival;
      x.sign_ = y.sign_;
      while (x.x_.back() == 0) x.x_.pop_back();
    }
  }
  return x;
}

bigint operator+(const bigint& x, bigint&& y) {
  return _namespace(std)::move(y) + x;
}

bigint operator+(bigint&& x, bigint&& y) {
  return (x.x_.size() >= y.x_.size() ?
          _namespace(std)::move(x) + y :
          _namespace(std)::move(y) + x);
}

bigint& bigint::operator+=(uintmax_t y) {
  return *this = *this + y;
}

bigint& bigint::operator+=(const bigint& y) {
  return *this = *this + y;
}

bigint& bigint::operator+=(bigint&& y) {
  return *this = *this + _namespace(std)::move(y);
}

bigint operator-(bigint x, uintmax_t y) {
  bigint::int_t carry = 0;
  constexpr bigint::int_t mask = bigint::int_t(-1);

  if (x.sign_ == bigint::NEG) {
    for (auto& xi : x.x_) {
      if (carry == 0 && y == 0) break;
      xi = addc(xi, bigint::int_t(y & mask), carry, &carry);
      for (unsigned int i = 0; i < sizeof(bigint::int_t); ++i) y >>= CHAR_BIT;
    }

    while (y || carry) {
      x.x_.push_back(addc(bigint::int_t(0), bigint::int_t(y & mask),
                          carry, &carry));
      for (unsigned int i = 0; i < sizeof(bigint::int_t); ++i) y >>= CHAR_BIT;
    }
  } else {
    for (auto& xi : x.x_) {
      if (carry == 0 && y == 0) break;
      xi = subc(xi, bigint::int_t(y & mask), carry, &carry);
      for (unsigned int i = 0; i < sizeof(bigint::int_t); ++i) y >>= CHAR_BIT;
    }

    const bool flip_sign = y || carry;
    while (y || carry) {
      x.x_.push_back(addc(bigint::int_t(0), bigint::int_t(y & mask),
                          carry, &carry));
      for (unsigned int i = 0; i < sizeof(bigint::int_t); ++i) y >>= CHAR_BIT;
    }

    if (flip_sign) {
      for (auto& xi : x.x_) xi = ~xi;
      while (x.x_.back() == 0) x.x_.pop_back();
      x.sign_ = bigint::NEG;
    }
  }
  return x;
}

bigint operator-(uintmax_t x, bigint y) {
  y.sign_ = (y.sign_ == bigint::POS ? bigint::NEG : bigint::POS);
  return x + _namespace(std)::move(y);
}

bigint operator-(const bigint& x, bigint y) {
  y.sign_ = (y.sign_ == bigint::POS ? bigint::NEG : bigint::POS);
  return x + _namespace(std)::move(y);
}

bigint operator-(bigint&& x, bigint y) {
  y.sign_ = (y.sign_ == bigint::POS ? bigint::NEG : bigint::POS);
  return _namespace(std)::move(x) + _namespace(std)::move(y);
}

bigint& bigint::operator-=(uintmax_t y) {
  return *this = *this - y;
}

bigint& bigint::operator-=(const bigint& y) {
  return *this = *this - y;
}

bigint& bigint::operator-=(bigint&& y) {
  return *this = *this - _namespace(std)::move(y);
}

bigint operator*(bigint x, uintmax_t y) {
  using abi::ext::is_pow2;

  if (y == 0) {
    x.x_.clear();
    x.sign_ = bigint::POS;
    return x;
  }
  if (_predict_false(y >
                     _namespace(std)::numeric_limits<bigint::int_t>::max())) {
    x *= bigint(y);
    return x;
  }
  if (_predict_false(is_pow2(y))) {
    x <<= log2_down(y);
    return x;
  }

  bigint::int_t carry = 0;
  bigint::int_t next = 0;
  for (auto& xi : x.x_) {
    bigint::int_t next_carry;
    _namespace(std)::tie(xi, next_carry) = umul_extend(xi, bigint::int_t(y));
    xi = addc(xi, next, carry, &carry);
    next = next_carry;
  }
  if (carry) next += _namespace(std)::exchange(carry, 0);
  if (next) x.x_.push_back(next);
  return x;
}

bigint operator*(uintmax_t x, bigint y) {
  return _namespace(std)::move(y) * x;
}

bigint operator*(bigint x, const bigint& y) {
  const auto sign = (x.sign_ == y.sign_ ? bigint::POS : bigint::NEG);
  x.sign_ = bigint::POS;
  x.x_.reserve(x.x_.size() + y.x_.size());

  bigint rv;
  rv.x_.reserve(x.x_.size() + y.x_.size() + 1U);
  for (auto yi : y.x_) {
    if (yi != 0U) rv += x * yi;
    x.x_.insert(x.x_.begin(), bigint::int_t(0));
  }

  rv.sign_ = sign;
  return rv;
}

bigint operator*(const bigint& x, bigint&& y) {
  return _namespace(std)::move(y) * x;
}

bigint operator*(bigint&& x, bigint&& y) {
  return _namespace(std)::move(x) * y;
}

bigint& bigint::operator*=(uintmax_t y) {
  return *this = *this * y;
}

bigint& bigint::operator*=(const bigint& y) {
  return *this = *this * y;
}

bigint& bigint::operator*=(bigint&& y) {
  return *this = *this * _namespace(std)::move(y);
}

uintmax_t operator&(const bigint& x, uintmax_t y) noexcept {
  uintmax_t mask = 0;
  unsigned int sh = 0;

  for (auto xi : x.x_) {
    if (sh >= _namespace(std)::numeric_limits<uintmax_t>::digits) break;
    mask |= (uintmax_t(xi) << sh);
    sh += _namespace(std)::numeric_limits<bigint::int_t>::digits;
  }

  return mask & y;
}

uintmax_t operator&(uintmax_t x, const bigint& y) noexcept {
  return y & x;
}

bigint operator&(bigint x, const bigint& y) noexcept {
  auto xi = x.x_.begin();
  auto yi = y.x_.begin();

  while (xi != x.x_.end() && yi != y.x_.end()) {
    *xi &= *yi;
    ++xi;
    ++yi;
  }
  if (xi != x.x_.end()) x.x_.resize(xi - x.x_.begin());
  return x;
}

bigint& bigint::operator&=(uintmax_t y) noexcept {
  auto xi = x_.begin();
  for (; xi != x_.end(); ++xi) {
    if (y == 0) break;
    *xi &= y;
    for (unsigned int i = 0; i < sizeof(bigint::int_t); ++i) y >>= CHAR_BIT;
  }
  x_.resize(xi - x_.begin());
  return *this;
}

bigint& bigint::operator&=(const bigint& y) noexcept {
  auto xi = x_.begin();
  auto yi = y.x_.begin();

  while (xi != x_.end() && yi != y.x_.end()) {
    *xi &= *yi;
    ++xi;
    ++yi;
  }
  if (xi != x_.end()) x_.resize(xi - x_.begin());
  return *this;
}

bigint operator|(bigint x, uintmax_t y) {
  constexpr bigint::int_t mask = bigint::int_t(-1);

  for (auto& xi : x.x_) {
    if (y == 0) break;
    xi |= (y & mask);
    for (unsigned int i = 0; i < sizeof(bigint::int_t); ++i) y >>= CHAR_BIT;
  }
  while (y != 0) {
    x.x_.push_back(y & mask);
    for (unsigned int i = 0; i < sizeof(bigint::int_t); ++i) y >>= CHAR_BIT;
  }
  return x;
}

bigint operator|(uintmax_t x, bigint y) {
  return _namespace(std)::move(y) | x;
}

bigint operator|(bigint x, const bigint& y) {
  auto xi = x.x_.begin();
  auto yi = y.x_.begin();

  while (xi != x.x_.end() && yi != y.x_.end()) {
    *xi |= *yi;
    ++xi;
    ++yi;
  }
  if (yi != y.x_.end()) x.x_.insert(x.x_.end(), yi, y.x_.end());

  return x;
}

bigint& bigint::operator|=(uintmax_t y) {
  return *this = *this | y;
}

bigint& bigint::operator|=(const bigint& y) {
  return *this = *this | y;
}

bigint operator^(bigint x, uintmax_t y) {
  constexpr bigint::int_t mask = bigint::int_t(-1);

  for (auto& xi : x.x_) {
    if (y == 0) break;
    xi ^= (y & mask);
    for (unsigned int i = 0; i < sizeof(bigint::int_t); ++i) y >>= CHAR_BIT;
  }
  while (y != 0) {
    x.x_.push_back(y & mask);
    for (unsigned int i = 0; i < sizeof(bigint::int_t); ++i) y >>= CHAR_BIT;
  }
  return x;
}

bigint operator^(uintmax_t x, bigint y) {
  return _namespace(std)::move(y) ^ x;
}

bigint operator^(bigint x, const bigint& y) {
  auto xi = x.x_.begin();
  auto yi = y.x_.begin();

  while (xi != x.x_.end() && yi != y.x_.end()) {
    *xi ^= *yi;
    ++xi;
    ++yi;
  }
  if (yi != y.x_.end()) x.x_.insert(x.x_.end(), yi, y.x_.end());

  return x;
}

bigint& bigint::operator^=(uintmax_t y) {
  return *this = *this ^ y;
}

bigint& bigint::operator^=(const bigint& y) {
  return *this = *this ^ y;
}

bigint operator>>(bigint x, uintmax_t y) noexcept {
  x >>= y;
  return x;
}

bigint& bigint::operator>>=(uintmax_t y) noexcept {
  constexpr auto N = _namespace(std)::numeric_limits<bigint::int_t>::digits;
  unsigned int bits = y % N;
  unsigned int prepend = y / N;

  if (prepend) {
    if (x_.size() < prepend)
      x_.clear();
    else
      x_.erase(x_.begin(), x_.begin() + prepend);
  }

  if (bits) {
    bigint::int_t carry = 0;
    auto xi = x_.rbegin();
    while (xi != x_.rend()) {
      _namespace(std)::tie(carry, *xi) =
          _namespace(std)::forward_as_tuple(*xi << (N - bits),
                                            (*xi >> bits) | carry);
      ++xi;
    }

    while (x_.back() == 0) x_.pop_back();
  }

  if (x_.empty()) sign_ = bigint::POS;

  return *this;
}

bigint operator<<(bigint x, uintmax_t y) {
  constexpr auto N = _namespace(std)::numeric_limits<bigint::int_t>::digits;
  unsigned int bits = y % N;
  unsigned int prepend = y / N;

  if (bits) {
    bigint::int_t carry = 0;
    for (auto& xi : x.x_) {
      _namespace(std)::tie(carry, xi) =
          _namespace(std)::forward_as_tuple(xi >> (N - bits),
                                            (xi << bits) | carry);
    }
    if (carry) x.x_.push_back(carry);
  }

  if (prepend) x.x_.insert(x.x_.begin(), prepend, bigint::int_t(0));

  return x;
}

bigint& bigint::operator<<=(uintmax_t y) {
  return *this = *this << y;
}


bigint operator/(bigint x, uintmax_t y) {
  return _namespace(std)::get<0>(bigint::divmod(_namespace(std)::move(x), y));
}

bigint operator/(uintmax_t x, bigint y) {
  return _namespace(std)::get<0>(bigint::divmod(x, _namespace(std)::move(y)));
}

bigint operator/(bigint x, bigint y) {
  return _namespace(std)::get<0>(bigint::divmod(_namespace(std)::move(x),
                                                _namespace(std)::move(y)));
}

bigint& bigint::operator/=(uintmax_t y) {
  return *this = *this / y;
}

bigint& bigint::operator/=(bigint y) {
  return *this = *this / _namespace(std)::move(y);
}


uintmax_t operator%(bigint x, uintmax_t y) {
  return _namespace(std)::get<1>(bigint::divmod(_namespace(std)::move(x), y));
}

uintmax_t operator%(uintmax_t x, bigint y) {
  bigint bigmod;
  _namespace(std)::tie(_namespace(std)::ignore, bigmod) =
      bigint::divmod(x, _namespace(std)::move(y));

  uintmax_t mod = 0;
  for (auto mi = bigmod.x_.rbegin(); mi != bigmod.x_.rend(); ++mi) {
    for (unsigned int i = 0; i < sizeof(bigint::int_t); ++i) mod <<= CHAR_BIT;
    mod |= *mi;
  }
  return mod;
}

bigint operator%(bigint x, bigint y) {
  return _namespace(std)::get<1>(bigint::divmod(_namespace(std)::move(x),
                                                _namespace(std)::move(y)));
}

bigint& bigint::operator%=(uintmax_t y) {
  return *this = *this % y;
}

bigint& bigint::operator%=(bigint y) {
  return *this = *this % _namespace(std)::move(y);
}


_namespace(std)::string to_string(bigint x) {
  _namespace(std)::string rv;

  const bool neg = (x.sign_ == bigint::NEG);
  x.sign_ = bigint::POS;

  while (!x.x_.empty()) {
    unsigned int digit;
    _namespace(std)::tie(x, digit) =
        bigint::divmod(_namespace(std)::move(x), 10U);
    rv.insert(rv.begin(), char('0' + digit));
  }

  if (neg) rv.insert(rv.begin(), '-');
  return rv;
}


}} /* namespace __cxxabiv1::ext */
