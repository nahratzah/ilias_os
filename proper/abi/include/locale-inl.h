#ifndef _LOCALE_INL_H_
#define _LOCALE_INL_H_

#include <locale>
#include <algorithm>
#include <ios>
#include <string>
#include <stdimpl/locale_num_put.h>
#include <streambuf>

#ifndef _LOCALE_INLINE
# define _LOCALE_INLINE  extern inline
#endif

_namespace_begin(std)
namespace impl {

template<typename Facet>
facet_ref<Facet>::facet_ref(locale loc_arg)
: loc(move(loc_arg)),
  impl(use_facet<Facet>(this->loc))
{}

} /* namespace impl */


template<typename Facet>
const Facet& use_facet(const locale& loc) {
  return dynamic_cast<const Facet&>(loc.use_facet_(&Facet::id));
}

template<typename Facet>
bool has_facet(const locale& loc) noexcept {
  return dynamic_cast<const Facet*>(loc.has_facet_(&Facet::id)) != nullptr;
}


inline locale::locale(locale&& loc) noexcept {
  swap(*this, loc);
}

inline locale::locale(locale_t data) noexcept
: data_(data)
{}

inline locale::locale(const string& name)
: locale(string_ref(name))
{}

template<typename Facet>
locale::locale(const locale& loc, Facet* f)
: locale(loc, &Facet::id, f)
{}

inline locale::locale(const locale& loc, const string& name, category cat)
: locale(loc, string_ref(name), cat)
{}

inline auto locale::operator=(locale&& loc) noexcept -> const locale& {
  swap(*this, loc);
  return *this;
}

template<typename Facet>
auto locale::combine(const locale& loc) const -> locale {
  return locale(*this, &use_facet<Facet>(loc));
}

inline bool locale::operator==(const locale& loc) const {
  return data_ == loc.data_;
}

inline bool locale::operator!=(const locale& loc) const {
  return !(*this == loc);
}

template<typename Char, typename Traits, typename Allocator>
bool locale::operator()(const basic_string<Char, Traits, Allocator>& x,
                        const basic_string<Char, Traits, Allocator>& y) const {
  return (*this)(basic_string_ref<Char, Traits>(x),
                 basic_string_ref<Char, Traits>(y));
}

template<typename Char, typename Traits>
bool locale::operator()(basic_string_ref<Char, Traits> x,
                        basic_string_ref<Char, Traits> y) const {
  using collate_t = _namespace(std)::collate<Char>;

  return use_facet<collate_t>(*this).compare(x.begin(), x.end(),
                                             y.begin(), y.end()) < 0;
}

inline auto swap(locale& x, locale& y) noexcept -> void {
  using _namespace(std)::swap;
  swap(x.data_, y.data_);
}


template<typename Char>
bool isspace(Char c, const locale& loc) {
  return use_facet<ctype<Char>>(loc).is(ctype_base::space, c);
}

template<typename Char>
bool isprint(Char c, const locale& loc) {
  return use_facet<ctype<Char>>(loc).is(ctype_base::print, c);
}

template<typename Char>
bool iscntrl(Char c, const locale& loc) {
  return use_facet<ctype<Char>>(loc).is(ctype_base::cntrl, c);
}

template<typename Char>
bool isupper(Char c, const locale& loc) {
  return use_facet<ctype<Char>>(loc).is(ctype_base::upper, c);
}

template<typename Char>
bool islower(Char c, const locale& loc) {
  return use_facet<ctype<Char>>(loc).is(ctype_base::lower, c);
}

template<typename Char>
bool isalpha(Char c, const locale& loc) {
  return use_facet<ctype<Char>>(loc).is(ctype_base::alpha, c);
}

template<typename Char>
bool isdigit(Char c, const locale& loc) {
  return use_facet<ctype<Char>>(loc).is(ctype_base::digit, c);
}

template<typename Char>
bool ispunct(Char c, const locale& loc) {
  return use_facet<ctype<Char>>(loc).is(ctype_base::punct, c);
}

template<typename Char>
bool isxdigit(Char c, const locale& loc) {
  return use_facet<ctype<Char>>(loc).is(ctype_base::xdigit, c);
}

template<typename Char>
bool isalnum(Char c, const locale& loc) {
  return use_facet<ctype<Char>>(loc).is(ctype_base::alnum, c);
}

template<typename Char>
bool isgraph(Char c, const locale& loc) {
  return use_facet<ctype<Char>>(loc).is(ctype_base::graph, c);
}

template<typename Char>
bool isblank(Char c, const locale& loc) {
  return use_facet<ctype<Char>>(loc).is(ctype_base::blank, c);
}

template<typename Char>
Char toupper(Char c, const locale& loc) {
  return use_facet<ctype<Char>>(loc).toupper(c);
}

template<typename Char>
Char tolower(Char c, const locale& loc) {
  return use_facet<ctype<Char>>(loc).tolower(c);
}


template<typename Codecvt, typename Elem, typename WA, typename BA>
wstring_convert<Codecvt, Elem, WA, BA>::wstring_convert(Codecvt* cvt)
: cvt_(cvt)
{
  __builtin_bzero(&cvt_state_, sizeof(cvt_state_));
  if (!cvt_) throw invalid_argument("wstring_convert: null codecvt");
}

template<typename Codecvt, typename Elem, typename WA, typename BA>
wstring_convert<Codecvt, Elem, WA, BA>::wstring_convert(
    Codecvt* cvt, state_type cvt_state)
: cvt_(cvt),
  cvt_state_(cvt_state)
{}

template<typename Codecvt, typename Elem, typename WA, typename BA>
wstring_convert<Codecvt, Elem, WA, BA>::wstring_convert(
    const byte_string& byte_err_string, const wide_string& wide_err_string)
: wstring_convert(),
  byte_err_string_(byte_err_string),
  wide_err_string_(wide_err_string)
{}

template<typename Codecvt, typename Elem, typename WA, typename BA>
auto wstring_convert<Codecvt, Elem, WA, BA>::from_bytes(char c) ->
    wide_string {
  using string_ref = basic_string_ref<char, typename byte_string::traits_type>;
  return from_bytes(string_ref(&c, 1));
}

template<typename Codecvt, typename Elem, typename WA, typename BA>
auto wstring_convert<Codecvt, Elem, WA, BA>::from_bytes(const char* s) ->
    wide_string {
  using string_ref = basic_string_ref<char, typename byte_string::traits_type>;
  return from_bytes(string_ref(s));
}

template<typename Codecvt, typename Elem, typename WA, typename BA>
auto wstring_convert<Codecvt, Elem, WA, BA>::from_bytes(
    const byte_string& s) -> wide_string {
  using string_ref = basic_string_ref<char, typename byte_string::traits_type>;
  return from_bytes(string_ref(s));
}

template<typename Codecvt, typename Elem, typename WA, typename BA>
auto wstring_convert<Codecvt, Elem, WA, BA>::from_bytes(
    basic_string_ref<char, typename byte_string::traits_type> s) ->
    wide_string {
  return from_bytes(s.begin(), s.end());
}

template<typename Codecvt, typename Elem, typename WA, typename BA>
auto wstring_convert<Codecvt, Elem, WA, BA>::from_bytes(
    const char* b, const char* e) -> wide_string {
  assert(cvt_);
  assert(e >= b);

  wide_string result;
  const auto mul = 1;  // Expected Elems per byte.
  const auto maxlen = 1;  // Expected additional Elems for empty input.

  for (bool do_continue = true; do_continue; do_continue = true) {
    /* Set up space in output string. */
    const auto result_pre_size = result.size();
    const typename wide_string::size_type min_resize =
        result.size() + mul * (e - b) + maxlen;
    result.resize(max(min_resize, result.capacity()));
    const auto out_begin = result.begin() + result_pre_size;
    auto out_end = out_begin;

    /* Perform conversion. */
    const auto old_b = b;
    auto rv = cvt_->in(cvt_state_, b, e, b, out_begin, result.end(), out_end);
    result.resize(out_end - result.begin());
    cvt_count_ += size_t(b - old_b);

    /* Handle conversion result. */
    switch (rv) {
    case codecvt_base::partial:
      /* SKIP: keep converting, we need more output Elems. */
      break;
    case codecvt_base::ok:
      do_continue = false;  // Done, successful conversion
      break;
    case codecvt_base::error:
      if (wide_err_string_.empty()) throw range_error("wstring_convert");
      result = wide_err_string_;
      do_continue = false;  // Failed, returning error string
      break;
    case codecvt_base::noconv:
      result.append(b, e);
      do_continue = false;  // No conversion required
      break;
    }
  }

  return result;
}

template<typename Codecvt, typename Elem, typename WA, typename BA>
auto wstring_convert<Codecvt, Elem, WA, BA>::to_bytes(Elem c) -> byte_string {
  using string_ref = basic_string_ref<Elem, typename wide_string::traits_type>;
  return to_bytes(string_ref(&c, 1));
}

template<typename Codecvt, typename Elem, typename WA, typename BA>
auto wstring_convert<Codecvt, Elem, WA, BA>::to_bytes(const Elem* s) ->
    byte_string {
  using string_ref = basic_string_ref<Elem, typename wide_string::traits_type>;
  return to_bytes(string_ref(s));
}

template<typename Codecvt, typename Elem, typename WA, typename BA>
auto wstring_convert<Codecvt, Elem, WA, BA>::to_bytes(const wide_string& s) ->
    byte_string {
  using string_ref = basic_string_ref<Elem, typename wide_string::traits_type>;
  return to_bytes(string_ref(s));
}

template<typename Codecvt, typename Elem, typename WA, typename BA>
auto wstring_convert<Codecvt, Elem, WA, BA>::to_bytes(
    basic_string_ref<Elem, typename wide_string::traits_type> s) ->
    byte_string {
  return to_bytes(s.begin(), s.end());
}

template<typename Codecvt, typename Elem, typename WA, typename BA>
auto wstring_convert<Codecvt, Elem, WA, BA>::to_bytes(
    const Elem* b, const Elem* e) -> byte_string {
  assert(cvt_);
  assert(e >= b);

  byte_string result;
  const auto mul = max(cvt_->encoding(), 1);  // Expected Elems per byte.
  const auto maxlen = max(cvt_->max_length(), 1);  // Expected additional Elems
                                                   // for empty input.

  for (bool do_continue = true; do_continue; do_continue = true) {
    /* Set up space in output string. */
    const auto result_pre_size = result.size();
    const typename wide_string::size_type min_resize =
        result.size() + mul * (e - b) + maxlen;
    result.resize(max(min_resize, result.capacity()));
    const auto out_begin = result.begin() + result_pre_size;
    auto out_end = out_begin;

    /* Perform conversion. */
    const auto old_b = b;
    auto rv = cvt_->out(cvt_state_, b, e, b, out_begin, result.end(), out_end);
    result.resize(out_end - result.begin());
    cvt_count_ += size_t(b - old_b);

    /* Handle conversion result. */
    switch (rv) {
    case codecvt_base::partial:
      /* SKIP: keep converting, we need more output Elems. */
      break;
    case codecvt_base::ok:
      do_continue = false;  // Done, successful conversion
      break;
    case codecvt_base::error:
      if (wide_err_string_.empty()) throw range_error("wstring_convert");
      result = wide_err_string_;
      do_continue = false;  // Failed, returning error string
      break;
    case codecvt_base::noconv:
      result.append(b, e);
      do_continue = false;  // No conversion required
      break;
    }
  }

  return result;
}

template<typename Codecvt, typename Elem, typename WA, typename BA>
auto wstring_convert<Codecvt, Elem, WA, BA>::converted() const -> size_t {
  return cvt_count_;
}

template<typename Codecvt, typename Elem, typename WA, typename BA>
auto wstring_convert<Codecvt, Elem, WA, BA>::state() const -> state_type {
  return cvt_state_;
}


template<typename Codecvt, typename Elem, typename Tr>
wbuffer_convert<Codecvt, Elem, Tr>::wbuffer_convert(streambuf* buf,
                                                    Codecvt* cvt,
                                                    state_type cvt_state)
: buf_(buf),
  cvt_(cvt),
  cvt_state_(cvt_state),
  always_noconv_(cvt->always_noconv())
{}

template<typename Codecvt, typename Elem, typename Tr>
auto wbuffer_convert<Codecvt, Elem, Tr>::rdbuf() const -> streambuf* {
  return buf_;
}

template<typename Codecvt, typename Elem, typename Tr>
auto wbuffer_convert<Codecvt, Elem, Tr>::rdbuf(streambuf* buf) -> streambuf* {
  return exchange(buf_, buf);
}

template<typename Codecvt, typename Elem, typename Tr>
auto wbuffer_convert<Codecvt, Elem, Tr>::state() const -> state_type {
  return cvt_state_;
}

template<typename Codecvt, typename Elem, typename Tr>
auto wbuffer_convert<Codecvt, Elem, Tr>::showmanyc() ->
    streamsize {
  return -1;
}

template<typename Codecvt, typename Elem, typename Tr>
auto wbuffer_convert<Codecvt, Elem, Tr>::xsgetn(
    typename basic_streambuf<Elem, Tr>::char_type* s, streamsize n) ->
    streamsize {
  using traits_type = typename basic_streambuf<Elem, Tr>::traits_type;
  using int_type = typename basic_streambuf<Elem, Tr>::int_type;
  using char_type = typename basic_streambuf<Elem, Tr>::char_type;

  if (!buf_) return 0;  // XXX exception?
  if (_predict_false(always_noconv_)) return buf_->sgetn(s, n);
  char_type*const e = s + n;

  streamsize read_count = 0;
  while (n > 0) {
    const int_type cc;
    bool succes;
    try {
      tie(cc, ignore, succes) = uflow();
    } catch (...) {
      if (read_count == 0) throw;
      break;
    }

    if (traits_type::eq_int_type(cc, traits_type::eof())) break;
    *s = traits_type::to_char_type(cc);

    ++read_count;
    ++s;
    --n;
  }
  return read_count;
}

template<typename Codecvt, typename Elem, typename Tr>
auto wbuffer_convert<Codecvt, Elem, Tr>::underflow() ->
    typename basic_streambuf<Elem, Tr>::int_type {
  typename basic_streambuf<Elem, Tr>::int_type rv;
  bool succes;
  tie(rv, ignore, succes) = underflow_(false);
  if (!succes) throw range_error("wbuffer_convert: failed to convert input");
  return rv;
}

template<typename Codecvt, typename Elem, typename Tr>
auto wbuffer_convert<Codecvt, Elem, Tr>::uflow() ->
    typename basic_streambuf<Elem, Tr>::int_type {
  typename basic_streambuf<Elem, Tr>::int_type rv;
  bool succes;
  tie(rv, ignore, succes) = underflow_(true);
  if (!succes) throw range_error("wbuffer_convert: failed to convert input");
  return rv;
}

template<typename Codecvt, typename Elem, typename Tr>
auto wbuffer_convert<Codecvt, Elem, Tr>::xsputn(
    const typename basic_streambuf<Elem, Tr>::char_type* s, streamsize n) ->
    streamsize {
  using traits_type = typename basic_streambuf<Elem, Tr>::traits_type;
  using char_type = typename basic_streambuf<Elem, Tr>::char_type;

  if (!buf_) return 0;  // XXX exception?
  const char_type* s_end = s + n;
  const char_type* s_next = s;

  /* Convert and write entire string. */
  while (s_next != s_end) {
    array<streambuf::char_type, 64> tmp;
    auto out_next = begin(tmp);

    /* Conversion into temporary buffer. */
    auto cvt_rv = cvt_->out(cvt_state_,
                            s_next, s_end, s_next,
                            tmp.begin(), tmp.end(), out_next);
    /* Copy temporary buffer to underlying streambuf. */
    auto out_begin = tmp.begin();
    while (out_begin != out_next) {
      const streamsize wcount = buf_->sputn(out_begin, out_next - out_begin);
      out_begin += wcount;
      if (wcount == 0) {
        if (!streambuf::traits_type::eq_int_type(streambuf::traits_type::eof(),
                                                 buf_->sputc(*out_begin)))
          ++out_begin;
        else  // XXX
          assert_msg(false, "No idea how to break out of infinite loop...");
      }
    }

    /* Handle conversion result. */
    switch (cvt_rv) {
    case codecvt_base::ok:
    case codecvt_base::partial:
      break;
    case codecvt_base::error:
      if (s_next == s)
        throw range_error("wbuffer_convert: failed to convert output");
      return s_next - s;
      break;
    case codecvt_base::noconv:
      /* No conversion required: copy immediately. */
      while (s_next != s_end) {
        if (streambuf::traits_type::eq_int_type(streambuf::traits_type::eof(),
                                                buf_->sputc(*s_next)))
          break;
        ++s_next;
      }
      return s_next - s;
    }
  }

  return s_next - s;
}

template<typename Codecvt, typename Elem, typename Tr>
auto wbuffer_convert<Codecvt, Elem, Tr>::overflow(
    typename basic_streambuf<Elem, Tr>::int_type cc) ->
    typename basic_streambuf<Elem, Tr>::int_type {
  using traits_type = typename basic_streambuf<Elem, Tr>::traits_type;
  using char_type = typename basic_streambuf<Elem, Tr>::char_type;

  if (!traits_type::eq_int_type(cc, traits_type::eof())) {
    char_type c = traits_type::to_char_type(cc);
    sputn(&c, 1);
  }
  return cc;
}

template<typename Codecvt, typename Elem, typename Tr>
auto wbuffer_convert<Codecvt, Elem, Tr>::underflow_(bool do_bump) ->
    tuple<typename basic_streambuf<Elem, Tr>::int_type, streamsize, bool> {
  using traits_type = typename basic_streambuf<Elem, Tr>::traits_type;
  using int_type = typename basic_streambuf<Elem, Tr>::int_type;
  using char_type = typename basic_streambuf<Elem, Tr>::char_type;

  if (!buf_) return { traits_type::eof(), 0, false };  // XXX exception?

  /* Set up result values. */
  streamsize bumpcount = 0;
  char_type result;
  auto out_next = &result;
  const auto out_e = &result + 1;

  /* Allow loop to first clean out pending out chars in codecvt. */
  bool do_read = false;

  /* Operate on a copy of streamstate. */
  state_type state_ = this->state_;

  /* Read and convert from underlying buffer. */
  while (out_next != out_e) {
    typename streambuf::int_type cc;
    if (do_read) {
      /* Read 1 byte; increments position regardless
       * (will undo position increment afterwards, iff !do_bump). */
      cc = buf_->sbumpc();

      /* Handle eof. */
      if (streambuf::traits_type::eq_int_type(cc,
                                              streambuf::traits_type::eof())) {
        for (auto i = bumpcount; i > 0; --i) buf_->sungetc();
        return { traits_type::eof(), 0, bumpcount == 0 };
      }

      ++bumpcount;
    } else {
      cc = 0;
    }

    /* Convert read character, which may yield an output character. */
    typename streambuf::char_type c = streambuf::traits_type::to_char_type(cc);
    auto in_next = &c;
    const auto in_e = &c + (do_read ? 1 : 0);
    auto cvt_result = cvt_->in(cvt_state_,
                               in_next, in_e, in_next,
                               out_next, out_e, out_next);

    /* Handle conversion result. */
    switch(cvt_result) {
    case codecvt_base::ok:
    case codecvt_base::partial:
      break;
    case codecvt_base::error:
      for (auto i = bumpcount; i > 0; --i) buf_->sungetc();
      return { traits_type::eof(), 0, false };
    case codecvt_base::noconv:
      result = c;
      ++out_next;
      break;
    }

    do_read = true;
  }

  /* Undo bumping, if we were not supposed to do that. */
  if (!do_bump)
    for (auto i = bumpcount; i > 0; --i) buf_->sungetc();

  /* Save state. */
  this->state_ = state_;

  return { traits_type::to_int_type(result), bumpcount, true };
}


inline locale::facet::facet(size_t refs)
: refs_(refs)
{}

_LOCALE_INLINE
locale::facet::~facet() noexcept {}


template<typename Char>
ctype<Char>::ctype(size_t refs)
: locale::facet(refs)
{}

template<typename Char>
auto ctype<Char>::is(mask m, char_type c) const -> bool {
  return do_is(m, c);
}

template<typename Char>
auto ctype<Char>::is(const char_type* b, const char_type* e, mask* vec)
    const -> const char_type* {
  return do_is(b, e, vec);
}

template<typename Char>
auto ctype<Char>::scan_is(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_is(m, b, e);
}

template<typename Char>
auto ctype<Char>::scan_not(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_not(m, b, e);
}

template<typename Char>
auto ctype<Char>::toupper(char_type c) const -> char_type {
  return do_toupper(c);
}

template<typename Char>
auto ctype<Char>::toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_toupper(b, e);
}

template<typename Char>
auto ctype<Char>::tolower(char_type c) const -> char_type {
  return do_tolower(c);
}

template<typename Char>
auto ctype<Char>::tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_tolower(b, e);
}

template<typename Char>
auto ctype<Char>::widen(char c) const -> char_type {
  return do_widen(c);
}

template<typename Char>
auto ctype<Char>::widen(const char* b, const char* e, char_type* out) const ->
    const char* {
  return do_widen(b, e, out);
}

template<typename Char>
auto ctype<Char>::narrow(char_type c, char dfl) const -> char {
  return do_narrow(c, dfl);
}

template<typename Char>
auto ctype<Char>::narrow(const char_type* b, const char_type* e, char dfl,
                         char* out) const -> const char_type* {
  return do_narrow(b, e, dfl, out);
}

template<typename Char>
ctype<Char>::~ctype() noexcept {}


_LOCALE_INLINE
ctype<char>::ctype(const mask* tab, bool del, size_t refs)
: locale::facet(refs),
  tbl_(tab),
  del_(del)
{}

_LOCALE_INLINE
auto ctype<char>::is(mask m, char_type c) const -> bool {
  const size_t idx = static_cast<unsigned char>(c);
  return (idx < table_size && table() != nullptr ?
          bool(table()[idx] & m) :
          false);
}

_LOCALE_INLINE
auto ctype<char>::is(const char_type* b, const char_type* e, mask* vec)
    const -> const char_type* {
  transform(b, e, vec,
            [this](char_type c) -> mask {
              const size_t idx = static_cast<unsigned char>(c);
              return (idx < table_size && table() != nullptr ?
                      table()[idx] :
                      0);
            });
  return e;
}

_LOCALE_INLINE
auto ctype<char>::scan_is(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if(b, e,
                 [m, this](char_type c) -> bool {
                   return this->is(m, c);
                 });
}

_LOCALE_INLINE
auto ctype<char>::scan_not(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if_not(b, e,
                     [m, this](char_type c) -> bool {
                       return this->is(m, c);
                     });
}

_LOCALE_INLINE
auto ctype<char>::toupper(char_type c) const -> char_type {
  return do_toupper(c);
}

_LOCALE_INLINE
auto ctype<char>::toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_toupper(b, e);
}

_LOCALE_INLINE
auto ctype<char>::tolower(char_type c) const -> char_type {
  return do_tolower(c);
}

_LOCALE_INLINE
auto ctype<char>::tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_tolower(b, e);
}

_LOCALE_INLINE
auto ctype<char>::widen(char c) const -> char_type {
  return do_widen(c);
}

_LOCALE_INLINE
auto ctype<char>::widen(const char* b, const char* e, char_type* out) const ->
    const char* {
  return do_widen(b, e, out);
}

_LOCALE_INLINE
auto ctype<char>::narrow(char_type c, char dfl) const -> char {
  return do_narrow(c, dfl);
}

_LOCALE_INLINE
auto ctype<char>::narrow(const char_type* b, const char_type* e, char dfl,
                         char* out) const -> const char_type* {
  return do_narrow(b, e, dfl, out);
}

_LOCALE_INLINE
auto ctype<char>::table() const noexcept -> const mask* {
  return tbl_;
}

_LOCALE_INLINE
ctype<char>::~ctype() noexcept {
  if (del_ && tbl_) delete[] tbl_;
}

_LOCALE_INLINE
auto ctype<char>::do_toupper(char_type c) const -> char_type {
  return (c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c);
}

_LOCALE_INLINE
auto ctype<char>::do_toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->toupper(c);
                   });
}

_LOCALE_INLINE
auto ctype<char>::do_tolower(char_type c) const -> char_type {
  return (c >= 'A' && c <= 'A' ? c - 'A' + 'a' : c);
}

_LOCALE_INLINE
auto ctype<char>::do_tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->tolower(c);
                   });
}

_LOCALE_INLINE
auto ctype<char>::do_widen(char c) const -> char_type {
  return c;
}

_LOCALE_INLINE
auto ctype<char>::do_widen(const char* b, const char* e, char_type* out)
    const -> const char* {
  transform(b, e, out,
            [this](char c) -> char_type {
              return this->widen(c);
            });
  return e;
}

_LOCALE_INLINE
auto ctype<char>::do_narrow(char_type c, char) const -> char {
  return c;
}

_LOCALE_INLINE
auto ctype<char>::do_narrow(const char_type* b, const char_type* e,
                            char dfl, char* out) const -> const char_type* {
  transform(b, e, out,
            [this, dfl](char c) -> char_type {
              return this->narrow(c, dfl);
            });
  return e;
}


template<typename Char>
ctype_byname<Char>::ctype_byname(const char* name, size_t refs)
: ctype_byname(string_ref(name), refs)
{}

template<typename Char>
ctype_byname<Char>::ctype_byname(const string& name, size_t refs)
: ctype_byname(string_ref(name), refs)
{}

template<typename Char>
ctype_byname<Char>::ctype_byname(string_ref name, size_t refs)
: ctype<Char>(refs),
  impl::facet_ref<ctype<Char>>(locale(name))
{}

template<typename Char>
auto ctype_byname<Char>::do_is(mask m, char_type c) const -> bool {
  return this->impl.is(m, c);
}

template<typename Char>
auto ctype_byname<Char>::do_is(const char_type* b, const char_type* e,
                               mask* vec) const -> const char_type* {
  return this->impl.is(b, e, vec);
}

template<typename Char>
auto ctype_byname<Char>::do_scan_is(mask m,
                                    const char_type* b, const char_type* e)
    const -> const char_type* {
  return this->impl.scan_is(m, b, e);
}

template<typename Char>
auto ctype_byname<Char>::do_scan_not(mask m,
                                     const char_type* b, const char_type* e)
    const -> const char_type* {
  return this->impl.scan_not(m, b, e);
}

template<typename Char>
auto ctype_byname<Char>::do_toupper(char_type c) const -> char_type {
  return this->impl.toupper(c);
}

template<typename Char>
auto ctype_byname<Char>::do_toupper(char_type* b, const char_type* e)
    const -> const char_type* {
  return this->impl.toupper(b, e);
}

template<typename Char>
auto ctype_byname<Char>::do_tolower(char_type c) const -> char_type {
  return this->impl.tolower(c);
}

template<typename Char>
auto ctype_byname<Char>::do_tolower(char_type* b, const char_type* e)
    const -> const char_type* {
  return this->impl.tolower(b, e);
}

template<typename Char>
auto ctype_byname<Char>::do_widen(char c) const -> char_type {
  return this->impl.widen(c);
}

template<typename Char>
auto ctype_byname<Char>::do_widen(const char* b, const char* e, char_type* out)
    const -> const char* {
  return this->impl.widen(b, e, out);
}

template<typename Char>
auto ctype_byname<Char>::do_narrow(char_type c, char dfl) const -> char {
  return this->impl.narrow(c, dfl);
}

template<typename Char>
auto ctype_byname<Char>::do_narrow(const char_type* b, const char_type* e,
                                   char dfl, char* out)
    const -> const char_type* {
  return this->impl.narrow(b, e, dfl, out);
}


inline ctype_byname<char>::ctype_byname(const char* name, size_t refs)
: ctype_byname(string_ref(name), refs)
{}

inline ctype_byname<char>::ctype_byname(const string& name, size_t refs)
: ctype_byname(string_ref(name), refs)
{}

_LOCALE_INLINE
ctype_byname<char>::ctype_byname(string_ref name, size_t refs)
: impl::facet_ref<ctype<char>>(locale(name)),
  ctype<char>(this->impl.table(), false, refs)
{}

_LOCALE_INLINE
ctype_byname<char>::~ctype_byname() noexcept {}

_LOCALE_INLINE
auto ctype_byname<char>::do_toupper(char_type c) const -> char_type {
  return this->impl.toupper(c);
}

_LOCALE_INLINE
auto ctype_byname<char>::do_toupper(char_type* b, const char_type* e)
    const -> const char_type* {
  return this->impl.toupper(b, e);
}

_LOCALE_INLINE
auto ctype_byname<char>::do_tolower(char_type c) const -> char_type {
  return this->impl.tolower(c);
}

_LOCALE_INLINE
auto ctype_byname<char>::do_tolower(char_type* b, const char_type* e)
    const -> const char_type* {
  return this->impl.tolower(b, e);
}

_LOCALE_INLINE
auto ctype_byname<char>::do_widen(char c) const -> char_type {
  return this->impl.widen(c);
}

_LOCALE_INLINE
auto ctype_byname<char>::do_widen(const char* b, const char* e, char_type* out)
    const -> const char* {
  return this->impl.widen(b, e, out);
}

_LOCALE_INLINE
auto ctype_byname<char>::do_narrow(char_type c, char dfl) const -> char {
  return this->impl.narrow(c, dfl);
}

_LOCALE_INLINE
auto ctype_byname<char>::do_narrow(const char_type* b, const char_type* e,
                                   char dfl, char* out)
    const -> const char_type* {
  return this->impl.narrow(b, e, dfl, out);
}


template<typename Intern, typename Extern, typename St>
codecvt<Intern, Extern, St>::codecvt(size_t refs)
: locale::facet(refs)
{}

template<typename Intern, typename Extern, typename St>
auto codecvt<Intern, Extern, St>::out(state_type& s,
                                      const intern_type* from,
                                      const intern_type* from_end,
                                      const intern_type*& from_next,
                                      extern_type* to,
                                      extern_type* to_end,
                                      extern_type*& to_next) const -> result {
  return do_out(s, from, from_end, from_next, to, to_end, to_next);
}

template<typename Intern, typename Extern, typename St>
auto codecvt<Intern, Extern, St>::in(state_type& s,
                                     const extern_type* from,
                                     const extern_type* from_end,
                                     const extern_type*& from_next,
                                     intern_type* to,
                                     intern_type* to_end,
                                     intern_type*& to_next) const -> result {
  return do_in(s, from, from_end, from_next, to, to_end, to_next);
}

template<typename Intern, typename Extern, typename St>
auto codecvt<Intern, Extern, St>::unshift(state_type& s,
                                          extern_type* to,
                                          extern_type* to_end,
                                          extern_type*& to_next)
    const -> result {
  return do_unshift(s, to, to_end, to_next);
}

template<typename Intern, typename Extern, typename St>
auto codecvt<Intern, Extern, St>::encoding() const noexcept -> int {
  return do_encoding();
}

template<typename Intern, typename Extern, typename St>
auto codecvt<Intern, Extern, St>::always_noconv() const noexcept -> bool {
  return do_always_noconv();
}

template<typename Intern, typename Extern, typename St>
auto codecvt<Intern, Extern, St>::length(state_type& s,
                                         const extern_type* from,
                                         const extern_type* end,
                                         size_t max) const -> int {
  return do_length(s, from, end, max);
}

template<typename Intern, typename Extern, typename St>
auto codecvt<Intern, Extern, St>::max_length() const noexcept -> int {
  return do_max_length();
}

template<typename Intern, typename Extern, typename St>
codecvt<Intern, Extern, St>::~codecvt() noexcept {}


template<typename Intern, typename Extern, typename St>
codecvt_byname<Intern, Extern, St>::codecvt_byname(const char* name,
                                                   size_t refs)
: codecvt_byname(string_ref(name), refs)
{}

template<typename Intern, typename Extern, typename St>
codecvt_byname<Intern, Extern, St>::codecvt_byname(const string& name,
                                                   size_t refs)
: codecvt_byname(string_ref(name), refs)
{}

template<typename Intern, typename Extern, typename St>
codecvt_byname<Intern, Extern, St>::codecvt_byname(string_ref name,
                                                   size_t refs)
: codecvt<Intern, Extern, St>(refs),
  impl::facet_ref<codecvt<Intern, Extern, St>>(locale(name))
{}

template<typename Intern, typename Extern, typename St>
auto codecvt_byname<Intern, Extern, St>::do_out(state_type& s,
                                                const intern_type* from,
                                                const intern_type* from_end,
                                                const intern_type*& from_next,
                                                extern_type* to,
                                                extern_type* to_end,
                                                extern_type*& to_next)
    const -> result {
  return this->impl.out(s, from, from_end, from_next, to, to_end, to_next);
}

template<typename Intern, typename Extern, typename St>
auto codecvt_byname<Intern, Extern, St>::do_in(state_type& s,
                                               const extern_type* from,
                                               const extern_type* from_end,
                                               const extern_type*& from_next,
                                               intern_type* to,
                                               intern_type* to_end,
                                               intern_type*& to_next)
    const -> result {
  return this->impl.in(s, from, from_end, from_next, to, to_end, to_next);
}

template<typename Intern, typename Extern, typename St>
auto codecvt_byname<Intern, Extern, St>::do_unshift(state_type& s,
                                                    extern_type* to,
                                                    extern_type* to_end,
                                                    extern_type*& to_next)
    const -> result {
  return this->impl.unshift(s, to, to_end, to_next);
}

template<typename Intern, typename Extern, typename St>
auto codecvt_byname<Intern, Extern, St>::do_encoding() const noexcept -> int {
  return this->impl.encoding();
}

template<typename Intern, typename Extern, typename St>
auto codecvt_byname<Intern, Extern, St>::do_always_noconv()
    const noexcept -> bool {
  return this->impl.always_noconv();
}

template<typename Intern, typename Extern, typename St>
auto codecvt_byname<Intern, Extern, St>::do_length(state_type& s,
                                                   const extern_type* from,
                                                   const extern_type* end,
                                                   size_t max) const -> int {
  return this->impl.length(s, from, end, max);
}

template<typename Intern, typename Extern, typename St>
auto codecvt_byname<Intern, Extern, St>::do_max_length()
    const noexcept -> int {
  return this->impl.max_length();
}


/* Implement codecvt classes. */
#define _SPECIALIZE_CODECVT(Intern, Extern)				\
_LOCALE_INLINE								\
codecvt<Intern, Extern, mbstate_t>::codecvt(size_t refs)		\
: locale::facet(refs)							\
{}									\
									\
_LOCALE_INLINE								\
auto codecvt<Intern, Extern, mbstate_t>::out(				\
    state_type& s,							\
    const intern_type* from, const intern_type* from_end,		\
    const intern_type*& from_next,					\
    extern_type* to, extern_type* to_end,				\
    extern_type*& to_next) const -> result {				\
  return do_out(s, from, from_end, from_next, to, to_end, to_next);	\
}									\
									\
_LOCALE_INLINE								\
auto codecvt<Intern, Extern, mbstate_t>::in(				\
    state_type& s,							\
    const extern_type* from, const extern_type* from_end,		\
    const extern_type*& from_next,					\
    intern_type* to, intern_type* to_end,				\
    intern_type*& to_next) const -> result {				\
  return do_in(s, from, from_end, from_next, to, to_end, to_next);	\
}									\
									\
_LOCALE_INLINE								\
auto codecvt<Intern, Extern, mbstate_t>::unshift(			\
    state_type& s,							\
    extern_type* to, extern_type* to_end,				\
    extern_type*& to_next) const -> result {				\
  return do_unshift(s, to, to_end, to_next);				\
}									\
									\
_LOCALE_INLINE								\
auto codecvt<Intern, Extern, mbstate_t>::encoding()			\
    const noexcept -> int {						\
  return do_encoding();							\
}									\
									\
_LOCALE_INLINE								\
auto codecvt<Intern, Extern, mbstate_t>::always_noconv()		\
    const noexcept -> bool {						\
  return do_always_noconv();						\
}									\
									\
_LOCALE_INLINE								\
auto codecvt<Intern, Extern, mbstate_t>::length(			\
    state_type& s,							\
    const extern_type* from, const extern_type* end,			\
    size_t max) const -> int {						\
  return do_length(s, from, end, max);					\
}									\
									\
_LOCALE_INLINE								\
auto codecvt<Intern, Extern, mbstate_t>::max_length()			\
    const noexcept -> int {						\
  return do_max_length();						\
}									\
									\
_LOCALE_INLINE								\
codecvt<Intern, Extern, mbstate_t>::~codecvt() noexcept {}


/* Allow identity mapping to be inlined easily. */
#define _SPECIALIZE_CODECVT_IDENTITY(Intern, Extern)			\
_LOCALE_INLINE								\
auto codecvt<Intern, Extern, mbstate_t>::do_out(			\
    state_type&,							\
    const intern_type* from, const intern_type* from_end,		\
    const intern_type*& from_next,					\
    extern_type* to, extern_type* to_end,				\
    extern_type*& to_next) const -> result {				\
  assert(from <= from_end);						\
  assert(to <= to_end);							\
									\
  from_next = from;							\
  to_next = to;								\
  return noconv;							\
}									\
									\
_LOCALE_INLINE								\
auto codecvt<Intern, Extern, mbstate_t>::do_in(				\
    state_type&,							\
    const extern_type* from, const extern_type* from_end,		\
    const extern_type*& from_next,					\
    intern_type* to, intern_type* to_end,				\
    intern_type*& to_next) const -> result {				\
  assert(from <= from_end);						\
  assert(to <= to_end);							\
									\
  from_next = from;							\
  to_next = to;								\
  return noconv;							\
}									\
									\
_LOCALE_INLINE								\
auto codecvt<Intern, Extern, mbstate_t>::do_unshift(			\
    state_type&,							\
    extern_type* to, extern_type* to_end,				\
    extern_type*& to_next) const -> result {				\
  assert(to <= to_end);							\
									\
  to_next = to;								\
  return noconv;							\
}									\
									\
_LOCALE_INLINE								\
auto codecvt<Intern, Extern, mbstate_t>::do_encoding()			\
    const noexcept -> int {						\
  return 1;								\
}									\
									\
_LOCALE_INLINE								\
auto codecvt<Intern, Extern, mbstate_t>::do_always_noconv()		\
    const noexcept -> bool {						\
  return true;								\
}									\
									\
_LOCALE_INLINE								\
auto codecvt<Intern, Extern, mbstate_t>::do_length(			\
    state_type&,							\
    const extern_type* from,						\
    const extern_type* from_end,					\
    size_t max) const -> int {						\
  assert(from <= from_end);						\
									\
  /* Calculate result. */						\
  const auto rv = min(uintptr_t(from_end - from), uintptr_t(max));	\
									\
  /*									\
   * Handle int overflow.						\
   * Interface really does specify an int return type...		\
   */									\
  if (_predict_false(rv > size_t(INT_MAX)))				\
    throw overflow_error("codecvt::do_length");				\
  return rv;								\
}									\
									\
_LOCALE_INLINE								\
auto codecvt<Intern, Extern, mbstate_t>::do_max_length()		\
    const noexcept -> int {						\
  return 1;								\
}

_SPECIALIZE_CODECVT(char,     char)
_SPECIALIZE_CODECVT(char16_t, char16_t)
_SPECIALIZE_CODECVT(char32_t, char32_t)
_SPECIALIZE_CODECVT(wchar_t,  wchar_t)

_SPECIALIZE_CODECVT_IDENTITY(char,     char)
_SPECIALIZE_CODECVT_IDENTITY(char16_t, char16_t)
_SPECIALIZE_CODECVT_IDENTITY(char32_t, char32_t)
_SPECIALIZE_CODECVT_IDENTITY(wchar_t,  wchar_t)

/*
 * Clean up temporary macros, unless compiling locale.cc, in which case
 * we leave them, for locale.cc to use.
 */
#ifndef _COMPILING_LOCALE
# undef _SPECIALIZE_CODECVT
# undef _SPECIALIZE_CODECVT_IDENTITY
#endif


template<typename Char, typename Iter>
num_put<Char, Iter>::num_put(size_t refs)
: locale::facet(refs)
{}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              bool v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              long v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              long long v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              unsigned long v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              unsigned long long v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              double v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              long double v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::put(iter_type out, ios_base& str, char_type fill,
                              const void* v) const -> iter_type {
  return do_put(out, str, fill, v);
}

template<typename Char, typename Iter>
num_put<Char, Iter>::~num_put() noexcept {}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 bool v) const -> iter_type {
  using impl::render_num_encoder;

  if ((str.flags() & ios_base::boolalpha) != ios_base::boolalpha)
    return do_put(out, str, fill, static_cast<long>(v));

  const numpunct<char_type>& punct =
      use_facet<numpunct<char_type>>(str.getloc());
  const auto vname = (v ? punct.truename() : punct.falsename());
  return render_num_encoder(out, str,
                            basic_string_ref<char_type>(),
                            basic_string_ref<char_type>(vname),
                            fill, false);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 long v) const -> iter_type {
  using impl::render_num_encoder;
  using impl::num_encoder;

  return render_num_encoder(out, str,
                            num_encoder<char_type, long>(v, str),
                            fill);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 long long v) const -> iter_type {
  using impl::render_num_encoder;
  using impl::num_encoder;

  return render_num_encoder(out, str,
                            num_encoder<char_type, long long>(v, str),
                            fill);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 unsigned long v) const -> iter_type {
  using impl::render_num_encoder;
  using impl::num_encoder;

  return render_num_encoder(out, str,
                            num_encoder<char_type, unsigned long>(v, str),
                            fill);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 unsigned long long v) const -> iter_type {
  using impl::render_num_encoder;
  using impl::num_encoder;

  return render_num_encoder(out, str,
                            num_encoder<char_type, unsigned long long>(v, str),
                            fill);
}

#if _USE_INT128
template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 int128_t v) const -> iter_type {
  using impl::render_num_encoder;
  using impl::num_encoder;

  return render_num_encoder(out, str,
                            num_encoder<char_type, int128_t>(v, str),
                            fill);
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 uint128_t v) const -> iter_type {
  using impl::render_num_encoder;
  using impl::num_encoder;

  return render_num_encoder(out, str,
                            num_encoder<char_type, uint128_t>(v, str),
                            fill);
}
#endif

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& /*str*/, char_type /*fill*/,
                                 double /*v*/) const -> iter_type {
  assert_msg(false, "XXX implement");  // XXX implement
  return out;
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& /*str*/, char_type /*fill*/,
                                 long double /*v*/) const -> iter_type {
  assert_msg(false, "XXX implement");  // XXX implement
  return out;
}

template<typename Char, typename Iter>
auto num_put<Char, Iter>::do_put(iter_type out, ios_base& str, char_type fill,
                                 const void* v) const -> iter_type {
  constexpr auto nhexdigits = numeric_limits<uintptr_t>::digits / 4U;
  constexpr Char xdigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                 '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
  using data_t = char_type[nhexdigits + 2];

  uintptr_t pv = reinterpret_cast<uintptr_t>(v);
  data_t data;
  decltype(begin(data)) i;
  for (i = begin(data);
       (pv != 0 || i == begin(data)) && i != begin(data) + nhexdigits;
       ++i, pv /= 16)
    *i = xdigits[pv % 16];

  *i++ = 'x';
  *i++ = '0';
  reverse(begin(data), i);

  const auto data_str = basic_string_ref<char_type>(begin(data),
                                                    i - begin(data));
  const auto prefix = data_str.substr(0, 2);
  const auto digits = data_str.substr(2);
  return impl::render_num_encoder(out, str,
                                  prefix, digits, fill, false);
}


inline numpunct<char>::numpunct(size_t refs)
: locale::facet(refs)
{}
inline numpunct<char16_t>::numpunct(size_t refs)
: locale::facet(refs)
{}
inline numpunct<char32_t>::numpunct(size_t refs)
: locale::facet(refs)
{}
inline numpunct<wchar_t>::numpunct(size_t refs)
: locale::facet(refs)
{}

_LOCALE_INLINE numpunct<char>::~numpunct() noexcept {}
_LOCALE_INLINE numpunct<char16_t>::~numpunct() noexcept {}
_LOCALE_INLINE numpunct<char32_t>::~numpunct() noexcept {}
_LOCALE_INLINE numpunct<wchar_t>::~numpunct() noexcept {}


template<typename Char>
numpunct_byname<Char>::numpunct_byname(const char* name, size_t refs)
: numpunct_byname(string_ref(name), refs)
{}

template<typename Char>
numpunct_byname<Char>::numpunct_byname(const string& name, size_t refs)
: numpunct_byname(string_ref(name), refs)
{}

template<typename Char>
numpunct_byname<Char>::numpunct_byname(string_ref name, size_t refs)
: numpunct<Char>(refs),
  impl::facet_ref<numpunct<Char>>(locale(name))
{}

template<typename Char>
auto numpunct_byname<Char>::do_decimal_point() const -> char_type {
  return this->impl.decimal_point();
}

template<typename Char>
auto numpunct_byname<Char>::do_thousands_sep() const -> char_type {
  return this->impl.thousands_sep();
}

template<typename Char>
auto numpunct_byname<Char>::do_grouping() const -> string {
  return this->impl.grouping();
}

template<typename Char>
auto numpunct_byname<Char>::do_truename() const -> string_type {
  return this->impl.truename();
}

template<typename Char>
auto numpunct_byname<Char>::do_falsename() const -> string_type {
  return this->impl.falsename();
}


template<typename Char>
collate<Char>::collate(size_t refs)
: locale::facet(refs)
{}

template<typename Char>
collate<Char>::~collate() noexcept {}

template<typename Char>
auto collate<Char>::compare(const char_type* b1, const char_type* e1,
                      const char_type* b2, const char_type* e2) const -> int {
  return do_compare(b1, e1, b2, e2);
}

template<typename Char>
auto collate<Char>::transform(const char_type* b, const char_type* e) const ->
    string_type {
  return do_transform(b, e);
}

template<typename Char>
auto collate<Char>::hash(const char_type* b, const char_type* e) const ->
    long {
  return do_hash(b, e);
}

template<typename Char>
auto collate<Char>::do_compare(const char_type* b1, const char_type* e1,
                         const char_type* b2, const char_type* e2) const ->
    int {
  int rv = char_traits<Char>::compare(b1, e1, min(e1 - b1, e2 - b2));
  if (rv == 0) {
    if (e1 - b1 < e2 - b2) return -1;
    if (e1 - b1 > e2 - b2) return  1;
  }
  return 0;
}

template<typename Char>
auto collate<Char>::do_transform(const char_type* b, const char_type* e)
    const -> string_type {
  return string_type(b, e);
}

template<typename Char>
auto collate<Char>::do_hash(const char_type* b, const char_type* e) const ->
    long {
  using string = basic_string_ref<Char>;
  using hash_t = _namespace(std)::hash<string>;

  return hash_t()(string(b, typename string::size_type(e - b)));
}


template<typename Char>
collate_byname<Char>::collate_byname(const char* name, size_t refs)
: collate_byname(string_ref(name), refs)
{}

template<typename Char>
collate_byname<Char>::collate_byname(const string& name, size_t refs)
: collate_byname(string_ref(name), refs)
{}

template<typename Char>
collate_byname<Char>::collate_byname(string_ref name, size_t refs)
: collate<Char>(refs),
  impl::facet_ref<collate<Char>>(locale(name))
{}

template<typename Char>
auto collate_byname<Char>::do_compare(const char_type* b1, const char_type* e1,
                                      const char_type* b2, const char_type* e2)
    const -> int {
  return this->impl.compare(b1, e1, b2, e2);
}

template<typename Char>
auto collate_byname<Char>::do_transform(const char_type* b, const char_type* e)
    const -> string_type {
  return this->impl.transform(b, e);
}

template<typename Char>
auto collate_byname<Char>::do_hash(const char_type* b, const char_type* e)
    const -> long {
  return this->impl.hash(b, e);
}


template<typename Char>
messages<Char>::messages(size_t refs)
: locale::facet(refs)
{}

template<typename Char>
auto messages<Char>::open(const string& name, const locale& loc) const ->
    catalog {
  return do_open(name, loc);
}

template<typename Char>
auto messages<Char>::get(catalog cat, int set, int msgid,
                         const string_type& dfl) const -> string_type {
  return do_get(cat, set, msgid, dfl);
}

template<typename Char>
auto messages<Char>::close(catalog cat) const -> void {
  do_close(cat);
}

template<typename Char>
messages<Char>::~messages() noexcept {}


template<typename Char>
messages_byname<Char>::messages_byname(const char* name, size_t refs)
: messages_byname(string_ref(name), refs)
{}

template<typename Char>
messages_byname<Char>::messages_byname(const string& name, size_t refs)
: messages_byname(string_ref(name), refs)
{}

template<typename Char>
messages_byname<Char>::messages_byname(string_ref name, size_t refs)
: messages<Char>(refs),
  impl::facet_ref<messages<Char>>(locale(name))
{}

template<typename Char>
auto messages_byname<Char>::do_open(const string& name, const locale& loc)
    const -> catalog {
  return this->impl.open(name, loc);
}

template<typename Char>
auto messages_byname<Char>::do_get(catalog cat, int set, int msgid,
                                   const string_type& dfl) const ->
    string_type {
  return this->impl.get(cat, set, msgid, dfl);
}

template<typename Char>
auto messages_byname<Char>::do_close(catalog cat) const -> void {
  return this->impl.close(cat);
}


_namespace_end(std)

#undef _LOCALE_INLINE

#endif /* _LOCALE_INL_H_ */
