_namespace_begin(std)


#define _GENERATE_BITMASK_OPS(_TYPE)					\
inline constexpr _TYPE operator&(_TYPE x, _TYPE y) noexcept {		\
  using int_type = typename underlying_type<_TYPE>::type;		\
									\
  return static_cast<_TYPE>(						\
      static_cast<int_type>(x) & static_cast<int_type>(y));		\
}									\
									\
inline constexpr _TYPE operator|(_TYPE x, _TYPE y) noexcept {		\
  using int_type = typename underlying_type<_TYPE>::type;		\
									\
  return static_cast<_TYPE>(						\
      static_cast<int_type>(x) | static_cast<int_type>(y));		\
}									\
									\
inline constexpr _TYPE operator^(_TYPE x, _TYPE y) noexcept {		\
  using int_type = typename underlying_type<_TYPE>::type;		\
									\
  return static_cast<_TYPE>(						\
      static_cast<int_type>(x) ^ static_cast<int_type>(y));		\
}									\
									\
inline constexpr _TYPE operator~(_TYPE x) noexcept {			\
  using int_type = typename underlying_type<_TYPE>::type;		\
									\
  return static_cast<_TYPE>(~static_cast<int_type>(x));			\
}									\
									\
inline _TYPE& operator&=(_TYPE& x, _TYPE y) {				\
  x = x & y;								\
  return x;								\
}									\
									\
inline _TYPE& operator|=(_TYPE& x, _TYPE y) {				\
  x = x & y;								\
  return x;								\
}									\
									\
inline _TYPE& operator^=(_TYPE& x, _TYPE y) {				\
  x = x & y;								\
  return x;								\
}

_GENERATE_BITMASK_OPS(ios_base::fmtflags)
_GENERATE_BITMASK_OPS(ios_base::iostate)
_GENERATE_BITMASK_OPS(ios_base::openmode)

#undef _GENERATE_BITMASK_OPS


inline auto ios_base::flags() const noexcept -> fmtflags {
  return fmtflags_;
}

inline auto ios_base::flags(fmtflags f) noexcept -> fmtflags {
  return exchange(fmtflags_, f);
}

inline auto ios_base::setf(fmtflags f) noexcept -> fmtflags {
  return exchange(fmtflags_, fmtflags_ | f);
}

inline auto ios_base::setf(fmtflags f, fmtflags mask) noexcept -> fmtflags {
  return exchange(fmtflags_, (fmtflags_ & ~mask) | (f & mask));
}

inline auto ios_base::unsetf(fmtflags mask) noexcept -> void {
  fmtflags_ &= ~mask;
}

inline streamsize ios_base::precision() const noexcept {
  return precision_;
}

inline streamsize ios_base::precision(streamsize prec) noexcept {
  return exchange(precision_, prec);
}

inline streamsize ios_base::width() const noexcept {
  return width_;
}

inline streamsize ios_base::width(streamsize w) noexcept {
  return exchange(width_, w);
}


template<typename State>
auto fpos<State>::state() const -> State { return state_; }

template<typename State>
auto fpos<State>::state(State s) -> void { state_ = s; }

template<typename State>
auto fpos<State>::operator=(const fpos& other) -> fpos& {
  state_ = other.state_;
  return *this;
}

template<typename State>
auto fpos<State>::operator=(State s) -> fpos& {
  state_ = s;
  return *this;
}

template<typename State>
fpos<State>::fpos(State s) : state_(s) {}

template<typename State>
bool operator==(const fpos<State>& a, const fpos<State>& b)
    noexcept(noexcept(a.state() == b.state())) {
  return a.state() == b.state();
}

template<typename State>
bool operator!=(const fpos<State>& a, const fpos<State>& b)
    noexcept(noexcept(!(a == b))) {
  return !(a == b);
}


namespace impl {

inline void basic_ios_derived::move(basic_ios_derived&& rhs) noexcept {
  using _namespace(std)::move;

  iostate_ = move(rhs.iostate_);
  exceptions_ = move(rhs.exceptions_);
}

inline void basic_ios_derived::swap(basic_ios_derived& rhs) noexcept {
  using _namespace(std)::swap;

  swap(iostate_, rhs.iostate_);
  swap(exceptions_, rhs.exceptions_);
}

inline basic_ios_derived::operator bool() const noexcept {
  return !fail();
}

inline auto basic_ios_derived::operator!() const noexcept -> bool {
  return fail();
}

inline auto basic_ios_derived::rdstate() const noexcept -> ios_base::iostate {
  return iostate_;
}

inline auto basic_ios_derived::setstate(ios_base::iostate state) -> void {
  clear_(rdstate() | state);
}

inline auto basic_ios_derived::good() const noexcept -> bool {
  return rdstate() == ios_base::goodbit;
}

inline auto basic_ios_derived::eof() const noexcept -> bool {
  return (rdstate() & ios_base::eofbit) != ios_base::goodbit;
}

inline auto basic_ios_derived::fail() const noexcept -> bool {
  return (rdstate() & (ios_base::failbit | ios_base::badbit)) !=
         ios_base::goodbit;
}

inline auto basic_ios_derived::bad() const noexcept -> bool {
  return (rdstate() & ios_base::badbit) != ios_base::goodbit;
}

inline auto basic_ios_derived::exceptions() const noexcept ->
    ios_base::iostate {
  return exceptions_;
}

inline auto basic_ios_derived::exceptions(ios_base::iostate exc) -> void {
  exceptions_ = exc;
  clear_(rdstate());
}

} /* namespace std::impl */


template<typename Char, typename Traits>
basic_ios<Char, Traits>::basic_ios(
    basic_streambuf<char_type, traits_type>* sb) {
  init(sb);
}

template<typename Char, typename Traits>
basic_ios<Char, Traits>::basic_ios() {}

template<typename Char, typename Traits>
basic_ios<Char, Traits>::~basic_ios() noexcept {}

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::init(
    basic_streambuf<char_type, traits_type>* sb) -> void {
  rdbuf_ = sb;
  tie_ = nullptr;
  this->exceptions(goodbit);
  this->clear();
  this->flags(skipws | dec);
  this->width(0);
  this->precision(6);
#if __has_include(<locale>)
  fill(widen(' '));
#else
  fill(char_type(' '));
#endif
}

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::tie() const noexcept ->
    basic_ostream<char_type, traits_type>* {
  return tie_;
}

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::tie(basic_ostream<char_type, traits_type>* s)
    noexcept ->
    basic_ostream<char_type, traits_type>* {
  return exchange(tie_, s);
}

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::rdbuf() const noexcept ->
    basic_streambuf<char_type, traits_type>* {
  return rdbuf_;
}

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::rdbuf(
    basic_streambuf<char_type, traits_type>* sb) noexcept ->
    basic_streambuf<char_type, traits_type>* {
  clear();
  return exchange(rdbuf_, sb);
}

#if __has_include(<locale>)
template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::imbue(const locale& loc) -> locale {
  auto rv = ios_base::imbue(loc);
  if (rdbuf_) rdbuf_->pubimbue(loc);
  return rv;
}

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::narrow(char_type c, char dfault) const -> char {
  return use_facet<ctype<char_type>>(getloc()).narrow(c, dfault);
}

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::narrow(char c) const -> char_type {
  return use_facet<ctype<char_type>>(getloc()).widen(c);
}
#endif

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::fill() const noexcept -> char_type {
  return fill_;
}

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::fill(char_type c) noexcept -> char_type {
  return exchange(fill_, c);
}

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::copyfmt(const basic_ios& rhs) -> basic_ios& {
  using _namespace(std)::move;

  if (this == &rhs) return *this;

  /* Prepare allocated data structures (this may throw). */
  auto parray = rhs.parray_;
  auto iarray = rhs.iarray_;

  /* Notify events that the stream state is going away. */
  this->invoke_event_cb(erase_event);

  /* Move all values over, except for rdbuf, rdstate and exceptions. */
  tie_ = rhs.tie_;
  this->flags(rhs.flags());
  this->width(rhs.width());
  this->precision(rhs.precision());
  fill(rhs.fill());
  this->parray_ = move(parray);
  this->iarray_ = move(iarray);
#if __has_include(<locale>)
  this->loc_ = move(rhs.loc_);
#endif

  /* Notify events that the stream state has been copied. */
  this->invoke_event_cb(copyfmt_event);

  /* Copy over exceptions (this may throw). */
  exceptions(rhs.exceptions());

  return *this;
}

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::move(basic_ios& rhs) -> void {
  using _namespace(std)::move;

#if __has_include(<locale>)
  loc_ = move(rhs.loc_);
#endif
  precision_ = move(rhs.precision_);
  width_ = move(rhs.width_);
  fmtflags_ = move(rhs.fmtflags_);
  events_ = move(rhs.events_);
  iarray_ = move(rhs.iarray_);
  parray_ = move(rhs.parray_);
  this->impl::basic_ios_derived::move(move(rhs));
  /* rdbuf is _not_ copyied, as per the standard. */
  tie_ = move(rhs.tie_);
  fill_ = move(rhs.fill_);

  rdbuf_ = nullptr;
  rhs.tie_ = nullptr;
}

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::move(basic_ios&& rhs_) -> void {
  basic_ios& rhs = rhs_;
  move(rhs);  // Invokes move by reference.
}

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::swap(basic_ios& rhs) noexcept -> void {
  using _namespace(std)::swap;

#if __has_include(<locale>)
  swap(loc_, rhs.loc_);
#endif
  swap(precision_, rhs.precision_);
  swap(width_, rhs.width_);
  swap(fmtflags_, rhs.fmtflags_);
  swap(events_, rhs.events_);
  swap(iarray_, rhs.iarray_);
  swap(parray_, rhs.parray_);
  this->impl::basic_ios_derived::swap(rhs);
  /* rdbuf is not swapped, as per the standard. */
  swap(tie_, rhs.tie_);
  swap(fill_, rhs.fill_);
}

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::set_rdbuf(
    basic_streambuf<char_type, traits_type>* sb) noexcept -> void {
  assert(sb != nullptr);
  rdbuf_ = sb;
}

template<typename Char, typename Traits>
auto basic_ios<Char, Traits>::clear(iostate state) -> void {
  if (rdbuf() == nullptr) state |= ios_base::badbit;
  this->clear_(state);
}

inline ios_base& boolalpha(ios_base& s) noexcept {
  s.setf(ios_base::boolalpha);
  return s;
}
inline ios_base& noboolalpha(ios_base& s) noexcept {
  s.unsetf(ios_base::boolalpha);
  return s;
}
inline ios_base& showbase(ios_base& s) noexcept {
  s.setf(ios_base::showbase);
  return s;
}
inline ios_base& noshowbase(ios_base& s) noexcept {
  s.unsetf(ios_base::showbase);
  return s;
}
inline ios_base& showpoint(ios_base& s) noexcept {
  s.setf(ios_base::showpoint);
  return s;
}
inline ios_base& noshowpoint(ios_base& s) noexcept {
  s.unsetf(ios_base::showpoint);
  return s;
}
inline ios_base& showpos(ios_base& s) noexcept {
  s.setf(ios_base::showpos);
  return s;
}
inline ios_base& noshowpos(ios_base& s) noexcept {
  s.unsetf(ios_base::showpos);
  return s;
}
inline ios_base& skipws(ios_base& s) noexcept {
  s.setf(ios_base::skipws);
  return s;
}
inline ios_base& noskipws(ios_base& s) noexcept {
  s.unsetf(ios_base::skipws);
  return s;
}
inline ios_base& uppercase(ios_base& s) noexcept {
  s.setf(ios_base::uppercase);
  return s;
}
inline ios_base& nouppercase(ios_base& s) noexcept {
  s.unsetf(ios_base::uppercase);
  return s;
}
inline ios_base& unitbuf(ios_base& s) noexcept {
  s.setf(ios_base::unitbuf);
  return s;
}
inline ios_base& nounitbuf(ios_base& s) noexcept {
  s.unsetf(ios_base::unitbuf);
  return s;
}
inline ios_base& internal(ios_base& s) noexcept {
  s.setf(ios_base::internal, ios_base::adjustfield);
  return s;
}
inline ios_base& left(ios_base& s) noexcept {
  s.setf(ios_base::left, ios_base::adjustfield);
  return s;
}
inline ios_base& right(ios_base& s) noexcept {
  s.setf(ios_base::right, ios_base::adjustfield);
  return s;
}
inline ios_base& dec(ios_base& s) noexcept {
  s.setf(ios_base::dec, ios_base::basefield);
  return s;
}
inline ios_base& hex(ios_base& s) noexcept {
  s.setf(ios_base::hex, ios_base::basefield);
  return s;
}
inline ios_base& oct(ios_base& s) noexcept {
  s.setf(ios_base::oct, ios_base::basefield);
  return s;
}
inline ios_base& fixed(ios_base& s) noexcept {
  s.setf(ios_base::fixed, ios_base::floatfield);
  return s;
}
inline ios_base& scientific(ios_base& s) noexcept {
  s.setf(ios_base::scientific, ios_base::floatfield);
  return s;
}
inline ios_base& hexfloat(ios_base& s) noexcept {
  s.setf(ios_base::fixed | ios_base::scientific, ios_base::floatfield);
  return s;
}
inline ios_base& defaultfloat(ios_base& s) noexcept {
  s.unsetf(ios_base::floatfield);
  return s;
}

inline error_code make_error_code(io_errc e) noexcept {
  return error_code(static_cast<int>(e), iostream_category());
}

inline error_condition make_error_condition(io_errc e) noexcept {
  return error_condition(static_cast<int>(e), iostream_category());
}


_namespace_end(std)
