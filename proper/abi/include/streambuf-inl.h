#ifndef _STREAMBUF_INL_H_
#define _STREAMBUF_INL_H_

#include <streambuf>

_namespace_begin(std)


#if _ILIAS_LOCALE
template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::pubimbue(const locale& loc) -> locale {
  imbue(loc);
  return exchange(loc_, loc);
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::getloc() const -> locale {
  return loc_;
}
#endif

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::pubsetbuf(char_type* s, streamsize n) ->
    basic_streambuf<Char, Traits>* {
  return setbuf(s, n);
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::pubseekoff(
    off_type off, ios_base::seekdir way, ios_base::openmode which) ->
    pos_type {
  return seekoff(off, way, which);
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::pubseekpos(
    pos_type pos, ios_base::openmode which) ->
    pos_type {
  return seekpos(pos, which);
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::pubsync() -> int {
  return sync();
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::in_avail() -> streamsize {
  return (gptr_ < egptr_ ? egptr_ - gptr_ : showmanyc());
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::snextc() -> int_type {
  return sbumpc();
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::sbumpc() -> int_type {
  __builtin_prefetch(gptr_, 0, 0);
  return (_predict_true(gptr_ < egptr_) ?
          traits_type::to_int_type(*gptr_++) :
          uflow());
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::sgetc() -> int_type {
  __builtin_prefetch(gptr_, 0, 0);
  return (_predict_true(gptr_ < egptr_) ?
          traits_type::to_int_type(*gptr_) :
          underflow());
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::sgetn(char_type* s, streamsize n) ->
    streamsize {
  return xsgetn(s, n);
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::sputbackc(char_type c) -> int_type {
  if (_predict_true(eback_ < gptr_ && traits_type::eq(*(gptr_ - 1), c)))
    return traits_type::to_int_type(*--gptr_);
  else
    return pbackfail(traits_type::to_int_type(c));
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::sungetc() -> int_type {
  if (_predict_true(eback_ < gptr_))
    return traits_type::to_int_type(*--gptr_);
  else
    return pbackfail();
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::sputc(char_type c) -> int_type {
  if (_predict_true(pptr_ < epptr_))
    return traits_type::to_int_type(*pptr_++ = c);
  else
    return overflow(traits_type::to_int_type(c));
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::sputn(const char_type* s, streamsize n) ->
    streamsize {
  return xsputn(s, n);
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::swap(basic_streambuf& rhs) noexcept ->
    void {
  using _namespace(std)::swap;

  swap(eback_, rhs.eback_);
  swap(gptr_, rhs.gptr_);
  swap(egptr_, rhs.egptr_);
  swap(pbase_, rhs.pbase_);
  swap(pptr_, rhs.pptr_);
  swap(epptr_, rhs.epptr_);
#if _ILIAS_LOCALE
  swap(loc_, rhs.loc_);
#endif
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::eback() const noexcept -> char_type* {
  return eback_;
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::gptr() const noexcept -> char_type* {
  return gptr_;
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::egptr() const noexcept -> char_type* {
  return egptr_;
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::gbump(int n) -> void {
  gptr_ += n;
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::setg(char_type* gbeg, char_type* gnext,
                                         char_type* gend) -> void {
  eback_ = gbeg;
  gptr_ = gnext;
  egptr_ = gend;
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::pbase() const noexcept -> char_type* {
  return pbase_;
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::pptr() const noexcept -> char_type* {
  return pptr_;
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::epptr() const noexcept -> char_type* {
  return epptr_;
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::pbump(int n) -> void {
  pptr_ += n;
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::setp(char_type* pbeg, char_type* pend) ->
    void {
  pbase_ = pbeg;
  pptr_ = pbeg;
  epptr_ = pend;
}

#if _ILIAS_LOCALE
template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::imbue(const locale&) -> void {}
#endif

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::setbuf(char_type*, streamsize) ->
    basic_streambuf* {
  return this;
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::seekoff(off_type, ios_base::seekdir,
                                            ios_base::openmode) ->
    pos_type {
  return pos_type(off_type(-1));
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::seekpos(pos_type, ios_base::openmode) ->
    pos_type {
  return pos_type(off_type(-1));
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::sync() -> int {
  return 0;
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::showmanyc() -> streamsize {
  return 0;
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::xsgetn(char_type* s, streamsize n) ->
    streamsize {
  streamsize read_count = 0;
  while (n > 0) {
    if (gptr_ < egptr_) {
      streamsize spc = min(streamsize(egptr_ - gptr_), n);
      traits_type::copy(s, gptr_, spc);
      gptr_ += spc;
      s += spc;
      n -= spc;
      read_count += spc;
    } else if (traits_type::eq_int_type(underflow(), traits_type::eof())) {
      break;
    }
  }
  return read_count;
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::underflow() -> int_type {
  return traits_type::eof();
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::uflow() -> int_type {
  if (traits_type::eq_int_type(underflow(), traits_type::eof()))
    return traits_type::eof();
  return traits_type::to_int_type(*gptr());
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::pbackfail(int_type) -> int_type {
  return traits_type::eof();
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::xsputn(const char_type* s, streamsize n) ->
    streamsize {
  streamsize write_count = 0;
  while (n > 0) {
    if (pptr_ < epptr_) {
      streamsize spc = min(streamsize(epptr_ - pptr_), n);
      traits_type::copy(pptr_, s, spc);
      pptr_ += spc;
      s += spc;
      n -= spc;
      write_count += spc;
    } else if (traits_type::eq_int_type(overflow(), traits_type::eof())) {
      break;
    }
  }
  return write_count;
}

template<typename Char, typename Traits>
auto basic_streambuf<Char, Traits>::overflow(int_type) -> int_type {
  return traits_type::eof();
}


namespace impl {


template<typename Char, typename Traits>
streamsize copybuf(basic_streambuf<Char, Traits>& dst,
                   basic_streambuf<Char, Traits>& src) {
  assert(&dst != &src);
  streamsize count = 0;

  for (;;) {
    /*
     * Figure out avail bytes.  Clamp to INT_MAX, otherwise
     * dst.pbump() and src.gbump() will fail.
     */
    streamsize dst_avail = dst.epptr() - dst.pptr();
    streamsize src_avail = src.egptr() - src.gptr();
    if (dst_avail > INT_MAX) dst_avail = INT_MAX;
    if (src_avail > INT_MAX) src_avail = INT_MAX;

    if (dst_avail == 0 && src_avail == 0) {
      /* No buffer available: transfer a single char across. */
      const typename Traits::int_type cc = src.sgetc();
      if (Traits::eq_int_type(cc, Traits::eof())) break;
      const typename Traits::char_type c = Traits::to_char_type(cc);
      if (Traits::eq_int_type(dst.sputc(c), Traits::eof())) break;
      src.sbumpc();
      ++count;
    } else if (dst_avail == 0) {
      /* Dst buffer unavailable: invoke virtual function for transfer. */
      const streamsize put_sz = dst.sputn(src.gptr(), src_avail);
      if (put_sz == 0) break;
      count += put_sz;
      src.gbump(put_sz);
    } else if (src_avail == 0) {
      /* Src buffer unavailable: invoke virtual function for transfer. */
      const streamsize get_sz = src.sgetn(dst.pptr(), dst_avail);
      if (get_sz == 0) break;
      count += get_sz;
      dst.pbump(get_sz);
    } else {
      /* Buffers available, use memcpy to transfer. */
      auto copylen = min(dst_avail, src_avail);
      Traits::copy(dst.pptr(), src.gptr(), copylen);
      src.gbump(copylen);
      dst.pbump(copylen);
      count += copylen;
    }
  }
  return count;
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STREAMBUF_INL_H_ */
