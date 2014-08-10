#ifndef _LOCALE_MISC_MESSAGES_H_
#define _LOCALE_MISC_MESSAGES_H_

#include <cdecl.h>
#include <locale_misc/locale.h>

_namespace_begin(std)


class messages_base {
 public:
  using catalog = int;
};

template<typename Char>
class messages
: public locale::facet,
  public messages_base
{
 public:
  using char_type = Char;
  using string_type = basic_string<char_type>;

  explicit messages(size_t = 0);

  catalog open(const string&, const locale&) const;
  string_type get(catalog, int, int, const string_type&) const;
  void close(catalog) const;

  static const locale::id id;

 protected:
  ~messages() noexcept override;

  virtual catalog do_open(const string&, const locale&) const = 0;
  virtual string_type do_get(catalog, int, int, const string_type&) const = 0;
  virtual void do_close(catalog) const = 0;
};

template<typename Char>
const locale::id messages<Char>::id{ locale::messages };

template<typename Char>
class messages_byname
: public messages<Char>,
  private impl::facet_ref<messages<Char>>
{
 public:
  using catalog = typename messages<Char>::catalog;
  using char_type = typename messages<Char>::char_type;
  using string_type = typename messages<Char>::string_type;

  explicit messages_byname(const char*, size_t = 0);
  explicit messages_byname(const string&, size_t = 0);
  explicit messages_byname(string_ref, size_t = 0);

 protected:
  ~messages_byname() noexcept override = default;

  catalog do_open(const string&, const locale&) const override;
  string_type do_get(catalog, int, int, const string_type&) const override;
  void do_close(catalog) const override;
};


extern template class messages<char>;
extern template class messages<char16_t>;
extern template class messages<char32_t>;
extern template class messages<wchar_t>;

extern template class messages_byname<char>;
extern template class messages_byname<char16_t>;
extern template class messages_byname<char32_t>;
extern template class messages_byname<wchar_t>;


_namespace_end(std)

#include <locale_misc/messages-inl.h>

#endif /* _LOCALE_MISC_MESSAGES_H_ */
