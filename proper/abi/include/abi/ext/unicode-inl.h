namespace __cxxabiv1 {
namespace ext {
namespace {


inline constexpr int utf8_need_bytes(wchar_t c) noexcept {
  int need = 0;
  if (c <= 0x7f)
    need = 1;
  else if (c <= 0x7ff)
    need = 2;
  else if (c <= 0xffff)
    need = 3;
  else if (c <= 0x1fffff)
    need = 4;
  else if (c <= 0x3ffffff)
    need = 5;
  else if (c <= 0x7fffffff)
    need = 6;
  return need;
}

inline constexpr char utf8_mask(int bytes) noexcept {
  return (bytes <= 1 ? 0 : ~char((1U << (8 - bytes)) - 1U));
}

inline constexpr bool is_valid_unicode(wchar_t c) noexcept {
  return c >= 0 && c <= 0x10ffff && !(c >= 0xd800 && c <= 0xdfff);
}


template<typename CB>
int unicode_conv_in<char>::operator()(char c, CB& cb)
    noexcept(noexcept(std::declval<CB>()(std::declval<const wchar_t>()))) {
  if (_predict_true(!(c & 0x80))) {
    if (_predict_false(need_ != 0))
      return _ABI_EILSEQ;  // Expected continuation.
    if (!is_valid_unicode(wchar_t(c))) return _ABI_EILSEQ;
    return cb(wchar_t(c));
  }

  if (need_ != 0) {
    if ((c & 0xc0) != 0x80) return _ABI_EILSEQ;
    partial_ <<= 6;
    partial_ |= (c & 0x3f);
    if (--need_ == 0) {
      // Verify the encoding is the shortest possible.
      if (initial_need_ != utf8_need_bytes(partial_)) return _ABI_EILSEQ;
      // Verify the character is a valid unicode character.
      if (!is_valid_unicode(partial_)) return _ABI_EILSEQ;
      return cb(partial_);
    }
    return 0;
  }

  initial_need_ = need_ = clz(~c);
  if (need_ <= 1) return _ABI_EILSEQ;  // Continuation char.
  if (need_ > 6) return _ABI_EILSEQ;  // Oversized length.
  const char mask = (char(1) << (8 - need_));
  partial_ = (c & mask);
  --need_;
  return 0;
}

inline bool unicode_conv_in<char>::is_clear() const noexcept {
  return need_ == 0;
}

inline void unicode_conv_in<char>::reset() noexcept {
  need_ = 0;
}


template<typename CB>
int unicode_conv_in<char16_t>::operator()(char16_t c, CB& cb)
    noexcept(noexcept(std::declval<CB>()(std::declval<const wchar_t>()))) {
  if (_predict_false(c >= 0xd800 && c <= 0xdbff)) {
    if (need_) return _ABI_EILSEQ;
    partial_ = (c - 0xd800);
    partial_ <<= 10;
    return 0;
  }
  if (_predict_false(c >= 0xdc00 && c <= 0xdfff)) {
    if (!need_) return _ABI_EILSEQ;
    partial_ |= (c - 0xdc00);
    need_ = false;
    if (!is_valid_unicode(partial_)) return _ABI_EILSEQ;
    return cb(partial_);
  }

  wchar_t cc = c;
  if (!is_valid_unicode(cc)) return _ABI_EILSEQ;
  return cb(cc);
}

inline bool unicode_conv_in<char16_t>::is_clear() const noexcept {
  return !need_;
}

inline void unicode_conv_in<char16_t>::reset() noexcept {
  need_ = false;
}


template<typename CB>
int unicode_conv_in<char32_t>::operator()(char32_t c, CB& cb)
    noexcept(noexcept(std::declval<CB>()(std::declval<const wchar_t>()))) {
  wchar_t cc = c;
  if (!is_valid_unicode(cc)) return _ABI_EILSEQ;
  return cb(cc);
}

inline bool unicode_conv_in<char32_t>::is_clear() const noexcept {
  return true;
}

inline void unicode_conv_in<char32_t>::reset() noexcept {}


template<typename CB>
int unicode_conv_in<wchar_t>::operator()(wchar_t c, CB& cb)
    noexcept(noexcept(std::declval<CB>()(std::declval<const wchar_t>()))) {
  return cb(c);
}

inline bool unicode_conv_in<wchar_t>::is_clear() const noexcept {
  return true;
}

inline void unicode_conv_in<wchar_t>::reset() noexcept {}


template<typename CB>
int unicode_conv_out<char>::operator()(wchar_t c, CB& cb)
    noexcept(noexcept(std::declval<CB>()(std::declval<const char>()))) {
  if (!is_valid_unicode(c)) return _ABI_EILSEQ;

  constexpr char mask1 = 0x80;
  const int need = utf8_need_bytes(c);
  if (_predict_true(need == 1)) return cb(char(c));
  if (need == 0) return _ABI_EILSEQ;
  const char mask0 = utf8_mask(need);

  char buf[6];
  int i = need;
  while (i-- > 1) {
    buf[i] = ((c & 0x3f) | mask1);
    c >>= 6;
  }
  buf[0] = (c | mask0);

  for (i = 0; i < need; ++i) {
    int err = cb(buf[i]);
    if (err) return err;
  }
  return 0;
}

inline bool unicode_conv_out<char>::is_clear() const noexcept {
  return true;
}

inline void unicode_conv_out<char>::reset() noexcept {}


template<typename CB>
int unicode_conv_out<char16_t>::operator()(wchar_t c, CB& cb)
    noexcept(noexcept(std::declval<CB>()(std::declval<const char16_t>()))) {
  if (!is_valid_unicode(c)) return _ABI_EILSEQ;

  if (_predict_true(c <= 0xd7ff || (c >= 0xe000 && c <= 0xffff)))
    return cb(char16_t(c));
  if (c > 0x10ffff) return _ABI_EILSEQ;
  c -= 0x010000;
  int err = cb(char16_t(0xd800 + (c >> 10)));
  if (!err) err = cb(char16_t(0xdc00 + (c & 0x3ff)));
  return err;
}

inline bool unicode_conv_out<char16_t>::is_clear() const noexcept {
  return true;
}

inline void unicode_conv_out<char16_t>::reset() noexcept {}


template<typename CB>
int unicode_conv_out<char32_t>::operator()(wchar_t c, CB& cb)
    noexcept(noexcept(std::declval<CB>()(std::declval<const char32_t>()))) {
  if (c > 0x7fffffff) return _ABI_EILSEQ;  // Cannot encode this.
  if (!is_valid_unicode(c)) return _ABI_EILSEQ;  // Invalid code point.
  return cb(char32_t(c));
}

inline bool unicode_conv_out<char32_t>::is_clear() const noexcept {
  return true;
}

inline void unicode_conv_out<char32_t>::reset() noexcept {}


template<typename CB>
int unicode_conv_out<wchar_t>::operator()(wchar_t c, CB& cb)
    noexcept(noexcept(std::declval<CB>()(std::declval<const wchar_t>()))) {
  return cb(c);
}

inline bool unicode_conv_out<wchar_t>::is_clear() const noexcept {
  return true;
}

inline void unicode_conv_out<wchar_t>::reset() noexcept {}


template<typename COut, typename CIn>
template<typename CB>
auto unicode_conv<COut, CIn>::operator()(CIn c, CB& cb)
    noexcept(noexcept(std::declval<CB>()(std::declval<const COut>()))) ->
    int {
  auto intermediate = [&](wchar_t c)
        noexcept(noexcept(std::declval<CB>()(std::declval<const COut>()))) ->
        int {
      return out_(c, cb);
    };

  return in_(c, intermediate);
}

template<typename COut, typename CIn>
auto unicode_conv<COut, CIn>::is_clear() const noexcept -> bool {
  return in_.is_clear() && out_.is_clear();
}

template<typename COut, typename CIn>
auto unicode_conv<COut, CIn>::reset() noexcept -> void {
  in_.reset();
  out_.reset();
}


template<typename C>
template<typename CB>
auto unicode_conv<C, C>::operator()(C c, CB& cb)
    noexcept(noexcept(std::declval<CB>()(std::declval<const C>()))) ->
    int {
  int err = in_(c, [](wchar_t) noexcept -> int { return 0; });
  return (err ? err : in_(c, cb));
}

template<typename C>
auto unicode_conv<C, C>::is_clear() const noexcept -> bool {
  return in_.is_clear();
}

template<typename C>
auto unicode_conv<C, C>::reset() noexcept -> void {
  in_.reset();
}


}}} /* namespace __cxxabiv1::ext::<unnamed> */
