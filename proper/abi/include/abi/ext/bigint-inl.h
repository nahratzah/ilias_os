namespace __cxxabiv1 {
namespace ext {


inline bool operator==(const bigint& lhs, const bigint& rhs) noexcept {
  return compare(lhs, rhs) == 0;
}

inline bool operator!=(const bigint& lhs, const bigint& rhs) noexcept {
  return compare(lhs, rhs) != 0;
}

inline bool operator<(const bigint& lhs, const bigint& rhs) noexcept {
  return compare(lhs, rhs) < 0;
}

inline bool operator>(const bigint& lhs, const bigint& rhs) noexcept {
  return compare(lhs, rhs) > 0;
}

inline bool operator<=(const bigint& lhs, const bigint& rhs) noexcept {
  return compare(lhs, rhs) <= 0;
}

inline bool operator>=(const bigint& lhs, const bigint& rhs) noexcept {
  return compare(lhs, rhs) >= 0;
}


template<typename Int, typename Cast>
bigint::bigint(Int i)
: bigint(Cast(i))
{}

template<typename Char, typename Traits>
bigint::bigint(
    _namespace(std)::basic_string_ref<Char, Traits> in,
    typename _namespace(std)::basic_string_ref<Char, Traits>::size_type*
      len_ptr) {
  typename _namespace(std)::basic_string_ref<Char, Traits>::size_type len = 0;

  sign s;  // Save sign for after parsing the numbers.
  /* Eat the sign symbol. */
  if (_predict_true(!in.empty())) {
    switch (in[0]) {
    default:
      s = POS;
      break;
    case '+':
      s = POS;
      ++len;
      in = in.substr(1);
      break;
    case '-':
      s = NEG;
      ++len;
      in = in.substr(1);
      break;
    }
  }

  // This loop will malfunction if sign is NEG.
  for (auto c : in) {
    if (c < '0' || c > '9') break;
    multadd(10, static_cast<unsigned char>(c - '0'));
    ++len;
  }
  sign_ = s;

  if (len_ptr)
    *len_ptr = len;
  else if (len < in.size())
    throw _namespace(std)::invalid_argument("bigint: invalid string");
  return;
}

inline bigint::bigint(bigint&& other) noexcept
: sign_(other.sign_),
  x_(other.x_)
{}

inline bigint& bigint::operator=(bigint other) noexcept {
  swap(other);
  return *this;
}

inline void bigint::swap(bigint& other) noexcept {
  using _namespace(std)::swap;

  swap(sign_, other.sign_);
  x_.swap(other.x_);
}

inline void bigint::shrink_to_fit()
    noexcept(noexcept(_namespace(std)::declval<
        _namespace(std)::vector<int_t>>().shrink_to_fit())) {
  x_.shrink_to_fit();
}

inline bigint operator-(bigint v) noexcept {
  v.sign_ = (v.sign_ == bigint::POS ? bigint::NEG : bigint::POS);
  return v;
}

inline bigint abs(bigint v) noexcept {
  v.sign_ = bigint::POS;
  return v;
}

inline bigint::operator bool() const noexcept {
  return !x_.empty();
}

inline bool bigint::operator!() const noexcept {
  return !bool(*this);
}


template<typename Int>
auto operator+(bigint x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint> {
  return _namespace(std)::move(x) + uintmax_t(y);
}

template<typename Int>
auto operator+(Int x, bigint y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint> {
  return uintmax_t(x) + _namespace(std)::move(y);
}

template<typename Int>
auto operator+(bigint x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint> {
  if (y < 0)
    return _namespace(std)::move(x) - (uintmax_t(-(y + 1)) + 1U);
  else
    return _namespace(std)::move(x) + uintmax_t(y);
}

template<typename Int>
auto operator+(Int x, bigint y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint> {
  if (x < 0)
    return -((uintmax_t(-(x + 1)) + 1U) + _namespace(std)::move(y));
  else
    return uintmax_t(x) - _namespace(std)::move(y);
}


template<typename Int>
auto operator-(bigint x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint> {
  return _namespace(std)::move(x) - uintmax_t(y);
}

template<typename Int>
auto operator-(Int x, bigint y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint> {
  return uintmax_t(x) - _namespace(std)::move(y);
}

template<typename Int>
auto operator-(bigint x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint> {
  if (y < 0)
    return _namespace(std)::move(x) + (uintmax_t(-(y + 1)) + 1U);
  else
    return _namespace(std)::move(x) - uintmax_t(y);
}

template<typename Int>
auto operator-(Int x, bigint y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint> {
  if (x < 0)
    return -((uintmax_t(-(x + 1)) + 1U) + _namespace(std)::move(y));
  else
    return uintmax_t(x) - _namespace(std)::move(y);
}


template<typename Int>
auto operator*(bigint x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint> {
  return _namespace(std)::move(x) * uintmax_t(y);
}

template<typename Int>
auto operator*(Int x, bigint y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint> {
  return uintmax_t(x) * _namespace(std)::move(y);
}

template<typename Int>
auto operator*(bigint x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint> {
  if (y < 0)
    return -_namespace(std)::move(x) * (uintmax_t(-(y + 1)) + 1U);
  else
    return _namespace(std)::move(x) * uintmax_t(y);
}

template<typename Int>
auto operator*(Int x, bigint y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint> {
  if (x < 0)
    return (uintmax_t(-(x + 1)) + 1U) * -_namespace(std)::move(y);
  else
    return uintmax_t(x) * _namespace(std)::move(y);
}


template<typename Int>
auto operator>>(bigint x, Int y) noexcept ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint> {
  return _namespace(std)::move(x) >> uintmax_t(y);
}

template<typename Int>
auto operator>>(bigint x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint> {
  if (y < 0)
    return _namespace(std)::move(x) << (uintmax_t(-(y + 1)) + 1U);
  else
    return _namespace(std)::move(x) >> uintmax_t(y);
}


template<typename Int>
auto operator<<(bigint x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint> {
  return _namespace(std)::move(x) << uintmax_t(y);
}

template<typename Int>
auto operator<<(bigint x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint> {
  if (y < 0)
    return _namespace(std)::move(x) >> (uintmax_t(-(y + 1)) + 1U);
  else
    return _namespace(std)::move(x) << uintmax_t(y);
}


template<typename Int>
auto operator>>=(bigint& x, Int y) noexcept ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint&> {
  return x >>= uintmax_t(y);
}

template<typename Int>
auto operator>>=(bigint& x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint&> {
  if (y < 0)
    return x <<= (uintmax_t(-(y + 1)) + 1U);
  else
    return x >>= uintmax_t(y);
}


template<typename Int>
auto operator<<=(bigint& x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint&> {
  return x <<= uintmax_t(y);
}

template<typename Int>
auto operator<<=(bigint& x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint&> {
  if (y < 0)
    return x >>= (uintmax_t(-(y + 1)) + 1U);
  else
    return x <<= uintmax_t(y);
}


template<typename Int>
auto operator/(bigint x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint> {
  return _namespace(std)::move(x) / uintmax_t(y);
}

template<typename Int>
auto operator/(Int x, bigint y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint> {
  return uintmax_t(x) / _namespace(std)::move(y);
}

template<typename Int>
auto operator/(bigint x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint> {
  if (y < 0)
    return _namespace(std)::move(x) / bigint(intmax_t(y));
  else
    return _namespace(std)::move(x) / uintmax_t(y);
}

template<typename Int>
auto operator/(Int x, bigint y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint> {
  if (x < 0)
    return bigint(intmax_t(x)) / -_namespace(std)::move(y);
  else
    return uintmax_t(x) / _namespace(std)::move(y);
}


template<typename Int>
auto operator%(bigint x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint> {
  return _namespace(std)::move(x) % uintmax_t(y);
}

template<typename Int>
auto operator%(Int x, bigint y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_unsigned<Int>::value,
                                 bigint> {
  return uintmax_t(x) % _namespace(std)::move(y);
}

template<typename Int>
auto operator%(bigint x, Int y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint> {
  if (y < 0)
    return _namespace(std)::move(x) % bigint(intmax_t(y));
  else
    return _namespace(std)::move(x) % uintmax_t(y);
}

template<typename Int>
auto operator%(Int x, bigint y) ->
    _namespace(std)::enable_if_t<_namespace(std)::is_integral<Int>::value &&
                                 _namespace(std)::is_signed<Int>::value,
                                 bigint> {
  if (x < 0)
    return bigint(intmax_t(x)) % -_namespace(std)::move(y);
  else
    return uintmax_t(x) % _namespace(std)::move(y);
}


}} /* namespace __cxxabiv1::ext */
