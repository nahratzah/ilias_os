#define _LOCALE_CTYPE_INLINE  // Instantiate here.

#include <locale_misc/ctype.h>

_namespace_begin(std)


constexpr ctype_base::mask ctype_base::space,
                           ctype_base::print,
                           ctype_base::cntrl,
                           ctype_base::upper,
                           ctype_base::lower,
                           ctype_base::alpha,
                           ctype_base::digit,
                           ctype_base::punct,
                           ctype_base::xdigit,
                           ctype_base::blank,
                           ctype_base::graph,
                           ctype_base::alnum;


namespace {

constexpr ctype_base::mask ascii_classify(char c) noexcept {
  const bool ascii_  = (c >= 0x00 && c <= 0x7f);
  const bool upper_  = (c >= 'A' && c <= 'Z');
  const bool lower_  = (c >= 'a' && c <= 'z');
  const bool alpha_  = upper_ || lower_;
  const bool digit_  = (c >= '0' && c <= '9');
  const bool alnum_  = alpha_ || digit_;
  const bool xdigit_ = digit_ ||
                       (c >= 'A' && c <= 'F') ||
                       (c >= 'a' && c <= 'f');
  const bool blank_  = (c == ' ' || c == '\t');
  const bool space_  = blank_ ||
                       c == '\f' || c == '\n' ||
                       c == '\r' || c == '\v';
  const bool cntrl_  = (c >= 0x00 && c <= 0x1f) ||
                       c == 0x7f;
  const bool print_  = ascii_ && !cntrl_;
  const bool graph_  = print_ && c != ' ';
  const bool punct_  = graph_ && !alnum_;

  return (upper_  ? ctype_base::upper  : 0) |
         (lower_  ? ctype_base::lower  : 0) |
         (alpha_  ? ctype_base::alpha  : 0) |
         (digit_  ? ctype_base::digit  : 0) |
         (alnum_  ? ctype_base::alnum  : 0) |
         (xdigit_ ? ctype_base::xdigit : 0) |
         (blank_  ? ctype_base::blank  : 0) |
         (space_  ? ctype_base::space  : 0) |
         (cntrl_  ? ctype_base::cntrl  : 0) |
         (print_  ? ctype_base::print  : 0) |
         (graph_  ? ctype_base::graph  : 0) |
         (punct_  ? ctype_base::punct  : 0);
}

constexpr ctype_base::mask ascii_masks[] = {
  ascii_classify(  0),
  ascii_classify(  1),
  ascii_classify(  2),
  ascii_classify(  3),
  ascii_classify(  4),
  ascii_classify(  5),
  ascii_classify(  6),
  ascii_classify(  7),
  ascii_classify(  8),
  ascii_classify(  9),
  ascii_classify( 10),
  ascii_classify( 11),
  ascii_classify( 12),
  ascii_classify( 13),
  ascii_classify( 14),
  ascii_classify( 15),
  ascii_classify( 16),
  ascii_classify( 17),
  ascii_classify( 18),
  ascii_classify( 19),
  ascii_classify( 20),
  ascii_classify( 21),
  ascii_classify( 22),
  ascii_classify( 23),
  ascii_classify( 24),
  ascii_classify( 25),
  ascii_classify( 26),
  ascii_classify( 27),
  ascii_classify( 28),
  ascii_classify( 29),
  ascii_classify( 30),
  ascii_classify( 31),
  ascii_classify( 32),
  ascii_classify( 33),
  ascii_classify( 34),
  ascii_classify( 35),
  ascii_classify( 36),
  ascii_classify( 37),
  ascii_classify( 38),
  ascii_classify( 39),
  ascii_classify( 40),
  ascii_classify( 41),
  ascii_classify( 42),
  ascii_classify( 43),
  ascii_classify( 44),
  ascii_classify( 45),
  ascii_classify( 46),
  ascii_classify( 47),
  ascii_classify( 48),
  ascii_classify( 49),
  ascii_classify( 50),
  ascii_classify( 51),
  ascii_classify( 52),
  ascii_classify( 53),
  ascii_classify( 54),
  ascii_classify( 55),
  ascii_classify( 56),
  ascii_classify( 57),
  ascii_classify( 58),
  ascii_classify( 59),
  ascii_classify( 60),
  ascii_classify( 61),
  ascii_classify( 62),
  ascii_classify( 63),
  ascii_classify( 64),
  ascii_classify( 65),
  ascii_classify( 66),
  ascii_classify( 67),
  ascii_classify( 68),
  ascii_classify( 69),
  ascii_classify( 70),
  ascii_classify( 71),
  ascii_classify( 72),
  ascii_classify( 73),
  ascii_classify( 74),
  ascii_classify( 75),
  ascii_classify( 76),
  ascii_classify( 77),
  ascii_classify( 78),
  ascii_classify( 79),
  ascii_classify( 80),
  ascii_classify( 81),
  ascii_classify( 82),
  ascii_classify( 83),
  ascii_classify( 84),
  ascii_classify( 85),
  ascii_classify( 86),
  ascii_classify( 87),
  ascii_classify( 88),
  ascii_classify( 89),
  ascii_classify( 90),
  ascii_classify( 91),
  ascii_classify( 92),
  ascii_classify( 93),
  ascii_classify( 94),
  ascii_classify( 95),
  ascii_classify( 96),
  ascii_classify( 97),
  ascii_classify( 98),
  ascii_classify( 99),
  ascii_classify(100),
  ascii_classify(101),
  ascii_classify(102),
  ascii_classify(103),
  ascii_classify(104),
  ascii_classify(105),
  ascii_classify(106),
  ascii_classify(107),
  ascii_classify(108),
  ascii_classify(109),
  ascii_classify(110),
  ascii_classify(111),
  ascii_classify(112),
  ascii_classify(113),
  ascii_classify(114),
  ascii_classify(115),
  ascii_classify(116),
  ascii_classify(117),
  ascii_classify(118),
  ascii_classify(119),
  ascii_classify(120),
  ascii_classify(121),
  ascii_classify(122),
  ascii_classify(123),
  ascii_classify(124),
  ascii_classify(125),
  ascii_classify(126),
  ascii_classify(127),
  ascii_classify(128),
  ascii_classify(129),
  ascii_classify(130),
  ascii_classify(131),
  ascii_classify(132),
  ascii_classify(133),
  ascii_classify(134),
  ascii_classify(135),
  ascii_classify(136),
  ascii_classify(137),
  ascii_classify(138),
  ascii_classify(139),
  ascii_classify(140),
  ascii_classify(141),
  ascii_classify(142),
  ascii_classify(143),
  ascii_classify(144),
  ascii_classify(145),
  ascii_classify(146),
  ascii_classify(147),
  ascii_classify(148),
  ascii_classify(149),
  ascii_classify(150),
  ascii_classify(151),
  ascii_classify(152),
  ascii_classify(153),
  ascii_classify(154),
  ascii_classify(155),
  ascii_classify(156),
  ascii_classify(157),
  ascii_classify(158),
  ascii_classify(159),
  ascii_classify(160),
  ascii_classify(161),
  ascii_classify(162),
  ascii_classify(163),
  ascii_classify(164),
  ascii_classify(165),
  ascii_classify(166),
  ascii_classify(167),
  ascii_classify(168),
  ascii_classify(169),
  ascii_classify(170),
  ascii_classify(171),
  ascii_classify(172),
  ascii_classify(173),
  ascii_classify(174),
  ascii_classify(175),
  ascii_classify(176),
  ascii_classify(177),
  ascii_classify(178),
  ascii_classify(179),
  ascii_classify(180),
  ascii_classify(181),
  ascii_classify(182),
  ascii_classify(183),
  ascii_classify(184),
  ascii_classify(185),
  ascii_classify(186),
  ascii_classify(187),
  ascii_classify(188),
  ascii_classify(189),
  ascii_classify(190),
  ascii_classify(191),
  ascii_classify(192),
  ascii_classify(193),
  ascii_classify(194),
  ascii_classify(195),
  ascii_classify(196),
  ascii_classify(197),
  ascii_classify(198),
  ascii_classify(199),
  ascii_classify(200),
  ascii_classify(201),
  ascii_classify(202),
  ascii_classify(203),
  ascii_classify(204),
  ascii_classify(205),
  ascii_classify(206),
  ascii_classify(207),
  ascii_classify(208),
  ascii_classify(209),
  ascii_classify(210),
  ascii_classify(211),
  ascii_classify(212),
  ascii_classify(213),
  ascii_classify(214),
  ascii_classify(215),
  ascii_classify(216),
  ascii_classify(217),
  ascii_classify(218),
  ascii_classify(219),
  ascii_classify(220),
  ascii_classify(221),
  ascii_classify(222),
  ascii_classify(223),
  ascii_classify(224),
  ascii_classify(225),
  ascii_classify(226),
  ascii_classify(227),
  ascii_classify(228),
  ascii_classify(229),
  ascii_classify(230),
  ascii_classify(231),
  ascii_classify(232),
  ascii_classify(233),
  ascii_classify(234),
  ascii_classify(235),
  ascii_classify(236),
  ascii_classify(237),
  ascii_classify(238),
  ascii_classify(239),
  ascii_classify(240),
  ascii_classify(241),
  ascii_classify(242),
  ascii_classify(243),
  ascii_classify(244),
  ascii_classify(245),
  ascii_classify(246),
  ascii_classify(247),
  ascii_classify(248),
  ascii_classify(249),
  ascii_classify(250),
  ascii_classify(251),
  ascii_classify(252),
  ascii_classify(253),
  ascii_classify(254),
  ascii_classify(255),
};

} /* namespace std::<unnamed> */


const size_t ctype<char>::table_size =
    sizeof(ascii_masks) / sizeof(ascii_masks[0]);

auto ctype<char>::classic_table() noexcept -> const mask* {
  return ascii_masks;
}


ctype<char16_t>::ctype(size_t refs)
: locale::facet(refs)
{}

auto ctype<char16_t>::is(mask m, char_type c) const -> bool {
  return do_is(m, c);
}

auto ctype<char16_t>::is(const char_type* b, const char_type* e,
                         mask* vec) const -> const char_type* {
  return do_is(b, e, vec);
}

auto ctype<char16_t>::scan_is(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_is(m, b, e);
}

auto ctype<char16_t>::scan_not(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_not(m, b, e);
}

auto ctype<char16_t>::toupper(char_type c) const -> char_type {
  return do_toupper(c);
}

auto ctype<char16_t>::toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_toupper(b, e);
}

auto ctype<char16_t>::tolower(char_type c) const -> char_type {
  return do_tolower(c);
}

auto ctype<char16_t>::tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_tolower(b, e);
}

auto ctype<char16_t>::widen(char c) const -> char_type {
  return do_widen(c);
}

auto ctype<char16_t>::widen(const char* b, const char* e,
                            char_type* out) const -> const char* {
  return do_widen(b, e, out);
}

auto ctype<char16_t>::narrow(char_type c, char dfl) const -> char {
  return do_narrow(c, dfl);
}

auto ctype<char16_t>::narrow(const char_type* b, const char_type* e,
                             char dfl, char* out) const -> const char_type* {
  return do_narrow(b, e, dfl, out);
}

ctype<char16_t>::~ctype() noexcept {}

auto ctype<char16_t>::do_is(mask m, char_type c) const -> bool {
  return (c >= 0 && c <= 0x7f && bool(ascii_masks[c] & m));  // XXX: all the code points!
}

auto ctype<char16_t>::do_is(const char_type* b, const char_type* e, mask* vec)
    const -> const char_type* {
  transform(b, e, vec,
            [this](char_type c) -> mask {
              if (c >= 0 && c <= 0x7f) return ascii_masks[c];  // XXX: all the code points!
              return 0;
            });
  return e;
}

auto ctype<char16_t>::do_scan_is(mask m,
                                 const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if(b, e,
                 [m, this](char_type c) -> bool {
                   return this->is(m, c);
                 });
}

auto ctype<char16_t>::do_scan_not(mask m,
                                  const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if_not(b, e,
                     [m, this](char_type c) -> bool {
                       return this->is(m, c);
                     });
}

auto ctype<char16_t>::do_toupper(char_type c) const -> char_type {
  // XXX: all the code points!
  return (c >= 'a' && c <= 'z' ?
          c - 'a' + 'A' :
          c);
}

auto ctype<char16_t>::do_toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->toupper(c);
                   });
}

auto ctype<char16_t>::do_tolower(char_type c) const -> char_type {
  // XXX: all the code points!
  return (c >= 'A' && c <= 'A' ?
          c - 'A' + 'a' :
          c);
}

auto ctype<char16_t>::do_tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->tolower(c);
                   });
}

auto ctype<char16_t>::do_widen(char c) const -> char_type {
  return c;
}

auto ctype<char16_t>::do_widen(const char* b, const char* e,
                              char_type* out) const -> const char* {
  transform(b, e, out,
            [this](char c) -> char_type {
              return this->widen(c);
            });
  return e;
}

auto ctype<char16_t>::do_narrow(char_type c, char dfl) const -> char {
  return (c <= 0 || c > 0x7f ? dfl : c);
}

auto ctype<char16_t>::do_narrow(const char_type* b, const char_type* e,
                               char dfl, char* out) const -> const char_type* {
  transform(b, e, out,
            [this, dfl](char c) -> char_type {
              return this->narrow(c, dfl);
            });
  return e;
}


ctype<char32_t>::ctype(size_t refs)
: locale::facet(refs)
{}

auto ctype<char32_t>::is(mask m, char_type c) const -> bool {
  return do_is(m, c);
}

auto ctype<char32_t>::is(const char_type* b, const char_type* e,
                         mask* vec) const -> const char_type* {
  return do_is(b, e, vec);
}

auto ctype<char32_t>::scan_is(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_is(m, b, e);
}

auto ctype<char32_t>::scan_not(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_not(m, b, e);
}

auto ctype<char32_t>::toupper(char_type c) const -> char_type {
  return do_toupper(c);
}

auto ctype<char32_t>::toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_toupper(b, e);
}

auto ctype<char32_t>::tolower(char_type c) const -> char_type {
  return do_tolower(c);
}

auto ctype<char32_t>::tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_tolower(b, e);
}

auto ctype<char32_t>::widen(char c) const -> char_type {
  return do_widen(c);
}

auto ctype<char32_t>::widen(const char* b, const char* e,
                            char_type* out) const -> const char* {
  return do_widen(b, e, out);
}

auto ctype<char32_t>::narrow(char_type c, char dfl) const -> char {
  return do_narrow(c, dfl);
}

auto ctype<char32_t>::narrow(const char_type* b, const char_type* e,
                             char dfl, char* out) const -> const char_type* {
  return do_narrow(b, e, dfl, out);
}

ctype<char32_t>::~ctype() noexcept {}

auto ctype<char32_t>::do_is(mask m, char_type c) const -> bool {
  return (c >= 0 && c <= 0x7f && bool(ascii_masks[c] & m));  // XXX: all the code points!
}

auto ctype<char32_t>::do_is(const char_type* b, const char_type* e, mask* vec)
    const -> const char_type* {
  transform(b, e, vec,
            [this](char_type c) -> mask {
              if (c >= 0 && c <= 0x7f) return ascii_masks[c];  // XXX: all the code points!
              return 0;
            });
  return e;
}

auto ctype<char32_t>::do_scan_is(mask m,
                                 const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if(b, e,
                 [m, this](char_type c) -> bool {
                   return this->is(m, c);
                 });
}

auto ctype<char32_t>::do_scan_not(mask m,
                                  const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if_not(b, e,
                     [m, this](char_type c) -> bool {
                       return this->is(m, c);
                     });
}

auto ctype<char32_t>::do_toupper(char_type c) const -> char_type {
  // XXX: all the code points!
  return (c >= 'a' && c <= 'z' ?
          c - 'a' + 'A' :
          c);
}

auto ctype<char32_t>::do_toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->toupper(c);
                   });
}

auto ctype<char32_t>::do_tolower(char_type c) const -> char_type {
  // XXX: all the code points!
  return (c >= 'A' && c <= 'A' ?
          c - 'A' + 'a' :
          c);
}

auto ctype<char32_t>::do_tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->tolower(c);
                   });
}

auto ctype<char32_t>::do_widen(char c) const -> char_type {
  return c;
}

auto ctype<char32_t>::do_widen(const char* b, const char* e,
                              char_type* out) const -> const char* {
  transform(b, e, out,
            [this](char c) -> char_type {
              return this->widen(c);
            });
  return e;
}

auto ctype<char32_t>::do_narrow(char_type c, char dfl) const -> char {
  return (c <= 0 || c > 0x7f ? dfl : c);
}

auto ctype<char32_t>::do_narrow(const char_type* b, const char_type* e,
                               char dfl, char* out) const -> const char_type* {
  transform(b, e, out,
            [this, dfl](char c) -> char_type {
              return this->narrow(c, dfl);
            });
  return e;
}


ctype<wchar_t>::ctype(size_t refs)
: locale::facet(refs)
{}

auto ctype<wchar_t>::is(mask m, char_type c) const -> bool {
  return do_is(m, c);
}

auto ctype<wchar_t>::is(const char_type* b, const char_type* e,
                        mask* vec) const -> const char_type* {
  return do_is(b, e, vec);
}

auto ctype<wchar_t>::scan_is(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_is(m, b, e);
}

auto ctype<wchar_t>::scan_not(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_not(m, b, e);
}

auto ctype<wchar_t>::toupper(char_type c) const -> char_type {
  return do_toupper(c);
}

auto ctype<wchar_t>::toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_toupper(b, e);
}

auto ctype<wchar_t>::tolower(char_type c) const -> char_type {
  return do_tolower(c);
}

auto ctype<wchar_t>::tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_tolower(b, e);
}

auto ctype<wchar_t>::widen(char c) const -> char_type {
  return do_widen(c);
}

auto ctype<wchar_t>::widen(const char* b, const char* e,
                           char_type* out) const -> const char* {
  return do_widen(b, e, out);
}

auto ctype<wchar_t>::narrow(char_type c, char dfl) const -> char {
  return do_narrow(c, dfl);
}

auto ctype<wchar_t>::narrow(const char_type* b, const char_type* e,
                            char dfl, char* out) const -> const char_type* {
  return do_narrow(b, e, dfl, out);
}

ctype<wchar_t>::~ctype() noexcept {}

auto ctype<wchar_t>::do_is(mask m, char_type c) const -> bool {
  return (c >= 0 && c <= 0x7f && bool(ascii_masks[c] & m));  // XXX: all the code points!
}

auto ctype<wchar_t>::do_is(const char_type* b, const char_type* e, mask* vec)
    const -> const char_type* {
  transform(b, e, vec,
            [this](char_type c) -> mask {
              if (c >= 0 && c <= 0x7f) return ascii_masks[c];  // XXX: all the code points!
              return 0;
            });
  return e;
}

auto ctype<wchar_t>::do_scan_is(mask m,
                                const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if(b, e,
                 [m, this](char_type c) -> bool {
                   return this->is(m, c);
                 });
}

auto ctype<wchar_t>::do_scan_not(mask m,
                                 const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if_not(b, e,
                     [m, this](char_type c) -> bool {
                       return this->is(m, c);
                     });
}

auto ctype<wchar_t>::do_toupper(char_type c) const -> char_type {
  // XXX: all the code points!
  return (c >= 'a' && c <= 'z' ?
          c - 'a' + 'A' :
          c);
}

auto ctype<wchar_t>::do_toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->toupper(c);
                   });
}

auto ctype<wchar_t>::do_tolower(char_type c) const -> char_type {
  // XXX: all the code points!
  return (c >= 'A' && c <= 'A' ?
          c - 'A' + 'a' :
          c);
}

auto ctype<wchar_t>::do_tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->tolower(c);
                   });
}

auto ctype<wchar_t>::do_widen(char c) const -> char_type {
  return c;
}

auto ctype<wchar_t>::do_widen(const char* b, const char* e,
                              char_type* out) const -> const char* {
  transform(b, e, out,
            [this](char c) -> char_type {
              return this->widen(c);
            });
  return e;
}

auto ctype<wchar_t>::do_narrow(char_type c, char dfl) const -> char {
  return (c <= 0 || c > 0x7f ? dfl : c);
}

auto ctype<wchar_t>::do_narrow(const char_type* b, const char_type* e,
                               char dfl, char* out) const -> const char_type* {
  transform(b, e, out,
            [this, dfl](char c) -> char_type {
              return this->narrow(c, dfl);
            });
  return e;
}


constexpr locale::id ctype<char>::id;
constexpr locale::id ctype<char16_t>::id;
constexpr locale::id ctype<char32_t>::id;
constexpr locale::id ctype<wchar_t>::id;

template class ctype_byname<char>;
template class ctype_byname<char16_t>;
template class ctype_byname<char32_t>;
template class ctype_byname<wchar_t>;


template bool isspace(char, const locale&);
template bool isprint(char, const locale&);
template bool iscntrl(char, const locale&);
template bool isupper(char, const locale&);
template bool islower(char, const locale&);
template bool isalpha(char, const locale&);
template bool isdigit(char, const locale&);
template bool ispunct(char, const locale&);
template bool isxdigit(char, const locale&);
template bool isalnum(char, const locale&);
template bool isgraph(char, const locale&);
template bool isblank(char, const locale&);
template char toupper(char, const locale&);
template char tolower(char, const locale&);

template bool isspace(char16_t, const locale&);
template bool isprint(char16_t, const locale&);
template bool iscntrl(char16_t, const locale&);
template bool isupper(char16_t, const locale&);
template bool islower(char16_t, const locale&);
template bool isalpha(char16_t, const locale&);
template bool isdigit(char16_t, const locale&);
template bool ispunct(char16_t, const locale&);
template bool isxdigit(char16_t, const locale&);
template bool isalnum(char16_t, const locale&);
template bool isgraph(char16_t, const locale&);
template bool isblank(char16_t, const locale&);
template char16_t toupper(char16_t, const locale&);
template char16_t tolower(char16_t, const locale&);

template bool isspace(char32_t, const locale&);
template bool isprint(char32_t, const locale&);
template bool iscntrl(char32_t, const locale&);
template bool isupper(char32_t, const locale&);
template bool islower(char32_t, const locale&);
template bool isalpha(char32_t, const locale&);
template bool isdigit(char32_t, const locale&);
template bool ispunct(char32_t, const locale&);
template bool isxdigit(char32_t, const locale&);
template bool isalnum(char32_t, const locale&);
template bool isgraph(char32_t, const locale&);
template bool isblank(char32_t, const locale&);
template char32_t toupper(char32_t, const locale&);
template char32_t tolower(char32_t, const locale&);

template bool isspace(wchar_t, const locale&);
template bool isprint(wchar_t, const locale&);
template bool iscntrl(wchar_t, const locale&);
template bool isupper(wchar_t, const locale&);
template bool islower(wchar_t, const locale&);
template bool isalpha(wchar_t, const locale&);
template bool isdigit(wchar_t, const locale&);
template bool ispunct(wchar_t, const locale&);
template bool isxdigit(wchar_t, const locale&);
template bool isalnum(wchar_t, const locale&);
template bool isgraph(wchar_t, const locale&);
template bool isblank(wchar_t, const locale&);
template wchar_t toupper(wchar_t, const locale&);
template wchar_t tolower(wchar_t, const locale&);


_namespace_end(std)
