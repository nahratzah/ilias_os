#ifndef _LOCALE_MISC_NUMPUNCT_H_
#define _LOCALE_MISC_NUMPUNCT_H_

#include <cdecl.h>
#include <locale_misc/locale.h>

_namespace_begin(std)


template<typename Char> class numpunct;

template<>
class numpunct<char>
: public locale::facet
{
 public:
  using char_type = char;
  using string_type = basic_string<char_type>;

  explicit numpunct(size_t = 0);

  char_type decimal_point() const;
  char_type thousands_sep() const;
  string grouping() const;
  string_type truename() const;
  string_type falsename() const;

  static constexpr locale::id id{ locale::numeric };

 protected:
  ~numpunct() noexcept override;

  virtual char_type do_decimal_point() const;
  virtual char_type do_thousands_sep() const;
  virtual string do_grouping() const;
  virtual string_type do_truename() const;
  virtual string_type do_falsename() const;
};

template<>
class numpunct<char16_t>
: public locale::facet
{
 public:
  using char_type = char16_t;
  using string_type = basic_string<char_type>;

  explicit numpunct(size_t = 0);

  char_type decimal_point() const;
  char_type thousands_sep() const;
  string grouping() const;
  string_type truename() const;
  string_type falsename() const;

  static constexpr locale::id id{ locale::numeric };

 protected:
  ~numpunct() noexcept override;

  virtual char_type do_decimal_point() const;
  virtual char_type do_thousands_sep() const;
  virtual string do_grouping() const;
  virtual string_type do_truename() const;
  virtual string_type do_falsename() const;
};

template<>
class numpunct<char32_t>
: public locale::facet
{
 public:
  using char_type = char32_t;
  using string_type = basic_string<char_type>;

  explicit numpunct(size_t = 0);

  char_type decimal_point() const;
  char_type thousands_sep() const;
  string grouping() const;
  string_type truename() const;
  string_type falsename() const;

  static constexpr locale::id id{ locale::numeric };

 protected:
  ~numpunct() noexcept override;

  virtual char_type do_decimal_point() const;
  virtual char_type do_thousands_sep() const;
  virtual string do_grouping() const;
  virtual string_type do_truename() const;
  virtual string_type do_falsename() const;
};

template<>
class numpunct<wchar_t>
: public locale::facet
{
 public:
  using char_type = wchar_t;
  using string_type = basic_string<char_type>;

  explicit numpunct(size_t = 0);

  char_type decimal_point() const;
  char_type thousands_sep() const;
  string grouping() const;
  string_type truename() const;
  string_type falsename() const;

  static constexpr locale::id id{ locale::numeric };

 protected:
  ~numpunct() noexcept override;

  virtual char_type do_decimal_point() const;
  virtual char_type do_thousands_sep() const;
  virtual string do_grouping() const;
  virtual string_type do_truename() const;
  virtual string_type do_falsename() const;
};

template<typename Char>
class numpunct_byname
: public numpunct<Char>,
  private impl::facet_ref<numpunct<Char>>
{
 public:
  using char_type = typename numpunct<Char>::char_type;
  using string_type = typename numpunct<Char>::string_type;

  explicit numpunct_byname(const char*, size_t = 0);
  explicit numpunct_byname(const string&, size_t = 0);
  explicit numpunct_byname(string_ref, size_t = 0);

 protected:
  ~numpunct_byname() noexcept override = default;

  char_type do_decimal_point() const override;
  char_type do_thousands_sep() const override;
  string do_grouping() const override;
  string_type do_truename() const override;
  string_type do_falsename() const override;
};


extern template class numpunct_byname<char>;
extern template class numpunct_byname<char16_t>;
extern template class numpunct_byname<char32_t>;
extern template class numpunct_byname<wchar_t>;


_namespace_end(std)

#include <locale_misc/numpunct-inl.h>

#endif /* _LOCALE_MISC_NUMPUNCT_H_ */
