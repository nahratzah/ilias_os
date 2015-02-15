#ifndef _CSTDIO_FILE_H_
#define _CSTDIO_FILE_H_

#include <cdecl.h>
#include <cstdio_misc/file.h>
#include <ios>
#include <istream>
#include <ostream>
#include <streambuf>
#include <utility>
#include <mutex>

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

  void lock() { mtx_.lock(); }
  bool try_lock() noexcept { return mtx_.try_lock(); }
  void unlock() noexcept { mtx_.unlock(); }

 private:
  recursive_mutex mtx_;
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
  mutable IOStream f_;
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
  mutable IStream f_;
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
  mutable OStream f_;
};


class file_lock {
 public:
  file_lock() noexcept = default;
  explicit file_lock(FILE*);
  file_lock(FILE*, defer_lock_t) noexcept;
  file_lock(FILE*, try_to_lock_t);
  file_lock(FILE*, adopt_lock_t) noexcept;
  ~file_lock() noexcept;

  file_lock(const file_lock&) = delete;
  file_lock& operator=(const file_lock&) = delete;

  file_lock(file_lock&&) noexcept;
  file_lock& operator=(file_lock&&) noexcept;

  void lock();
  bool try_lock();
  void unlock();

  void swap(file_lock&) noexcept;
  FILE* release() noexcept;

  bool owns_lock() const noexcept { return locked_; }
  explicit operator bool() const noexcept { return locked_; }
  FILE* mutex() const noexcept { return file_; }

 private:
  FILE* file_ = nullptr;
  bool locked_ = false;
};

void swap(file_lock&, file_lock&) noexcept;


_namespace_end(std)

#include "cstdio_file-inl.h"

#endif /* _CSTDIO_FILE_H_ */
