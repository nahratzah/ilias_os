#ifndef _MEMSTREAM_H_
#define _MEMSTREAM_H_

#include <cdecl.h>
#include <streambuf>
#include "cstdio_file.h"

_namespace_begin(std)


template<typename Char, typename Traits = char_traits<Char>>
class basic_memstreambuf
: public basic_streambuf<Char, Traits>
{
 public:
  using char_type = typename basic_streambuf<Char, Traits>::char_type;
  using traits_type = typename basic_streambuf<Char, Traits>::traits_type;
  using int_type = typename basic_streambuf<Char, Traits>::int_type;
  using pos_type = typename basic_streambuf<Char, Traits>::pos_type;
  using off_type = typename basic_streambuf<Char, Traits>::off_type;
  using size_type = size_t;

  explicit basic_memstreambuf(
      ios_base::openmode = ios_base::in | ios_base::out);
  basic_memstreambuf(char_type*&, size_type&,
                     ios_base::openmode = ios_base::in | ios_base::out);
  basic_memstreambuf(basic_memstreambuf&&);
  ~basic_memstreambuf() noexcept override;

  basic_memstreambuf& operator=(basic_memstreambuf&&);
  void swap(basic_memstreambuf&);

 protected:
  int_type underflow() override;
  int_type pbackfail(int_type = traits_type::eof()) override;
  int_type overflow(int_type = traits_type::eof()) override;
  basic_streambuf<Char, Traits>* setbuf(char_type*, streamsize) override;

  pos_type seekoff(off_type, ios_base::seekdir,
                   ios_base::openmode = ios_base::in | ios_base::out)
      override;
  pos_type seekpos(pos_type,
                   ios_base::openmode = ios_base::in | ios_base::out)
      override;
  int sync() override;

 private:
  bool extend_(size_type);

  ios_base::openmode mode_;
  char_type* buf_ = nullptr;
  char_type* buf_end_ = nullptr;
  size_type len_ = 0;
  char_type** pub_buf_ = nullptr;
  size_type* pub_len_ = nullptr;
};

template<typename Char, typename Traits>
void swap(basic_memstreambuf<Char, Traits>&,
          basic_memstreambuf<Char, Traits>&);


template<typename Char, typename Traits = char_traits<Char>>
class basic_imemstream;

template<typename Char, typename Traits = char_traits<Char>>
class basic_omemstream;

template<typename Char, typename Traits = char_traits<Char>>
class basic_memstream;


template<typename Char, typename Traits>
class basic_imemstream
: public basic_istream<Char, Traits>
{
 public:
  using char_type = typename basic_istream<Char, Traits>::char_type;
  using traits_type = typename basic_istream<Char, Traits>::traits_type;
  using int_type = typename basic_istream<Char, Traits>::int_type;
  using pos_type = typename basic_istream<Char, Traits>::pos_type;
  using off_type = typename basic_istream<Char, Traits>::off_type;
  using size_type =
      typename basic_memstreambuf<char_type, traits_type>::size_type;

  explicit basic_imemstream(ios_base::openmode = ios_base::in);
  basic_imemstream(char_type*&, size_type&,
                   ios_base::openmode = ios_base::in);
  basic_imemstream(basic_imemstream&&);
  ~basic_imemstream() noexcept override = default;

  basic_imemstream& operator=(basic_imemstream&&);
  void swap(basic_imemstream&);

  basic_memstreambuf<char_type, traits_type>* rdbuf() const;

 private:
  basic_memstreambuf<char_type, traits_type> sb_;
};

template<typename Char, typename Traits>
void swap(basic_imemstream<Char, Traits>&,
          basic_imemstream<Char, Traits>&);


template<typename Char, typename Traits>
class basic_omemstream
: public basic_ostream<Char, Traits>
{
 public:
  using char_type = typename basic_ostream<Char, Traits>::char_type;
  using traits_type = typename basic_ostream<Char, Traits>::traits_type;
  using int_type = typename basic_ostream<Char, Traits>::int_type;
  using pos_type = typename basic_ostream<Char, Traits>::pos_type;
  using off_type = typename basic_ostream<Char, Traits>::off_type;
  using size_type =
      typename basic_memstreambuf<char_type, traits_type>::size_type;

  explicit basic_omemstream(ios_base::openmode = ios_base::out);
  basic_omemstream(char_type*&, size_type&,
                   ios_base::openmode = ios_base::out);
  basic_omemstream(basic_omemstream&&);
  ~basic_omemstream() noexcept override = default;

  basic_omemstream& operator=(basic_omemstream&&);
  void swap(basic_omemstream&);

  basic_memstreambuf<char_type, traits_type>* rdbuf() const;

 private:
  basic_memstreambuf<char_type, traits_type> sb_;
};

template<typename Char, typename Traits>
void swap(basic_omemstream<Char, Traits>&,
          basic_omemstream<Char, Traits>&);


template<typename Char, typename Traits>
class basic_memstream
: public basic_iostream<Char, Traits>
{
 public:
  using char_type = typename basic_iostream<Char, Traits>::char_type;
  using traits_type = typename basic_iostream<Char, Traits>::traits_type;
  using int_type = typename basic_iostream<Char, Traits>::int_type;
  using pos_type = typename basic_iostream<Char, Traits>::pos_type;
  using off_type = typename basic_iostream<Char, Traits>::off_type;
  using size_type =
      typename basic_memstreambuf<char_type, traits_type>::size_type;

  explicit basic_memstream(ios_base::openmode = ios_base::in | ios_base::out);
  basic_memstream(char_type*&, size_type&,
                   ios_base::openmode = ios_base::in | ios_base::out);
  basic_memstream(basic_memstream&&);
  ~basic_memstream() noexcept override = default;

  basic_memstream& operator=(basic_memstream&&);
  void swap(basic_memstream&);

  basic_memstreambuf<char_type, traits_type>* rdbuf() const;

 private:
  basic_memstreambuf<char_type, traits_type> sb_;
};

template<typename Char, typename Traits>
void swap(basic_memstream<Char, Traits>&,
          basic_memstream<Char, Traits>&);


_namespace_end(std)

#include "memstream-inl.h"

#endif /* _MEMSTREAM_H_ */
