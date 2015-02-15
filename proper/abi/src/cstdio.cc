#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <system_error>
#include <abi/ext/printf.h>

_namespace_begin(std)


#if 0  // XXX: need std::c{in,out,err}
extern FILE* const stdin = reinterpret_cast<FILE*>(&cin);
extern FILE* const stdout = reinterpret_cast<FILE*>(&cout);
extern FILE* const stderr = reinterpret_cast<FILE*>(&cerr);
#endif


void clearerr(FILE*) noexcept;
char* ctermid(char*) noexcept;
int fclose(FILE*) noexcept;
FILE* fdopen(int, const char*) noexcept;
int feof(FILE*) noexcept;
int ferror(FILE*) noexcept;
int fflush(FILE*) noexcept;
int fgetc(FILE*) noexcept;
int fgetpos(FILE*__restrict, fpos_t*__restrict) noexcept;
char* fgets(char*__restrict, int, FILE*__restrict) noexcept;
int fileno(FILE*) noexcept;
void flockfile(FILE*) noexcept;
FILE* fmemopen(void*__restrict, size_t, const char*__restrict) noexcept;
FILE* fopen(const char*__restrict, const char*__restrict) noexcept;
int fputc(int, FILE*) noexcept;
int fputs(const char*, FILE*) noexcept;
size_t fread(void*__restrict, size_t, size_t, FILE*__restrict) noexcept;
FILE* freopen(const char*__restrict, const char*__restrict, FILE*__restrict)
    noexcept;
int fscanf(FILE*__restrict, const char*__restrict, ...) noexcept;
int fseek(FILE*, long, int) noexcept;
int fseeko(FILE*, off_t, int) noexcept;
int fsetpos(FILE*, const fpos_t*) noexcept;
long ftell(FILE*) noexcept;
off_t ftello(FILE*) noexcept;
int ftrylockfile(FILE*) noexcept;
void funlockfile(FILE*) noexcept;
size_t fwrite(const void*__restrict, size_t, size_t, FILE*__restrict) noexcept;
int getc(FILE*) noexcept;
int getchar() noexcept;
int getc_unlocked(FILE*) noexcept;
int getchar_unlocked() noexcept;
ssize_t getdelim(char**__restrict, size_t*__restrict, int, FILE*__restrict)
    noexcept;
ssize_t getline(char**__restrict, size_t*__restrict, FILE*__restrict) noexcept;
char* gets(char*) noexcept;
FILE* open_memstream(char**, size_t) noexcept;
int pclose(FILE*) noexcept;
void perror(const char*) noexcept;
FILE* popen(const char*, const char*) noexcept;
int putc(int, FILE*) noexcept;
int putchar(int) noexcept;
int putc_unlocked(int, FILE*) noexcept;
int putchar_unlocked(int) noexcept;
int puts(const char*) noexcept;
int remove(const char*) noexcept;
int rename(const char*, const char*) noexcept;
int renameat(int, const char*, int, const char*) noexcept;
void rewind(FILE*) noexcept;
int scanf(const char*__restrict, ...) noexcept;
void setbuf(FILE*__restrict, char*__restrict) noexcept;
int setvbuf(FILE*__restrict, char*__restrict, int, size_t) noexcept;
int sscanf(const char*__restrict, const char*__restrict, ...) noexcept;
char* tempnam(const char*, const char*) noexcept;  // OBsolete
FILE* tmpfile() noexcept;
char* tmpnam(char*) noexcept;  // OBsolete
int ungetc(int, FILE*) noexcept;
int vfscanf(FILE*__restrict, const char*__restrict, va_list) noexcept;
int vscanf(const char*__restrict, va_list) noexcept;
int vsscanf(const char*__restrict, const char*__restrict, va_list) noexcept;


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
