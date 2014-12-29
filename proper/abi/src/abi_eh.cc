#include <abi/eh.h>
#include <cdecl.h>
#include <cstddef>
#include <abi/memory.h>
#include <abi/panic.h>
#include <abi/semaphore.h>
#include <array>
#include <mutex>
#include <type_traits>
#include <ilias/dwarf/registers.h>

#if __has_include(<ilias/stats.h>)
# include <ilias/stats.h>
#endif

namespace __cxxabiv1 {


struct _Unwind_Context {
  uintptr_t ip;

#if defined(__amd64__) || defined(__x86_64__)
  /* The only registers that are to be used to initialize landing pad. */
  uintptr_t rdi, rsi, rdx, rcx;
#endif
#if defined(__i386__)
  // XXX need some love!
#endif
};

static_assert(
    _namespace(std)::is_trivially_destructible<_Unwind_Context>::value,
    "Unwind context may not have destructors.");

uint64_t _Unwind_GetGR(_Unwind_Context* context, int index) noexcept {
  using _namespace(ilias)::dwarf::dwarf_reg;
  using _namespace(ilias)::dwarf::dwarf_reg_count;
  using _namespace(ilias)::dwarf::dwarf_reg_name;

  assert(index >= 0 && _namespace(std)::size_t(index) < dwarf_reg_count);

  switch (dwarf_reg(index)) {
#if defined(__amd64__) || defined(__x86_64__)
  case dwarf_reg::rdi:
    return context->rdi;
  case dwarf_reg::rsi:
    return context->rsi;
  case dwarf_reg::rdx:
    return context->rdx;
  case dwarf_reg::rcx:
    return context->rcx;
#elif defined(__i386__)
    // XXX need some love!
#endif
  default:
    panic("_Unwind_GetGR: cannot get register %s from context %p",
          dwarf_reg_name[index], context);
    __builtin_unreachable();
  }
}

void _Unwind_SetGR(_Unwind_Context* context, int index,
                   uint64_t new_value) noexcept {
  using _namespace(ilias)::dwarf::dwarf_reg;
  using _namespace(ilias)::dwarf::dwarf_reg_count;
  using _namespace(ilias)::dwarf::dwarf_reg_name;

  assert(index >= 0 && _namespace(std)::size_t(index) < dwarf_reg_count);

  switch (dwarf_reg(index)) {
#if defined(__amd64__) || defined(__x86_64__)
  case dwarf_reg::rdi:
    context->rdi = new_value;
    break;
  case dwarf_reg::rsi:
    context->rsi = new_value;
    break;
  case dwarf_reg::rdx:
    context->rdx = new_value;
    break;
  case dwarf_reg::rcx:
    context->rcx = new_value;
    break;
#elif defined(__i386__)
    // XXX need some love!
#endif
  default:
    panic("_Unwind_SetGR: cannot set register %s = %llx in context %p",
          dwarf_reg_name[index], static_cast<unsigned long long>(new_value),
          context);
    __builtin_unreachable();
  }
}

uint64_t _Unwind_GetIP(_Unwind_Context* context) noexcept {
  return context->ip;
}


_Unwind_Reason_Code __gxx_personality_v0(int version, _Unwind_Action actions,
                                         uint64_t exceptionClass,
                                         _Unwind_Exception* exceptionObject,
                                         _Unwind_Context* context) noexcept {
  using ull = unsigned long long;
  panic("TODO: implement %s(%i, %i, %#llu, %p, %p)", __func__,
        version, int(actions), ull(exceptionClass),
        exceptionObject, context);  // XXX implement
  __builtin_unreachable();
}

void _Unwind_Resume(_Unwind_Exception *exception_object) noexcept {
  panic("TODO: implement %s(%p)", __func__, exception_object);  // XXX implement
  __builtin_unreachable();
}

_Unwind_Reason_Code _Unwind_RaiseException(struct _Unwind_Exception *exception_object) noexcept {
  _Unwind_Context ctx;
  ctx.ip = reinterpret_cast<uintptr_t>(__builtin_extract_return_addr(
                                           __builtin_return_address(0)));

  panic("TODO: implement %s(%p)", __func__, exception_object);  // XXX implement
  __builtin_unreachable();
}


} /* namespace __cxxabiv1 */
