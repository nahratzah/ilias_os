#ifndef _LOCALE_MISC_NUMPUNCT_INL_H_
#define _LOCALE_MISC_NUMPUNCT_INL_H_

#include <locale_misc/numpunct.h>

#ifndef _LOCALE_NUMPUNCT_INLINE
# define _LOCALE_NUMPUNCT_INLINE  extern inline
#endif

_namespace_begin(std)


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

_LOCALE_NUMPUNCT_INLINE numpunct<char>::~numpunct() noexcept {}
_LOCALE_NUMPUNCT_INLINE numpunct<char16_t>::~numpunct() noexcept {}
_LOCALE_NUMPUNCT_INLINE numpunct<char32_t>::~numpunct() noexcept {}
_LOCALE_NUMPUNCT_INLINE numpunct<wchar_t>::~numpunct() noexcept {}


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


_namespace_end(std)

#undef _LOCALE_NUMPUNCT_INLINE

#endif /* _LOCALE_MISC_NUMPUNCT_INL_H_ */
