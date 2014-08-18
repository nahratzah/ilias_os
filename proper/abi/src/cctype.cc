#include <cctype>
#include <locale>
#include <cerrno>
#include <string>

_namespace_begin(std)


static_assert(sizeof(locale) == sizeof(locale_t));


int isalnum(int c) noexcept {
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isalnum(char_traits<char>::to_char_type(c), locale());
}

int isalnum_l(int c, locale_t loc) noexcept {
  if (loc == nullptr) {
    errno = _ABI_EINVAL;
    return 0;
  }
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isalnum(char_traits<char>::to_char_type(c),
                 reinterpret_cast<const locale&>(loc));
}

int isalpha(int c) noexcept {
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isalpha(char_traits<char>::to_char_type(c), locale());
}

int isalpha_l(int c, locale_t loc) noexcept {
  if (loc == nullptr) {
    errno = _ABI_EINVAL;
    return 0;
  }
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isalpha(char_traits<char>::to_char_type(c),
                 reinterpret_cast<const locale&>(loc));
}

int isblank(int c) noexcept {
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isalpha(char_traits<char>::to_char_type(c), locale());
}

int isblank_l(int c, locale_t loc) noexcept {
  if (loc == nullptr) {
    errno = _ABI_EINVAL;
    return 0;
  }
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isblank(char_traits<char>::to_char_type(c),
                 reinterpret_cast<const locale&>(loc));
}

int iscntrl(int c) noexcept {
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return iscntrl(char_traits<char>::to_char_type(c), locale());
}

int iscntrl_l(int c, locale_t loc) noexcept {
  if (loc == nullptr) {
    errno = _ABI_EINVAL;
    return 0;
  }
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return iscntrl(char_traits<char>::to_char_type(c),
                 reinterpret_cast<const locale&>(loc));
}

int isdigit(int c) noexcept {
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isdigit(char_traits<char>::to_char_type(c), locale());
}

int isdigit_l(int c, locale_t loc) noexcept {
  if (loc == nullptr) {
    errno = _ABI_EINVAL;
    return 0;
  }
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isdigit(char_traits<char>::to_char_type(c),
                 reinterpret_cast<const locale&>(loc));
}

int isgraph(int c) noexcept {
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isgraph(char_traits<char>::to_char_type(c), locale());
}

int isgraph_l(int c, locale_t loc) noexcept {
  if (loc == nullptr) {
    errno = _ABI_EINVAL;
    return 0;
  }
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isgraph(char_traits<char>::to_char_type(c),
                 reinterpret_cast<const locale&>(loc));
}

int islower(int c) noexcept {
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return islower(char_traits<char>::to_char_type(c), locale());
}

int islower_l(int c, locale_t loc) noexcept {
  if (loc == nullptr) {
    errno = _ABI_EINVAL;
    return 0;
  }
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return islower(char_traits<char>::to_char_type(c),
                 reinterpret_cast<const locale&>(loc));
}

int isprint(int c) noexcept {
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isprint(char_traits<char>::to_char_type(c), locale());
}

int isprint_l(int c, locale_t loc) noexcept {
  if (loc == nullptr) {
    errno = _ABI_EINVAL;
    return 0;
  }
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isprint(char_traits<char>::to_char_type(c),
                 reinterpret_cast<const locale&>(loc));
}

int ispunct(int c) noexcept {
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return ispunct(char_traits<char>::to_char_type(c), locale());
}

int ispunct_l(int c, locale_t loc) noexcept {
  if (loc == nullptr) {
    errno = _ABI_EINVAL;
    return 0;
  }
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return ispunct(char_traits<char>::to_char_type(c),
                 reinterpret_cast<const locale&>(loc));
}

int isspace(int c) noexcept {
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isspace(char_traits<char>::to_char_type(c), locale());
}

int isspace_l(int c, locale_t loc) noexcept {
  if (loc == nullptr) {
    errno = _ABI_EINVAL;
    return 0;
  }
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isspace(char_traits<char>::to_char_type(c),
                 reinterpret_cast<const locale&>(loc));
}

int isupper(int c) noexcept {
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isupper(char_traits<char>::to_char_type(c), locale());
}

int isupper_l(int c, locale_t loc) noexcept {
  if (loc == nullptr) {
    errno = _ABI_EINVAL;
    return 0;
  }
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isupper(char_traits<char>::to_char_type(c),
                 reinterpret_cast<const locale&>(loc));
}

int isxdigit(int c) noexcept {
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isxdigit(char_traits<char>::to_char_type(c), locale());
}

int isxdigit_l(int c, locale_t loc) noexcept {
  if (loc == nullptr) {
    errno = _ABI_EINVAL;
    return 0;
  }
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return isxdigit(char_traits<char>::to_char_type(c),
                  reinterpret_cast<const locale&>(loc));
}

int tolower(int c) noexcept {
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return tolower(char_traits<char>::to_char_type(c), locale());
}

int tolower_l(int c, locale_t loc) noexcept {
  if (loc == nullptr) {
    errno = _ABI_EINVAL;
    return 0;
  }
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return tolower(char_traits<char>::to_char_type(c),
                 reinterpret_cast<const locale&>(loc));
}

int toupper(int c) noexcept {
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return toupper(char_traits<char>::to_char_type(c), locale());
}

int toupper_l(int c, locale_t loc) noexcept {
  if (loc == nullptr) {
    errno = _ABI_EINVAL;
    return 0;
  }
  if (char_traits::eq_int_type(c, char_traits<char>::eof())) return 0;
  return toupper(char_traits<char>::to_char_type(c),
                 reinterpret_cast<const locale&>(loc));
}


_namespace_end(std)
