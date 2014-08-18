#include <locale_misc/numpunct.h>

_namespace_begin(std)


auto numpunct<char>::decimal_point() const -> char_type {
  return do_decimal_point();
}
auto numpunct<char16_t>::decimal_point() const -> char_type {
  return do_decimal_point();
}
auto numpunct<char32_t>::decimal_point() const -> char_type {
  return do_decimal_point();
}
auto numpunct<wchar_t>::decimal_point() const -> char_type {
  return do_decimal_point();
}

auto numpunct<char>::thousands_sep() const -> char_type {
  return do_thousands_sep();
}
auto numpunct<char16_t>::thousands_sep() const -> char_type {
  return do_thousands_sep();
}
auto numpunct<char32_t>::thousands_sep() const -> char_type {
  return do_thousands_sep();
}
auto numpunct<wchar_t>::thousands_sep() const -> char_type {
  return do_thousands_sep();
}

auto numpunct<char>::grouping() const -> string {
  return do_grouping();
}
auto numpunct<char16_t>::grouping() const -> string {
  return do_grouping();
}
auto numpunct<char32_t>::grouping() const -> string {
  return do_grouping();
}
auto numpunct<wchar_t>::grouping() const -> string {
  return do_grouping();
}

auto numpunct<char>::truename() const -> string_type {
  return do_truename();
}
auto numpunct<char16_t>::truename() const -> string_type {
  return do_truename();
}
auto numpunct<char32_t>::truename() const -> string_type {
  return do_truename();
}
auto numpunct<wchar_t>::truename() const -> string_type {
  return do_truename();
}

auto numpunct<char>::falsename() const -> string_type {
  return do_falsename();
}
auto numpunct<char16_t>::falsename() const -> string_type {
  return do_falsename();
}
auto numpunct<char32_t>::falsename() const -> string_type {
  return do_falsename();
}
auto numpunct<wchar_t>::falsename() const -> string_type {
  return do_falsename();
}

auto numpunct<char>::do_decimal_point() const -> char_type {
  return '.';
}
auto numpunct<char16_t>::do_decimal_point() const -> char_type {
  return '.';
}
auto numpunct<char32_t>::do_decimal_point() const -> char_type {
  return '.';
}
auto numpunct<wchar_t>::do_decimal_point() const -> char_type {
  return L'.';
}

auto numpunct<char>::do_thousands_sep() const -> char_type {
  return ',';
}
auto numpunct<char16_t>::do_thousands_sep() const -> char_type {
  return u',';
}
auto numpunct<char32_t>::do_thousands_sep() const -> char_type {
  return U',';
}
auto numpunct<wchar_t>::do_thousands_sep() const -> char_type {
  return L',';
}

auto numpunct<char>::do_grouping() const -> string {
  return string();
}
auto numpunct<char16_t>::do_grouping() const -> string {
  return string();
}
auto numpunct<char32_t>::do_grouping() const -> string {
  return string();
}
auto numpunct<wchar_t>::do_grouping() const -> string {
  return string();
}

auto numpunct<char>::do_truename() const -> string_type {
  return "true";
}
auto numpunct<char16_t>::do_truename() const -> string_type {
  return u"true";
}
auto numpunct<char32_t>::do_truename() const -> string_type {
  return U"true";
}
auto numpunct<wchar_t>::do_truename() const -> string_type {
  return L"true";
}

auto numpunct<char>::do_falsename() const -> string_type {
  return "false";
}
auto numpunct<char16_t>::do_falsename() const -> string_type {
  return u"false";
}
auto numpunct<char32_t>::do_falsename() const -> string_type {
  return U"false";
}
auto numpunct<wchar_t>::do_falsename() const -> string_type {
  return L"false";
}


constexpr locale::id numpunct<char>::id;
constexpr locale::id numpunct<char16_t>::id;
constexpr locale::id numpunct<char32_t>::id;
constexpr locale::id numpunct<wchar_t>::id;

template class numpunct_byname<char>;
template class numpunct_byname<char16_t>;
template class numpunct_byname<char32_t>;
template class numpunct_byname<wchar_t>;


_namespace_end(std)
