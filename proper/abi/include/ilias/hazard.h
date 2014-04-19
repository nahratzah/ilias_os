#ifndef _ILIAS_HAZARD_H_
#define _ILIAS_HAZARD_H_

#include <cdecl.h>
#include <cstdint>
#include <atomic>
#include <cassert>
#include <stdimpl/invoke.h>

_namespace_begin(ilias)


bool hazard_is_lock_free() noexcept;


enum class hazard_acquisition {
  once,
  twice
};

enum class hazard_deallocate {
  unowned,
  owned
};

class alignas(64) basic_hazard_data {
  friend bool hazard_is_lock_free() noexcept;

 public:
  static constexpr uintptr_t ALIGN = 2;
  static constexpr uintptr_t MASK = ALIGN - 1;
  static const size_t N_SLOTS;

  basic_hazard_data() noexcept = default;
  basic_hazard_data(const basic_hazard_data&) = delete;
  basic_hazard_data& operator=(const basic_hazard_data&) = delete;

  static basic_hazard_data* allocate(uintptr_t, uintptr_t) noexcept;
  static auto deallocate(basic_hazard_data*) noexcept -> hazard_deallocate;
  static bool grant_1(uintptr_t, uintptr_t) noexcept;
  static size_t grant_all(uintptr_t, uintptr_t, size_t) noexcept;

 private:
  static bool try_claim(basic_hazard_data&, uintptr_t) noexcept;
  static bool try_grant(basic_hazard_data&, uintptr_t, uintptr_t) noexcept;

  _namespace(std)::atomic<uintptr_t> header_;
  _namespace(std)::atomic<uintptr_t> satelite_;
};

namespace basic_hazard_support {

using _namespace(std)::impl::invoke;
using _namespace(std)::declval;

template<typename Fn, typename Header, typename Satelite, typename = int>
struct _invoke_n {
  void operator()(Fn& fn, Header* hdr, Satelite* sat, size_t n) const
      noexcept {
    while (n-- > 0) invoke(fn, hdr, sat);
  }
};

template<typename Fn, typename Header, typename Satelite>
struct _invoke_n<Fn, Header, Satelite,
                decltype(invoke(declval<Fn&>(),
                                declval<Header*>(),
                                declval<Satelite*>(),
                                declval<size_t>()),
                         int(0))> {
  void operator()(Fn& fn, Header* hdr, Satelite* sat, size_t n) const
      noexcept {
    invoke(fn, hdr, sat, n);
  }
};

template<typename Fn, typename Header, typename Satelite>
void invoke_n(Fn& fn, Header* hdr, Satelite* sat, size_t n) {
  _invoke_n<Fn, Header, Satelite>()(fn, hdr, sat, n);
}

} /* namespace ilias::basic_hazard_support */

template<typename Header, typename Satelite>
class basic_hazard {
 public:
  static_assert(alignof(Header) % basic_hazard_data::ALIGN == 0,
                "Header must have alignment of at least 2 bytes.");

  constexpr basic_hazard() noexcept;
  basic_hazard(const basic_hazard&) = delete;
  basic_hazard(basic_hazard&&) noexcept;
  basic_hazard(Header*, Satelite*) noexcept;
  ~basic_hazard() noexcept;

  static bool grant_1(Header*, Satelite*) noexcept;

  template<typename AcquireFn, typename ReleaseFn>
  static void grant_all(Header*, Satelite*, AcquireFn, ReleaseFn, size_t = 0)
      noexcept;

  void lock(Header*, Satelite*) noexcept;
  hazard_deallocate unlock() noexcept;

 private:
  basic_hazard_data* hd_ = nullptr;
};


template<typename Header, typename Satelite>
basic_hazard<Header, Satelite>::basic_hazard(basic_hazard&& other) noexcept
: hd_(exchange(other.hd_, nullptr))
{}

template<typename Header, typename Satelite>
basic_hazard<Header, Satelite>::basic_hazard(Header* hdr, Satelite* sat)
    noexcept {
  lock(hdr, sat);
}

template<typename Header, typename Satelite>
basic_hazard<Header, Satelite>::~basic_hazard() noexcept {
  assert(hd_ == nullptr);
}

template<typename Header, typename Satelite>
auto basic_hazard<Header, Satelite>::grant_1(Header* hdr, Satelite* sat)
    noexcept -> bool {
  return basic_hazard_data::grant_1(reinterpret_cast<uintptr_t>(hdr),
                                    reinterpret_cast<uintptr_t>(sat));
}

template<typename Header, typename Satelite>
template<typename AcquireFn, typename ReleaseFn>
void basic_hazard<Header, Satelite>::grant_all(Header* hdr, Satelite* sat,
                                               AcquireFn acquire,
                                               ReleaseFn release,
                                               size_t n) noexcept {
  const auto N_SLOTS = basic_hazard_data::N_SLOTS;

  if (n < N_SLOTS) {
    basic_hazard_support::invoke_n(ref(acquire), hdr, sat, N_SLOTS - n);
    n = N_SLOTS;
  }

  n -= basic_hazard_data::grant_all(reinterpret_cast<uintptr_t>(hdr),
                                    reinterpret_cast<uintptr_t>(sat), n);
  if (n != 0) basic_hazard_support::invoke_n(ref(release), hdr, sat, n);
}

template<typename Header, typename Satelite>
auto basic_hazard<Header, Satelite>::lock(Header* hdr, Satelite* sat)
    noexcept -> void {
  hd_ = basic_hazard_data::allocate(reinterpret_cast<uintptr_t>(hdr),
                                    reinterpret_cast<uintptr_t>(sat));
}

template<typename Header, typename Satelite>
auto basic_hazard<Header, Satelite>::unlock() noexcept -> hazard_deallocate {
  assert(hd_ != nullptr);
  return basic_hazard_data::deallocate(hd_);
}


_namespace_end(ilias)

#endif /* _ILIAS_HAZARD_H_ */
