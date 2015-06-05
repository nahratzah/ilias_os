#ifndef _SSTREAM_INL_H_
#define _SSTREAM_INL_H_

#include <sstream>
#include <utility>
#include <type_traits>
#include <new>
#include <cstdio>

_namespace_begin(std)


template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::free_::operator()(Char* p)
    const noexcept -> void {
  return_temporary_buffer(p);
}


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
  swap(data_, other.data_);
  swap(buf_, other.buf_);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::str() const ->
    basic_string<char_type, traits_type, allocator_type> {
  const_cast<basic_stringbuf<Char, Traits, Allocator>&>(*this).sync();
  return data_;
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::str(
    const basic_string<char_type, traits_type, allocator_type>& s) -> void {
  str(basic_string_ref<Char, Traits>(s.data(), s.size()));
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::str(
    basic_string_ref<char_type, traits_type> s) -> void {
  data_ = s;

  /* Update get/put pointers. */
  if ((mode_ & ios_base::in) == ios_base::in)
    this->setg(data_.begin(), data_.begin(), data_.end());
  if ((mode_ & ios_base::out) == ios_base::out)
    this->setp(nullptr, nullptr);
}

template<typename Char, typename Traits, typename Allocator>
auto basic_stringbuf<Char, Traits, Allocator>::underflow() -> int_type {
  sync();

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
  sync();

  if (!traits_type::eq_int_type(c, traits_type::eof())) {
    try {
      data_.push_back(traits_type::to_char_type(c));
    } catch (const bad_alloc&) {
      return traits_type::eof();
    }
  }

  if (_predict_false(!buf_)) {
    buf_ = unique_ptr<char_type[], free_>(
        get_temporary_buffer<char_type>(BUFSIZ).first);
    if (_predict_false(!buf_)) {
      this->setp(nullptr, nullptr);
      __throw_bad_alloc();
    }
  }
  this->setp(buf_.get(), buf_.get() + BUFSIZ);
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
  sync();

  off_type new_off;

  switch (way) {
  case ios_base::cur:
    /* New offset relative to current offset. */
    switch (which & (ios_base::in | ios_base::out)) {
    case ios_base::in:
      new_off = size_t(this->gptr() - this->eback()) + off;
      break;
    case ios_base::out:
      if (this->pbase() >= data_.begin() && this->pbase() < data_.end())
        new_off = this->pbase() - data_.begin();
      else
        new_off = data_.length();
      new_off += size_t(this->pptr() - this->pbase()) + off;
      break;
    default:
      return pos_type(off_type(-1));  // Cannot reposition both in rel mode.
    }
    break;

  case ios_base::beg:
    new_off = off;
    break;

  case ios_base::end:
    if (off < 0 ||
        static_cast<make_unsigned_t<off_type>>(off) > data_.length())
      return pos_type(off_type(-1));  // Past-the-end.
    new_off = data_.length() - off;
    break;

  default:
    return pos_type(off_type(-1));  // Invalid argument.
  }

  /* Verify requested seek mode is open. */
  if ((which & ios_base::in) == ios_base::in &&
      (mode_ && ios_base::in) != ios_base::in)
    return pos_type(off_type(-1));
  if ((which & ios_base::out) == ios_base::out &&
      (mode_ && ios_base::out) != ios_base::out)
    return pos_type(off_type(-1));
  /*
   * If this is just a request for the current offset,
   * don't bother updating the get/put zones.
   */
  if (way == ios_base::cur && off == 0) return pos_type(new_off);

  /* Validate new offset. */
  if (off < 0 || make_unsigned_t<off_type>(off) >= data_.length())
    return pos_type(off_type(-1));

  /* Update requested pointers. */
  if ((which & ios_base::in) == ios_base::in)
    this->setg(data_.begin(), data_.begin() + off, data_.end());
  if ((which & ios_base::out) == ios_base::out) {
    if (new_off == data_.length()) {
      if (buf_)
        this->setp(buf_.get(), buf_.get() + BUFSIZ);
      else
        this->setp(nullptr, nullptr);
    } else {
      this->setp(data_.begin() + off, data_.end());
    }
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
auto basic_stringbuf<Char, Traits, Allocator>::sync() -> int {
  if (this->pbase() == buf_.get()) {
    data_.append(this->pbase(), this->pptr());

    this->setg(data_.begin(), data_.begin() + (this->gptr() - this->eback()),
               data_.end());
    this->setp(this->pbase(), this->epptr());
  }
  return 0;
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
  other.set_rdbuf(&other.sb_);
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
    ios_base::openmode which)
: basic_ostream<Char, Traits>(&this->sb_),
  sb_(which)
{}

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
  other.set_rdbuf(&other.sb_);
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
  other.set_rdbuf(&other.sb_);
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
