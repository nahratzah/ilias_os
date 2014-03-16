#ifndef _ABI_EXT_UNICODE_H_
#define _ABI_EXT_UNICODE_H_

#include <utility>
#include <abi/errno.h>
#include <abi/misc_int.h>

namespace __cxxabiv1 {
namespace ext {
namespace {


/*
 * Return the minimum number of bytes
 * required to represent argument in UTF-8.
 */
inline constexpr int utf8_need_bytes(wchar_t) noexcept;
/*
 * Return the mask of the first character in a multi-byte
 * UTF-8 encoding.
 */
inline constexpr char utf8_mask(int) noexcept;
/* Test if the given character is a valid unicode code-point. */
inline constexpr bool is_valid_unicode(wchar_t) noexcept;


/*
 * Functor, invoke with char and callback, invokes callback with
 * the wide character assembled from the input chars.
 * Converter is stateful.
 *
 * Synopsis:
 *
 * template<typename Char> class unicode_conv_in {
 *  public:
 *   template<typename CB> int operator()(Char, CB&);
 *       noexcept(noexcept(std::declval<CB>()(std::declval<const wchar_t>())));
 *   bool is_clear() const noexcept;
 *   void reset() noexcept;
 * };
 */
template<typename Char> class unicode_conv_in;

/*
 * Functor, invoke with char and callback, invokes callback with
 * the converted character from the input chars.
 * Converter is stateful.
 *
 * Synopsis:
 *
 * template<typename Char> class unicode_conv_out {
 *  public:
 *   template<typename CB> int operator()(wchar_t, CB&);
 *       noexcept(noexcept(std::declval<CB>()(std::declval<const Char>())));
 *   bool is_clear() const noexcept;
 *   void reset() noexcept;
 * };
 */
template<typename Char> class unicode_conv_out;


template<> class unicode_conv_in<char> {
 public:
  template<typename CB> int operator()(char, CB&)
      noexcept(noexcept(std::declval<CB>()(std::declval<const wchar_t>())));
  bool is_clear() const noexcept;
  void reset() noexcept;

 private:
  wchar_t partial_;
  unsigned char need_ = 0;
  unsigned char initial_need_;
};

template<> class unicode_conv_in<char16_t> {
 public:
  template<typename CB> int operator()(char16_t, CB&)
      noexcept(noexcept(std::declval<CB>()(std::declval<const wchar_t>())));
  bool is_clear() const noexcept;
  void reset() noexcept;

 private:
  wchar_t partial_;
  bool need_ = false;
};

template<> class unicode_conv_in<char32_t> {
 public:
  template<typename CB> int operator()(char32_t, CB&)
      noexcept(noexcept(std::declval<CB>()(std::declval<const wchar_t>())));
  bool is_clear() const noexcept;
  void reset() noexcept;
};

template<> class unicode_conv_in<wchar_t> {
 public:
  template<typename CB> int operator()(wchar_t, CB&)
      noexcept(noexcept(std::declval<CB>()(std::declval<const wchar_t>())));
  bool is_clear() const noexcept;
  void reset() noexcept;
};


template<> class unicode_conv_out<char> {
 public:
  template<typename CB> int operator()(wchar_t, CB&)
      noexcept(noexcept(std::declval<CB>()(std::declval<const char>())));
  bool is_clear() const noexcept;
  void reset() noexcept;
};

template<> class unicode_conv_out<char16_t> {
 public:
  template<typename CB> int operator()(wchar_t, CB&)
      noexcept(noexcept(std::declval<CB>()(std::declval<const char32_t>())));
  bool is_clear() const noexcept;
  void reset() noexcept;
};

template<> class unicode_conv_out<char32_t> {
 public:
  template<typename CB> int operator()(wchar_t, CB&)
      noexcept(noexcept(std::declval<CB>()(std::declval<const char32_t>())));
  bool is_clear() const noexcept;
  void reset() noexcept;
};

template<> class unicode_conv_out<wchar_t> {
 public:
  template<typename CB> int operator()(wchar_t, CB&)
      noexcept(noexcept(std::declval<CB>()(std::declval<const wchar_t>())));
  bool is_clear() const noexcept;
  void reset() noexcept;
};


template<typename COut, typename CIn> class unicode_conv {
 public:
  template<typename CB> int operator()(CIn, CB&)
      noexcept(noexcept(std::declval<CB>()(std::declval<const COut>())));
  bool is_clear() const noexcept;
  void reset() noexcept;

 private:
  unicode_conv_in<CIn> in_;
  unicode_conv_out<COut> out_;
};

template<typename C> class unicode_conv<C, C> {
 public:
  template<typename CB> int operator()(C, CB&)
      noexcept(noexcept(std::declval<CB>()(std::declval<const C>())));
  bool is_clear() const noexcept;
  void reset() noexcept;

 private:
  unicode_conv_in<C> in_;  // For validation.
};


}}} /* namespace __cxxabiv1::ext::<unnamed> */

#include <abi/ext/unicode-inl.h>

#endif /* _ABI_EXT_UNICODE_H_ */
