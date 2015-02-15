#ifndef _CSTDIO_FILE_H_
#define _CSTDIO_FILE_H_

#include <cdecl.h>
#include <cstdio_misc/file.h>
#include <ios>
#include <istream>
#include <ostream>
#include <streambuf>
#include <utility>

_namespace_begin(std)


class FILE {
 public:
  using istream = _namespace(std)::istream;
  using ostream = _namespace(std)::ostream;
  using char_type = basic_ios<char>::char_type;
  using traits_type = basic_ios<char>::traits_type;

  FILE() noexcept = default;
  FILE(const FILE&) = delete;
  FILE& operator=(const FILE&) = delete;

  virtual ~FILE() noexcept;

  virtual basic_ios<char>& get_ios() const noexcept = 0;
  streambuf& rdbuf() const noexcept;
  virtual istream& get_istream() const;
  virtual ostream& get_ostream() const;
};

template<typename IOStream>
class iostream_file
: public FILE
{
 public:
  iostream_file(IOStream&& f) noexcept : f_(move(f)) {}

  basic_ios<char>& get_ios() const noexcept override { return f_; }
  istream& get_istream() const noexcept override { return f_; }
  ostream& get_ostream() const noexcept override { return f_; }

 private:
  IOStream f_;
};

template<typename IStream>
class istream_file
: public FILE
{
 public:
  istream_file(IStream&& f) noexcept : f_(move(f)) {}

  basic_ios<char>& get_ios() const noexcept override { return f_; }
  istream& get_istream() const noexcept override { return f_; }

 private:
  IStream f_;
};

template<typename OStream>
class ostream_file
: public FILE
{
 public:
  ostream_file(OStream&& f) noexcept : f_(move(f)) {}

  basic_ios<char>& get_ios() const noexcept override { return f_; }
  ostream& get_ostream() const noexcept override { return f_; }

 private:
  OStream f_;
};


_namespace_end(std)

#endif /* _CSTDIO_FILE_H_ */
