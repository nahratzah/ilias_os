#include <ilias/hazard.h>
#include <array>
#include <atomic>

using namespace _namespace(std);

_namespace_begin(ilias)
namespace {


#ifdef _LOADER
constexpr size_t N_HAZARDS = 4;
#else
constexpr size_t N_HAZARDS = 256;
#endif

array<basic_hazard_data, N_HAZARDS> hazards;
atomic<size_t> hint;


} /* namespace ilias::<unnamed> */


bool basic_hazard_data::try_claim(basic_hazard_data& hd,
                                  uintptr_t header) noexcept {
  uintptr_t expect = 0;
  return hd.header_.compare_exchange_strong(expect, header,
                                            memory_order_relaxed,
                                            memory_order_relaxed);
}

bool basic_hazard_data::try_grant(basic_hazard_data& hd,
                                  uintptr_t hdr, uintptr_t sat) noexcept {
  uintptr_t expect = hdr;
  while (_predict_false(!hd.header_.compare_exchange_weak(
      hdr, hdr | MASK, memory_order_acquire, memory_order_relaxed))) {
    if (expect != hdr) return false;
  }

  const bool succes = hd.satelite_.compare_exchange_strong(
      sat, 0, memory_order_release, memory_order_relaxed);
  hd.header_.fetch_and(~basic_hazard_data::MASK, memory_order_release);
  return succes;
}


const size_t basic_hazard_data::N_SLOTS = N_HAZARDS;

auto basic_hazard_data::allocate(uintptr_t hdr, uintptr_t sat)
    noexcept -> basic_hazard_data* {
  using _namespace(std)::placeholders::_1;

  assert_msg(hdr != 0,
             "Cannot allocate hazard structure for nullptr header.");
  assert_msg(sat != 0,
             "Cannot allocate hazard structure for nullptr satelite.");

  auto off = hint.fetch_add(1U, memory_order_relaxed);
  off %= N_HAZARDS;

  auto found = find_if(hazards.begin() + off, hazards.end(),
                       bind(&basic_hazard_data::try_claim, _1, hdr));
  while (_predict_false(found == hazards.end())) {
    found = find_if(hazards.begin(), hazards.end(),
                    bind(&basic_hazard_data::try_claim, _1, hdr));
  }

  auto old_sat = found->satelite_.exchange(sat, memory_order_acquire);
  assert(old_sat == 0);
  return &*found;
}

auto basic_hazard_data::deallocate(basic_hazard_data* hd)
    noexcept -> hazard_deallocate {
  assert(hd != nullptr);

  uintptr_t sat = hd->satelite_.exchange(0, memory_order_relaxed);
  hd->header_.fetch_and(MASK, memory_order_release);
  return (sat == 0 ? hazard_deallocate::owned : hazard_deallocate::unowned);
}

auto basic_hazard_data::grant_1(uintptr_t hdr, uintptr_t sat) noexcept ->
    bool {
  using _namespace(std)::placeholders::_1;

  assert_msg(hdr != 0,
             "Cannot grant on hazard structure for nullptr header.");
  assert_msg(sat != 0,
             "Cannot grant on hazard structure for nullptr satelite.");

  auto found = find_if(hazards.begin(), hazards.end(),
                       bind(&basic_hazard_data::try_grant, _1, hdr, sat));
  return found != hazards.end();
}

auto basic_hazard_data::grant_all(uintptr_t hdr, uintptr_t sat, size_t n)
    noexcept -> size_t {
  using _namespace(std)::placeholders::_1;

  assert_msg(hdr != 0,
             "Cannot grant on hazard structure for nullptr header.");
  assert_msg(sat != 0,
             "Cannot grant on hazard structure for nullptr satelite.");
  assert_msg(n >= N_HAZARDS, "Insufficient references available to hand out.");

  return count_if(hazards.begin(), hazards.end(),
                  bind(&basic_hazard_data::try_grant, _1, hdr, sat));
}


bool hazard_is_lock_free() noexcept {
  return hazards[0].header_.is_lock_free() &&
         hazards[0].satelite_.is_lock_free();
}


_namespace_end(ilias)
