#ifndef _ISTREAM_INL_H_
#define _ISTREAM_INL_H_

#include <istream>
#include <algorithm>
#include <ostream>
#include <locale_misc/locale.h>
#include <locale_misc/ctype.h>
#include <locale_misc/num_get.h>

_namespace_begin(std)


template<typename Char, typename Traits>
basic_istream<Char, Traits>::basic_istream(
    basic_streambuf<char_type, traits_type>* sb) {
  this->basic_ios<Char, Traits>::init(sb);
}

template<typename Char, typename Traits>
basic_istream<Char, Traits>::basic_istream(
    basic_istream&& rhs) {
  using _namespace(std)::swap;

  this->basic_ios<Char, Traits>::move(rhs);
  swap(gcount_, rhs.gcount_);
}

template<typename Char, typename Traits>
basic_istream<Char, Traits>::~basic_istream() noexcept {
  /* SKIP */
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator=(basic_istream&& rhs) ->
    basic_istream& {
  swap(rhs);
  return *this;
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::swap(basic_istream& rhs) -> void {
  using _namespace(std)::swap;

  this->basic_ios<Char, Traits>::swap(rhs);
  swap(gcount_, rhs.gcount_);
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (
    basic_istream& (*pf)(basic_istream&)) -> basic_istream& {
  return pf(*this);
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (
    basic_ios<Char, Traits>& (*pf)(basic_ios<Char, Traits>&)) ->
    basic_istream& {
  pf(*this);
  return *this;
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (
    ios_base& (*pf)(ios_base&)) -> basic_istream& {
  pf(*this);
  return *this;
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (bool& val) -> basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, val);
                      this->setstate(err);
                    });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (short& val) -> basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      long lval;
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, lval);

                      if (lval > numeric_limits<short>::max()) {
                        err |= ios_base::failbit;
                        val = numeric_limits<short>::max();
                      } else if (lval < numeric_limits<short>::min()) {
                        err |= ios_base::failbit;
                        val = numeric_limits<short>::min();
                      } else {
                        val = lval;
                      }

                      this->setstate(err);
                    });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (unsigned short& val) ->
    basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, val);
                      this->setstate(err);
                    });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (int& val) ->
    basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      long lval;
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, lval);

                      if (lval > numeric_limits<int>::max()) {
                        err |= ios_base::failbit;
                        val = numeric_limits<int>::max();
                      } else if (lval < numeric_limits<int>::min()) {
                        err |= ios_base::failbit;
                        val = numeric_limits<int>::min();
                      } else {
                        val = lval;
                      }

                      this->setstate(err);
                    });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (unsigned int& val) ->
    basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, val);
                      this->setstate(err);
                    });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (long& val) ->
    basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, val);
                      this->setstate(err);
                    });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (unsigned long& val) ->
    basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, val);
                      this->setstate(err);
                    });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (long long& val) ->
    basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, val);
                      this->setstate(err);
                    });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (unsigned long long& val) ->
    basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, val);
                      this->setstate(err);
                    });
}

#if _USE_INT128
template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (int128_t& val) ->
    basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, val);
                      this->setstate(err);
                    });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (uint128_t& val) ->
    basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, val);
                      this->setstate(err);
                    });
}
#endif // _USE_INT128

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (float& val) ->
    basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, val);
                      this->setstate(err);
                    });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (double& val) ->
    basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, val);
                      this->setstate(err);
                    });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (long double& val) ->
    basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, val);
                      this->setstate(err);
                    });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (void*& val) ->
    basic_istream& {
  using facet_t = num_get<char_type,
                          istreambuf_iterator<char_type, traits_type>>;

  return op_rshift_([&]() -> void {
                      ios_base::iostate err = ios_base::goodbit;
                      use_facet<facet_t>(this->getloc()).
                          get(*this, nullptr, *this, err, val);
                      this->setstate(err);
                    });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::operator>> (
    basic_streambuf<char_type, traits_type>* sb) -> basic_istream& {
  return unformatted_([&]() -> streamsize {
                        streamsize len = 0;
                        if (sb == nullptr ||
                            (len = impl::copybuf(*sb, *this->rdbuf())) == 0)
                          this->setstate(ios_base::failbit);
                        return len;
                      });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::gcount() const -> streamsize {
  return gcount_;
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::get() -> int_type {
  int_type rv = traits_type::eof();
  unformatted_([&]() -> streamsize {
                 rv = this->rdbuf()->sbumpc();
                 if (traits_type::eq_int_type(rv, traits_type::eof())) {
                   this->setstate(ios_base::failbit);
                   return 0;
                 }
                 return 1;
               });
  return rv;
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::get(char_type& c) -> basic_istream& {
  return unformatted_([&]() -> streamsize {
                        int_type cc = this->rdbuf()->sbumpc();
                        if (traits_type::eq_int_type(cc, traits_type::eof())) {
                          this->setstate(ios_base::failbit);
                          return 0;
                        }
                        c = traits_type::to_char_type(cc);
                        return 1;
                      });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::get(char_type* s, streamsize n) ->
    basic_istream& {
  return get(s, n, this->widen('\n'));
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::get(char_type* s, streamsize n,
                                      char_type delim) -> basic_istream& {
  /* Assign nul terminator now, in case unformatted_ decides not to
   * invoke this. */
  if (n >= 1) traits_type::assign(s[0], char_type());

  return unformatted_([&]() -> streamsize {
                        streamsize n_read = 0;
                        char_type* i = s;

                        bool eof_seen = false;
                        try {
                          bool delim_seen = false;
                          while (!eof_seen && !delim_seen && n > 1) {
                            const streamsize avail = this->rdbuf()->in_avail();
                            if (avail <= 0) {
                              int_type cc = this->rdbuf()->sgetc();
                              if (traits_type::eq_int_type(
                                      cc, traits_type::eof())) {
                                eof_seen = true;
                              } else {
                                const auto c = traits_type::to_char_type(cc);
                                if (traits_type::eq(c, delim)) {
                                  delim_seen = true;
                                } else {
                                  traits_type::assign(*i, c);
                                  ++i;
                                  ++n_read;
                                  --n;
                                  this->rdbuf()->sbumpc();
                                }
                              }
                            } else {
                              const streamsize n_get =
                                  this->rdbuf()->sgetn(i, min(avail, n - 1));
                              char_type*const end = find_if(
                                  i, i + n_get,
                                  [delim](char_type c) -> bool {
                                    return traits_type::eq(c, delim);
                                  });
                              const auto delta = (end - i);
                              delim_seen = (delta != n_get);
                              for (streamsize unget_count = n_get - delta;
                                   unget_count > 0;
                                   --unget_count)
                                this->rdbuf()->sungetc();
                              i = end;
                              n_read += delta;
                              n -= delta;
                            }
                          }
                        } catch (...) {
                          /* Handle potential overwrite from sgetn. */
                          traits_type::assign(s[0], char_type());
                          throw;
                        }

                        if (n >= 1) traits_type::assign(*i, char_type());

                        ios_base::iostate err = (n_read == 0 ?
                                                 ios_base::failbit :
                                                 ios_base::goodbit);
                        if (eof_seen) err |= ios_base::eofbit;
                        this->setstate(err);

                        return n_read;
                      });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::get(
    basic_streambuf<char_type, traits_type>& sb) -> basic_istream& {
  return get(sb, this->widen('\n'));
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::get(
    basic_streambuf<char_type, traits_type>& sb, char_type delim) ->
    basic_istream& {
  return unformatted_([&]() -> streamsize {
                        streamsize len = 0;
                        if ((len = impl::copybuf(sb, *this->rdbuf(),
                                                 delim)) == 0)
                          this->setstate(ios_base::failbit);
                        return len;
                      });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::getline(char_type* s, streamsize n) ->
    basic_istream& {
  return getline(s, n, this->widen('\n'));
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::getline(char_type* s, streamsize n,
                                          char_type delim) -> basic_istream& {
  /* Assign nul terminator now, in case unformatted_ decides not to
   * invoke this. */
  if (n >= 1) traits_type::assign(s[0], char_type());

  return unformatted_([&]() -> streamsize {
                        streamsize n_read = 0;
                        char_type* i = s;

                        bool eof_seen = false;
                        try {
                          bool delim_seen = false;
                          while (!eof_seen && !delim_seen && n > 1) {
                            const streamsize avail = this->rdbuf()->in_avail();
                            if (avail <= 0) {
                              int_type cc = this->rdbuf()->sbumpc();
                              if (traits_type::eq_int_type(
                                      cc, traits_type::eof())) {
                                eof_seen = true;
                              } else {
                                const auto c = traits_type::to_char_type(cc);
                                if (traits_type::eq(c, delim)) {
                                  delim_seen = true;
                                } else {
                                  traits_type::assign(*i, c);
                                  ++i;
                                  ++n_read;
                                  --n;
                                }
                              }
                            } else {
                              const streamsize n_get =
                                  this->rdbuf()->sgetn(i, min(avail, n - 1));
                              char_type*const end = find_if(
                                  i, i + n_get,
                                  [delim](char_type c) {
                                    return traits_type::eq(c, delim);
                                  });
                              const auto delta = (end - i);
                              delim_seen = (delta != n_get);
                              for (streamsize unget_count = n_get - delta;
                                   unget_count > 1;
                                   --unget_count)
                                this->rdbuf()->sungetc();
                              i = end;
                              n_read += delta;
                              n -= delta;
                            }

                            if (delim_seen) ++n_read;  // Delim was extracted.
                          }
                        } catch (...) {
                          /* Handle potential overwrite from sgetn. */
                          traits_type::assign(s[0], char_type());
                          throw;
                        }

                        if (n >= 1) traits_type::assign(*i, char_type());

                        ios_base::iostate err = (n_read == 0 ?
                                                 ios_base::failbit :
                                                 ios_base::goodbit);
                        if (eof_seen) err |= ios_base::eofbit;
                        this->setstate(err);

                        return n_read;
                      });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::ignore(streamsize n, int_type delim) ->
    basic_istream& {
  const bool unlimited = (n == numeric_limits<streamsize>::max());

  // XXX: implement peeking into stream, to skip chars more efficiently
  return unformatted_([&]() -> streamsize {
                        streamsize n_read = 0;
                        ios_base::iostate err = ios_base::goodbit;

                        while (unlimited || n_read < n) {
                          const int_type cc = this->rdbuf()->sbumpc();
                          if (traits_type::eq_int_type(cc,
                                                       traits_type::eof())) {
                            err = ios_base::eofbit;
                            break;  // GUARD
                          }

                          ++n_read;
                          if (traits_type::eq_int_type(cc, delim))
                            break;  // GUARD
                        }

                        if (n_read == 0) err |= ios_base::failbit;
                        this->setstate(err);
                        return n_read;
                      });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::peek() -> int_type {
  int_type rv = traits_type::eof();
  unformatted_([&]() -> streamsize {
                 rv = this->rdbuf()->sgetc();
                 if (traits_type::eq_int_type(rv, traits_type::eof())) {
                   this->setstate(ios_base::failbit);
                   return 0;
                 }
                 return 1;
               });
  return rv;
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::read(char_type* s, streamsize n) ->
    basic_istream& {
  return unformatted_([&]() -> streamsize {
                        streamsize n_read = 0;
                        if (n > 0) {
                          n_read = this->rdbuf()->sgetn(s, n);
                          if (n_read < n &&
                              traits_type::eq_int_type(this->rdbuf()->sgetc(),
                                                       traits_type::eof()))
                            this->setstate(ios_base::eofbit);
                        }
                        return n_read;
                      });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::readsome(char_type* s, streamsize n) ->
    streamsize {
  streamsize n_read = 0;

  unformatted_([&]() -> streamsize {
                 const streamsize avail = this->rdbuf()->in_avail();
                 if (avail < 0) {
                   this->setstate(ios_base::failbit);
                   return 0;
                 }
                 n = min(n, avail);

                 if (n > 0) {
                   n_read = this->rdbuf()->sgetn(s, n);
                   if (n_read < n &&
                       traits_type::eq_int_type(this->rdbuf()->sgetc(),
                                                traits_type::eof()))
                     this->setstate(ios_base::eofbit);
                 }
                 return n_read;
               });
  return n_read;
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::putback(char_type c) -> basic_istream& {
  return unformatted_([&]() -> streamsize {
                        if (traits_type::eq_int_type(
                                this->rdbuf()->sputbackc(c),
                                traits_type::eof())) {
                          this->setstate(ios_base::badbit);
                          return 0;
                        }
                        this->clear_(this->rdstate() & ~ios_base::eofbit,
                                     false);
                        return 0;
                      });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::ungetc() -> basic_istream& {
  return unformatted_([&]() -> streamsize {
                        if (traits_type::eq_int_type(this->rdbuf()->sungetc(),
                                                     traits_type::eof())) {
                          this->setstate(ios_base::badbit);
                          return 0;
                        }
                        this->clear_(this->rdstate() & ~ios_base::eofbit,
                                     false);
                        return 0;
                      });
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::sync() -> int {
  try {
    sentry s{ *this, true };
    if (!this->rdbuf() ||
        (s && this->rdbuf()->pubsync() == -1)) {
      this->setstate(ios_base::badbit);
      return -1;
    }
  } catch (...) {
    this->setstate_nothrow_(ios_base::badbit);
    if (this->exceptions() & ios_base::badbit) throw;
    return -1;
  }

  return 0;
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::tellg() -> pos_type {
  try {
    sentry s{ *this, true };
    if (s) return this->rdbuf()->pubseekoff(0, ios_base::cur, ios_base::in);
  } catch (...) {
    this->setstate_nothrow_(ios_base::badbit);
    if (this->exceptions() & ios_base::badbit) throw;
  }

  return pos_type(-1);
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::seekg(pos_type pos) -> basic_istream& {
  try {
    sentry s{ *this, true };
    bool fail = !s;
    if (!fail) {
      this->clear_(this->rdstate() & ~ios_base::eofbit, false);
      fail = (this->rdbuf()->pubseekpos(pos, ios_base::in) == pos_type(-1));
    }

    if (fail) this->setstate(ios_base::failbit);
  } catch (...) {
    this->setstate_nothrow_(ios_base::badbit);
    if (this->exceptions() & ios_base::badbit) throw;
  }

  return *this;
}

template<typename Char, typename Traits>
auto basic_istream<Char, Traits>::seekg(off_type off, ios_base::seekdir way) ->
    basic_istream& {
  try {
    sentry s{ *this, true };
    bool fail = !s;
    if (!fail) {
      this->clear_(this->rdstate() & ~ios_base::eofbit, false);
      fail = (this->rdbuf()->pubseekoff(off, way, ios_base::in) ==
              pos_type(-1));
    }

    if (fail) this->setstate(ios_base::failbit);
  } catch (...) {
    this->setstate_nothrow_(ios_base::badbit);
    if (this->exceptions() & ios_base::badbit) throw;
  }

  return *this;
}

template<typename Char, typename Traits>
template<typename Fn, typename... Args>
auto basic_istream<Char, Traits>::op_rshift_(Fn fn, Args&&... args) ->
    basic_istream& {
  try {
    sentry s{ *this };
    if (s)
      impl::invoke(fn, forward<Args>(args)...);
  } catch (...) {
    this->setstate_nothrow_(ios_base::badbit);
    if (this->exceptions() & ios_base::badbit) throw;
  }
  return *this;
}

template<typename Char, typename Traits>
template<typename Fn, typename... Args>
auto basic_istream<Char, Traits>::unformatted_(Fn fn, Args&&... args) ->
    basic_istream& {
  gcount_ = 0;
  try {
    sentry s{ *this, true };
    if (s)
      gcount_ = impl::invoke(fn, forward<Args>(args)...);
  } catch (...) {
    this->setstate_nothrow_(ios_base::badbit);
    if (this->exceptions() & ios_base::badbit) throw;
  }
  return *this;
}


template<typename Char, typename Traits>
auto operator>> (basic_istream<Char, Traits>& is, Char& c) ->
    basic_istream<Char, Traits>& {
  using traits_type = typename basic_istream<Char, Traits>::traits_type;
  using int_type = typename basic_istream<Char, Traits>::int_type;

  return is.unformatted_([&]() -> streamsize {
                           int_type cc = is.rdbuf()->sbumpc();
                           if (traits_type::eq_int_type(cc,
                                                        traits_type::eof())) {
                             is.setstate(ios_base::failbit |
                                         ios_base::eofbit);
                             return 0;
                           } else {
                             is.setstate(ios_base::goodbit);
                             c = traits_type::to_char_type(cc);
                             return 1;
                           }
                         });
}

template<typename Traits>
auto operator>> (basic_istream<char, Traits>& is, unsigned char& c) ->
    basic_istream<char, Traits>& {
  return is >> reinterpret_cast<char&>(c);
}

template<typename Traits>
auto operator>> (basic_istream<char, Traits>& is, signed char& c) ->
    basic_istream<char, Traits>& {
  return is >> reinterpret_cast<char&>(c);
}

template<typename Char, typename Traits>
auto operator>> (basic_istream<Char, Traits>& is, Char* s) ->
    basic_istream<Char, Traits>& {
  using traits_type = typename basic_istream<Char, Traits>::traits_type;
  using int_type = typename basic_istream<Char, Traits>::int_type;
  using char_type = typename basic_istream<Char, Traits>::char_type;

  /* Calculate n: max number of characters read. */
  streamsize n = is.width();
  if (n <= 0) n = SIZE_MAX - 1U;

  /* Assign nul terminator now, in case unformatted_ decides not to
   * invoke this. */
  traits_type::assign(s[0], char_type());

  return is.op_rshift_([&]() -> streamsize {
                         streamsize n_read = 0;
                         auto& buf = *is.rdbuf();
                         const _namespace(std)::ctype<char_type>& ctype =
                             use_facet<_namespace(std)::ctype<char_type>>(
                                 is.getloc());

                         ios_base::iostate err = ios_base::goodbit;
                         bool eof_seen = false;
                         try {
                           while (n > 0) {
                             int_type cc = buf.sgetc();
                             if (traits_type::eq_int_type(traits_type::eof(),
                                                          cc)) {
                               eof_seen = true;
                               break;  // GUARD
                             }
                             const char_type c = traits_type::to_char_type(cc);
                             if (ctype.is(ctype.space, c))
                               break;  // GUARD
                             traits_type::assign(*s, c);

                             buf.sbumpc();
                             ++s;
                             --n;
                             ++n_read;
                           }
                         } catch (...) {
                           traits_type::assign(*s, char_type());
                           throw;
                         }

                         traits_type::assign(*s, char_type());
                         if (n_read == 0)
                           err = ios_base::failbit;
                         if (eof_seen) err |= ios_base::eofbit;
                         is.setstate(err);
                         return n_read;
                       });
}

template<typename Traits>
auto operator>> (basic_istream<char, Traits>& is, unsigned char* s) ->
    basic_istream<char, Traits>& {
  return is >> reinterpret_cast<char*>(s);
}

template<typename Traits>
auto operator>> (basic_istream<char, Traits>& is, signed char* s) ->
    basic_istream<char, Traits>& {
  return is >> reinterpret_cast<char*>(s);
}


template<typename Char, typename Traits>
basic_istream<Char, Traits>::sentry::sentry(basic_istream& is, bool noskipws)
: is_(is)
{
  if (is_.good()) {
    if (is_.tie()) is_.tie()->flush();

    /* Consume white space. */
    if (!noskipws && (is_.flags() & ios_base::skipws)) {
      const ctype<Char>& ct = use_facet<ctype<Char>>(is_.getloc());

      // XXX change to use in_avail(), sgetn().
      for (int_type cc = is_.rdbuf()->sgetc();
           ;
           cc = is_.rdbuf()->sgetc()) {
        /* Handle eof. */
        if (traits_type::eq_int_type(cc, traits_type::eof())) {
          ok_ = false;
          is_.setstate(ios_base::failbit | ios_base::eofbit);
          return;
        }

        /* Stop at non-space character. */
        if (!ct.is(ctype_base::space, traits_type::to_char_type(cc)))
          break;  // GUARD

	/* Claim character. */
        is_.rdbuf()->sbumpc();
      }
    }
  }

  ok_ = is_.good();
  if (!ok_) is_.setstate(ios_base::failbit);
}

template<typename Char, typename Traits>
basic_istream<Char, Traits>::sentry::operator bool() const noexcept {
  return ok_;
}


template<typename Char, typename Traits>
auto ws(basic_istream<Char, Traits>& is) -> basic_istream<Char, Traits>& {
  using traits_type = typename basic_istream<Char, Traits>::traits_type;
  using int_type = typename basic_istream<Char, Traits>::int_type;
  using char_type = typename basic_istream<Char, Traits>::char_type;
  using sentry = typename basic_istream<Char, Traits>::sentry;

  try {
    sentry s{ is, true };
    if (_predict_false(!s)) return is;

    const _namespace(std)::ctype<char_type>& ctype =
        use_facet<_namespace(std)::ctype<char_type>>(is.getloc());
    ios_base::iostate err = ios_base::goodbit;
    for (;;) {
      int_type cc = is.rdbuf()->sgetc();
      if (traits_type::eq_int_type(cc, traits_type::eof())) {
        err |= ios_base::eofbit;
        break;
      }

      char_type c = traits_type::to_char_type(cc);
      if (ctype.is(ctype.space, c)) break;
      is.rdbuf()->sbumpc();
    }
    is.setstate(err);
  } catch (...) {
    is.setstate_nothrow_(ios_base::badbit);
    if (is.exceptions() & ios_base::badbit) throw;
  }
  return is;
}


_namespace_end(std)

#endif /* _ISTREAM_INL_H_ */
