#ifndef _LOCALE_INL_H_
#define _LOCALE_INL_H_

#include <locale>
#include <algorithm>

#ifndef _LOCALE_INLINE
# define _LOCALE_INLINE  extern inline
#endif

_namespace_begin(std)


_LOCALE_INLINE
auto ctype<char>::is(mask m, char_type c) const -> bool {
  return do_is(m, c);
}

_LOCALE_INLINE
auto ctype<char>::is(const char_type* b, const char_type* e, mask* vec)
    const -> const char_type* {
  return do_is(b, e, vec);
}

_LOCALE_INLINE
auto ctype<char>::scan_is(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_is(m, b, e);
}

_LOCALE_INLINE
auto ctype<char>::scan_not(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_not(m, b, e);
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
ctype<char>::~ctype() noexcept {}

_LOCALE_INLINE
auto ctype<char>::do_is(mask m, char_type c) const -> bool {
  const size_t idx = static_cast<unsigned char>(c);
  return (idx < table_size && table() != nullptr ?
          bool(table()[idx] & m) :
          false);
}

_LOCALE_INLINE
auto ctype<char>::do_is(const char_type* b, const char_type* e, mask* vec)
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
auto ctype<char>::do_scan_is(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return std::find_if(b, e,
                      [m, this](char_type c) -> bool {
                        return this->is(m, c);
                      });
}

_LOCALE_INLINE
auto ctype<char>::do_scan_not(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return std::find_if_not(b, e,
                          [m, this](char_type c) -> bool {
                            return this->is(m, c);
                          });
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


_namespace_end(std)

#endif /* _LOCALE_INL_H_ */
