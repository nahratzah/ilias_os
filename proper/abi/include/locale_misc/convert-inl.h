#ifndef _LOCALE_MISC_CONVERT_INL_H_
#define _LOCALE_MISC_CONVERT_INL_H_

#include <cdecl.h>
#include <locale_misc/convert.h>
#include <locale_misc/ctype.h>
#include <array>
#include <streambuf>

_namespace_begin(std)


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


_namespace_end(std)


#endif /* _LOCALE_MISC_CONVERT_INL_H_ */
