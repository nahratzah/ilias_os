#ifndef _LOCALE_MISC_CONVERT_H_
#define _LOCALE_MISC_CONVERT_H_

#include <cdecl.h>
#include <string>
#include <streambuf>

_namespace_begin(std)


template<typename Codecvt, typename Elem = wchar_t,
         typename WideAlloc = allocator<Elem>,
         typename ByteAlloc = allocator<char>>
class wstring_convert {
 public:
  using byte_string = basic_string<char, char_traits<char>, ByteAlloc>;
  using wide_string = basic_string<Elem, char_traits<Elem>, WideAlloc>;
  using state_type = typename Codecvt::state_type;
  using int_type = typename wide_string::traits_type::int_type;

  wstring_convert(Codecvt* = new Codecvt());
  wstring_convert(Codecvt*, state_type);
  wstring_convert(const byte_string&, const wide_string& = wide_string());

  wide_string from_bytes(char);
  wide_string from_bytes(const char*);
  wide_string from_bytes(const byte_string&);
  wide_string from_bytes(
      basic_string_ref<char, typename byte_string::traits_type>);
  wide_string from_bytes(const char*, const char*);

  byte_string to_bytes(Elem);
  byte_string to_bytes(const Elem*);
  byte_string to_bytes(const wide_string&);
  byte_string to_bytes(
      basic_string_ref<Elem, typename wide_string::traits_type>);
  byte_string to_bytes(const Elem*, const Elem*);

  size_t converted() const;
  state_type state() const;

 private:
  unique_ptr<Codecvt> cvt_;
  size_t cvt_count_ = 0;
  state_type cvt_state_;
  byte_string byte_err_string_;
  wide_string wide_err_string_;
};

template<typename Codecvt, typename Elem = wchar_t,
         typename Tr = char_traits<Elem>>
class wbuffer_convert
: public basic_streambuf<Elem, Tr>
{
 public:
  using state_type = typename Tr::state_type;

  wbuffer_convert(streambuf* = nullptr, Codecvt* = new Codecvt(),
                  state_type = state_type());

  streambuf* rdbuf() const;
  streambuf* rdbuf(streambuf*);

  state_type state() const;

 protected:
  streamsize showmanyc() override;
  streamsize xsgetn(typename basic_streambuf<Elem, Tr>::char_type*, streamsize)
      override;
  typename basic_streambuf<Elem, Tr>::int_type underflow() override;
  typename basic_streambuf<Elem, Tr>::int_type uflow() override;

  streamsize xsputn(const typename basic_streambuf<Elem, Tr>::char_type*,
                    streamsize) override;
  typename basic_streambuf<Elem, Tr>::int_type overflow(
      typename basic_streambuf<Elem, Tr>::int_type) override;

 private:
  auto underflow_(bool) ->
      tuple<typename basic_streambuf<Elem, Tr>::int_type, streamsize, bool>;

  streambuf* buf_;
  unique_ptr<Codecvt> cvt_;
  state_type cvt_state_;
  const bool always_noconv_;
};


_namespace_end(std)

#include <locale_misc/convert-inl.h>

#endif /* _LOCALE_MISC_CONVERT_H_ */
