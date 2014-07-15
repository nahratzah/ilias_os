#include <locale>
#include <algorithm>

_namespace_begin(std)

constexpr locale::category locale::none,
                           locale::collate,
                           locale::ctype,
                           locale::monetary,
                           locale::numeric,
                           locale::time,
                           locale::messages,
                           locale::all;

constexpr ctype_base::mask ctype_base::space,
                           ctype_base::print,
                           ctype_base::cntrl,
                           ctype_base::upper,
                           ctype_base::lower,
                           ctype_base::alpha,
                           ctype_base::digit,
                           ctype_base::punct,
                           ctype_base::xdigit,
                           ctype_base::alnum,
                           ctype_base::graph;


auto ctype<char>::is(mask m, char_type c) const -> bool {
  return do_is(m, c);
}

auto ctype<char>::is(const char_type* b, const char_type* e, mask* vec)
    const -> const char_type* {
  return do_is(b, e, vec);
}

auto ctype<char>::scan_is(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_is(m, b, e);
}

auto ctype<char>::scan_not(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_not(m, b, e);
}

auto ctype<char>::toupper(char_type c) const -> char_type {
  return do_toupper(c);
}

auto ctype<char>::toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_toupper(b, e);
}

auto ctype<char>::tolower(char_type c) const -> char_type {
  return do_tolower(c);
}

auto ctype<char>::tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_tolower(b, e);
}

auto ctype<char>::widen(char c) const -> char_type {
  return do_widen(c);
}

auto ctype<char>::widen(const char* b, const char* e, char_type* out) const ->
    const char* {
  return do_widen(b, e, out);
}

auto ctype<char>::narrow(char_type c, char dfl) const -> char {
  return do_narrow(c, dfl);
}

auto ctype<char>::narrow(const char_type* b, const char_type* e, char dfl,
                         char* out) const -> const char_type* {
  return do_narrow(b, e, dfl, out);
}

ctype<char>::~ctype() noexcept {}

namespace {

ctype_base::mask ascii_classify(char c) noexcept {
  ctype_base::mask rv = 0;

  if (c >= 'A' && c <= 'Z') rv |= (ctype_base::upper | ctype_base::alpha);
  if (c >= 'a' && c <= 'z') rv |= (ctype_base::lower | ctype_base::alpha);
  if (c >= '0' && c <= '9') rv |= (ctype_base::digit | ctype_base::xdigit);
  if ((c >= 'A' && c <= 'F') ||
      (c >= 'a' && c <= 'f'))
    rv |= ctype_base::xdigit;

  switch (c) {
  case ' ':   // space
  case '\t':  // tab
    rv |= ctype_base::blank;
    /* FALLTHROUGH */
  case '\f':  // form-feed
  case '\n':  // new-line
  case '\r':  // carriage return
  case '\v':  // vertical tab
    rv |= ctype_base::space;
  }

  // NUL - US, DEL
  if ((c >= 0x00 && c <= 0x1f) || c == 0x7f)
    rv |= ctype_base::cntrl;
  else if (c >= 0x00 && c <= 0x7f)
    rv |= ctype_base::print;

  if (c != ' ' && (rv & ctype_base::print)) rv |= ctype_base::graph;

  if ((rv & (ctype_base::graph | ctype_base::alnum)) == ctype_base::graph)
    rv |= ctype_base::punct;

  return rv;
}

} /* namespace std::<unnamed> */

auto ctype<char>::do_is(mask m, char_type c) const -> bool {
  return ascii_classify(c) & m;
}

auto ctype<char>::do_is(const char_type* b, const char_type* e, mask* vec)
    const -> const char_type* {
  std::transform(b, e, vec, &ascii_classify);
  return e;
}

auto ctype<char>::do_scan_is(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return std::find_if(b, e,
                      [m, this](char_type c) -> bool {
                        return this->is(m, c);
                      });
}

auto ctype<char>::do_scan_not(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return std::find_if_not(b, e,
                          [m, this](char_type c) -> bool {
                            return this->is(m, c);
                          });
}

auto ctype<char>::do_toupper(char_type c) const -> char_type {
  return (c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c);
}

auto ctype<char>::do_toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return std::transform(bb, e, b,
                        [this](char_type c) -> char_type {
                          return this->toupper(c);
                        });
}

auto ctype<char>::do_tolower(char_type c) const -> char_type {
  return (c >= 'A' && c <= 'A' ? c - 'A' + 'a' : c);
}

auto ctype<char>::do_tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return std::transform(bb, e, b,
                        [this](char_type c) -> char_type {
                          return this->tolower(c);
                        });
}

auto ctype<char>::do_widen(char c) const -> char_type {
  return c;
}

auto ctype<char>::do_widen(const char* b, const char* e, char_type* out)
    const -> const char* {
  std::transform(b, e, out,
                 [this](char c) -> char_type {
                   return this->widen(c);
                 });
  return e;
}

auto ctype<char>::do_narrow(char_type c, char) const -> char {
  return c;
}

auto ctype<char>::do_narrow(const char_type* b, const char_type* e,
                            char dfl, char* out) const -> const char_type* {
  std::transform(b, e, out,
                 [this, dfl](char c) -> char_type {
                   return this->narrow(c, dfl);
                 });
  return e;
}


_namespace_end(std)
