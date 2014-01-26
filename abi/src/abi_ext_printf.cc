#include <abi/ext/printf.h>
#include <abi/panic.h>
#include <cerrno>
#include <cstdarg>
#include <limits>

namespace __cxxabiv1 {
namespace ext {
namespace {


enum printf_type {
  PFT_CHAR,
  PFT_WINT_T,
  PFT_UCHAR,
  PFT_SCHAR,
  PFT_SHRT,
  PFT_USHRT,
  PFT_INT,
  PFT_UINT,
  PFT_LONG,
  PFT_ULONG,
  PFT_LLONG,
  PFT_ULLONG,
  PFT_SIZE_T,
  PFT_SSIZE_T,
  PFT_PTRDIFF_T,
  PFT_UPTRDIFF_T,
  PFT_INTMAX_T,
  PFT_UINTMAX_T,
  PFT_FLT,
  PFT_DBL,
  PFT_LDBL,
  PFT_STRING,
  PFT_WSTRING,
  PFT_PTR = 0x80,
  PFT_SCHAR_PTR,
  PFT_SHRT_PTR,
  PFT_INT_PTR,
  PFT_LONG_PTR,
  PFT_LLONG_PTR,
  PFT_SIZE_T_PTR,
  PFT_PTRDIFF_T_PTR,
  PFT_INTMAX_T_PTR,
  PFT_INVAL = 0xff
};

enum printf_len {
  PFL_hh,
  PFL_h,
  PFL_l,
  PFL_ll,
  PFL_j,
  PFL_z,
  PFL_t,
  PFL_L,
  PFL_NONE
};

using printf_flags = unsigned char;
constexpr printf_flags PFF_THOUSANDS_GROUPING = 0x01;  // '\''
constexpr printf_flags PFF_LEFT_JUSTIFY = 0x02;  // '-'
constexpr printf_flags PFF_SIGN = 0x04;  // '+'
constexpr printf_flags PFF_SIGN_SPACE = 0x08;  // ' '  (ignored if sign is
                                               // printed)
constexpr printf_flags PFF_ALT_FORM = 0x10;  // '#'  (alternative form output)
constexpr printf_flags PFF_ZERO_PADDING = 0x20;  // '0'  (pad with zeroes,
                                                 // instead of spaces)
constexpr printf_flags PFF_FIELDWIDTH_IS_ARGIDX = 0x40;  // Fieldwidth is
                                                         // specified in int
                                                         // argument
constexpr printf_flags PFF_PRECISION_IS_ARGIDX = 0x80;  // Precision is
                                                        // specified in int
                                                        // argument

/* Describe how to render a specific value. */
struct printf_spec {
  int fieldwidth = -1;
  int precision = -1;
  short argidx = -1;
  printf_flags pff = 0;
  unsigned char base : 7;
  bool uppercase : 1;
};

class printf_arg {
 public:
  int apply(printf_renderer&, printf_spec) const noexcept;
  int read(va_list&) noexcept;
  int assign_type(printf_type) noexcept;
  int as_int(int*) const noexcept;

 private:
  union data_t {
    char d_char;
    wint_t d_wint_t;
    unsigned char d_uchar;
    signed char d_schar;
    short d_shrt;
    unsigned short d_ushrt;
    int d_int;
    unsigned int d_uint;
    long d_long;
    unsigned long d_ulong;
    long long d_llong;
    unsigned long long d_ullong;
    size_t d_size_t;
    ssize_t d_ssize_t;
    ptrdiff_t d_ptrdiff_t;
    uptrdiff_t d_uptrdiff_t;
    intmax_t d_intmax_t;
    uintmax_t d_uintmax_t;
    float d_flt;
    double d_dbl;
    long double d_ldbl;
    const char* d_string;
    const wchar_t* d_wstring;
    const void* d_ptr;
    signed char* d_schar_ptr;
    short* d_shrt_ptr;
    int* d_int_ptr;
    long* d_long_ptr;
    long long* d_llong_ptr;
    size_t* d_size_t_ptr;
    ptrdiff_t* d_ptrdiff_t_ptr;
    intmax_t* d_intmax_t_ptr;
  };

  data_t data_;
  printf_type pft_ = PFT_INVAL;
};


constexpr unsigned int MAX_BASE = 36;
const char DIGITS[MAX_BASE] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z' };
const char DIGITS_U[MAX_BASE] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
                                  'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                  'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                                  'W', 'X', 'Y', 'Z' };


int parse_num(c_string_piece& sp) noexcept {
  int rv;

  while (!sp.empty() && *sp >= '0' && *sp <= '9')
    (rv *= 10) += (*sp++ - '0');
  return rv;
}

int parse_num_spec(c_string_piece& sp, int* v, bool* is_argidx,
                   bool* is_argfield) noexcept {
  if (sp.empty()) return EINVAL;
  if (!v || !is_argidx || !is_argfield) return EINVAL;
  *is_argidx = false;
  *is_argfield = false;

  if (*sp == '*') {
    *is_argfield = true;
    ++sp;
  }

  if (_predict_false(sp.empty() || *sp < '0' || *sp > '9')) {
    if (is_argfield) {
      *v = -1;  // auto-select next arg
      return 0;
    }
    return EINVAL;
  }
  *v = parse_num(sp);

  if (sp.empty()) return (is_argfield ? EINVAL : 0);
  if (*sp == '$') {
    *is_argidx = !*is_argfield;
    ++sp;
  }

  return 0;
}

int render_spaces(printf_renderer& renderer, int n) noexcept {
  c_string_piece spaces = "    ";

  int error = 0;
  while (n > 0 && !error) {
    error = renderer.append(spaces.substr(0, n));
    n -= spaces.size();
  }
  return error;
}

int render_partial_num_sp(printf_renderer& renderer, int pos,
                          c_string_piece content, c_string_piece thousand_sep)
    noexcept {
  if (thousand_sep.empty()) return renderer.append(content);

  while (pos > 0) {
    int error;
    int n = (pos - 1) % 3 + 1;  // Number between 1 and 3.

    /* Render n numbers, until the first thousand separator. */
    if ((error = renderer.append(content.substr(0, n)))) return error;
    /* Append thousand separator, unless the number ends here. */
    if (pos != n && (error = renderer.append(thousand_sep))) return error;

    pos -= n;
    content += n;
  }
  return 0;
}

int render_leading_zeroes(printf_renderer& renderer, int add_leading_zeroes,
                          int numsz, c_string_piece thousand_sep) noexcept {
  c_string_piece zeroes = "0000";
  int pos = add_leading_zeroes + numsz;

  while (add_leading_zeroes > 0) {
    int error;
    if ((error = render_partial_num_sp(renderer, pos, zeroes, thousand_sep)))
      return error;
    pos -= zeroes.size();
    add_leading_zeroes -= zeroes.size();
  }
  return 0;
}

/*
 * Type-agnostic part of rendering an int.
 */
int render_int_agnostic(printf_renderer& renderer,
                        bool negative, bool zero, c_string_piece number,
                        printf_spec spec) noexcept {
  int add_leading_zeroes = 0;

  /* Figure out how to render thousands-separator, if requested. */
  char THOUSAND_SEP = '\0';
  c_string_piece thousand_sep;
  if (spec.pff & PFF_THOUSANDS_GROUPING) {
    THOUSAND_SEP = renderer.get_thousand_sep();
    thousand_sep = c_string_piece(&THOUSAND_SEP, 1U);
  }

  /* Figure out which sign to print. */
  c_string_piece sign;
  if (negative)
    sign = "-";
  else if (spec.pff & PFF_SIGN)
    sign = "+";
  else if (spec.pff & PFF_SIGN_SPACE)
    sign = " ";

  /*
   * Alternate form.  Note that base-8 numbers are corrected by
   * increasing precision to force a leading zero.
   * Note that the spec says 0 is never prefixed.
   */
  c_string_piece prefix;
  if (!zero && (spec.pff & PFF_ALT_FORM)) {
    if (spec.base == 8)
      add_leading_zeroes = 1;
    else if (spec.base == 16)
      prefix = (spec.uppercase ? "0X" : "0x");
  }

  /* Figure out how many zeroes to prepend, to reach precision. */
  if (number.size() < spec.precision)
    add_leading_zeroes = spec.precision - number.size();

  /*
   * Print c_string_pieces so far:
   * - sign
   * - prefix
   * - '0' (repeated add_leading_zeroes times)
   * - number
   * - thousand-sep: (number.size() + add_leading_zeroes) / 3
   */
  int len = sign.length() + prefix.length() + add_leading_zeroes +
            number.size() +
            thousand_sep.size() * ((number.size() + add_leading_zeroes) / 3);

  /* Number of bytes of padding to add. */
  int pad_len = (len < spec.fieldwidth ? spec.fieldwidth - len : 0);

  /*
   * If zero-padding, convert pad-len to zeroes
   * (but keep in mind the thousand separator).
   */
  if (spec.pff & PFF_ZERO_PADDING) {
    for (;;) {
      const int delta = 1 + ((number.size() + add_leading_zeroes) % 3 == 0 ?
                             thousand_sep.size() : 0);
      if (pad_len < delta) break;  // GUARD
      pad_len -= delta;
      len += delta;
      ++add_leading_zeroes;
    }
  }

  /*
   * Conversion complete, we are (finally) ready to print this thing.
   */
  int error;

  /* Print spaces to the left, unless left justifying output. */
  if (!(spec.pff & PFF_LEFT_JUSTIFY) &&
      (error = render_spaces(renderer, pad_len))) return error;

  /* Print sign. */
  if ((error = renderer.append(sign))) return error;
  /* Print prefix. */
  if ((error = renderer.append(prefix))) return error;
  /* Print leading zeroes. */
  if ((error = render_leading_zeroes(renderer, add_leading_zeroes,
                                    number.size(), thousand_sep)))
    return error;
  /* Print actual number. */
  if ((error = render_partial_num_sp(renderer, number.size(), number,
                                    thousand_sep))) return error;

  /* Print spaces to the left, iff left justifying output. */
  if ((spec.pff & PFF_LEFT_JUSTIFY) &&
      (error = render_spaces(renderer, pad_len))) return error;

  return 0;
}


template<typename T> struct render_int {
  using type = T;
  static constexpr type ZERO = T(0);
  static constexpr type ONE = T(1);
  static constexpr int N = std::numeric_limits<T>::digits10;

  int operator()(printf_renderer& renderer, type v, printf_spec spec) noexcept;
};

template<typename T> int render_int<T>::operator()(printf_renderer& renderer,
                                                   type v, printf_spec spec)
    noexcept {
  /* Deduce base. */
  if (spec.base <= 0 || spec.base > MAX_BASE) return EINVAL;
  const type BASE = spec.base;

  /* Select digits to use. */
  const auto& digits = (spec.uppercase ? DIGITS_U : DIGITS);

  /* Save negation sign prior to destructively iterating v. */
  const bool negative = (v < ZERO);
  const bool zero = (v == ZERO);

  /* Render digits of v, destroying v in the process. */
  char buf[N];
  int i = N;
  do {
    T m = v % BASE;
    if (m > 0 && v < ZERO) {
      m -= BASE;
      v += BASE;
    }
    v /= BASE;

    if (m < ZERO) m = -m;  // Convert modulo to array index.
    if (i == 0) panic("abi::ext::vxprintf: render_int() -- "
                      "running too many digits.");
    buf[--i] = digits[m];
  } while (v != ZERO);
  c_string_piece number = c_string_piece(&buf[i], N - i);

  return render_int_agnostic(renderer, negative, zero, number, spec);
}


/* Render for type.  Default implementation panics. */
template<printf_type> struct render {
  template<typename T> int operator()(printf_renderer&, T, printf_spec) const
      noexcept;
};

/* Specializations for int. */
template<> struct render<PFT_UCHAR>      : render_int<unsigned char> {};
template<> struct render<PFT_SCHAR>      : render_int<signed char> {};
template<> struct render<PFT_SHRT>       : render_int<short> {};
template<> struct render<PFT_USHRT>      : render_int<unsigned short> {};
template<> struct render<PFT_INT>        : render_int<int> {};
template<> struct render<PFT_UINT>       : render_int<unsigned int> {};
template<> struct render<PFT_LONG>       : render_int<long> {};
template<> struct render<PFT_ULONG>      : render_int<unsigned long> {};
template<> struct render<PFT_LLONG>      : render_int<long long> {};
template<> struct render<PFT_ULLONG>     : render_int<unsigned long long> {};
template<> struct render<PFT_SIZE_T>     : render_int<size_t> {};
template<> struct render<PFT_SSIZE_T>    : render_int<ssize_t> {};
template<> struct render<PFT_PTRDIFF_T>  : render_int<ptrdiff_t> {};
template<> struct render<PFT_UPTRDIFF_T> : render_int<uptrdiff_t> {};
template<> struct render<PFT_INTMAX_T>   : render_int<intmax_t> {};
template<> struct render<PFT_UINTMAX_T>  : render_int<uintmax_t> {};

/* Char/wint_t. */
template<> struct render<PFT_CHAR> {
  using type = char;

  int operator()(printf_renderer&, char, printf_spec) const noexcept;
};
#if 0 // XXX implement
template<> struct render<PFT_WINT_T> {
  using type = wint_t;
};
#endif

/* Floating point types. */
#if 0 // XXX implement
template<> struct render<PFT_FLT> { using type = float; };
template<> struct render<PFT_DBL> { using type = double; };
template<> struct render<PFT_LDBL> { using type = long double; };
#endif

/* Pointer. */
template<> struct render<PFT_PTR> {
  int operator()(printf_renderer&, const void*, printf_spec) const noexcept;
};

/* Strings. */
template<> struct render<PFT_STRING> {
  int operator()(printf_renderer&, const char*, printf_spec) const noexcept;
};
#if 0 // XXX implement
template<> struct render<PFT_WSTRING> { using type = const wchar_t*; };
#endif


template<printf_type Pft>
template<typename T>
int render<Pft>::operator()(printf_renderer&, T, printf_spec) const noexcept {
  panic("UNIMPLEMENTED printf TYPE");
  return ENOSYS;
}

int render<PFT_CHAR>::operator()(printf_renderer& renderer, char c,
                                 printf_spec spec) const noexcept {
  int pad_len = (spec.fieldwidth > 1 ? spec.fieldwidth - 1 : 0);
  int error;

  /* Print spaces to the left, unless left justifying output. */
  if (!(spec.pff & PFF_LEFT_JUSTIFY) &&
      (error = render_spaces(renderer, pad_len))) return error;

  /* Print character. */
  if ((error = renderer.append(c_string_piece(&c, 1)))) return error;

  /* Print spaces to the left, iff left justifying output. */
  if ((spec.pff & PFF_LEFT_JUSTIFY) &&
      (error = render_spaces(renderer, pad_len))) return error;

  return 0;
}

int render<PFT_PTR>::operator()(printf_renderer& renderer, const void* p,
                                printf_spec spec) const noexcept {
  /*
   * Treat pointer as "%#x" for uintptr_t.
   * Spec allows us to do anything we want, but this is quite common across
   * unix.
   */
  spec.base = 16;
  spec.pff |= PFF_ALT_FORM;
  spec.uppercase = false;
  return render<PFT_UINTMAX_T>()(renderer, reinterpret_cast<uintptr_t>(p),
                                 spec);
}

int render<PFT_STRING>::operator()(printf_renderer& renderer, const char* s,
                                   printf_spec spec) const noexcept {
  c_string_piece v;
  if (spec.precision >= 0) {
    /*
     * The standard seems to suggest that, if precision is specified,
     * the string may not contain a nul byte if it is longer than the
     * precision.  Therefore, limit searching for a nul byte to within
     * the precision length.
     */
    v = c_string_piece(s, spec.precision);
    auto v_end = v + (v.find('\0') - v);
    if (!v_end.empty()) v = v.substr(0, v_end - v);
  } else {
    v = c_string_piece(s);
  }

  int pad_len = (v.size() < spec.fieldwidth ? spec.fieldwidth - v.size() : 0);
  int error;

  /* Print spaces to the left, unless left justifying output. */
  if (!(spec.pff & PFF_LEFT_JUSTIFY) &&
      (error = render_spaces(renderer, pad_len))) return error;

  /* Print character. */
  if ((error = renderer.append(v))) return error;

  /* Print spaces to the left, iff left justifying output. */
  if ((spec.pff & PFF_LEFT_JUSTIFY) &&
      (error = render_spaces(renderer, pad_len))) return error;

  return 0;
}

/*
 * Parse the length specifier from a printf format specifier,
 * moving the c_string_piece argument the appropriate number
 * of positions forward.
 */
printf_len deduce_printf_len(c_string_piece& sp) noexcept {
  if (sp.empty()) return PFL_NONE;

  if (sp.size() >= 2) {
    if (sp == c_string_piece("hh")) {
      sp += 2;
      return PFL_hh;
    }
    if (sp == c_string_piece("ll")) {
      sp += 2;
      return PFL_ll;
    }
  }

  switch (*sp) {
  default:
    return PFL_NONE;
  case 'h':
    return PFL_h;
  case 'l':
    return PFL_l;
  case 'j':
    return PFL_j;
  case 'z':
    return PFL_z;
  case 't':
    return PFL_t;
  case 'L':
    return PFL_L;
  }
}

/*
 * Given a conversion specifier and length specifier,
 * deduce the argument type.
 *
 * Return PFT_INVAL if the conversion specifier is not recognized.
 * Return PFT_INVAL if the combination of conversion specifier and
 * length specifier is not recognized.
 */
printf_type deduce_printf_type(char conversion_specifier,
                               printf_len pf_len) noexcept {
  switch (conversion_specifier) {
  case 'd':
  case 'i':
    switch (pf_len) {
    case PFL_hh:
      return PFT_SCHAR;
    case PFL_h:
      return PFT_SHRT;
    case PFL_l:
      return PFT_LONG;
    case PFL_ll:
      return PFT_LLONG;
    case PFL_j:
      return PFT_INTMAX_T;
    case PFL_z:
      return PFT_SSIZE_T;
    case PFL_t:
      return PFT_PTRDIFF_T;
    case PFL_NONE:
      return PFT_INT;
    case PFL_L:
      /* IGNORE */
      break;
    }
    break;
  case 'o':
  case 'u':
  case 'x':
  case 'X':
    switch (pf_len) {
    case PFL_hh:
      return PFT_UCHAR;
    case PFL_h:
      return PFT_USHRT;
    case PFL_l:
      return PFT_ULONG;
    case PFL_ll:
      return PFT_ULLONG;
    case PFL_j:
      return PFT_UINTMAX_T;
    case PFL_z:
      return PFT_SIZE_T;
    case PFL_t:
      return PFT_UPTRDIFF_T;
    case PFL_NONE:
      return PFT_UINT;
    case PFL_L:
      /* IGNORE */
      break;
    }
    break;
  case 'n':
    switch (pf_len) {
    case PFL_hh:
      return PFT_SCHAR_PTR;
    case PFL_h:
      return PFT_SHRT_PTR;
    case PFL_l:
      return PFT_LONG_PTR;
    case PFL_ll:
      return PFT_LLONG_PTR;
    case PFL_j:
      return PFT_INTMAX_T_PTR;
    case PFL_z:
      return PFT_SIZE_T_PTR;
    case PFL_t:
      return PFT_PTRDIFF_T_PTR;
    case PFL_NONE:
      return PFT_INT_PTR;
    case PFL_L:
      /* IGNORE */
      break;
    }
    break;
  case 'a':
  case 'A':
  case 'e':
  case 'E':
  case 'f':
  case 'F':
  case 'g':
  case 'G':
    switch (pf_len) {
    case PFL_L:
      return PFT_LDBL;
    case PFL_NONE:
      return PFT_DBL;
    case PFL_hh:
    case PFL_h:
    case PFL_l:
    case PFL_ll:
    case PFL_j:
    case PFL_z:
    case PFL_t:
      /* IGNORE */
      break;
    }
  case 'c':
    switch (pf_len) {
    case PFL_l:
      return PFT_WINT_T;
    case PFL_NONE:
      return PFT_CHAR;
    case PFL_hh:
    case PFL_h:
    case PFL_ll:
    case PFL_j:
    case PFL_z:
    case PFL_t:
    case PFL_L:
      /* IGNORE */
      break;
    }
    break;
  case 's':
    switch (pf_len) {
    case PFL_l:
      return PFT_WSTRING;
    case PFL_NONE:
      return PFT_STRING;
    case PFL_hh:
    case PFL_h:
    case PFL_ll:
    case PFL_j:
    case PFL_z:
    case PFL_t:
    case PFL_L:
      /* IGNORE */
      break;
    }
    break;
  case 'C':
    switch (pf_len) {
    case PFL_NONE:
      return PFT_WINT_T;
    case PFL_hh:
    case PFL_h:
    case PFL_l:
    case PFL_ll:
    case PFL_j:
    case PFL_z:
    case PFL_t:
    case PFL_L:
      /* IGNORE */
      break;
    }
    break;
  case 'S':
    switch (pf_len) {
    case PFL_NONE:
      return PFT_WSTRING;
    case PFL_hh:
    case PFL_h:
    case PFL_l:
    case PFL_ll:
    case PFL_j:
    case PFL_z:
    case PFL_t:
    case PFL_L:
      /* IGNORE */
      break;
    }
    break;
  case 'p':
    switch (pf_len) {
    case PFL_NONE:
      return PFT_PTR;
    case PFL_hh:
    case PFL_h:
    case PFL_l:
    case PFL_ll:
    case PFL_j:
    case PFL_z:
    case PFL_t:
    case PFL_L:
      /* IGNORE */
      break;
    }
    break;
  }

  return PFT_INVAL;
}

/*
 * Given a conversion specifier, give the base that a number should
 * be printed in.
 * Returns 0 if the conversion specifier does not represent a number.
 */
unsigned char deduce_printf_base(char c) noexcept {
  switch (c) {
  case 'd':
  case 'i':
  case 'u':
    return 10;
  case 'o':
    return 8;
  case 'x':
  case 'X':
    return 16;
  default:
    return 0;
  }
}


int printf_arg::apply(printf_renderer& renderer, printf_spec spec) const
    noexcept {
  int error = 0;

  switch (pft_) {
  case PFT_CHAR:
    error = render<PFT_CHAR>()(renderer, data_.d_char, spec);
    break;
  case PFT_WINT_T:
    error = render<PFT_WINT_T>()(renderer, data_.d_wint_t, spec);
    break;
  case PFT_UCHAR:
    error = render<PFT_UCHAR>()(renderer, data_.d_uchar, spec);
    break;
  case PFT_SCHAR:
    error = render<PFT_SCHAR>()(renderer, data_.d_schar, spec);
    break;
  case PFT_SHRT:
    error = render<PFT_SHRT>()(renderer, data_.d_shrt, spec);
    break;
  case PFT_USHRT:
    error = render<PFT_USHRT>()(renderer, data_.d_ushrt, spec);
    break;
  case PFT_INT:
    error = render<PFT_INT>()(renderer, data_.d_int, spec);
    break;
  case PFT_UINT:
    error = render<PFT_UINT>()(renderer, data_.d_uint, spec);
    break;
  case PFT_LONG:
    error = render<PFT_LONG>()(renderer, data_.d_long, spec);
    break;
  case PFT_ULONG:
    error = render<PFT_ULONG>()(renderer, data_.d_ulong, spec);
    break;
  case PFT_LLONG:
    error = render<PFT_LLONG>()(renderer, data_.d_llong, spec);
    break;
  case PFT_ULLONG:
    error = render<PFT_ULLONG>()(renderer, data_.d_ullong, spec);
    break;
  case PFT_SIZE_T:
    error = render<PFT_SIZE_T>()(renderer, data_.d_size_t, spec);
    break;
  case PFT_SSIZE_T:
    error = render<PFT_SSIZE_T>()(renderer, data_.d_ssize_t, spec);
    break;
  case PFT_PTRDIFF_T:
    error = render<PFT_PTRDIFF_T>()(renderer, data_.d_ptrdiff_t, spec);
    break;
  case PFT_UPTRDIFF_T:
    error = render<PFT_UPTRDIFF_T>()(renderer, data_.d_uptrdiff_t, spec);
    break;
  case PFT_INTMAX_T:
    error = render<PFT_INTMAX_T>()(renderer, data_.d_intmax_t, spec);
    break;
  case PFT_UINTMAX_T:
    error = render<PFT_UINTMAX_T>()(renderer, data_.d_uintmax_t, spec);
    break;
  case PFT_FLT:
    error = render<PFT_FLT>()(renderer, data_.d_flt, spec);
    break;
  case PFT_DBL:
    error = render<PFT_DBL>()(renderer, data_.d_dbl, spec);
    break;
  case PFT_LDBL:
    error = render<PFT_LDBL>()(renderer, data_.d_ldbl, spec);
    break;
  case PFT_STRING:
    error = render<PFT_STRING>()(renderer, data_.d_string, spec);
    break;
  case PFT_WSTRING:
    error = render<PFT_WSTRING>()(renderer, data_.d_wstring, spec);
    break;
  case PFT_PTR:
    error = render<PFT_PTR>()(renderer, data_.d_ptr, spec);
    break;
  case PFT_SCHAR_PTR:
    if (data_.d_schar_ptr == nullptr)
      error = EINVAL;
    else
      *data_.d_schar_ptr = renderer.length();
    break;
  case PFT_SHRT_PTR:
    if (data_.d_shrt_ptr == nullptr)
      error = EINVAL;
    else
      *data_.d_shrt_ptr = renderer.length();
    break;
  case PFT_INT_PTR:
    if (data_.d_int_ptr == nullptr)
      error = EINVAL;
    else
      *data_.d_int_ptr = renderer.length();
    break;
  case PFT_LONG_PTR:
    if (data_.d_long_ptr == nullptr)
      error = EINVAL;
    else
      *data_.d_long_ptr = renderer.length();
    break;
  case PFT_LLONG_PTR:
    if (data_.d_llong_ptr == nullptr)
      error = EINVAL;
    else
      *data_.d_llong_ptr = renderer.length();
    break;
  case PFT_SIZE_T_PTR:
    if (data_.d_size_t_ptr == nullptr)
      error = EINVAL;
    else
      *data_.d_size_t_ptr = renderer.length();
    break;
  case PFT_PTRDIFF_T_PTR:
    if (data_.d_ptrdiff_t_ptr == nullptr)
      error = EINVAL;
    else
      *data_.d_ptrdiff_t_ptr = renderer.length();
    break;
  case PFT_INTMAX_T_PTR:
    if (data_.d_intmax_t_ptr == nullptr)
      error = EINVAL;
    else
      *data_.d_intmax_t_ptr = renderer.length();
    break;
  case PFT_INVAL:
    error = EINVAL;
    break;
  }
  return error;
}

int printf_arg::read(va_list& ap) noexcept {
  int error = 0;

  switch (pft_) {
  case PFT_CHAR:
    data_.d_char = va_arg(ap, int);
    break;
  case PFT_WINT_T:
    data_.d_wint_t = va_arg(ap, wint_t);
    break;
  case PFT_UCHAR:
    data_.d_uchar = va_arg(ap, unsigned int);
    break;
  case PFT_SCHAR:
    data_.d_schar = va_arg(ap, int);
    break;
  case PFT_SHRT:
    data_.d_shrt = va_arg(ap, int);
    break;
  case PFT_USHRT:
    data_.d_ushrt = va_arg(ap, unsigned int);
    break;
  case PFT_INT:
    data_.d_int = va_arg(ap, int);
    break;
  case PFT_UINT:
    data_.d_uint = va_arg(ap, unsigned int);
    break;
  case PFT_LONG:
    data_.d_long = va_arg(ap, long);
    break;
  case PFT_ULONG:
    data_.d_ulong = va_arg(ap, unsigned long);
    break;
  case PFT_LLONG:
    data_.d_llong = va_arg(ap, long long);
    break;
  case PFT_ULLONG:
    data_.d_ullong = va_arg(ap, unsigned long long);
    break;
  case PFT_SIZE_T:
    data_.d_size_t = va_arg(ap, size_t);
    break;
  case PFT_SSIZE_T:
    data_.d_ssize_t = va_arg(ap, ssize_t);
    break;
  case PFT_PTRDIFF_T:
    data_.d_ptrdiff_t = va_arg(ap, ptrdiff_t);
    break;
  case PFT_UPTRDIFF_T:
    data_.d_uptrdiff_t = va_arg(ap, uptrdiff_t);
    break;
  case PFT_INTMAX_T:
    data_.d_intmax_t = va_arg(ap, intmax_t);
    break;
  case PFT_UINTMAX_T:
    data_.d_uintmax_t = va_arg(ap, uintmax_t);
    break;
  case PFT_FLT:
    data_.d_flt = va_arg(ap, double);
    break;
  case PFT_DBL:
    data_.d_dbl = va_arg(ap, double);
    break;
  case PFT_LDBL:
    data_.d_ldbl = va_arg(ap, long double);
    break;
  case PFT_STRING:
    data_.d_string = va_arg(ap, const char*);
    break;
  case PFT_WSTRING:
    data_.d_wstring = va_arg(ap, const wchar_t*);
    break;
  case PFT_PTR:
    data_.d_ptr = va_arg(ap, const void*);
    break;
  case PFT_SCHAR_PTR:
    data_.d_schar_ptr = va_arg(ap, signed char*);
    break;
  case PFT_SHRT_PTR:
    data_.d_shrt_ptr = va_arg(ap, short*);
    break;
  case PFT_INT_PTR:
    data_.d_int_ptr = va_arg(ap, int*);
    break;
  case PFT_LONG_PTR:
    data_.d_long_ptr = va_arg(ap, long*);
    break;
  case PFT_LLONG_PTR:
    data_.d_llong_ptr = va_arg(ap, long long*);
    break;
  case PFT_SIZE_T_PTR:
    data_.d_size_t_ptr = va_arg(ap, size_t*);
    break;
  case PFT_PTRDIFF_T_PTR:
    data_.d_ptrdiff_t_ptr = va_arg(ap, ptrdiff_t*);
    break;
  case PFT_INTMAX_T_PTR:
    data_.d_intmax_t_ptr = va_arg(ap, intmax_t*);
    break;
  case PFT_INVAL:
    error = EINVAL;
    break;
  }
  return error;
}

int printf_arg::as_int(int* v) const noexcept {
  if (pft_ != PFT_INT) return EINVAL;
  *v = data_.d_int;
  return 0;
}

int printf_arg::assign_type(printf_type pft) noexcept {
  if (pft_ != PFT_INVAL && pft_ != pft) return EINVAL;
  pft_ = pft;
  return 0;
}


int deduce_printf_spec(c_string_piece& sp, printf_spec* spec,
                       printf_type* type) noexcept {
  if (_predict_false(!spec || !type)) return EINVAL;

  bool argidx_seen = false;
  bool flag_seen = false;
  bool fieldwidth_seen = false;
  bool precision_seen = false;
  bool stop_stage1 = false;

  while (!stop_stage1) {
    int n;
    bool is_argidx;
    bool is_argfield;
    printf_flags fl_mod = 0;
    int error = 0;

    if (sp.empty()) return EINVAL;
    switch (*sp) {
    case '.':
      if (precision_seen) return EINVAL;
      precision_seen = true;
      error = parse_num_spec(++sp, &n, &is_argidx, &is_argfield);
      if (is_argidx) return EINVAL;
      spec->precision = n;
      if (is_argfield) spec->pff |= PFF_PRECISION_IS_ARGIDX;
      break;
    case '*':
      if (fieldwidth_seen) return EINVAL;
      if (precision_seen) return EINVAL;
      fieldwidth_seen = true;
      error = parse_num_spec(sp, &n, &is_argidx, &is_argfield);
      if (!is_argfield) return EINVAL;
      spec->fieldwidth = n;
      spec->pff |= PFF_FIELDWIDTH_IS_ARGIDX;
      break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      error = parse_num_spec(sp, &n, &is_argidx, &is_argfield);
      if (is_argidx) {
        if (argidx_seen) return EINVAL;
        if (flag_seen || fieldwidth_seen || precision_seen) return EINVAL;
        argidx_seen = true;
        spec->argidx = n;
      } else {
        if (fieldwidth_seen) return EINVAL;
        if (precision_seen) return EINVAL;
        fieldwidth_seen = true;
        spec->fieldwidth = n;
        if (is_argfield) spec->pff |= PFF_FIELDWIDTH_IS_ARGIDX;
      }
      break;
    case '\'':
      fl_mod = PFF_THOUSANDS_GROUPING;
      break;
    case '-':
      fl_mod = PFF_LEFT_JUSTIFY;
      break;
    case '+':
      fl_mod = PFF_SIGN;
      break;
    case ' ':
      fl_mod = PFF_SIGN_SPACE;
      break;
    case '#':
      fl_mod = PFF_ALT_FORM;
      break;
    case '0':
      fl_mod = PFF_ZERO_PADDING;
      break;
    default:
      stop_stage1 = true;
    }

    if (error) return error;

    /* Apply flag modification. */
    if (fl_mod) {
      if (spec->pff & fl_mod) return EINVAL;
      spec->pff |= fl_mod;
      if (fieldwidth_seen || precision_seen) return EINVAL;
      flag_seen = true;
      ++sp;
    }
  }

  /* Figure out the argument type. */
  printf_len pfl = deduce_printf_len(sp);
  if (sp.empty()) return EINVAL;
  char conv = *sp++;
  if ((*type = deduce_printf_type(conv, pfl)) == PFT_INVAL) return EINVAL;
  spec->base = deduce_printf_base(conv);
  spec->uppercase = (conv == 'X');
  return 0;
}


} /* namespace __cxxabiv1::ext::<unnamed> */


printf_renderer::~printf_renderer() noexcept {}

int printf_renderer::append(c_string_piece sp) noexcept {
  if (sp.empty()) return 0;
  len_ += sp.length();
  return append(sp);
}

char printf_renderer::get_thousand_sep() const noexcept {
  return ' ';
}


int vxprintf(printf_renderer& renderer, c_string_piece fmt, va_list& ap)
    noexcept {
  int lit_count = 0;
  int argsize = 0;
  c_string_piece lit[NL_ARGMAX];
  c_string_piece tail;
  printf_arg va[NL_ARGMAX];
  printf_spec specs[NL_ARGMAX];

  /* Parse fmt, gathering types and collecting specs. */
  {
    c_string_piece p = fmt;

    while (!(p = (fmt = p).find('%')).empty()) {
      if (lit_count >= NL_ARGMAX) return EINVAL;
      lit[lit_count] = fmt.substr(0, p - fmt);
      printf_spec* spec = &specs[lit_count];

      if (*++p != '%') {
        printf_type type;
        int error = deduce_printf_spec(p, spec, &type);
        if (error) return error;

        /* Process fieldwidth from argument list. */
        if (spec->pff & PFF_FIELDWIDTH_IS_ARGIDX) {
          if (spec->fieldwidth == -1) spec->fieldwidth = argsize++;
          if (spec->fieldwidth >= NL_ARGMAX) return EINVAL;
          if (argsize <= spec->fieldwidth) argsize = spec->fieldwidth + 1U;
          if ((error = va[spec->fieldwidth].assign_type(PFT_INT)))
            return error;
        }

        /* Process precision from argument list. */
        if (spec->pff & PFF_PRECISION_IS_ARGIDX) {
          if (spec->precision == -1) spec->precision = argsize++;
          if (spec->precision >= NL_ARGMAX) return EINVAL;
          if (argsize <= spec->precision) argsize = spec->precision + 1U;
          if ((error = va[spec->precision].assign_type(PFT_INT)))
            return error;
        }

        /* Process the argument itself list. */
        if (spec->argidx == -1) spec->argidx = argsize++;
        if (argsize <= spec->argidx) argsize = spec->argidx + 1U;
        if (spec->argidx >= NL_ARGMAX) return EINVAL;

        /* Save the type information. */
        if ((error = va[spec->argidx].assign_type(type))) return error;
      }

      ++lit_count;
    }
    tail = fmt;
  }

  /* Read all va_list arguments. */
  {
    printf_arg*const b = &va[0];
    printf_arg*const e = b + argsize;
    for (printf_arg* i = b; i != e; ++i) {
      int error = i->read(ap);
      if (error) return error;
    }
  }

  /* Resolve all fieldwidth, precision. */
  {
    printf_spec*const b = &specs[0];
    printf_spec*const e = b + lit_count;
    for (printf_spec* i = b; i != e; ++i) {
      if (i->pff & PFF_FIELDWIDTH_IS_ARGIDX) {
        int v;
        int error = va[i->fieldwidth].as_int(&v);
        i->fieldwidth = v;
        if (error) return error;
      }

      if (i->pff & PFF_PRECISION_IS_ARGIDX) {
        int v;
        int error = va[i->precision].as_int(&v);
        i->precision = v;
        if (error) return error;
      }

      i->pff &= ~(PFF_FIELDWIDTH_IS_ARGIDX | PFF_PRECISION_IS_ARGIDX);
    }
  }

  /* Start rendering loop. */
  for (int i = 0; i < lit_count; ++i) {
    int error;
    /* Carbon copy literal data to renderer. */
    if ((error = renderer.append(lit[i]))) return error;
    /* Format argument and send to renderer. */
    if (specs[i].argidx != -1 &&
        (error = va[specs[i].argidx].apply(renderer, specs[i]))) return error;
  }

  /* Verbatim copy of tail in format string. */
  return renderer.append(tail);
}


}} /* namespace __cxxabiv1::ext */
