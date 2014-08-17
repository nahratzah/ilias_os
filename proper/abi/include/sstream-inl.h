#ifndef _SSTREAM_INL_H_
#define _SSTREAM_INL_H_

#include <sstream>
#include <utility>
#include <type_traits>

_namespace_begin(std)


template<typename Char, typename Traits, typename Allocator>
basic_stringbuf<Char, Traits, Allocator>::basic_stringbuf(
    ios_base::openmode mode)
: mode_(mode)
{}

template<typename Char, typename Traits, typename Allocator>
basic_stringbuf<Char, Traits, Allocator>::basic_stringbuf(
    const basic_string<Char, Traits, Allocator>& s,
    ios_base::openmode mode)
: basic_stringbuf(mode)
{
  str(s);
}

template<typename Char, typename Traits, typename Allocator>
basic_stringbuf<Char, Traits, Allocator>::basic_stringbuf(
    basic_string_ref<Char, Traits> s,
    ios_base::openmode mode)
: basic_stringbuf(mode)
{
  str(s);
}

template<typename Char, typename Traits, typename Allocator>
basic_stringbuf<Char, Traits, Allocator>::basic_stringbuf(
    basic_stringbuf&& other)
: mode_(other.mode_)
{
  other.swap(*this);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::operator=(
    basic_stringbuf&& other) -> basic_stringbuf& {
  basic_stringbuf(move(other)).swap(*this);
  return *this;
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::swap(
    basic_stringbuf& other) -> void {
  using _namespace(std)::swap;

  this->basic_streambuf<Char, Traits>::swap(other);
  swap(mode_, other.mode_);
  swap(buf_, other.buf_);
  swap(buf_end_, other.buf_end_);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::str() const ->
    basic_string<Char, Traits, Allocator> {
  char_type* lo = nullptr;
  char_type* hi = nullptr;

  if ((mode_ & ios_base::out) == ios_base::out) {
    lo = this->pbase();
    hi = this->epptr();
  } else if ((mode_ & ios_base::in) == ios_base::in) {
    lo = this->eback();
    hi = this->egptr();
  }
  return basic_string<Char, Traits, Allocator>(lo, hi);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::str(
    const basic_string<Char, Traits, Allocator>& s) -> void {
  str(basic_string_ref<Char, Traits>(s.data(), s.size()));
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::str(
    basic_string_ref<Char, Traits> s) -> void {
  /* Allocate buffer if required. */
  if (uintptr_t(buf_end_ - buf_.get()) < s.size()) {
    buf_ = make_unique<char_type[]>(s.size());
    buf_end_ = buf_.get() + s.size();
  }

  /* Copy new data into buffer. */
  traits_type::copy(buf_.get(), s.data(), s.size());

  /* Update get/put pointers. */
  if ((mode_ & ios_base::in) == ios_base::in)
    this->setg(buf_.get(), buf_.get(), buf_.get() + s.size());
  if ((mode_ & ios_base::out) == ios_base::out)
    this->setp(buf_.get(), buf_.get() + s.size());
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::underflow() -> int_type {
  if (this->gptr() < this->egptr())
    return traits_type::to_int_type(*this->egptr());
  return traits_type::eof();
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::pbackfail(int_type c) ->
    int_type {
  if (this->gptr() == this->eback()) return traits_type::eof();

  char_type*const pos = this->gptr() - 1;
  if (traits_type::eq_int_type(c, traits_type::eof()) ||
      traits_type::eq_int_type(traits_type::to_int_type(*pos), c)) {
    /* SKIP */
  } else if ((mode_ & ios_base::out) == ios_base::out) {
    *pos = traits_type::to_char_type(c);
  } else {
    return traits_type::eof();
  }

  this->gbump(-1);
  return traits_type::not_eof(c);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::overflow(int_type c) ->
    int_type {
  if (!traits_type::eq_int_type(c, traits_type::eof())) {
    if (!extend_(1)) return traits_type::eof();
    assert(this->pptr() < this->epptr());
    this->sputc(traits_type::to_char_type(c));
  }
  return traits_type::not_eof(c);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::setbuf(
    char_type*, streamsize) -> basic_streambuf<Char, Traits>* {
  // Implementation defined behaviour: do nothing.
  return this;
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::seekoff(
    off_type off, ios_base::seekdir way, ios_base::openmode which) ->
    pos_type {
  off_type new_off;
  const auto end = uintptr_t((this->epptr() == nullptr ?
                              this->egptr() :
                              this->epptr()) -
                             buf_.get());

  switch (way) {
  case ios_base::cur:
    /* New offset relative to current offset. */
    switch (which & (ios_base::in | ios_base::out)) {
    case ios_base::in:
      new_off = size_t(this->gptr() - this->eback()) + off;
      break;
    case ios_base::out:
      new_off = size_t(this->pptr() - this->pbase()) + off;
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

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::seekpos(
    pos_type p, ios_base::openmode which) -> pos_type {
  return seekoff(p - pos_type(0), ios_base::beg, which);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::extend_(size_t n) -> bool {
  assert((mode_ & ios_base::out) == ios_base::out);

  /* No allocation required. */
  if (uintptr_t(this->epptr() - this->pptr()) >= n) return true;

  /* If there is sufficient space, just mark it as initialized. */
  if (uintptr_t(buf_end_ - this->pptr()) >= n) {
    size_t cur_pos = this->pptr() - this->pbase();
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

  const char_type*const begin = this->buf_.get();
  const char_type*const end = this->epptr();
  const size_t cur_size = end - begin;
  size_t new_size;
  unique_ptr<char_type[]> buf;

  /* Allocate buffer. */
  if (n < cur_size && SIZE_MAX / 2 > cur_size) {
    new_size = 2 * cur_size;
    buf.reset(new (nothrow) char_type[new_size]);
  }
  if (!buf) {
    new_size = cur_size + n;
    buf.reset(new (nothrow) char_type[new_size]);
  }
  if (!buf) return false;

  /* Copy current content. */
  traits_type::copy(buf.get(), begin, end - begin);

  /* Update get pointers. */
  if ((mode_ & ios_base::in) == ios_base::in) {
    this->setg(buf.get(),
               buf.get() + (this->gptr() - begin),
               buf.get() + cur_size + n);
  }
  /* Update put pointers. */
  size_t cur_pos = this->pptr() - begin;
  this->setp(buf.get(),
             buf.get() + cur_size + n);
  while (_predict_false(cur_pos > INT_MAX)) {
    this->pbump(INT_MAX);
    cur_pos -= INT_MAX;
  }
  this->pbump(cur_pos);
  buf_end_ = buf.get() + new_size;

  /* Update ownership. */
  this->buf_ = move(buf);
  return true;
}

template<typename Char, typename Traits, typename Allocator>
auto swap(basic_stringbuf<Char, Traits, Allocator>& x,
          basic_stringbuf<Char, Traits, Allocator>& y) -> void {
  x.swap(y);
}


template<typename Char, typename Traits, typename Allocator>
basic_istringstream<Char, Traits, Allocator>::basic_istringstream(
    ios_base::openmode which)
: basic_istream<Char, Traits>(&this->sb_),
  sb_(which)
{}

template<typename Char, typename Traits, typename Allocator>
basic_istringstream<Char, Traits, Allocator>::basic_istringstream(
    const basic_string<char_type, traits_type, allocator_type>& s,
    ios_base::openmode which)
: basic_istream<Char, Traits>(&this->sb_),
  sb_(s, which)
{}

template<typename Char, typename Traits, typename Allocator>
basic_istringstream<Char, Traits, Allocator>::basic_istringstream(
    basic_string_ref<char_type, traits_type> s,
    ios_base::openmode which)
: basic_istream<Char, Traits>(&this->sb_),
  sb_(s, which)
{}

template<typename Char, typename Traits, typename Allocator>
basic_istringstream<Char, Traits, Allocator>::basic_istringstream(
    basic_istringstream&& other)
: basic_istream<Char, Traits>(move(other)),
  sb_(move(other.sb_))
{
  this->set_rdbuf(&sb_);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_istringstream<Char, Traits, Allocator>::operator=(
    basic_istringstream&& other) -> basic_istringstream& {
  this->basic_istream<Char, Traits>::operator=(move(other));
  sb_ = move(other.sb_);
  this->set_rdbuf(&sb_);
  return *this;
}

template<typename Char, typename Traits, typename Allocator>
auto basic_istringstream<Char, Traits, Allocator>::swap(
    basic_istringstream& other) -> void {
  this->basic_istream<Char, Traits>::swap(other);
  sb_.swap(other.sb_);
  this->set_rdbuf(&sb_);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_istringstream<Char, Traits, Allocator>::rdbuf() const ->
    basic_stringbuf<char_type, traits_type, allocator_type>* {
  return const_cast<basic_stringbuf<char_type, traits_type, allocator_type>*>(
      &this->sb_);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_istringstream<Char, Traits, Allocator>::str() const ->
    basic_string<char_type, traits_type, allocator_type> {
  return sb_.str();
}

template<typename Char, typename Traits, typename Allocator>
auto basic_istringstream<Char, Traits, Allocator>::str(
    const basic_string<char_type, traits_type, allocator_type>& s) -> void {
  sb_.str(s);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_istringstream<Char, Traits, Allocator>::str(
    basic_string_ref<char_type, traits_type> s) -> void {
  sb_.str(s);
}

template<typename Char, typename Traits, typename Allocator>
auto swap(basic_istringstream<Char, Traits, Allocator>& x,
          basic_istringstream<Char, Traits, Allocator>& y) -> void {
  x.swap(y);
}


template<typename Char, typename Traits, typename Allocator>
basic_ostringstream<Char, Traits, Allocator>::basic_ostringstream(
    const basic_string<char_type, traits_type, allocator_type>& s,
    ios_base::openmode which)
: basic_ostream<Char, Traits>(&this->sb_),
  sb_(s, which)
{}

template<typename Char, typename Traits, typename Allocator>
basic_ostringstream<Char, Traits, Allocator>::basic_ostringstream(
    basic_string_ref<char_type, traits_type> s,
    ios_base::openmode which)
: basic_ostream<Char, Traits>(&this->sb_),
  sb_(s, which)
{}

template<typename Char, typename Traits, typename Allocator>
basic_ostringstream<Char, Traits, Allocator>::basic_ostringstream(
    basic_ostringstream&& other)
: basic_ostream<Char, Traits>(move(other)),
  sb_(move(other.sb_))
{
  this->set_rdbuf(&sb_);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_ostringstream<Char, Traits, Allocator>::operator=(
    basic_ostringstream&& other) -> basic_ostringstream& {
  this->basic_ostream<Char, Traits>::operator=(move(other));
  sb_ = move(other.sb_);
  this->set_rdbuf(&sb_);
  return *this;
}

template<typename Char, typename Traits, typename Allocator>
auto basic_ostringstream<Char, Traits, Allocator>::swap(
    basic_ostringstream& other) -> void {
  this->basic_ostream<Char, Traits>::swap(other);
  sb_.swap(other.sb_);
  this->set_rdbuf(&sb_);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_ostringstream<Char, Traits, Allocator>::rdbuf() const ->
    basic_stringbuf<char_type, traits_type, allocator_type>* {
  return const_cast<basic_stringbuf<char_type, traits_type, allocator_type>*>(
      &this->sb_);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_ostringstream<Char, Traits, Allocator>::str() const ->
    basic_string<char_type, traits_type, allocator_type> {
  return sb_.str();
}

template<typename Char, typename Traits, typename Allocator>
auto basic_ostringstream<Char, Traits, Allocator>::str(
    const basic_string<char_type, traits_type, allocator_type>& s) -> void {
  sb_.str(s);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_ostringstream<Char, Traits, Allocator>::str(
    basic_string_ref<char_type, traits_type> s) -> void {
  sb_.str(s);
}

template<typename Char, typename Traits, typename Allocator>
auto swap(basic_ostringstream<Char, Traits, Allocator>& x,
          basic_ostringstream<Char, Traits, Allocator>& y) -> void {
  x.swap(y);
}


template<typename Char, typename Traits, typename Allocator>
basic_stringstream<Char, Traits, Allocator>::basic_stringstream(
    const basic_string<char_type, traits_type, allocator_type>& s,
    ios_base::openmode which)
: basic_iostream<Char, Traits>(&this->sb_),
  sb_(s, which)
{}

template<typename Char, typename Traits, typename Allocator>
basic_stringstream<Char, Traits, Allocator>::basic_stringstream(
    basic_string_ref<char_type, traits_type> s,
    ios_base::openmode which)
: basic_iostream<Char, Traits>(&this->sb_),
  sb_(s, which)
{}

template<typename Char, typename Traits, typename Allocator>
basic_stringstream<Char, Traits, Allocator>::basic_stringstream(
    basic_stringstream&& other)
: basic_iostream<Char, Traits>(move(other)),
  sb_(move(other.sb_))
{
  this->set_rdbuf(&sb_);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringstream<Char, Traits, Allocator>::operator=(
    basic_stringstream&& other) -> basic_stringstream& {
  this->basic_ostream<Char, Traits>::operator=(move(other));
  sb_ = move(other.sb_);
  this->set_rdbuf(&sb_);
  return *this;
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringstream<Char, Traits, Allocator>::swap(
    basic_stringstream& other) -> void {
  this->basic_ostream<Char, Traits>::swap(other);
  sb_.swap(other.sb_);
  this->set_rdbuf(&sb_);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringstream<Char, Traits, Allocator>::rdbuf() const ->
    basic_stringbuf<char_type, traits_type, allocator_type>* {
  return const_cast<basic_stringbuf<char_type, traits_type, allocator_type>*>(
      &this->sb_);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringstream<Char, Traits, Allocator>::str() const ->
    basic_string<char_type, traits_type, allocator_type> {
  return sb_.str();
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringstream<Char, Traits, Allocator>::str(
    const basic_string<char_type, traits_type, allocator_type>& s) -> void {
  sb_.str(s);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringstream<Char, Traits, Allocator>::str(
    basic_string_ref<char_type, traits_type> s) -> void {
  sb_.str(s);
}

template<typename Char, typename Traits, typename Allocator>
auto swap(basic_stringstream<Char, Traits, Allocator>& x,
          basic_stringstream<Char, Traits, Allocator>& y) -> void {
  x.swap(y);
}


_namespace_end(std)

#endif /* _SSTREAM_INL_H_ */
