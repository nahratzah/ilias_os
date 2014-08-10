#ifndef _LOCALE_MISC_CTYPE_INL_H_
#define _LOCALE_MISC_CTYPE_INL_H_

#include <locale_misc/ctype.h>
#include <string>

#ifndef _LOCALE_CTYPE_INLINE
# define _LOCALE_CTYPE_INLINE  extern inline
#endif

_namespace_begin(std)


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


_LOCALE_CTYPE_INLINE
ctype<char>::ctype(const mask* tab, bool del, size_t refs)
: locale::facet(refs),
  tbl_(tab),
  del_(del)
{}

_LOCALE_CTYPE_INLINE
auto ctype<char>::is(mask m, char_type c) const -> bool {
  const size_t idx = static_cast<unsigned char>(c);
  return (idx < table_size && table() != nullptr ?
          bool(table()[idx] & m) :
          false);
}

_LOCALE_CTYPE_INLINE
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

_LOCALE_CTYPE_INLINE
auto ctype<char>::scan_is(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if(b, e,
                 [m, this](char_type c) -> bool {
                   return this->is(m, c);
                 });
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::scan_not(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if_not(b, e,
                     [m, this](char_type c) -> bool {
                       return this->is(m, c);
                     });
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::toupper(char_type c) const -> char_type {
  return do_toupper(c);
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_toupper(b, e);
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::tolower(char_type c) const -> char_type {
  return do_tolower(c);
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_tolower(b, e);
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::widen(char c) const -> char_type {
  return do_widen(c);
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::widen(const char* b, const char* e, char_type* out) const ->
    const char* {
  return do_widen(b, e, out);
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::narrow(char_type c, char dfl) const -> char {
  return do_narrow(c, dfl);
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::narrow(const char_type* b, const char_type* e, char dfl,
                         char* out) const -> const char_type* {
  return do_narrow(b, e, dfl, out);
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::table() const noexcept -> const mask* {
  return tbl_;
}

_LOCALE_CTYPE_INLINE
ctype<char>::~ctype() noexcept {
  if (del_ && tbl_) delete[] tbl_;
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::do_toupper(char_type c) const -> char_type {
  return (c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c);
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::do_toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->toupper(c);
                   });
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::do_tolower(char_type c) const -> char_type {
  return (c >= 'A' && c <= 'A' ? c - 'A' + 'a' : c);
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::do_tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->tolower(c);
                   });
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::do_widen(char c) const -> char_type {
  return c;
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::do_widen(const char* b, const char* e, char_type* out)
    const -> const char* {
  transform(b, e, out,
            [this](char c) -> char_type {
              return this->widen(c);
            });
  return e;
}

_LOCALE_CTYPE_INLINE
auto ctype<char>::do_narrow(char_type c, char) const -> char {
  return c;
}

_LOCALE_CTYPE_INLINE
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

_LOCALE_CTYPE_INLINE
ctype_byname<char>::ctype_byname(string_ref name, size_t refs)
: impl::facet_ref<ctype<char>>(locale(name)),
  ctype<char>(this->impl.table(), false, refs)
{}

_LOCALE_CTYPE_INLINE
ctype_byname<char>::~ctype_byname() noexcept {}

_LOCALE_CTYPE_INLINE
auto ctype_byname<char>::do_toupper(char_type c) const -> char_type {
  return this->impl.toupper(c);
}

_LOCALE_CTYPE_INLINE
auto ctype_byname<char>::do_toupper(char_type* b, const char_type* e)
    const -> const char_type* {
  return this->impl.toupper(b, e);
}

_LOCALE_CTYPE_INLINE
auto ctype_byname<char>::do_tolower(char_type c) const -> char_type {
  return this->impl.tolower(c);
}

_LOCALE_CTYPE_INLINE
auto ctype_byname<char>::do_tolower(char_type* b, const char_type* e)
    const -> const char_type* {
  return this->impl.tolower(b, e);
}

_LOCALE_CTYPE_INLINE
auto ctype_byname<char>::do_widen(char c) const -> char_type {
  return this->impl.widen(c);
}

_LOCALE_CTYPE_INLINE
auto ctype_byname<char>::do_widen(const char* b, const char* e, char_type* out)
    const -> const char* {
  return this->impl.widen(b, e, out);
}

_LOCALE_CTYPE_INLINE
auto ctype_byname<char>::do_narrow(char_type c, char dfl) const -> char {
  return this->impl.narrow(c, dfl);
}

_LOCALE_CTYPE_INLINE
auto ctype_byname<char>::do_narrow(const char_type* b, const char_type* e,
                                   char dfl, char* out)
    const -> const char_type* {
  return this->impl.narrow(b, e, dfl, out);
}


_namespace_end(std)

#undef _LOCALE_CTYPE_INLINE

#endif /* _LOCALE_MISC_CTYPE_INL_H_ */
