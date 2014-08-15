#ifndef _LOCALE_MISC_COLLATE_H_
#define _LOCALE_MISC_COLLATE_H_

#include <cdecl.h>
#include <locale_misc/locale.h>
#include <iosfwd>

_namespace_begin(std)


template<typename Char>
class collate
: public locale::facet
{
 public:
  using char_type = Char;
  using string_type = basic_string<Char>;

  explicit collate(size_t = 0);

  int compare(const char_type*, const char_type*,
              const char_type*, const char_type*) const;
  string_type transform(const char_type*, const char_type*) const;
  long hash(const char_type*, const char_type*) const;

  static const locale::id id;

 protected:
  ~collate() noexcept override;

  virtual int do_compare(const char_type*, const char_type*,
                         const char_type*, const char_type*) const;
  virtual string_type do_transform(const char_type*, const char_type*) const;
  virtual long do_hash(const char_type*, const char_type*) const;
};

template<typename Char> const locale::id collate<Char>::id{ locale::collate };

template<typename Char>
class collate_byname
: public collate<Char>,
  private impl::facet_ref<collate<Char>>
{
 public:
  using char_type = typename collate<Char>::char_type;
  using string_type = typename collate<Char>::string_type;

  explicit collate_byname(const char*, size_t = 0);
  explicit collate_byname(const string&, size_t = 0);
  explicit collate_byname(string_ref, size_t = 0);

 protected:
  ~collate_byname() noexcept override = default;

  int do_compare(const char_type*, const char_type*,
                 const char_type*, const char_type*) const override;
  string_type do_transform(const char_type*, const char_type*) const override;
  long do_hash(const char_type*, const char_type*) const override;
};


extern template class collate<char>;
extern template class collate<char16_t>;
extern template class collate<char32_t>;
extern template class collate<wchar_t>;

extern template class collate_byname<char>;
extern template class collate_byname<char16_t>;
extern template class collate_byname<char32_t>;
extern template class collate_byname<wchar_t>;


_namespace_end(std)

#include <locale_misc/collate-inl.h>

#endif /* _LOCALE_MISC_COLLATE_H_ */
