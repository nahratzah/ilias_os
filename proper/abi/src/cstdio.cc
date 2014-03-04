#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <system_error>
#include <abi/ext/printf.h>

namespace std {


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
  if (_predict_false(s == nullptr && sz != 0)) return EINVAL;
  if (_predict_false(fmt == nullptr)) return EINVAL;

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
  if (_predict_false(s == nullptr)) return EINVAL;
  if (_predict_false(fmt == nullptr)) return EINVAL;

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
        if (!alt) return ENOMEM;

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
  if (_predict_false(sptr == nullptr)) return EINVAL;
  if (_predict_false(fmt == nullptr)) return EINVAL;

  /* Rendering stage. */
  vas_renderer renderer;
  if (!renderer) return ENOMEM;
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
  } catch (const std::length_error&) {
    return ENOMEM;
  } catch (const std::bad_alloc&) {
    return ENOMEM;
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

  if (_predict_false(error == ENOMEM)) throw std::bad_alloc();
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

  if (_predict_false(error == ENOMEM)) throw std::bad_alloc();
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

  if (_predict_false(error == ENOMEM)) throw std::bad_alloc();
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

  if (_predict_false(error == ENOMEM)) throw std::bad_alloc();
  if (_predict_false(error != 0)) throw system_error(error, system_category(),
                                                     "std::format");
  return rv;
}


} /* namespace std */
