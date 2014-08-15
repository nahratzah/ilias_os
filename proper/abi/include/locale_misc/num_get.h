#ifndef _LOCALE_MISC_NUM_GET_H_
#define _LOCALE_MISC_NUM_GET_H_

#include <cdecl.h>
#include <locale_misc/locale.h>
#include <ios>
#include <cstdint>

_namespace_begin(std)


template<typename Char, typename InputIter>
class num_get
: public locale::facet
{
 public:
  using char_type = Char;
  using iter_type = InputIter;

  explicit num_get(size_t = 0);

  iter_type get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                bool&) const;
  iter_type get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                long&) const;
  iter_type get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                long long&) const;
  iter_type get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                unsigned short&) const;
  iter_type get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                unsigned int&) const;
  iter_type get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                unsigned long&) const;
  iter_type get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                unsigned long long&) const;
#if _USE_INT128
  iter_type get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                int128_t&) const;
  iter_type get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                uint128_t&) const;
#endif
  iter_type get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                float&) const;
  iter_type get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                double&) const;
  iter_type get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                long double&) const;
  iter_type get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                void*&) const;

  static const locale::id id;

 protected:
  ~num_get() noexcept override;

  virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                           bool&) const;
  virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                           long&) const;
  virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                           long long&) const;
  virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                           unsigned short&) const;
  virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                           unsigned int&) const;
  virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                           unsigned long&) const;
  virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                           unsigned long long&) const;
#if _USE_INT128
  virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                                   int128_t&) const;
  virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                                   uint128_t&) const;
#endif
  virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                                   float&) const;
  virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                                   double&) const;
  virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                                   long double&) const;
  virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&,
                                   void*&) const;
};

template<typename Char, typename InputIter>
const locale::id num_get<Char, InputIter>::id{ locale::numeric };


extern template class num_get<char>;
extern template class num_get<char16_t>;
extern template class num_get<char32_t>;
extern template class num_get<wchar_t>;


_namespace_end(std)

#include <locale_misc/num_get-inl.h>

#endif /* _LOCALE_MISC_NUM_GET_H_ */
