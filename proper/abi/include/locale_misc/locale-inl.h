#ifndef _LOCALE_MISC_LOCALE_INL_H_
#define _LOCALE_MISC_LOCALE_INL_H_

#include <locale_misc/locale.h>
#include <string>
#include <locale_misc/collate.h>

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


inline locale::facet::facet(size_t refs)
: refs_(refs)
{}

_LOCALE_INLINE
locale::facet::~facet() noexcept {}


_namespace_end(std)

#undef _LOCALE_INLINE

#endif /* _LOCALE_MISC_LOCALE_INL_H_ */
