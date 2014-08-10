#ifndef _LOCALE_MISC_CTYPE_H_
#define _LOCALE_MISC_CTYPE_H_

#include <cdecl.h>
#include <locale_misc/locale.h>

_namespace_begin(std)


template<typename Char> bool isspace(Char, const locale&);
template<typename Char> bool isprint(Char, const locale&);
template<typename Char> bool iscntrl(Char, const locale&);
template<typename Char> bool isupper(Char, const locale&);
template<typename Char> bool islower(Char, const locale&);
template<typename Char> bool isalpha(Char, const locale&);
template<typename Char> bool isdigit(Char, const locale&);
template<typename Char> bool ispunct(Char, const locale&);
template<typename Char> bool isxdigit(Char, const locale&);
template<typename Char> bool isalnum(Char, const locale&);
template<typename Char> bool isgraph(Char, const locale&);
template<typename Char> bool isblank(Char, const locale&);
template<typename Char> Char toupper(Char, const locale&);
template<typename Char> Char tolower(Char, const locale&);


class ctype_base {
 public:
  using mask = unsigned short;

  static constexpr mask space  = 0x001,
                        print  = 0x002,
                        cntrl  = 0x004,
                        upper  = 0x008,
                        lower  = 0x010,
                        alpha  = 0x020,
                        digit  = 0x040,
                        punct  = 0x080,
                        xdigit = 0x100,
                        blank  = 0x200,
                        graph  = 0x400,
                        alnum  = alpha | digit;
};

template<typename Char>
class ctype
: public locale::facet,
  public ctype_base
{
 public:
  using char_type = Char;

  explicit ctype(size_t = 0);

  bool is(mask, char_type) const;
  const char_type* is(const char_type*, const char_type*, mask*) const;
  const char_type* scan_is(mask, const char_type*, const char_type*) const;
  const char_type* scan_not(mask, const char_type*, const char_type*) const;
  char_type toupper(char_type) const;
  const char_type* toupper(char_type*, const char_type*) const;
  char_type tolower(char_type) const;
  const char_type* tolower(char_type*, const char_type*) const;

  char_type widen(char) const;
  const char* widen(const char*, const char*, char_type*) const;
  char narrow(char_type, char) const;
  const char_type* narrow(const char_type*, const char_type*, char, char*)
      const;

  static const locale::id id;

 protected:
  ~ctype() noexcept override;

  virtual bool do_is(mask, char_type) const = 0;
  virtual const char_type* do_is(const char_type*, const char_type*, mask*)
      const = 0;
  virtual const char_type* do_scan_is(mask, const char_type*, const char_type*)
      const = 0;
  virtual const char_type* do_scan_not(mask,
                                       const char_type*, const char_type*)
      const = 0;
  virtual char_type do_toupper(char_type) const = 0;
  virtual const char_type* do_toupper(char_type*, const char_type*) const = 0;
  virtual char_type do_tolower(char_type) const = 0;
  virtual const char_type* do_tolower(char_type*, const char_type*) const = 0;
  virtual char_type do_widen(char) const = 0;
  virtual const char* do_widen(const char*, const char*, char_type*) const = 0;
  virtual char do_narrow(char_type, char) const = 0;
  virtual const char_type* do_narrow(const char_type*, const char_type*, char,
                                     char*) const = 0;
};

template<typename Char> const locale::id ctype<Char>::id{ locale::ctype };

template<> class ctype<char>
: public locale::facet,
  public ctype_base
{
 public:
  using char_type = char;

  explicit ctype(const mask* = nullptr, bool = false, size_t = 0);

  bool is(mask, char_type) const;
  const char_type* is(const char_type*, const char_type*, mask*) const;
  const char_type* scan_is(mask, const char_type*, const char_type*) const;
  const char_type* scan_not(mask, const char_type*, const char_type*) const;
  char_type toupper(char_type) const;
  const char_type* toupper(char_type*, const char_type*) const;
  char_type tolower(char_type) const;
  const char_type* tolower(char_type*, const char_type*) const;

  char_type widen(char) const;
  const char* widen(const char*, const char*, char_type*) const;
  char narrow(char_type, char) const;
  const char_type* narrow(const char_type*, const char_type*, char, char*)
      const;

  static constexpr locale::id id{ locale::ctype };
  static const size_t table_size;
  const mask* table() const noexcept;
  static const mask* classic_table() noexcept;

 protected:
  ~ctype() noexcept override;

  virtual char_type do_toupper(char_type) const;
  virtual const char_type* do_toupper(char_type*, const char_type*) const;
  virtual char_type do_tolower(char_type) const;
  virtual const char_type* do_tolower(char_type*, const char_type*) const;
  virtual char_type do_widen(char) const;
  virtual const char* do_widen(const char*, const char*, char_type*) const;
  virtual char do_narrow(char_type, char) const;
  virtual const char_type* do_narrow(const char_type*, const char_type*, char,
                                     char*) const;

 private:
  const mask* tbl_;
  bool del_;
};

template<>
class ctype<char16_t>
: public locale::facet,
  public ctype_base
{
 public:
  using char_type = char16_t;

  explicit ctype(size_t = 0);

  bool is(mask, char_type) const;
  const char_type* is(const char_type*, const char_type*, mask*) const;
  const char_type* scan_is(mask, const char_type*, const char_type*) const;
  const char_type* scan_not(mask, const char_type*, const char_type*) const;
  char_type toupper(char_type) const;
  const char_type* toupper(char_type*, const char_type*) const;
  char_type tolower(char_type) const;
  const char_type* tolower(char_type*, const char_type*) const;

  char_type widen(char) const;
  const char* widen(const char*, const char*, char_type*) const;
  char narrow(char_type, char) const;
  const char_type* narrow(const char_type*, const char_type*, char, char*)
      const;

  static constexpr locale::id id{ locale::ctype };

 protected:
  ~ctype() noexcept override;

  virtual bool do_is(mask, char_type) const;
  virtual const char_type* do_is(const char_type*, const char_type*, mask*)
      const;
  virtual const char_type* do_scan_is(mask, const char_type*, const char_type*)
      const;
  virtual const char_type* do_scan_not(mask,
                                       const char_type*, const char_type*)
      const;
  virtual char_type do_toupper(char_type) const;
  virtual const char_type* do_toupper(char_type*, const char_type*) const;
  virtual char_type do_tolower(char_type) const;
  virtual const char_type* do_tolower(char_type*, const char_type*) const;
  virtual char_type do_widen(char) const;
  virtual const char* do_widen(const char*, const char*, char_type*) const;
  virtual char do_narrow(char_type, char) const;
  virtual const char_type* do_narrow(const char_type*, const char_type*, char,
                                     char*) const;
};

template<>
class ctype<char32_t>
: public locale::facet,
  public ctype_base
{
 public:
  using char_type = char32_t;

  explicit ctype(size_t = 0);

  bool is(mask, char_type) const;
  const char_type* is(const char_type*, const char_type*, mask*) const;
  const char_type* scan_is(mask, const char_type*, const char_type*) const;
  const char_type* scan_not(mask, const char_type*, const char_type*) const;
  char_type toupper(char_type) const;
  const char_type* toupper(char_type*, const char_type*) const;
  char_type tolower(char_type) const;
  const char_type* tolower(char_type*, const char_type*) const;

  char_type widen(char) const;
  const char* widen(const char*, const char*, char_type*) const;
  char narrow(char_type, char) const;
  const char_type* narrow(const char_type*, const char_type*, char, char*)
      const;

  static constexpr locale::id id{ locale::ctype };

 protected:
  ~ctype() noexcept override;

  virtual bool do_is(mask, char_type) const;
  virtual const char_type* do_is(const char_type*, const char_type*, mask*)
      const;
  virtual const char_type* do_scan_is(mask, const char_type*, const char_type*)
      const;
  virtual const char_type* do_scan_not(mask,
                                       const char_type*, const char_type*)
      const;
  virtual char_type do_toupper(char_type) const;
  virtual const char_type* do_toupper(char_type*, const char_type*) const;
  virtual char_type do_tolower(char_type) const;
  virtual const char_type* do_tolower(char_type*, const char_type*) const;
  virtual char_type do_widen(char) const;
  virtual const char* do_widen(const char*, const char*, char_type*) const;
  virtual char do_narrow(char_type, char) const;
  virtual const char_type* do_narrow(const char_type*, const char_type*, char,
                                     char*) const;
};

template<>
class ctype<wchar_t>
: public locale::facet,
  public ctype_base
{
 public:
  using char_type = wchar_t;

  explicit ctype(size_t = 0);

  bool is(mask, char_type) const;
  const char_type* is(const char_type*, const char_type*, mask*) const;
  const char_type* scan_is(mask, const char_type*, const char_type*) const;
  const char_type* scan_not(mask, const char_type*, const char_type*) const;
  char_type toupper(char_type) const;
  const char_type* toupper(char_type*, const char_type*) const;
  char_type tolower(char_type) const;
  const char_type* tolower(char_type*, const char_type*) const;

  char_type widen(char) const;
  const char* widen(const char*, const char*, char_type*) const;
  char narrow(char_type, char) const;
  const char_type* narrow(const char_type*, const char_type*, char, char*)
      const;

  static constexpr locale::id id{ locale::ctype };

 protected:
  ~ctype() noexcept override;

  virtual bool do_is(mask, char_type) const;
  virtual const char_type* do_is(const char_type*, const char_type*, mask*)
      const;
  virtual const char_type* do_scan_is(mask, const char_type*, const char_type*)
      const;
  virtual const char_type* do_scan_not(mask,
                                       const char_type*, const char_type*)
      const;
  virtual char_type do_toupper(char_type) const;
  virtual const char_type* do_toupper(char_type*, const char_type*) const;
  virtual char_type do_tolower(char_type) const;
  virtual const char_type* do_tolower(char_type*, const char_type*) const;
  virtual char_type do_widen(char) const;
  virtual const char* do_widen(const char*, const char*, char_type*) const;
  virtual char do_narrow(char_type, char) const;
  virtual const char_type* do_narrow(const char_type*, const char_type*, char,
                                     char*) const;
};

template<typename Char>
class ctype_byname
: public ctype<Char>,
  private impl::facet_ref<ctype<Char>>
{
 public:
  using mask = typename ctype<Char>::mask;
  using char_type = typename ctype<Char>::char_type;

  explicit ctype_byname(const char*, size_t = 0);
  explicit ctype_byname(const string&, size_t = 0);
  explicit ctype_byname(string_ref, size_t = 0);

 protected:
  ~ctype_byname() noexcept override = default;

  bool do_is(mask, char_type) const override;
  const char_type* do_is(const char_type*, const char_type*, mask*)
      const override;
  const char_type* do_scan_is(mask, const char_type*, const char_type*)
      const override;
  const char_type* do_scan_not(mask, const char_type*, const char_type*)
      const override;
  char_type do_toupper(char_type) const override;
  const char_type* do_toupper(char_type*, const char_type*) const override;
  char_type do_tolower(char_type) const override;
  const char_type* do_tolower(char_type*, const char_type*) const override;
  char_type do_widen(char) const override;
  const char* do_widen(const char*, const char*, char_type*) const override;
  char do_narrow(char_type, char) const override;
  const char_type* do_narrow(const char_type*, const char_type*, char,
                             char*) const override;
};

template<>
class ctype_byname<char>
: private impl::facet_ref<ctype<char>>,
  public ctype<char>
{
 public:
  using mask = typename ctype<char>::mask;
  using char_type = typename ctype<char>::char_type;

  explicit ctype_byname(const char*, size_t = 0);
  explicit ctype_byname(const string&, size_t = 0);
  explicit ctype_byname(string_ref, size_t = 0);

 protected:
  ~ctype_byname() noexcept override;

  char_type do_toupper(char_type) const override;
  const char_type* do_toupper(char_type*, const char_type*) const override;
  char_type do_tolower(char_type) const override;
  const char_type* do_tolower(char_type*, const char_type*) const override;
  char_type do_widen(char) const override;
  const char* do_widen(const char*, const char*, char_type*) const override;
  char do_narrow(char_type, char) const override;
  const char_type* do_narrow(const char_type*, const char_type*, char,
                             char*) const override;
};


extern template class ctype_byname<char>;
extern template class ctype_byname<char16_t>;
extern template class ctype_byname<char32_t>;
extern template class ctype_byname<wchar_t>;


extern template bool isspace(char, const locale&);
extern template bool isprint(char, const locale&);
extern template bool iscntrl(char, const locale&);
extern template bool isupper(char, const locale&);
extern template bool islower(char, const locale&);
extern template bool isalpha(char, const locale&);
extern template bool isdigit(char, const locale&);
extern template bool ispunct(char, const locale&);
extern template bool isxdigit(char, const locale&);
extern template bool isalnum(char, const locale&);
extern template bool isgraph(char, const locale&);
extern template bool isblank(char, const locale&);
extern template char toupper(char, const locale&);
extern template char tolower(char, const locale&);

extern template bool isspace(char16_t, const locale&);
extern template bool isprint(char16_t, const locale&);
extern template bool iscntrl(char16_t, const locale&);
extern template bool isupper(char16_t, const locale&);
extern template bool islower(char16_t, const locale&);
extern template bool isalpha(char16_t, const locale&);
extern template bool isdigit(char16_t, const locale&);
extern template bool ispunct(char16_t, const locale&);
extern template bool isxdigit(char16_t, const locale&);
extern template bool isalnum(char16_t, const locale&);
extern template bool isgraph(char16_t, const locale&);
extern template bool isblank(char16_t, const locale&);
extern template char16_t toupper(char16_t, const locale&);
extern template char16_t tolower(char16_t, const locale&);

extern template bool isspace(char32_t, const locale&);
extern template bool isprint(char32_t, const locale&);
extern template bool iscntrl(char32_t, const locale&);
extern template bool isupper(char32_t, const locale&);
extern template bool islower(char32_t, const locale&);
extern template bool isalpha(char32_t, const locale&);
extern template bool isdigit(char32_t, const locale&);
extern template bool ispunct(char32_t, const locale&);
extern template bool isxdigit(char32_t, const locale&);
extern template bool isalnum(char32_t, const locale&);
extern template bool isgraph(char32_t, const locale&);
extern template bool isblank(char32_t, const locale&);
extern template char32_t toupper(char32_t, const locale&);
extern template char32_t tolower(char32_t, const locale&);

extern template bool isspace(wchar_t, const locale&);
extern template bool isprint(wchar_t, const locale&);
extern template bool iscntrl(wchar_t, const locale&);
extern template bool isupper(wchar_t, const locale&);
extern template bool islower(wchar_t, const locale&);
extern template bool isalpha(wchar_t, const locale&);
extern template bool isdigit(wchar_t, const locale&);
extern template bool ispunct(wchar_t, const locale&);
extern template bool isxdigit(wchar_t, const locale&);
extern template bool isalnum(wchar_t, const locale&);
extern template bool isgraph(wchar_t, const locale&);
extern template bool isblank(wchar_t, const locale&);
extern template wchar_t toupper(wchar_t, const locale&);
extern template wchar_t tolower(wchar_t, const locale&);


_namespace_end(std)

#include <locale_misc/ctype-inl.h>

#endif /* _LOCALE_MISC_CTYPE_H_ */
