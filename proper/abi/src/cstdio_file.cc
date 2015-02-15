#include "cstdio_file.h"

_namespace_begin(std)


FILE::~FILE() noexcept {}

auto FILE::rdbuf() const noexcept -> streambuf& {
  return *get_ios().rdbuf();
}

auto FILE::get_istream() const -> istream& {
  throw system_error(make_error_code(errc::bad_file_descriptor));
}

auto FILE::get_ostream() const -> ostream& {
  throw system_error(make_error_code(errc::bad_file_descriptor));
}


_namespace_end(std)
