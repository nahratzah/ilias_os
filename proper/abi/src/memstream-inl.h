#ifndef _MEMSTREAM_INL_H_
#define _MEMSTREAM_INL_H_

#include "memstream.h"
#include <abi/misc_int.h>

_namespace_begin(std)


template<typename Char, typename Traits>
basic_memstreambuf<Char, Traits>::basic_memstreambuf(ios_base::openmode mode)
: mode_(mode)
{}

template<typename Char, typename Traits>
basic_memstreambuf<Char, Traits>::basic_memstreambuf(char_type*& pub_buf,
                                                     size_type& pub_len,
                                                     ios_base::openmode mode)
: mode_(mode),
  pub_buf_(&pub_buf),
  pub_len_(&pub_len)
{
  buf_ = static_cast<char_type*>(malloc(sizeof(char_type)));
  if (buf_ == nullptr) throw bad_alloc();
  traits_type::assign(*buf_, char_type());
  len_ = 0;
  buf_end_ = buf_;
}

template<typename Char, typename Traits>
basic_memstreambuf<Char, Traits>::basic_memstreambuf(char_type*& pub_buf,
                                                     size_type& pub_len,
                                                     char_type* init_buf,
                                                     size_type init_len,
                                                     ios_base::openmode mode)
    noexcept
: mode_(mode),
  buf_(init_buf),
  buf_end_(init_buf + init_len),
  len_(init_len),
  pub_buf_(&pub_buf),
  pub_len_(&pub_len)
{}

template<typename Char, typename Traits>
basic_memstreambuf<Char, Traits>::basic_memstreambuf(no_publish,
                                                     char_type* init_buf,
                                                     size_type init_len,
                                                     ios_base::openmode mode)
    noexcept
: mode_(mode),
  buf_(init_buf),
  buf_end_(init_buf + init_len),
  len_(init_len)
{}

template<typename Char, typename Traits>
basic_memstreambuf<Char, Traits>::basic_memstreambuf(
    basic_memstreambuf&& other)
: mode_(other.mode_)
{
  other.swap(*this);
}

template<typename Char, typename Traits>
basic_memstreambuf<Char, Traits>::~basic_memstreambuf() noexcept {
  sync();
}

template<typename Char, typename Traits>
auto basic_memstreambuf<Char, Traits>::operator=(basic_memstreambuf&& other) ->
    basic_memstreambuf& {
  basic_memstreambuf(move(other)).swap(*this);
  return *this;
}

template<typename Char, typename Traits>
auto basic_memstreambuf<Char, Traits>::swap(basic_memstreambuf& other) ->
    void {
  using _namespace(std)::swap;

  this->basic_streambuf<Char, Traits>::swap(other);
  swap(mode_, other.mode_);
  swap(buf_, other.buf_);
  swap(len_, other.len_);
  swap(pub_buf_, other.pub_buf_);
  swap(pub_len_, other.pub_len_);
}

template<typename Char, typename Traits>
auto basic_memstreambuf<Char, Traits>::underflow() -> int_type {
  if (this->gptr() < this->egptr())
    return traits_type::to_int_type(*this->egptr());
  return traits_type::eof();
}

template<typename Char, typename Traits>
auto basic_memstreambuf<Char, Traits>::pbackfail(int_type c) -> int_type {
  if (this->gptr() == this->eback()) return traits_type::eof();

  char_type*const pos = this->gptr() - 1;
  if (traits_type::eq_int_type(c, traits_type::eof()) ||
      traits_type::eq_int_type(traits_type::to_int_type(*pos), c)) {
    /* SKIP */
  } else if ((mode_ & ios_base::out) == ios_base::out) {
    traits_type::assign(*pos, traits_type::to_char_type(c));
  } else {
    return traits_type::eof();
  }

  this->gbump(-1);
  return traits_type::not_eof(c);
}

template<typename Char, typename Traits>
auto basic_memstreambuf<Char, Traits>::overflow(int_type c) -> int_type {
  if (!traits_type::eq_int_type(c, traits_type::eof())) {
    if (!extend_(1)) return traits_type::eof();
    assert(this->pptr() < this->epptr());
    this->sputc(traits_type::to_char_type(c));
  }
  return traits_type::not_eof(c);
}

template<typename Char, typename Traits>
auto basic_memstreambuf<Char, Traits>::setbuf(char_type*, streamsize) ->
    basic_streambuf<Char, Traits>* {
  // Implementation defined behaviour: do nothing.
  return this;
}

template<typename Char, typename Traits>
auto basic_memstreambuf<Char, Traits>::seekoff(
    off_type off, ios_base::seekdir way, ios_base::openmode which) ->
    pos_type {
  off_type new_off;
  const auto end = uintptr_t((this->epptr() == nullptr ?
                              this->egptr() :
                              this->epptr()) -
                             buf_);

  switch (way) {
  case ios_base::cur:
    /* New offset relative to current offset. */
    switch (which & (ios_base::in | ios_base::out)) {
    case ios_base::in:
      new_off = size_type(this->gptr() - this->eback()) + off;
      break;
    case ios_base::out:
      new_off = size_type(this->pptr() - this->pbase()) + off;
      break;
    default:
      return pos_type(off_type(-1));  // Cannot reposition both in rel mode.
    }
    break;

  case ios_base::beg:
    new_off = off;
    break;

  case ios_base::end:
    new_off = end - off;
    break;

  default:
    return pos_type(off_type(-1));  // Invalid argument.
  }

  if ((which & ios_base::in) == ios_base::in &&
      this->gptr() == nullptr && off != 0)
    return pos_type(off_type(-1));
  if ((which & ios_base::out) == ios_base::out &&
      this->pptr() == nullptr && off != 0)
    return pos_type(off_type(-1));

  /* Validate new offset. */
  if (off < 0 || make_unsigned_t<off_type>(off) >= end)
    return pos_type(off_type(-1));

  /* Update requested pointers. */
  if ((which & ios_base::in) == ios_base::in)
    this->setg(this->eback(), this->eback() + off, this->egptr());
  if ((which & ios_base::out) == ios_base::out) {
    this->setp(this->pbase(), this->epptr());
    while (_predict_false(off > INT_MAX)) {
      this->pbump(INT_MAX);
      off -= INT_MAX;
    }
    this->pbump(off);
  }

  /* Return new offset. */
  return pos_type(off);
}

template<typename Char, typename Traits>
auto basic_memstreambuf<Char, Traits>::seekpos(
    pos_type p, ios_base::openmode which) -> pos_type {
  return seekoff(p - pos_type(0), ios_base::beg, which);
}

template<typename Char, typename Traits>
auto basic_memstreambuf<Char, Traits>::sync() -> int {
  if ((mode_ & ios_base::out) != ios_base::out) return 0;

  /* Append nul char_type (epptr always points at the end of the buffer). */
  if (this->epptr()) *this->epptr() = char_type();

  if (pub_buf_) *pub_buf_ = buf_;
  if (pub_len_) *pub_len_ = len_;
  return 0;
}

template<typename Char, typename Traits>
auto basic_memstreambuf<Char, Traits>::extend_(size_type n) -> bool {
  using abi::umul_overflow;
  using abi::addc;

  assert((mode_ & ios_base::out) == ios_base::out);

  /* No allocation required. */
  if (uintptr_t(this->epptr() - this->pptr()) >= n) return true;

  /* If there is sufficient space, just mark it as initialized. */
  if (uintptr_t(buf_end_ - this->pptr()) >= n) {
    size_type cur_pos = this->pptr() - this->pbase();
    this->setp(this->pbase(), this->epptr() + n);
    while (_predict_false(cur_pos > INT_MAX)) {
      this->pbump(INT_MAX);
      cur_pos -= INT_MAX;
    }
    this->pbump(cur_pos);

    if ((mode_ & ios_base::in) == ios_base::in)
      this->setg(this->eback(), this->gptr(), this->epptr());
    return true;
  }

  const char_type*const begin = this->buf_;
  const char_type*const end = this->epptr();
  const size_type cur_size = end - begin;
  char_type* buf = nullptr;
  size_type new_size;

  /* Allocate buffer. */
  tie(buf, new_size) = extend_realloc_(buf_, cur_size, n);
  if (!buf) return false;
  /* No exceptions past this point. */

  /* Update get pointers. */
  if ((mode_ & ios_base::in) == ios_base::in)
    this->setg(buf, buf + (this->gptr() - begin), buf + cur_size + n);
  /* Update put pointers. */
  size_type cur_pos = this->pptr() - begin;
  this->setp(buf, buf + cur_size + n);
  while (_predict_false(cur_pos > INT_MAX)) {
    this->pbump(INT_MAX);
    cur_pos -= INT_MAX;
  }
  this->pbump(cur_pos);
  buf_ = buf;
  buf_end_ = buf + new_size;

  return true;
}

/*
 * Allocate at least (cur_size+n+1) bytes, by reallocing buf.
 */
template<typename Char, typename Traits>
auto basic_memstreambuf<Char, Traits>::extend_realloc_(char_type* buf,
                                                       size_type cur_size,
                                                       size_type n) ->
    pair<char_type*, size_type> {
  size_type new_size;

  if (n < cur_size && SIZE_MAX / sizeof(char_type) / 2U > cur_size) {
    if (_predict_true(!umul_overflow(cur_size, 2, &new_size))) {
      size_t bytes;
      if (_predict_true(!umul_overflow(new_size, sizeof(char_type), &bytes))) {
        /* Add space for nul character at end of buffer. */
        size_t carry;
        bytes = addc(bytes, sizeof(char_type), 0, &carry);
        if (carry == 0)
          buf = static_cast<char_type*>(realloc(buf, bytes));
      }
    }
  }
  if (!buf) {
    size_type carry;
    new_size = addc(cur_size, n, 0, &carry);
    if (_predict_true(carry == 0U)) {
      size_t bytes;
      if (_predict_true(!umul_overflow(new_size, sizeof(char_type), &bytes))) {
        /* Add space for nul character at end of buffer. */
        size_t carry;
        bytes = addc(bytes, sizeof(char_type), 0, &carry);
        if (carry == 0)
          buf = static_cast<char_type*>(realloc(buf, bytes));
      }
    }
  }
  return make_pair(buf, new_size);
}

template<typename Char, typename Traits>
auto swap(basic_memstreambuf<Char, Traits>& x,
          basic_memstreambuf<Char, Traits>& y) -> void {
  x.swap(y);
}


template<typename Char, typename Traits>
auto basic_memstreambuf_noalloc<Char, Traits>::extend_realloc_(char_type*,
                                                               size_type,
                                                               size_type)
    noexcept -> pair<char_type*, size_type> {
  return make_pair(nullptr, 0);
}


template<typename Char, typename Traits>
basic_memstreambuf_exact<Char, Traits>::basic_memstreambuf_exact(
    basic_memstreambuf_exact&& other)
: basic_memstreambuf_noalloc<Char, Traits>(move(other))
{}

template<typename Char, typename Traits>
basic_memstreambuf_exact<Char, Traits>::basic_memstreambuf_exact(
    no_publish np, char_type*,
    size_type size, ios_base::openmode mode)
: basic_memstreambuf_noalloc<Char, Traits>(np, alloc_(size), size, mode)
{}

template<typename Char, typename Traits>
basic_memstreambuf_exact<Char, Traits>::~basic_memstreambuf_exact() noexcept {
  free(this->get_buf());
}

template<typename Char, typename Traits>
auto basic_memstreambuf_exact<Char, Traits>::alloc_(size_type len) ->
    char_type* {
  using abi::umul_overflow;
  using abi::addc;

  void* p = nullptr;
  size_type carry;
  size_t bytes;
  size_type elems = addc(len, 1, 0, &carry);
  if (_predict_true(carry == 0 &&
                    !umul_overflow(elems, sizeof(char_type), &bytes)))
    p = malloc(bytes);
  if (_predict_false(!p)) throw bad_alloc();
  return static_cast<char_type*>(p);
}


template<typename Char, typename Traits>
basic_imemstream<Char, Traits>::basic_imemstream(ios_base::openmode which)
: basic_istream<Char, Traits>(&this->sb_),
  sb_(which)
{}

template<typename Char, typename Traits>
basic_imemstream<Char, Traits>::basic_imemstream(char_type*& pub_buf,
                                                 size_type& pub_len,
                                                 ios_base::openmode which)
: basic_istream<Char, Traits>(&this->sb_),
  sb_(pub_buf, pub_len, which)
{}

template<typename Char, typename Traits>
basic_imemstream<Char, Traits>::basic_imemstream(char_type*& pub_buf,
                                                 size_type& pub_len,
                                                 char_type* init_buf,
                                                 size_type init_len,
                                                 ios_base::openmode which)
: basic_istream<Char, Traits>(&this->sb_),
  sb_(pub_buf, pub_len, init_buf, init_len, which)
{}

template<typename Char, typename Traits>
basic_imemstream<Char, Traits>::basic_imemstream(
    basic_imemstream&& other)
: basic_istream<Char, Traits>(move(other)),
  sb_(move(other.sb_))
{
  this->set_rdbuf(&sb_);
}

template<typename Char, typename Traits>
auto basic_imemstream<Char, Traits>::operator=(basic_imemstream&& other) ->
    basic_imemstream& {
  this->basic_istream<Char, Traits>::operator=(move(other));
  sb_ = move(other.sb_);
  this->set_rdbuf(&sb_);
  return *this;
}

template<typename Char, typename Traits>
auto basic_imemstream<Char, Traits>::swap(basic_imemstream& other) -> void {
  this->basic_istream<Char, Traits>::swap(other);
  sb_.swap(other.sb_);
  this->set_rdbuf(&sb_);
  other.set_rdbuf(&other.sb_);
}

template<typename Char, typename Traits>
auto basic_imemstream<Char, Traits>::rdbuf() const ->
    basic_memstreambuf<char_type, traits_type>* {
  return const_cast<basic_memstreambuf<char_type, traits_type>*>(&this->sb_);
}

template<typename Char, typename Traits>
auto swap(basic_imemstream<Char, Traits>& x,
          basic_imemstream<Char, Traits>& y) -> void {
  x.swap(y);
}


template<typename Char, typename Traits>
basic_omemstream<Char, Traits>::basic_omemstream(ios_base::openmode which)
: basic_ostream<Char, Traits>(&this->sb_),
  sb_(which)
{}

template<typename Char, typename Traits>
basic_omemstream<Char, Traits>::basic_omemstream(char_type*& pub_buf,
                                                 size_type& pub_len,
                                                 ios_base::openmode which)
: basic_ostream<Char, Traits>(&this->sb_),
  sb_(pub_buf, pub_len, which)
{}

template<typename Char, typename Traits>
basic_omemstream<Char, Traits>::basic_omemstream(char_type*& pub_buf,
                                                 size_type& pub_len,
                                                 char_type* init_buf,
                                                 size_type init_len,
                                                 ios_base::openmode which)
: basic_ostream<Char, Traits>(&this->sb_),
  sb_(pub_buf, pub_len, init_buf, init_len, which)
{}

template<typename Char, typename Traits>
basic_omemstream<Char, Traits>::basic_omemstream(
    basic_omemstream&& other)
: basic_ostream<Char, Traits>(move(other)),
  sb_(move(other.sb_))
{
  this->set_rdbuf(&sb_);
}

template<typename Char, typename Traits>
auto basic_omemstream<Char, Traits>::operator=(basic_omemstream&& other) ->
    basic_omemstream& {
  this->basic_ostream<Char, Traits>::operator=(move(other));
  sb_ = move(other.sb_);
  this->set_rdbuf(&sb_);
  return *this;
}

template<typename Char, typename Traits>
auto basic_omemstream<Char, Traits>::swap(basic_omemstream& other) -> void {
  this->basic_ostream<Char, Traits>::swap(other);
  sb_.swap(other.sb_);
  this->set_rdbuf(&sb_);
  other.set_rdbuf(&other.sb_);
}

template<typename Char, typename Traits>
auto basic_omemstream<Char, Traits>::rdbuf() const ->
    basic_memstreambuf<char_type, traits_type>* {
  return const_cast<basic_memstreambuf<char_type, traits_type>*>(&this->sb_);
}

template<typename Char, typename Traits>
auto swap(basic_omemstream<Char, Traits>& x,
          basic_omemstream<Char, Traits>& y) -> void {
  x.swap(y);
}


template<typename Char, typename Traits, typename S>
basic_memstream<Char, Traits, S>::basic_memstream(ios_base::openmode which)
: basic_iostream<Char, Traits>(&this->sb_),
  sb_(which)
{}

template<typename Char, typename Traits, typename S>
basic_memstream<Char, Traits, S>::basic_memstream(char_type*& pub_buf,
                                                  size_type& pub_len,
                                                  ios_base::openmode which)
: basic_iostream<Char, Traits>(&this->sb_),
  sb_(pub_buf, pub_len, which)
{}

template<typename Char, typename Traits, typename S>
basic_memstream<Char, Traits, S>::basic_memstream(char_type*& pub_buf,
                                                  size_type& pub_len,
                                                  char_type* init_buf,
                                                  size_type init_len,
                                                  ios_base::openmode which)
: basic_iostream<Char, Traits>(&this->sb_),
  sb_(pub_buf, pub_len, init_buf, init_len, which)
{}

template<typename Char, typename Traits, typename S>
basic_memstream<Char, Traits, S>::basic_memstream(no_publish np,
                                                  char_type* init_buf,
                                                  size_type init_len,
                                                  ios_base::openmode which)
: basic_iostream<Char, Traits>(&this->sb_),
  sb_(np, init_buf, init_len, which)
{}

template<typename Char, typename Traits, typename S>
basic_memstream<Char, Traits, S>::basic_memstream(
    basic_memstream&& other)
: basic_iostream<Char, Traits>(move(other)),
  sb_(move(other.sb_))
{
  this->set_rdbuf(&sb_);
}

template<typename Char, typename Traits, typename S>
auto basic_memstream<Char, Traits, S>::operator=(basic_memstream&& other) ->
    basic_memstream& {
  this->basic_iostream<Char, Traits>::operator=(move(other));
  sb_ = move(other.sb_);
  this->set_rdbuf(&sb_);
  return *this;
}

template<typename Char, typename Traits, typename S>
auto basic_memstream<Char, Traits, S>::swap(basic_memstream& other) -> void {
  this->basic_iostream<Char, Traits>::swap(other);
  sb_.swap(other.sb_);
  this->set_rdbuf(&sb_);
  other.set_rdbuf(&other.sb_);
}

template<typename Char, typename Traits, typename S>
auto basic_memstream<Char, Traits, S>::rdbuf() const -> S* {
  return const_cast<S*>(&this->sb_);
}

template<typename Char, typename Traits, typename S>
auto swap(basic_memstream<Char, Traits, S>& x,
          basic_memstream<Char, Traits, S>& y) -> void {
  x.swap(y);
}


_namespace_end(std)

#endif /* _MEMSTREAM_INL_H_ */
