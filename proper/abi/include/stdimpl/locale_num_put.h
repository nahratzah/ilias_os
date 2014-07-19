#ifndef _STDIMPL_LOCALE_NUM_PUT_H_
#define _STDIMPL_LOCALE_NUM_PUT_H_

#include <cdecl.h>
#include <type_traits>
#include <array>
#include <limits>
#include <string>

_namespace_begin(std)
namespace impl {


class num_encoder_base {
 protected:
  static constexpr char digits_char_lower[16] = { '0', '1', '2', '3',
                                                  '4', '5', '6', '7',
                                                  '8', '9', 'a', 'b',
                                                  'c', 'd', 'e', 'f' };
  static constexpr char digits_char_upper[16] = { '0', '1', '2', '3',
                                                  '4', '5', '6', '7',
                                                  '8', '9', 'A', 'B',
                                                  'C', 'D', 'E', 'F' };

  ~num_encoder_base() = default;

  template<typename T>
  static auto abs(const T&) ->
      enable_if_t<is_unsigned<T>::value, const T&>;
  template<typename T>
  static auto abs(const T&) ->
      enable_if_t<is_signed<T>::value, make_unsigned_t<T>>;
};


template<typename Char>
class basic_num_encoder
: public num_encoder_base
{
 public:
  using char_type = Char;

  basic_num_encoder(ios_base&);

  const _namespace(std)::ctype<char_type>& ctype;
  const array<char_type, 16> digits;

 protected:
  ~basic_num_encoder() = default;

 private:
  array<char_type, 16> get_digits_(bool);
};


template<typename Char, typename T>
class num_encoder
: public basic_num_encoder<Char>
{
  static_assert(numeric_limits<T>::is_specialized,
                "num_encoder requires initialized numeric_limits<T>.");
  static_assert(numeric_limits<T>::is_integer,
                "num_encoder can only encode integer types.");
  static_assert(numeric_limits<T>::digits != 0,
                "num_encoder can only encode fixed-size integers.");
  static_assert(numeric_limits<T>::digits10 != 0,
                "num_encoder requires numeric_limits<T>::digits10 to be set.");

 public:
  using char_type = Char;
  using value_type = T;

 private:
  static constexpr int hex_digits =
      (numeric_limits<uintptr_t>::digits + 3) / 4;
  static constexpr int oct_digits =
      (numeric_limits<uintptr_t>::digits + 2) / 3;
  static constexpr int dec_digits =
      numeric_limits<uintptr_t>::digits10;
  static constexpr int max_digits =
      (hex_digits > (oct_digits > dec_digits ? oct_digits : dec_digits) ?
       hex_digits :
       (oct_digits > dec_digits ? oct_digits : dec_digits));

  using data_type = array<char_type, max_digits>;

 public:
  num_encoder(value_type, ios_base&);

  basic_string_ref<char_type> get_digits() const noexcept;
  basic_string_ref<char_type> get_prefix() const noexcept;
  bool is_decimal() const noexcept;

 private:
  data_type data_;
  array<char_type, 2> prefix_;
  typename data_type::iterator data_begin_;  // First digit in data_.
  typename array<char_type, 2>::iterator prefix_begin_;
  bool decimal_;
};


template<typename CountT>
CountT count_grouping_sep(string_ref, CountT) noexcept;
template<typename Char, typename Iter>
Iter render_num_encoder_with_sep(Iter, basic_string_ref<Char>,
                                 string_ref, Char);
template<typename Char, typename Iter>
Iter render_num_encoder(Iter, ios_base&,
                        basic_string_ref<Char>, basic_string_ref<Char>, Char,
                        bool);
template<typename Char, typename Iter, typename T>
Iter render_num_encoder(Iter, ios_base&,
                        const num_encoder<Char, T>&, Char);


} /* namespace std::impl */
_namespace_end(std)

#include <stdimpl/locale_num_put-inl.h>

#endif /* _STDIMPL_LOCALE_NUM_PUT_H_ */
