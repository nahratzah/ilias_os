#ifndef _LOCALE_INL_H_
#define _LOCALE_INL_H_

#include <locale>
#include <algorithm>
#include <string>

#ifndef _LOCALE_INLINE
# define _LOCALE_INLINE  extern inline
#endif

_namespace_begin(std)


template<typename Facet>
const Facet& use_facet(const locale& loc) {
  return static_cast<const Facet&>(loc.use_facet_(&Facet::id));
}

template<typename Facet>
bool has_facet(const locale& loc) noexcept {
  return loc.has_facet_(&Facet::id);
}


inline locale::locale(locale&& loc) noexcept {
  using _namespace(std)::swap;
  swap(data_, loc.data_);
}

inline locale::locale(const void* data) noexcept
: data_(data)
{}

inline locale::locale(const char* name)
: locale(string_ref(name))
{}

inline locale::locale(const string& name)
: locale(string_ref(name))
{}

template<typename Facet>
locale::locale(const locale& loc, Facet* f)
: locale(loc, &Facet::id, f)
{}

inline locale::locale(const locale& loc, const char* name, category cat)
: locale(loc, string_ref(name), cat)
{}

inline locale::locale(const locale& loc, const string& name, category cat)
: locale(loc, string_ref(name), cat)
{}

inline auto locale::operator=(locale&& loc) noexcept -> const locale& {
  using _namespace(std)::swap;
  swap(data_, loc.data_);
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
  std::transform(b, e, vec,
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
  return std::find_if(b, e,
                      [m, this](char_type c) -> bool {
                        return this->is(m, c);
                      });
}

_LOCALE_INLINE
auto ctype<char>::scan_not(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return std::find_if_not(b, e,
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
  return std::transform(bb, e, b,
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
  return std::transform(bb, e, b,
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
  std::transform(b, e, out,
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
  std::transform(b, e, out,
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
  name_(name)
{}


_namespace_end(std)

#undef _LOCALE_INLINE

#endif /* _LOCALE_INL_H_ */
