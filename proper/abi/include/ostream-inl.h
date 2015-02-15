#ifndef _OSTREAM_INL_H_
#define _OSTREAM_INL_H_

#include <ostream>
#include <ios>
#include <exception>
#include <iterator>
#include <algorithm>
#include <locale_misc/locale.h>
#include <locale_misc/num_put.h>
#include <string>
#include <streambuf>
#include <stdimpl/invoke.h>

_namespace_begin(std)


template<typename Char, typename Traits>
basic_ostream<Char, Traits>::basic_ostream(
    basic_streambuf<char_type, traits_type>* sb) {
  this->basic_ios<Char, Traits>::init(sb);
}

template<typename Char, typename Traits>
basic_ostream<Char, Traits>::~basic_ostream() noexcept {
  /* SKIP */
}

template<typename Char, typename Traits>
basic_ostream<Char, Traits>::basic_ostream(basic_ostream&& other) {
  this->basic_ios<Char, Traits>::move(other);
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator=(basic_ostream&& other) ->
    basic_ostream& {
  this->swap(other);
  return *this;
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::swap(basic_ostream& other) -> void {
  this->basic_ios<Char, Traits>::swap(other);
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::tellp() -> pos_type {
  if (this->fail()) return pos_type(-1);
  return this->rdbuf()->pubseekoff(0, ios_base::cur, ios_base::out);
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::seekp(pos_type pos) ->
    basic_ostream& {
  sentry s{ *this };
  if (s) {
    if (this->rdbuf()->pubseekpos(pos, ios_base::out) == pos_type(-1))
      this->setstate(ios_base::failbit);
  }
  return *this;
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::seekp(off_type off, ios_base::seekdir dir) ->
    basic_ostream& {
  sentry s{ *this };
  if (s) {
    if (this->rdbuf()->pubseekoff(off, dir, ios_base::out) == pos_type(-1))
      this->setstate(ios_base::failbit);
  }
  return *this;
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (
    basic_ostream& (*pf)(basic_ostream&)) -> basic_ostream& {
  return pf(*this);
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (
    basic_ios<Char, Traits>& (*pf)(basic_ios<Char, Traits>&)) ->
    basic_ostream& {
  pf(*this);
  return *this;
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (
    ios_base& (*pf)(ios_base&)) ->
    basic_ostream& {
  pf(*this);
  return *this;
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (bool val) -> basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), val).
                          failed();
                    });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (short val) -> basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      ios_base::fmtflags baseflags =
                          ios_base::flags() & ios_base::basefield;
                      long vlong = (baseflags == ios_base::oct ||
                                    baseflags == ios_base::hex ?
                                    static_cast<long>(
                                        static_cast<unsigned short>(val)) :
                                    static_cast<long>(val));
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), vlong).
                          failed();
                    });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (unsigned short val) ->
    basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      unsigned long vlong = static_cast<unsigned long>(val);
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), vlong).
                          failed();
                    });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (int val) -> basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      ios_base::fmtflags baseflags =
                          ios_base::flags() & ios_base::basefield;
                      long vlong = (baseflags == ios_base::oct ||
                                    baseflags == ios_base::hex ?
                                    static_cast<long>(
                                        static_cast<unsigned int>(val)) :
                                    static_cast<long>(val));
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), vlong).
                          failed();
                    });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (unsigned int val) ->
    basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      unsigned long vlong = static_cast<unsigned long>(val);
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), vlong).
                          failed();
                    });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (long val) -> basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), val).
                          failed();
                    });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (unsigned long val) ->
    basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), val).
                          failed();
                    });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (long long val) ->
    basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), val).
                          failed();
                    });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (unsigned long long val) ->
    basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), val).
                          failed();
                    });
}

#if _USE_INT128
template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (int128_t val) ->
    basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), val).
                          failed();
                    });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (uint128_t val) ->
    basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), val).
                          failed();
                    });
}
#endif // _USE_INT128

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (float val) ->
    basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      double dval = static_cast<double>(val);
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), dval).
                          failed();
                    });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (double val) ->
    basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), val).
                          failed();
                    });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (long double val) ->
    basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), val).
                          failed();
                    });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (const void* val) ->
    basic_ostream& {
  using facet_t = num_put<char_type,
                          ostreambuf_iterator<char_type, traits_type>>;

  return op_lshift_([&]() -> bool {
                      return use_facet<facet_t>(this->getloc()).
                          put(*this, *this, this->fill(), val).
                          failed();
                    });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::operator<< (
    basic_streambuf<Char, Traits>* sb) -> basic_ostream& {
  return unformatted_([&]() {
                        return (sb == nullptr ||
                                impl::copybuf(*this->rdbuf(), *sb) == 0);
                      });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::put(char_type ch) -> basic_ostream& {
  return unformatted_([&]() -> bool {
                        return (traits_type::eq_int_type(
                                    this->rdbuf()->sputc(ch),
                                    traits_type::eof()));
                      });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::write(const char_type* s, streamsize n) ->
    basic_ostream& {
  return unformatted_([&]() -> bool {
                        return (this->rdbuf()->sputn(s, n) != n);
                      });
}

template<typename Char, typename Traits>
auto basic_ostream<Char, Traits>::flush() -> basic_ostream& {
  if (_predict_true(this->rdbuf() != nullptr)) {
    sentry s{ *this };
    if (_predict_true(s)) {
      if (_predict_false(this->rdbuf()->pubsync() == -1))
        this->setstate(ios_base::badbit);
    }
  }
  return *this;
}

template<typename Char, typename Traits>
template<typename Fn, typename... Args>
auto basic_ostream<Char, Traits>::op_lshift_(Fn fn, Args&&... args) ->
    basic_ostream& {
  try {
    sentry s{ *this };
    if (s) {
      const bool failed = impl::invoke(fn, forward<Args>(args)...);
      if (failed) this->setstate(ios_base::failbit | ios_base::badbit);
    }
  } catch (...) {
    this->setstate_nothrow_(ios_base::badbit);
    if (this->exceptions() & ios_base::badbit) throw;
  }
  return *this;
}

template<typename Char, typename Traits>
template<typename Fn, typename... Args>
auto basic_ostream<Char, Traits>::unformatted_(Fn fn, Args&&... args) ->
    basic_ostream& {
  try {
    sentry s{ *this };
    if (s) {
      const bool failed = impl::invoke(fn, forward<Args>(args)...);
      if (failed) this->setstate(ios_base::failbit | ios_base::badbit);
    }
  } catch (...) {
    this->setstate_nothrow_(ios_base::badbit);
    if (this->exceptions() & ios_base::badbit) throw;
  }
  return *this;
}


template<typename Char, typename Traits>
auto operator<< (basic_ostream<Char, Traits>& os, Char ch) ->
    basic_ostream<Char, Traits>& {
  return impl::op_lshift_stream(os, basic_string_ref<Char, Traits>(&ch, 1));
}

template<typename Char, typename Traits>
auto operator<< (basic_ostream<Char, Traits>& os, char ch) ->
    basic_ostream<Char, Traits>& {
  return impl::op_lshift_stream(os, basic_string_ref<char>(&ch, 1),
                                [&os](char c) { return os.widen(c); });
}

template<typename Traits>
auto operator<< (basic_ostream<char, Traits>& os, char ch) ->
    basic_ostream<char, Traits>& {
  return impl::op_lshift_stream(os, basic_string_ref<char, Traits>(&ch, 1));
}

template<typename Traits>
auto operator<< (basic_ostream<char, Traits>& os, signed char ch) ->
    basic_ostream<char, Traits>& {
  return os << static_cast<char>(ch);
}

template<typename Traits>
auto operator<< (basic_ostream<char, Traits>& os, unsigned char ch) ->
    basic_ostream<char, Traits>& {
  return os << static_cast<char>(ch);
}

template<typename Char, typename Traits>
auto operator<< (basic_ostream<Char, Traits>& os, const Char* s) ->
    basic_ostream<Char, Traits>& {
  return impl::op_lshift_stream(os, basic_string_ref<Char, Traits>(s));
}

template<typename Char, typename Traits>
auto operator<< (basic_ostream<Char, Traits>& os, const char* s) ->
    basic_ostream<Char, Traits>& {
  return impl::op_lshift_stream(os, basic_string_ref<char>(s),
                                [&os](char c) { return os.widen(c); });
}

template<typename Traits>
auto operator<< (basic_ostream<char, Traits>& os, const char* s) ->
    basic_ostream<char, Traits>& {
  return impl::op_lshift_stream(os, basic_string_ref<char, Traits>(s));
}

template<typename Traits>
auto operator<< (basic_ostream<char, Traits>& os, const signed char* s) ->
    basic_ostream<char, Traits>& {
  return os << reinterpret_cast<const char*>(s);
}

template<typename Traits>
auto operator<< (basic_ostream<char, Traits>& os, const unsigned char* s) ->
    basic_ostream<char, Traits>& {
  return os << reinterpret_cast<const char*>(s);
}


template<typename Char, typename Traits>
basic_ostream<Char, Traits>::sentry::sentry(basic_ostream& os)
: os_(os)
{
  if (os_.good() && os_.tie())
    os_.tie()->flush();
  ok_ = os_.good();
  if (!ok_) os_.setstate(ios_base::failbit);
}

template<typename Char, typename Traits>
basic_ostream<Char, Traits>::sentry::~sentry() noexcept {
  if ((os_.flags() & ios_base::unitbuf) == ios_base::unitbuf &&
      !uncaught_exception() && os_.good()) {
    if (os_.rdbuf()->pubsync() == -1)
      os_.setstate_nothrow_(ios_base::badbit);
  }
}

template<typename Char, typename Traits>
basic_ostream<Char, Traits>::sentry::operator bool() const noexcept {
  return ok_;
}


template<typename Char, typename Traits>
auto endl(basic_ostream<Char, Traits>& os) ->
    basic_ostream<Char, Traits>& {
  return os.put(os.widen("\n")).flush();
}

template<typename Char, typename Traits>
auto ends(basic_ostream<Char, Traits>& os) ->
    basic_ostream<Char, Traits>& {
  return os.put(Char());
}

template<typename Char, typename Traits>
auto flush(basic_ostream<Char, Traits>& os) ->
    basic_ostream<Char, Traits>& {
  return os.flush();
}

template<typename Char, typename Traits, typename T>
auto operator<< (basic_ostream<Char, Traits>&& os, const T& v) ->
    basic_ostream<Char, Traits>& {
  return os << v;
}


_namespace_end(std)

#endif /* _OSTREAM_INL_H_ */
