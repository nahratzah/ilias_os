#include <ios>
#include <string>
#include <atomic>
#include <cstdio>
#include <string>

_namespace_begin(std)


ios_base::failure::failure(const string& msg, const error_code& ec)
: system_error(ec, msg)
{}

ios_base::failure::failure(const char* msg, const error_code& ec)
: system_error(ec, msg)
{}

ios_base::failure::failure(string_ref msg, const error_code& ec)
: system_error(ec, msg)
{}

ios_base::failure::~failure() noexcept {}


ios_base::Init::Init() noexcept {
  // XXX increment a static reference counter,
  // force initialization of std::{w,}c{in,out,err,log}.
}

ios_base::Init::~Init() noexcept {
  // XXX decrement a static reference counter,
  // only allow destruction of std::{w,}c{in,out,err,log}
  // if the counter reaches zero
}


locale ios_base::imbue(const locale& l) {
  auto rv = exchange(loc_, l);
  invoke_event_cb(imbue_event);
  return rv;
}

locale ios_base::getloc() const {
  return loc_;
}

bool ios_base::sync_with_stdio(bool /*sync*/) {
  return true; // XXX implement
}

int ios_base::xalloc() {
  static atomic<unsigned int> impl;
  const unsigned int rv = impl.fetch_add(1U, memory_order_relaxed);
  if (_predict_false(rv > INT_MAX)) {
    impl.fetch_sub(1, memory_order_relaxed);
    throw runtime_error("ios_base::xalloc: all integers assigned");
  }
  return rv;
}

namespace {

long iword_fallback;
void* pword_fallback;

} /* namespace std::<unnamed> */

long& ios_base::iword(int idx) {
  try {
    if (_predict_false(idx < 0))
      throw invalid_argument("ios_base::iword: idx");
    unsigned int sz = idx;
    ++sz;
    if (_predict_false(sz > iarray_.size()))
      iarray_.resize(sz);
    return iarray_.at(idx);
  } catch (...) {
    impl::basic_ios_derived* self =
        dynamic_cast<impl::basic_ios_derived*>(this);
    if (self) self->setstate(badbit);
    iword_fallback = 0;
    return iword_fallback;
  }
}

void*& ios_base::pword(int idx) {
  try {
    if (_predict_false(idx < 0))
      throw invalid_argument("ios_base::pword: idx");
    unsigned int sz = idx;
    ++sz;
    if (_predict_false(sz > parray_.size()))
      parray_.resize(sz);
    return parray_.at(idx);
  } catch (...) {
    impl::basic_ios_derived* self =
        dynamic_cast<impl::basic_ios_derived*>(this);
    if (self) self->setstate(badbit);
    pword_fallback = nullptr;
    return pword_fallback;
  }
}

void ios_base::register_callback(event_callback cb, int index) {
  events_.emplace_back(cb, index);
}

void ios_base::invoke_event_cb(event ev) noexcept {
  /*
   * Invoke all events in reverse order of registration.
   * Events registered during callbacks are not registered.
   * We use an index to iterate, to avoid getting iterator invalidation
   * if someone registers a new callback during execution.
   */
  auto i = events_.size();
  while (i-- > 0) events_[i].invoke(ev, *this);
}

ios_base::ios_base() {}

ios_base::~ios_base() noexcept {
  invoke_event_cb(erase_event);
}


namespace impl {

basic_ios_derived::~basic_ios_derived() noexcept {}

auto basic_ios_derived::clear_(ios_base::iostate state, bool throw_allowed) ->
    ios_base::iostate {
  const auto rv = exchange(iostate_, state);

  ios_base::iostate masked = rdstate() & exceptions();
  if (_predict_false(masked != static_cast<ios_base::iostate>(0)) &&
      throw_allowed) {
    string msg;
    if ((masked & ios_base::eofbit) != static_cast<ios_base::iostate>(0)) {
      if (!msg.empty()) msg += ", ";
      msg += "eof";
    }
    if ((masked & ios_base::failbit) != static_cast<ios_base::iostate>(0)) {
      if (!msg.empty()) msg += ", ";
      msg += "fail";
    }
    if ((masked & ios_base::badbit) != static_cast<ios_base::iostate>(0)) {
      if (!msg.empty()) msg += ", ";
      msg += "bad";
    }
    throw ios_base::failure(msg);
  }

  return rv;
}

} /* namespace std::impl */


namespace {


class iostream_error_category : public error_category {
  const char* name() const noexcept override;
  string message(int) const override;
};

const char* iostream_error_category::name() const noexcept {
  return "iostream";
}

string iostream_error_category::message(int code) const {
  if (code == static_cast<int>(io_errc::stream))
    return "stream error";

  constexpr int N = 16;
  char buf[N];
  string rv;

  string_ref bufstr = string_ref(buf, N - 1);
  int buflen = snprintf(buf, N, ":%d", code);
  if (buflen < 0) {
    switch (abi::errno) {
    case _ABI_ENOMEM:
      throw bad_alloc();
    default:
      assert(false);
      throw runtime_error("iostream_error_category::message: printf failed");
    }
  }

  string_ref name = this->name();
  bufstr = bufstr.substr(0, buflen);
  rv.reserve(name.length() + bufstr.length());
  rv += name;
  rv += bufstr;
  return rv;
}

iostream_error_category iostream_error_category_impl;


} /* namespace std::<unnamed> */


const error_category& iostream_category() noexcept {
  return iostream_error_category_impl;
}


template class basic_ios<char>;
template class basic_ios<char16_t>;
template class basic_ios<char32_t>;
template class basic_ios<wchar_t>;

constexpr ios_base::fmtflags ios_base::boolalpha;
constexpr ios_base::fmtflags ios_base::dec;
constexpr ios_base::fmtflags ios_base::fixed;
constexpr ios_base::fmtflags ios_base::hex;
constexpr ios_base::fmtflags ios_base::internal;
constexpr ios_base::fmtflags ios_base::left;
constexpr ios_base::fmtflags ios_base::oct;
constexpr ios_base::fmtflags ios_base::right;
constexpr ios_base::fmtflags ios_base::scientific;
constexpr ios_base::fmtflags ios_base::showbase;
constexpr ios_base::fmtflags ios_base::showpoint;
constexpr ios_base::fmtflags ios_base::showpos;
constexpr ios_base::fmtflags ios_base::skipws;
constexpr ios_base::fmtflags ios_base::unitbuf;
constexpr ios_base::fmtflags ios_base::uppercase;
constexpr ios_base::fmtflags ios_base::adjustfield;
constexpr ios_base::fmtflags ios_base::basefield;
constexpr ios_base::fmtflags ios_base::floatfield;

constexpr ios_base::iostate ios_base::badbit;
constexpr ios_base::iostate ios_base::eofbit;
constexpr ios_base::iostate ios_base::failbit;
constexpr ios_base::iostate ios_base::goodbit;

constexpr ios_base::openmode ios_base::app;
constexpr ios_base::openmode ios_base::ate;
constexpr ios_base::openmode ios_base::binary;
constexpr ios_base::openmode ios_base::in;
constexpr ios_base::openmode ios_base::out;
constexpr ios_base::openmode ios_base::trunc;

constexpr ios_base::seekdir ios_base::beg;
constexpr ios_base::seekdir ios_base::cur;
constexpr ios_base::seekdir ios_base::end;


_namespace_end(std)
