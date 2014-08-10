#ifndef _LOCALE_MISC_CODECVT_INL_H_
#define _LOCALE_MISC_CODECVT_INL_H_

#include <locale_misc/codecvt.h>
#include <locale_misc/locale.h>
#include <algorithm>
#include <stdexcept>
#include <string>

#ifndef _LOCALE_CODECVT_INLINE
# define _LOCALE_CODECVT_INLINE	extern inline
#endif

_namespace_begin(std)


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
_LOCALE_CODECVT_INLINE							\
codecvt<Intern, Extern, mbstate_t>::codecvt(size_t refs)		\
: locale::facet(refs)							\
{}									\
									\
_LOCALE_CODECVT_INLINE							\
auto codecvt<Intern, Extern, mbstate_t>::out(				\
    state_type& s,							\
    const intern_type* from, const intern_type* from_end,		\
    const intern_type*& from_next,					\
    extern_type* to, extern_type* to_end,				\
    extern_type*& to_next) const -> result {				\
  return do_out(s, from, from_end, from_next, to, to_end, to_next);	\
}									\
									\
_LOCALE_CODECVT_INLINE							\
auto codecvt<Intern, Extern, mbstate_t>::in(				\
    state_type& s,							\
    const extern_type* from, const extern_type* from_end,		\
    const extern_type*& from_next,					\
    intern_type* to, intern_type* to_end,				\
    intern_type*& to_next) const -> result {				\
  return do_in(s, from, from_end, from_next, to, to_end, to_next);	\
}									\
									\
_LOCALE_CODECVT_INLINE							\
auto codecvt<Intern, Extern, mbstate_t>::unshift(			\
    state_type& s,							\
    extern_type* to, extern_type* to_end,				\
    extern_type*& to_next) const -> result {				\
  return do_unshift(s, to, to_end, to_next);				\
}									\
									\
_LOCALE_CODECVT_INLINE							\
auto codecvt<Intern, Extern, mbstate_t>::encoding()			\
    const noexcept -> int {						\
  return do_encoding();							\
}									\
									\
_LOCALE_CODECVT_INLINE							\
auto codecvt<Intern, Extern, mbstate_t>::always_noconv()		\
    const noexcept -> bool {						\
  return do_always_noconv();						\
}									\
									\
_LOCALE_CODECVT_INLINE							\
auto codecvt<Intern, Extern, mbstate_t>::length(			\
    state_type& s,							\
    const extern_type* from, const extern_type* end,			\
    size_t max) const -> int {						\
  return do_length(s, from, end, max);					\
}									\
									\
_LOCALE_CODECVT_INLINE							\
auto codecvt<Intern, Extern, mbstate_t>::max_length()			\
    const noexcept -> int {						\
  return do_max_length();						\
}									\
									\
_LOCALE_CODECVT_INLINE							\
codecvt<Intern, Extern, mbstate_t>::~codecvt() noexcept {}


/* Allow identity mapping to be inlined easily. */
#define _SPECIALIZE_CODECVT_IDENTITY(Intern, Extern)			\
_LOCALE_CODECVT_INLINE							\
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
_LOCALE_CODECVT_INLINE							\
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
_LOCALE_CODECVT_INLINE							\
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
_LOCALE_CODECVT_INLINE							\
auto codecvt<Intern, Extern, mbstate_t>::do_encoding()			\
    const noexcept -> int {						\
  return 1;								\
}									\
									\
_LOCALE_CODECVT_INLINE							\
auto codecvt<Intern, Extern, mbstate_t>::do_always_noconv()		\
    const noexcept -> bool {						\
  return true;								\
}									\
									\
_LOCALE_CODECVT_INLINE							\
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
_LOCALE_CODECVT_INLINE							\
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


_namespace_end(std)

#endif /* _LOCALE_MISC_CODECVT_INL_H_ */
