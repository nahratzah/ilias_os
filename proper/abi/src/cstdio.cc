#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <system_error>
#include <abi/ext/printf.h>
#include <abi/misc_int.h>
#include <stdimpl/exc_errno.h>
#include <sstream>
#include <streambuf>
#include "cstdio_file.h"
#include "memstream.h"

_namespace_begin(std)
namespace {


inline void check_null_file(FILE* f) {
  if (_predict_false(f == nullptr))
    throw system_error(make_error_code(errc::bad_file_descriptor));
}

ios_base::openmode openmode_from_string(string_ref s) {
  static constexpr string_ref s_r   = string_ref("r", 1);
  static constexpr string_ref s_rb  = string_ref("rb", 2);
  static constexpr string_ref s_w   = string_ref("w", 1);
  static constexpr string_ref s_wb  = string_ref("wb", 2);
  static constexpr string_ref s_a   = string_ref("a", 1);
  static constexpr string_ref s_ab  = string_ref("ab", 2);
  static constexpr string_ref s_rP  = string_ref("r+", 2);
  static constexpr string_ref s_rbP = string_ref("rb+", 3);
  static constexpr string_ref s_rPb = string_ref("r+b", 3);
  static constexpr string_ref s_wP  = string_ref("w+", 2);
  static constexpr string_ref s_wbP = string_ref("wb+", 3);
  static constexpr string_ref s_wPb = string_ref("w+b", 3);
  static constexpr string_ref s_aP  = string_ref("a+", 2);
  static constexpr string_ref s_abP = string_ref("ab+", 3);
  static constexpr string_ref s_aPb = string_ref("a+b", 3);

  /* Basics. */
  ios_base::openmode rv;
  if (s == s_r || s == s_rb)
    rv = ios_base::in;
  else if (s == s_w || s == s_wb)
    rv = ios_base::out;
  else if (s == s_a || s == s_ab)
    rv = ios_base::out | ios_base::ate;
  else if (s == s_rP || s == s_rbP || s == s_rPb)
    rv = ios_base::in | ios_base::out;
  else if (s == s_wP || s == s_wbP || s == s_wPb)
    rv = ios_base::in | ios_base::out | ios_base::trunc;
  else if (s == s_aP || s == s_abP || s == s_aPb)
    rv = ios_base::in | ios_base::out | ios_base::ate;
  else
    throw invalid_argument("openmode");

  /* Check for binary setting. */
  if (s == s_rb ||
      s == s_wb ||
      s == s_ab ||
      s == s_rbP || s == s_rPb ||
      s == s_wbP || s == s_wPb ||
      s == s_abP || s == s_aPb)
    rv |= ios_base::binary;

  return rv;
}


} /* namespace std::<unnamed> */


using impl::errno_catch_handler;


#if 0  // XXX: need std::c{in,out,err}
extern FILE* const stdin = reinterpret_cast<FILE*>(&cin);
extern FILE* const stdout = reinterpret_cast<FILE*>(&cout);
extern FILE* const stderr = reinterpret_cast<FILE*>(&cerr);
#endif


void clearerr(FILE* f) noexcept {
  file_lock lck{ f };

  check_null_file(f);
  f->get_ios().setstate(ios_base::goodbit);
}

char* ctermid(char*) noexcept;  // XXX: implement

int fclose(FILE* f) noexcept {
  file_lock lck{ f };

  try {
    check_null_file(f);
    delete f;
  } catch (...) {
    errno_catch_handler();

    try {
      check_null_file(f);
      f->rdbuf().pubsync();
      f->rdbuf().pubseekoff(0, ios_base::end);
    } catch (...) {
      /* Best effort. */
    }
    return EOF;
  }
  return 0;
}

FILE* fdopen(int, const char*) noexcept;  // XXX: implement file descriptors?

int feof(FILE* f) noexcept {
  file_lock lck{ f };

  check_null_file(f);
  return f->get_ios().eof();
}

int ferror(FILE* f) noexcept {
  file_lock lck{ f };

  check_null_file(f);
  return !f->get_ios();
}

int fflush(FILE* f) noexcept {
  file_lock lck{ f };

  if (f == nullptr) {
    throw runtime_error("XXX need implementation");  // XXX
#if 0  // Pseudocode
    for (_file& f : all_files) {
      try {
        f->get_ostream().pubsync();
        f->get_ostream().pubseekoff(0, ios_base::seekend);
      } catch (...) {
        /* SKIP: ignore */
      }
    }
#endif
  } else {
    try {
      f->rdbuf().pubsync();
      f->rdbuf().pubseekoff(0, ios_base::end);
    } catch (...) {
      errno_catch_handler();
      return EOF;
    }
  }
  return 0;
}

int fgetc(FILE* f) noexcept {
  file_lock lck{ f };

  try {
    return f->get_istream().get();
  } catch (...) {
    errno_catch_handler();
    return EOF;
  }
}

int fgetpos(FILE*__restrict f, fpos_t*__restrict p) noexcept {
  file_lock lck{ f };

  try {
    check_null_file(f);
    const auto pos = f->rdbuf().pubseekoff(0, ios_base::cur);
    const decltype(pos) zero{ fpos_t(0) };
    *p = pos - zero;
  } catch (...) {
    errno_catch_handler();
    return -1;
  }
  return 0;
}

char* fgets(char*__restrict s, int n, FILE*__restrict f) noexcept {
  file_lock lck{ f };

  try {
    check_null_file(f);
    if (_predict_false(s == nullptr)) throw invalid_argument("null string");
    if (_predict_false(n == 0)) throw invalid_argument("zero-length string");

    if (f->get_istream().eof()) return nullptr;
    f->get_istream().getline(s, n);
  } catch (...) {
    errno_catch_handler();
    return nullptr;
  }
  return s;
}

int fileno(FILE*) noexcept;  // XXX: implement file descriptors

void flockfile(FILE* f) noexcept {
  try {
    check_null_file(f);
    f->lock();
  } catch (...) {
    errno_catch_handler();
  }
}

FILE* fmemopen(void*__restrict buf, size_t size,
               const char*__restrict mode_str) noexcept {
  try {
    ios_base::openmode mode = openmode_from_string(mode_str);

    if (buf) {
      using buf_type = basic_memstreambuf_noalloc<char>;
      using stream_type = basic_memstream<char, char_traits<char>, buf_type>;
      using file_type = iostream_file<stream_type>;

      return new file_type(stream_type(no_publish(),
                                       static_cast<char*>(buf), size, mode));
    } else {
      using buf_type = basic_memstreambuf_exact<char>;
      using stream_type = basic_memstream<char, char_traits<char>, buf_type>;
      using file_type = iostream_file<stream_type>;

      return new file_type(stream_type(no_publish(), nullptr, size, mode));
    }
  } catch (...) {
    errno_catch_handler();
    return nullptr;
  }
}

FILE* fopen(const char*__restrict, const char*__restrict) noexcept;  // XXX fstream

int fputc(int cc, FILE* f) noexcept {
  file_lock lck{ f };

  try {
    check_null_file(f);
    f->get_ostream().put(FILE::traits_type::to_char_type(cc));
    if (_predict_false(f->get_ostream().bad())) return EOF;
  } catch (...) {
    errno_catch_handler();
    return EOF;
  }
  return 0;
}

int fputs(const char*__restrict s, FILE*__restrict f) noexcept {
  file_lock lck{ f };

  try {
    check_null_file(f);
    if (_predict_false(s == nullptr)) throw invalid_argument("null string");
    f->get_ostream().write(s, strlen(s));
    return 0;
  } catch (...) {
    errno_catch_handler();
    return EOF;
  }
}

size_t fread(void*__restrict ptr, size_t size, size_t nitems,
             FILE*__restrict f) noexcept {
  using abi::umul_overflow;
  using ussize = make_unsigned_t<streamsize>;

  file_lock lck{ f };

  try {
    check_null_file(f);
    auto& in = f->get_istream();

    size_t n = 0;
    char*__restrict p = static_cast<char*__restrict>(ptr);
    for (n = 0; n < nitems && in; p += size) {
      if (!in.read(p, n)) break;
      ++n;
    }
    return n;
  } catch (...) {
    errno_catch_handler();
    return 0;
  }
}

FILE* freopen(const char*__restrict, const char*__restrict, FILE*__restrict)
    noexcept;  // XXX

int fscanf(FILE*__restrict, const char*__restrict, ...) noexcept;  // XXX

int fseek(FILE* f, long off, int whence) noexcept {
  file_lock lck{ f };

  return fseeko(f, off, whence);
}

int fseeko(FILE* f, off_t off, int whence) noexcept {
  file_lock lck{ f };

  try {
    check_null_file(f);

    ios_base::seekdir seekdir;
    switch (whence) {
    default:
      throw invalid_argument("invalid seek direction");
    case SEEK_SET:
      seekdir = ios_base::beg;
      break;
    case SEEK_CUR:
      seekdir = ios_base::cur;
      break;
    case SEEK_END:
      seekdir = ios_base::end;
      break;
    }

    auto rv = f->rdbuf().pubseekoff(off, seekdir);
    const decltype(rv) fail{ fpos_t(-1) };
    if (_predict_false(rv == fail))
      throw system_error(make_error_code(errc::bad_file_descriptor));
    return 0;
  } catch (...) {
    errno_catch_handler();
    return -1;
  }
}

int fsetpos(FILE* f, const fpos_t* pos) noexcept {
  file_lock lck{ f };

  try {
    check_null_file(f);

    auto rv = f->rdbuf().pubseekpos(*pos);
    const decltype(rv) fail{ fpos_t(-1) };
    if (_predict_false(rv == fail))
      throw system_error(make_error_code(errc::bad_file_descriptor));
    return 0;
  } catch (...) {
    errno_catch_handler();
    return -1;
  }
}

long ftell(FILE* f) noexcept {
  off_t rv = ftello(f);
  if (_predict_false(rv > LONG_MAX)) {
    errno = _ABI_EOVERFLOW;
    return -1;
  }
  return rv;
}

off_t ftello(FILE* f) noexcept {
  file_lock lck{ f };

  try {
    auto rv = f->rdbuf().pubseekoff(0, ios_base::cur);
    const decltype(rv) fail{ fpos_t(-1) };
    const decltype(rv) zero{ fpos_t(0) };

    if (_predict_false(rv == fail))
      throw system_error(make_error_code(errc::bad_file_descriptor));
    return rv - zero;
  } catch (...) {
    errno_catch_handler();
    return -1;
  }
}

int ftrylockfile(FILE* f) noexcept {
  try {
    check_null_file(f);
    if (f->try_lock()) return 0;
  } catch (...) {
    errno_catch_handler();
  }
  return -1;
}

void funlockfile(FILE* f) noexcept {
  try {
    check_null_file(f);
    f->lock();
  } catch (...) {
    errno_catch_handler();
  }
}

size_t fwrite(const void*__restrict ptr, size_t size, size_t nitems,
              FILE*__restrict f) noexcept {
  using abi::umul_overflow;
  using ussize = make_unsigned_t<streamsize>;

  file_lock lck{ f };

  size_t n = 0;
  try {
    check_null_file(f);
    auto& out = f->get_ostream();

    const char*__restrict p = static_cast<const char*__restrict>(ptr);
    for (n = 0; n < nitems && out; p += size) {
      if (!out.write(p, n)) break;
      ++n;
    }
    return n;
  } catch (...) {
    errno_catch_handler();
    return n;
  }
}

int getc(FILE* f) noexcept {
  file_lock lck{ f };

  try {
    check_null_file(f);
    return f->get_istream().get();
  } catch (...) {
    errno_catch_handler();
    return EOF;
  }
}

int getchar() noexcept;  // XXX stdio

int getc_unlocked(FILE* f) noexcept {
  try {
    check_null_file(f);
    return f->get_istream().get();
  } catch (...) {
    errno_catch_handler();
    return EOF;
  }
}

int getchar_unlocked() noexcept;  // XXX stdio

ssize_t getdelim(char**__restrict lineptr, size_t*__restrict n, int delimiter,
                 FILE*__restrict f) noexcept {
  using impl::copybuf;

  file_lock lck{ f };

  try {
    check_null_file(f);
    if (_predict_false(lineptr == nullptr))
      throw invalid_argument("lineptr");
    if (_predict_false(n && *n != 0 && *lineptr == nullptr))
      throw invalid_argument("lineptr is a nullptr, but size is given");

    istream& in = f->get_istream();
    basic_ostringstream<FILE::char_type, FILE::traits_type> tmp;
    const auto len = copybuf(*tmp.rdbuf(), *in.rdbuf(),
                             FILE::traits_type::to_char_type(delimiter));

    if (!in.eof()) {
      const auto cc = in.rdbuf()->sgetc();
      if (!FILE::traits_type::eq_int_type(cc, FILE::traits_type::eof())) {
        assert(FILE::traits_type::eq_int_type(cc, delimiter));
        const auto c = FILE::traits_type::to_char_type(cc);
        auto put_result = tmp.rdbuf()->sputc(c);
        assert(!FILE::traits_type::eq_int_type(put_result,
                                               FILE::traits_type::eof()));
        in.rdbuf()->sbumpc();
      }
    } else if (len == 0) {
      return -1;
    }

    const auto result = tmp.str();
    if (!n || *n < result.size() + 1U) {
      char* repl = static_cast<char*>(realloc(*lineptr, result.size() + 1U));
      if (!repl) throw bad_alloc();

      *lineptr = repl;
      if (n) *n = result.size() + 1U;
    }
    copy_n(result.c_str(), result.size() + 1U, *lineptr);
    return result.size();
  } catch (...) {
    errno_catch_handler();
    return -1;
  }
}

ssize_t getline(char**__restrict lineptr, size_t*__restrict n,
                FILE*__restrict f) noexcept {
  return getdelim(lineptr, n, FILE::traits_type::to_int_type('\n'), f);
}

char* gets(char*) noexcept;  // XXX stdio

FILE* open_memstream(char** pub_buf, size_t* pub_len) noexcept {
  using memstream = basic_memstream<char>;
  using file = ostream_file<memstream>;

  try {
    return new file(memstream(*pub_buf, *pub_len));
  } catch (...) {
    errno_catch_handler();
    return nullptr;
  }
}

int pclose(FILE*) noexcept;  // XXX popen
void perror(const char*) noexcept;  // XXX stderr
FILE* popen(const char*, const char*) noexcept;  // XXX fork

int putc(int cc, FILE* f) noexcept {
  return fputc(cc, f);
}

int putchar(int) noexcept;  // XXX stdio

int putc_unlocked(int cc, FILE* f) noexcept {
  file_lock lck{ f };

  try {
    check_null_file(f);
    f->get_ostream().put(FILE::traits_type::to_char_type(cc));
    if (_predict_false(f->get_ostream().bad())) return EOF;
  } catch (...) {
    errno_catch_handler();
    return EOF;
  }
  return 0;
}

int putchar_unlocked(int) noexcept;  // XXX stdio
int puts(const char*) noexcept;  // XXX stdio
int remove(const char*) noexcept;  // XXX stdio
int rename(const char*, const char*) noexcept;  // XXX filesystem
int renameat(int, const char*, int, const char*) noexcept;  // XXX filesystem

void rewind(FILE* f) noexcept {
  file_lock lck{ f };

  try {
    check_null_file(f);

    f->rdbuf().pubseekoff(0, ios_base::beg);
    f->get_ios().setstate(ios_base::goodbit);
  } catch (...) {
    errno_catch_handler();
  }
}

int scanf(const char*__restrict, ...) noexcept;  // XXX

void setbuf(FILE*__restrict f, char*__restrict buf) noexcept {
  file_lock lck{ f };

  try {
    check_null_file(f);
    f->rdbuf().pubsetbuf(buf, BUFSIZ);
  } catch (...) {
    errno_catch_handler();
  }
}

int setvbuf(FILE*__restrict f, char*__restrict buf, int type, size_t size)
    noexcept {
  file_lock lck{ f };

  try {
    check_null_file(f);
    switch (type) {
    default:
      throw invalid_argument("type must be one of _IOFBF, _IOLBF or IONBF");
    case _IOFBF:
    case _IOLBF:
    case _IONBF:
      break;
    }

    f->rdbuf().pubsetbuf(buf, size);
    return 0;
  } catch (...) {
    errno_catch_handler();
    return -1;
  }
}

int sscanf(const char*__restrict, const char*__restrict, ...) noexcept;  // XXX
char* tempnam(const char*, const char*) noexcept;  // OBsolete
FILE* tmpfile() noexcept;  // XXX fstream, filesystem
char* tmpnam(char*) noexcept;  // OBsolete

int ungetc(int cc, FILE* f) noexcept {
  file_lock lck{ f };

  try {
    check_null_file(f);

    const auto c = FILE::traits_type::to_char_type(cc);
    const auto rv = f->get_istream().rdbuf()->sputbackc(c);
    if (FILE::traits_type::eq_int_type(rv, FILE::traits_type::eof()))
      return EOF;
  } catch (...) {
    errno_catch_handler();
    return EOF;
  }
  return 0;
}

int vfscanf(FILE*__restrict, const char*__restrict, va_list) noexcept;  // XXX
int vscanf(const char*__restrict, va_list) noexcept;  // XXX
int vsscanf(const char*__restrict, const char*__restrict, va_list) noexcept;  // XXX


/*
 * printf functions forward to their vprintf counterparts.
 */

#if 0 // XXX: need more environ
int dprintf(int fd, const char*__restrict fmt, ...) noexcept {
  va_list ap;

  va_start(ap, fmt);
  int rv = vdprintf(fd, fmt, ap);
  va_end(ap);
  return rv;
}

int fprintf(FILE*__restrict file, const char*__restrict fmt, ...) noexcept {
  va_list ap;

  va_start(ap, fmt);
  int rv = vfprintf(file, fmt, ap);
  va_end(ap);
  return rv;
}

int printf(const char*__restrict fmt, ...) noexcept {
  va_list ap;

  va_start(ap, fmt);
  int rv = vprintf(fmt, ap);
  va_end(ap);
  return rv;
}
#endif

int snprintf(char*__restrict s, size_t sz, const char*__restrict fmt, ...)
    noexcept {
  va_list ap;

  va_start(ap, fmt);
  int rv = vsnprintf(s, sz, fmt, ap);
  va_end(ap);
  return rv;
}

int sprintf(char*__restrict s, const char*__restrict fmt, ...) noexcept {
  va_list ap;

  va_start(ap, fmt);
  int rv = vsprintf(s, fmt, ap);
  va_end(ap);
  return rv;
}

int asprintf(char** sptr, const char*__restrict fmt, ...) noexcept {
  va_list ap;

  va_start(ap, fmt);
  int rv = vasprintf(sptr, fmt, ap);
  va_end(ap);
  return rv;
}


#if 0 // XXX: need more environ
int vdprintf(int, const char*__restrict, va_list) noexcept;
int vfprintf(FILE*__restrict, const char*__restrict, va_list) noexcept;
int vprintf(const char*__restrict, va_list) noexcept;
#endif

int vsnprintf(char*__restrict s, size_t sz, const char*__restrict fmt,
              va_list ap) noexcept {
  /* Local renderer implementation. */
  class vsn_renderer : public abi::ext::printf_renderer<char> {
   public:
    vsn_renderer(char* out, size_t outsz) noexcept
    : out_(outsz == 0 ? nullptr: out),
      spc_(outsz == 0 ? 0 : outsz - 1)
    {}

    ~vsn_renderer() noexcept override {
      if (this->out_) *this->out_ = '\0';
    }

   private:
    char* out_;
    size_t spc_;

    int do_append(string_ref sp) noexcept override {
      if (out_) {
        if (sp.size() > spc_) sp = sp.substr(0, spc_);
        memcpy(out_, sp.data(), sp.size());
        out_ += sp.size();
        spc_ -= sp.size();
      }
      return 0;
    }
  };

  /* Check arguments. */
  if (_predict_false(s == nullptr && sz != 0)) return _ABI_EINVAL;
  if (_predict_false(fmt == nullptr)) return _ABI_EINVAL;

  /* Rendering stage. */
  vsn_renderer renderer{ s, sz };
  int error = 0;
  {
    abi::ext::vxprintf_locals<char> locals;
    if (!error) error = locals.parse_fmt(fmt);
    if (!error) error = locals.load_arguments(ap);
    if (!error) error = locals.resolve_fieldwidth();
    if (!error) error = locals.render(renderer);
  }

  if (error) {
    errno = error;
    return -1;
  }
  return (renderer.length() > INT_MAX ? INT_MAX : renderer.length());
}

int vsprintf(char*__restrict s, const char*__restrict fmt, va_list ap)
    noexcept {
  /* Local renderer implementation. */
  class vs_renderer : public abi::ext::printf_renderer<char> {
   public:
    explicit vs_renderer(char* out) noexcept
    : out_(out)
    {}

    ~vs_renderer() noexcept override {
      *this->out_ = '\0';
    }

   private:
    char* out_;

    int do_append(string_ref sp) noexcept override {
      memcpy(out_, sp.data(), sp.size());
      out_ += sp.size();
      return 0;
    }
  };

  /* Check arguments. */
  if (_predict_false(s == nullptr)) return _ABI_EINVAL;
  if (_predict_false(fmt == nullptr)) return _ABI_EINVAL;

  /* Rendering stage. */
  vs_renderer renderer{ s };
  int error = 0;
  {
    abi::ext::vxprintf_locals<char> locals;
    if (!error) error = locals.parse_fmt(fmt);
    if (!error) error = locals.load_arguments(ap);
    if (!error) error = locals.resolve_fieldwidth();
    if (!error) error = locals.render(renderer);
  }

  if (error) {
    errno = error;
    return -1;
  }
  return (renderer.length() > INT_MAX ? INT_MAX : renderer.length());
}

int vasprintf(char** sptr, const char*__restrict fmt, va_list ap) noexcept {
  /* Local renderer implementation. */
  class vas_renderer : public abi::ext::printf_renderer<char> {
   public:
    vas_renderer(size_t sz = 64) noexcept
    : s_(nullptr),
      s_end_(nullptr),
      s_avail_(0)
    {
      if (sz == 0) sz = 1;
      if ((s_ = static_cast<char*>(malloc(sz)))) {
        s_end_ = s_;
        s_avail_ = sz - 1U;
      }
    }

    vas_renderer(const vas_renderer&) = delete;
    vas_renderer& operator=(const vas_renderer&) = delete;

    ~vas_renderer() noexcept override { if (s_ != nullptr) free(s_); }

    char* release() noexcept {
      if (!s_) return nullptr;
      char* rv = s_;
      *s_end_ = '\0';
      s_ = s_end_ = nullptr;
      return rv;
    }

    explicit operator bool() const noexcept { return bool(s_); }

   private:
    char* s_;  // Start of result string (not nul terminated).
    char* s_end_;  // First unused position of result string.
    size_t s_avail_;  // Bytes available at s_end_.

    int do_append(string_ref sp) noexcept override {
      if (s_avail_ < sp.size()) {
        const auto len = (s_end_ - s_);

        /* Try an opportunistic large allocation. */
        size_t alt_sz = 2 * (len + s_avail_ + sp.size()) + 1U;
        char* alt = static_cast<char*>(realloc(s_, alt_sz));
        if (!alt) {
          /* Failing a large allocation, only allocate what we need. */
          alt_sz = len + s_avail_ + sp.size() + 1U;
          alt = static_cast<char*>(realloc(s_, alt_sz));
        }
        if (!alt) return _ABI_ENOMEM;

        s_ = alt;
        s_end_ = s_ + len;
        s_avail_ = alt_sz - 1 - len;
      }

      memcpy(s_end_, sp.data(), sp.size());
      s_end_ += sp.size();
      s_avail_ -= sp.size();
      return 0;
    }
  };

  /* Check arguments. */
  if (_predict_false(sptr == nullptr)) return _ABI_EINVAL;
  if (_predict_false(fmt == nullptr)) return _ABI_EINVAL;

  /* Rendering stage. */
  vas_renderer renderer;
  if (!renderer) return _ABI_ENOMEM;
  int error = 0;
  {
    abi::ext::vxprintf_locals<char> locals;
    if (!error) error = locals.parse_fmt(fmt);
    if (!error) error = locals.load_arguments(ap);
    if (!error) error = locals.resolve_fieldwidth();
    if (!error) error = locals.render(renderer);
  }

  if (error) {
    *sptr = nullptr;  // Not required, but safe.
    errno = error;
    return -1;
  }
  *sptr = renderer.release();
  return (renderer.length() > INT_MAX ? INT_MAX : renderer.length());
}


namespace {


template<typename Char> class renderer
: public abi::ext::printf_renderer<Char> {
 public:
  renderer(basic_string<Char>& str) noexcept : str_(str) {}
  ~renderer() noexcept override;

  int do_append(basic_string_ref<Char>) noexcept override;

 private:
  basic_string<Char>& str_;
};

template<typename Char>
int renderer<Char>::do_append(basic_string_ref<Char> sp) noexcept {
  try {
    str_.append(sp);
  } catch (const length_error&) {
    return _ABI_ENOMEM;
  } catch (const bad_alloc&) {
    return _ABI_ENOMEM;
  }
  return 0;
}

template<typename Char>
renderer<Char>::~renderer() noexcept {}


} /* namespace std::<unnamed> */


string format(string_ref fmt, ...) {
  string rv;
  int error;
  {
    va_list ap;
    va_start(ap, fmt);
    renderer<char> r{ rv };
    error = abi::ext::vxprintf(r, fmt, ap);
    va_end(ap);
  }

  if (_predict_false(error == _ABI_ENOMEM)) throw bad_alloc();
  if (_predict_false(error != 0)) throw system_error(error, system_category(),
                                                     "std::format");
  return rv;
}

wstring format(wstring_ref fmt, ...) {
  wstring rv;
  int error;
  {
    va_list ap;
    va_start(ap, fmt);
    renderer<wchar_t> r{ rv };
    error = abi::ext::vxprintf(r, fmt, ap);
    va_end(ap);
  }

  if (_predict_false(error == _ABI_ENOMEM)) throw bad_alloc();
  if (_predict_false(error != 0)) throw system_error(error, system_category(),
                                                     "std::format");
  return rv;
}

u16string format(u16string_ref fmt, ...) {
  u16string rv;
  int error;
  {
    va_list ap;
    va_start(ap, fmt);
    renderer<char16_t> r{ rv };
    error = abi::ext::vxprintf(r, fmt, ap);
    va_end(ap);
  }

  if (_predict_false(error == _ABI_ENOMEM)) throw bad_alloc();
  if (_predict_false(error != 0)) throw system_error(error, system_category(),
                                                     "std::format");
  return rv;
}

u32string format(u32string_ref fmt, ...) {
  u32string rv;
  int error;
  {
    va_list ap;
    va_start(ap, fmt);
    renderer<char32_t> r{ rv };
    error = abi::ext::vxprintf(r, fmt, ap);
    va_end(ap);
  }

  if (_predict_false(error == _ABI_ENOMEM)) throw bad_alloc();
  if (_predict_false(error != 0)) throw system_error(error, system_category(),
                                                     "std::format");
  return rv;
}



_namespace_end(std)
