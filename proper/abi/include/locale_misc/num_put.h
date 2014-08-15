#ifndef _LOCALE_MISC_NUM_PUT_H_
#define _LOCALE_MISC_NUM_PUT_H_

#include <cdecl.h>
#include <locale_misc/locale.h>
#include <iosfwd>
#include <cstdint>

_namespace_begin(std)


template<typename Char, typename OutputIter>
class num_put
: public locale::facet
{
 public:
  using char_type = Char;
  using iter_type = OutputIter;

  explicit num_put(size_t = 0);

  iter_type put(iter_type, ios_base&, char_type, bool) const;
  iter_type put(iter_type, ios_base&, char_type, long) const;
  iter_type put(iter_type, ios_base&, char_type, long long) const;
  iter_type put(iter_type, ios_base&, char_type, unsigned long) const;
  iter_type put(iter_type, ios_base&, char_type, unsigned long long) const;
#if _USE_INT128
  iter_type put(iter_type, ios_base&, char_type, int128_t) const;
  iter_type put(iter_type, ios_base&, char_type, uint128_t) const;
#endif
  iter_type put(iter_type, ios_base&, char_type, double) const;
  iter_type put(iter_type, ios_base&, char_type, long double) const;
  iter_type put(iter_type, ios_base&, char_type, const void*) const;

  static const locale::id id;

 protected:
  ~num_put() noexcept override;

  virtual iter_type do_put(iter_type, ios_base&, char_type, bool) const;
  virtual iter_type do_put(iter_type, ios_base&, char_type, long) const;
  virtual iter_type do_put(iter_type, ios_base&, char_type, long long) const;
  virtual iter_type do_put(iter_type, ios_base&, char_type, unsigned long)
      const;
  virtual iter_type do_put(iter_type, ios_base&, char_type, unsigned long long)
      const;
#if _USE_INT128
  virtual iter_type do_put(iter_type, ios_base&, char_type, int128_t) const;
  virtual iter_type do_put(iter_type, ios_base&, char_type, uint128_t) const;
#endif
  virtual iter_type do_put(iter_type, ios_base&, char_type, double) const;
  virtual iter_type do_put(iter_type, ios_base&, char_type, long double) const;
  virtual iter_type do_put(iter_type, ios_base&, char_type, const void*) const;
};

template<typename Char, typename OutputIter>
const locale::id num_put<Char, OutputIter>::id{ locale::numeric };


extern template class num_put<char>;
extern template class num_put<char16_t>;
extern template class num_put<char32_t>;
extern template class num_put<wchar_t>;


_namespace_end(std)

#include <locale_misc/num_put-inl.h>

#endif /* _LOCALE_MISC_NUM_PUT_H_ */
