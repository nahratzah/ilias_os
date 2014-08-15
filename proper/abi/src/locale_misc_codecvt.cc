#define _LOCALE_CODECVT_INLINE  /* Not inline: generate here. */
#define _COMPILING_LOCALE  /* Expose hidden types. */

#include <locale_misc/codecvt.h>
#include <cdecl.h>
#include <array>
#include <iterator>
#include <abi/errno.h>
#include <abi/ext/unicode.h>

_namespace_begin(std)


/* Generate generic implementation. */
#define _LOCALE_CODECVT_INLINE  // Not inline; required by _SPECIALIZE_CODECVT
_SPECIALIZE_CODECVT(char16_t, char)
_SPECIALIZE_CODECVT(char32_t, char)
_SPECIALIZE_CODECVT(wchar_t,  char)

_SPECIALIZE_CODECVT(char,     char16_t)
_SPECIALIZE_CODECVT(char32_t, char16_t)
_SPECIALIZE_CODECVT(wchar_t,  char16_t)

_SPECIALIZE_CODECVT(char,     char32_t)
_SPECIALIZE_CODECVT(char16_t, char32_t)
_SPECIALIZE_CODECVT(wchar_t,  char32_t)

_SPECIALIZE_CODECVT(char,     wchar_t)
_SPECIALIZE_CODECVT(char16_t, wchar_t)
_SPECIALIZE_CODECVT(char32_t, wchar_t)
#undef _LOCALE_INLINE

namespace {

template<typename> struct mbstate_data;

template<>
struct __attribute__((packed)) mbstate_data<char> {
  array<char, 6> data;
  unsigned char len : 3;
};

template<>
struct __attribute__((packed)) mbstate_data<char16_t> {
  array<char16_t, 2> data;
  unsigned char len : 2;
};

template<>
struct __attribute__((packed)) mbstate_data<char32_t> {
  array<char32_t, 1> data;
  unsigned char len : 1;
};

template<>
struct __attribute__((packed)) mbstate_data<wchar_t> {
  array<wchar_t, 0> data;
  unsigned char len : 1;  // Always 0
};

static_assert(sizeof(mbstate_data<char>) <= sizeof(mbstate_t) - 1U,
              "Insufficient space in mbstate_t, for char");
static_assert(sizeof(mbstate_data<char16_t>) <= sizeof(mbstate_t) - 1U,
              "Insufficient space in mbstate_t, for char16_t");
static_assert(sizeof(mbstate_data<char32_t>) <= sizeof(mbstate_t) - 1U,
              "Insufficient space in mbstate_t, for char32_t");
static_assert(sizeof(mbstate_data<wchar_t>) <= sizeof(mbstate_t) - 1U,
              "Insufficient space in mbstate_t, for wchar_t");

/* Last byte in mbstate is used for flags. */
inline unsigned char mbstate_get_flags(const mbstate_t& s) noexcept {
  unsigned char result;
  __builtin_memcpy(&result,
                   reinterpret_cast<const unsigned char*>(&s) + sizeof(s) - 1U,
                   1U);
  return result;
}
inline void mbstate_set_flags(mbstate_t& s, unsigned char v) noexcept {
  __builtin_memcpy(reinterpret_cast<unsigned char*>(&s) + sizeof(s) - 1U,
                   &v,
                   1U);
}

constexpr unsigned char MBSTATE_IN = 0x1;
constexpr unsigned char MBSTATE_OUT = 0x2;
constexpr unsigned char MBSTATE_DIRECTION_MSK = MBSTATE_IN | MBSTATE_OUT;

template<typename Intern, typename Extern>
struct unicode_codecvt {
  using state_type = mbstate_t;
  using intern_type = Intern;
  using extern_type = Extern;
  using result = codecvt_base::result;
  using intern_state = mbstate_data<intern_type>;
  using extern_state = mbstate_data<extern_type>;
  static constexpr result ok = codecvt_base::ok;
  static constexpr result error = codecvt_base::error;
  static constexpr result partial = codecvt_base::partial;

  static auto do_out(state_type& st_arg,
                     const intern_type* from, const intern_type* from_end,
                     const intern_type*& from_next,
                     extern_type* to, extern_type* to_end,
                     extern_type*& to_next) -> result;

  static auto do_unshift(state_type& st_arg,
                         extern_type* to, extern_type* to_end,
                         extern_type*& to_next) -> result;

  static auto do_max_length() noexcept -> int;
};

template<typename Intern, typename Extern>
auto unicode_codecvt<Intern, Extern>::do_out(
    state_type& st_arg,
    const intern_type* from, const intern_type* from_end,
    const intern_type*& from_next,
    extern_type* to, extern_type* to_end,
    extern_type*& to_next) -> result {
  using conv_t = abi::ext::unicode_conv<extern_type, intern_type>;
  using _namespace(std)::begin;
  using _namespace(std)::end;

  assert(from <= from_end);
  assert(to <= to_end);

  /* Get mbstate configuration. */
  const unsigned char mbstate_flags = mbstate_get_flags(st_arg);

  /* Initialize iterators. */
  const intern_type* from_iter = from;
  extern_type* to_iter = to;
  extern_type* tmp_to_iter;

  /* Prepare converter. */
  conv_t conv;
  bool conv_fail = false;

  /* Flush out chars stored in mbstate. */
  mbstate_data<extern_type> co_st;
  if ((mbstate_flags & MBSTATE_DIRECTION_MSK) != MBSTATE_OUT) {
    __builtin_bzero(&co_st, sizeof(co_st));
  } else {
    __builtin_memcpy(&co_st, &st_arg, sizeof(co_st));
    assert(co_st.len <= co_st.data.size());
    while (to_iter != to_end && co_st.len > 0) {
      *to_iter++ = co_st.data[0];
      move(begin(co_st.data) + 1, end(co_st.data), begin(co_st.data));
      --co_st.len;
    }

    /* Return partial if output is not entirely flushed. */
    if (co_st.len != 0) {
      __builtin_memcpy(&st_arg, &co_st, sizeof(co_st));
      /* Skip flag update: no change in mbstate type of data. */
      from_next = from_iter;
      to_next = to_iter;
      return partial;
    }
  }

  /* Initialize converter with in chars stored in mbstate. */
  mbstate_data<intern_type> ci_st;
  if ((mbstate_flags & MBSTATE_DIRECTION_MSK) != MBSTATE_IN) {
    __builtin_bzero(&ci_st, sizeof(ci_st));
  } else {
    __builtin_memcpy(&ci_st, &st_arg, sizeof(co_st));
    assert(ci_st.len <= ci_st.data.size());

    const auto conv_cb = [](extern_type) -> int { return _ABI_EINVAL; };
    for (unsigned int i = 0; i < ci_st.len; ++i) {
      auto conv_err = conv(ci_st.data[i], conv_cb);
      assert(conv_err == 0 || conv_err == _ABI_EINVAL);
    }
  }

  /* Prepare converter callback. */
  const auto conv_cb = [&tmp_to_iter, to_end, &co_st, &ci_st](
                           extern_type c) -> int {
                         assert(co_st.len < co_st.data.size());
                         if (tmp_to_iter != to_end)
                           *tmp_to_iter++ = c;
                         else
                           co_st.data[co_st.len++] = c;
                         ci_st.len = 0;
                         return 0;
                       };

  /* Convert characters. */
  while (!conv_fail && from_iter != from_end && to_iter != to_end) {
    assert_msg(to_iter == to_end || co_st.len == 0, "loop invariant");

    tmp_to_iter = to_iter;
    const intern_type c = *from_iter;
    ci_st.data[ci_st.len++] = c;
    auto conv_err = conv(c, conv_cb);

    /* Update iterators only if conversion succeeded. */
    assert(conv_err == 0 || conv_err == _ABI_EILSEQ);
    if (conv_err == _ABI_EILSEQ) {
      conv_fail = true;
      --ci_st.len;
    } else {
      ++from_iter;
      to_iter = tmp_to_iter;
    }
  }

  /* Update output state. */
  assert(ci_st.len == 0 || co_st.len == 0);
  if (ci_st.len != 0) {
    __builtin_memcpy(&st_arg, &ci_st, sizeof(ci_st));
    mbstate_set_flags(st_arg,
                      (mbstate_flags & ~MBSTATE_DIRECTION_MSK) | MBSTATE_IN);
  } else if (co_st.len != 0) {
    __builtin_memcpy(&st_arg, &co_st, sizeof(co_st));
    mbstate_set_flags(st_arg,
                      (mbstate_flags & ~MBSTATE_DIRECTION_MSK) | MBSTATE_OUT);
  } else {
    __builtin_bzero(&st_arg, sizeof(st_arg) - 1U);
    mbstate_set_flags(st_arg,
                      (mbstate_flags & ~MBSTATE_DIRECTION_MSK));
  }
  from_next = from_iter;
  to_next = to_iter;

  /* Generate return value. */
  if (conv_fail) return error;
  if (from_iter == from_end && co_st.len == 0) return ok;
  return partial;
}

template<typename Intern, typename Extern>
auto unicode_codecvt<Intern, Extern>::do_unshift(
    state_type& st_arg,
    extern_type* to, extern_type* to_end,
    extern_type*& to_next) -> result {
  /* Cannot flush input state. */
  if (_predict_false((mbstate_get_flags(st_arg) & MBSTATE_DIRECTION_MSK) !=
                     MBSTATE_OUT))
    return error;

  const intern_type* empty_iter = nullptr;
  const auto rv = do_out(st_arg,
                         empty_iter, empty_iter, empty_iter,
                         to, to_end, to_next);
  return rv;
}

template<typename Intern, typename Extern>
auto unicode_codecvt<Intern, Extern>::do_max_length() noexcept -> int {
  mbstate_data<extern_type> co_st;
  return co_st.data.size();
}

} /* namespace std::<unnamed> */

#define _IMPLEMENT_CODECVT(Intern, Extern)				\
auto codecvt<Intern, Extern, mbstate_t>::do_out(			\
    state_type& st_arg,							\
    const intern_type* from, const intern_type* from_end,		\
    const intern_type*& from_next,					\
    extern_type* to, extern_type* to_end,				\
    extern_type*& to_next) const -> result {				\
  return unicode_codecvt<intern_type, extern_type>::do_out(		\
      st_arg, from, from_end, from_next, to, to_end, to_next);		\
}									\
									\
auto codecvt<Intern, Extern, mbstate_t>::do_in(				\
    state_type& st_arg,							\
    const extern_type* from, const extern_type* from_end,		\
    const extern_type*& from_next,					\
    intern_type* to, intern_type* to_end,				\
    intern_type*& to_next) const -> result {				\
  return unicode_codecvt<extern_type, intern_type>::do_out(		\
      st_arg, from, from_end, from_next, to, to_end, to_next);		\
}									\
									\
auto codecvt<Intern, Extern, mbstate_t>::do_unshift(			\
    state_type& st_arg,							\
    extern_type* to, extern_type* to_end,				\
    extern_type*& to_next) const -> result {				\
  return unicode_codecvt<intern_type, extern_type>::do_unshift(		\
      st_arg, to, to_end, to_next);					\
}									\
									\
auto codecvt<Intern, Extern, mbstate_t>::do_encoding()			\
    const noexcept -> int {						\
  return 0;								\
}									\
									\
auto codecvt<Intern, Extern, mbstate_t>::do_always_noconv()		\
    const noexcept -> bool {						\
  return false;								\
}									\
									\
auto codecvt<Intern, Extern, mbstate_t>::do_max_length()		\
    const noexcept -> int {						\
  return unicode_codecvt<intern_type, extern_type>::do_max_length();	\
}									\
									\
auto codecvt<Intern, Extern, mbstate_t>::do_length(			\
    state_type& st_arg,							\
    const extern_type* from, const extern_type* from_end,		\
    size_t max) const -> int {						\
  using impl_type = unicode_codecvt<extern_type, intern_type>;		\
									\
  /* Truncate max, so the result value fits. */				\
  if (max > INT_MAX) max = INT_MAX;					\
									\
  /* Initialize variables. */						\
  array<intern_type, 128 / sizeof(intern_type)> out;			\
  size_t n = 0;								\
  result in_rv = ok;							\
									\
  /* Repeatedly call in, to perform conversion. */			\
  while (from != from_end && in_rv == ok && n < max) {			\
    auto out_iter = begin(out);						\
    auto out_end = (max - n > out.size() ?				\
                    end(out) :						\
                    begin(out) + (max - n));				\
    in_rv = impl_type::do_out(st_arg,					\
                              from, from_end, from,			\
                              begin(out), out_end, out_iter);		\
    if (in_rv == partial) in_rv = ok;					\
    n += (out_iter - begin(out));					\
  }									\
									\
  return n;								\
}

_IMPLEMENT_CODECVT(char16_t, char)
_IMPLEMENT_CODECVT(char32_t, char)
_IMPLEMENT_CODECVT(wchar_t,  char)

_IMPLEMENT_CODECVT(char,     char16_t)
_IMPLEMENT_CODECVT(char32_t, char16_t)
_IMPLEMENT_CODECVT(wchar_t,  char16_t)

_IMPLEMENT_CODECVT(char,     char32_t)
_IMPLEMENT_CODECVT(char16_t, char32_t)
_IMPLEMENT_CODECVT(wchar_t,  char32_t)

_IMPLEMENT_CODECVT(char,     wchar_t)
_IMPLEMENT_CODECVT(char16_t, wchar_t)
_IMPLEMENT_CODECVT(char32_t, wchar_t)


const locale::id codecvt<char,     char,     mbstate_t>::id{ locale::ctype };
const locale::id codecvt<char,     char16_t, mbstate_t>::id{ locale::ctype };
const locale::id codecvt<char,     char32_t, mbstate_t>::id{ locale::ctype };
const locale::id codecvt<char,     wchar_t,  mbstate_t>::id{ locale::ctype };
const locale::id codecvt<char16_t, char,     mbstate_t>::id{ locale::ctype };
const locale::id codecvt<char16_t, char16_t, mbstate_t>::id{ locale::ctype };
const locale::id codecvt<char16_t, char32_t, mbstate_t>::id{ locale::ctype };
const locale::id codecvt<char16_t, wchar_t,  mbstate_t>::id{ locale::ctype };
const locale::id codecvt<char32_t, char,     mbstate_t>::id{ locale::ctype };
const locale::id codecvt<char32_t, char16_t, mbstate_t>::id{ locale::ctype };
const locale::id codecvt<char32_t, char32_t, mbstate_t>::id{ locale::ctype };
const locale::id codecvt<char32_t, wchar_t,  mbstate_t>::id{ locale::ctype };
const locale::id codecvt<wchar_t,  char,     mbstate_t>::id{ locale::ctype };
const locale::id codecvt<wchar_t,  char16_t, mbstate_t>::id{ locale::ctype };
const locale::id codecvt<wchar_t,  char32_t, mbstate_t>::id{ locale::ctype };
const locale::id codecvt<wchar_t,  wchar_t,  mbstate_t>::id{ locale::ctype };

template class codecvt_byname<char,     char,     mbstate_t>;
template class codecvt_byname<char16_t, char,     mbstate_t>;
template class codecvt_byname<char32_t, char,     mbstate_t>;
template class codecvt_byname<wchar_t,  char,     mbstate_t>;
template class codecvt_byname<char,     char16_t, mbstate_t>;
template class codecvt_byname<char16_t, char16_t, mbstate_t>;
template class codecvt_byname<char32_t, char16_t, mbstate_t>;
template class codecvt_byname<wchar_t,  char16_t, mbstate_t>;
template class codecvt_byname<char,     char32_t, mbstate_t>;
template class codecvt_byname<char16_t, char32_t, mbstate_t>;
template class codecvt_byname<char32_t, char32_t, mbstate_t>;
template class codecvt_byname<wchar_t,  char32_t, mbstate_t>;
template class codecvt_byname<char,     wchar_t,  mbstate_t>;
template class codecvt_byname<char16_t, wchar_t,  mbstate_t>;
template class codecvt_byname<char32_t, wchar_t,  mbstate_t>;
template class codecvt_byname<wchar_t,  wchar_t,  mbstate_t>;


_namespace_end(std)
