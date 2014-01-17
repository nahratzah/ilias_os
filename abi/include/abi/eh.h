#ifndef _ABI_EH_H_
#define _ABI_EH_H_

#include <cdecl.h>
#include <abi/abi.h>

#ifdef __cplusplus
# include <atomic>
# include <typeinfo>
#endif

namespace abi {

_cdecl_begin

#ifdef __cplusplus
# define Struct
#else
# define Struct struct
# define noexcept
#endif

enum _Unwind_Reason_Code {
  _URC_NO_REASON = 0,
  _URC_FOREIGN_EXCEPTION_CAUGHT = 1,
  _URC_FATAL_PHASE_2_ERROR = 2,
  _URC_FATAL_PHASE_1_ERROR = 3,
  _URC_NORMAL_STOP = 4,
  _URC_END_OF_STACK = 5,
  _URC_HANDLER_FOUND = 6,
  _URC_INSTALL_CONTEXT = 7,
  _URC_CONTINUE_UNWIND = 8,
};
#ifndef __cplusplus
typedef enum _Unwind_Reason_Code _Unwind_Reason_Code;
#endif


typedef int _Unwind_Action;
static const _Unwind_Action _UA_SEARCH_PHASE = 1;
static const _Unwind_Action _UA_CLEANUP_PHASE = 2;
static const _Unwind_Action _UA_HANDLER_FRAME = 4;
static const _Unwind_Action _UA_FORCE_UNWIND = 8;

/* GCC extension: http://gcc.gnu.org/ml/gcc-patches/2001-09/msg00082.html */
static const _Unwind_Action _UA_END_OF_STACK = 16;


struct _Unwind_Exception;
typedef void (*_Unwind_Exception_Cleanup_Fn)(_Unwind_Reason_Code reason,
    Struct _Unwind_Exception *exc);

struct _Unwind_Exception {
  uint64_t exception_class;
  _Unwind_Exception_Cleanup_Fn exception_cleanup;
  uint64_t private_1;
  uint64_t private_2;

#ifdef __cplusplus
  static inline constexpr uint64_t make_exception_class(uint32_t vendor,
                                                        uint32_t abi) {
    return (uint64_t(vendor) << 32) | uint64_t(abi);
  }
  static inline constexpr uint32_t exception_class_vendor(uint64_t exc_class) {
    return exc_class >> 32;
  }
  static inline constexpr uint32_t exception_class_abi(uint64_t exc_class) {
    return exc_class & 0xffffffffU;
  }

  static inline constexpr uint32_t str_to_vendorabi(char a, char b,
                                                    char c, char d) {
    return (uint32_t(a) << 24 | uint32_t(b) << 16 |
            uint32_t(c) << 8 | uint32_t(d));
  }

  static constexpr uint32_t GNU_vendor() {
    return str_to_vendorabi('G', 'N', 'U', 'C');
  }
  static constexpr uint32_t CXX_abi() {
    return str_to_vendorabi('C', '+', '+', '\0');
  }
  static constexpr uint64_t GNU_CXX() {
    return make_exception_class(GNU_vendor(), CXX_abi());
  }
#endif
};

struct _Unwind_Context;


/*
 * Returns:
 * _URC_END_OF_STACK
 * _URC_FATAL_PHASE_2_ERROR
 * _URC_FATAL_PHASE_1_ERROR
 */
_Unwind_Reason_Code _Unwind_RaiseException(struct _Unwind_Exception *exception_object);


typedef _Unwind_Reason_Code(*_Unwind_Stop_Fn)(int version,
    _Unwind_Action actions, uint64_t exceptionClass,
    Struct _Unwind_Exception *exceptionObject,
    Struct _Unwind_Context *context, void *stop_parameter);

/*
 * Returns:
 * _URC_NO_REASON
 * _URC_END_OF_STACK
 * _URC_FATAL_PHASE2_ERROR
 */
_Unwind_Reason_Code _Unwind_ForcedUnwind(
    Struct _Unwind_Exception *exception_object, _Unwind_Stop_Fn stop,
    void *stop_parameter);


/*
 * Called by landing pad (i.e. from generated code).
 */
void _Unwind_Resume(Struct _Unwind_Exception *exception_object);


/* Deletes exception_object. */
void _Unwind_DeleteException(struct _Unwind_Exception *exception_object);


/* Extract/modify register in context. */
uint64_t _Unwind_GetGR(Struct _Unwind_Context *context, int index);
void _Unwind_SetGR(Struct _Unwind_Context *context, int index, uint64_t new_value);

/* Return the Instruction Pointer of the context. */
uint64_t _Unwind_GetIP(Struct _Unwind_Context *context);

/* Returns the language specific data address from the context. */
uint64_t _Unwind_GetLanguageSpecificData(struct _Unwind_Context *context);

/* Returns the start of the procedure/code fragment described by current unwind descriptor block. */
uint64_t _Unwind_GetRegionStart(struct _Unwind_Context *context);

_Unwind_Reason_Code (*__personality_routine)(int version, _Unwind_Action actions, uint64_t exceptionClass, Struct _Unwind_Exception *exceptionObject, Struct _Unwind_Context *context);


/*
 * Layout of exception header.
 * Pointers to an exception point at the exception class immediately following
 * this struct.
 */
struct __cxa_exception {
#ifdef __cplusplus
  std::atomic<uintptr_t> refcount;  // # references to this exception.
#else
  _Atomic(uintptr_t)	refcount; // # references to this exception.
#endif

  /* ABI starts here.  Extensions go before. */
  const std::type_info*	exceptionType;
  void (*exceptionDestructor)(void *);
  unexpected_handler	unexpectedHandler;
  terminate_handler	terminateHandler;
  __cxa_exception*	nextException;

  int			handlerCount;
  int			handlerSwitchValue;
  const char*		actionRecord;
  const char*		languageSpecificData;
  void*			catchTemp;
  void*			adjustedPtr;

  _Unwind_Exception	unwindHeader;


#ifdef __cplusplus
  static inline void acquire(__cxa_exception& ex) noexcept {
    ex.refcount.fetch_add(1U, std::memory_order_acquire);
  }
  static inline bool release(__cxa_exception& ex) noexcept {
    return (ex.refcount.fetch_sub(1U, std::memory_order_release) == 1U);
  }
#endif /* __cplusplus */
};


/*
 * List of caught exceptions, number of uncaught exceptions.
 * Local to each thread.
 */
struct __cxa_eh_globals {
  __cxa_exception*	caughtExceptions;
  unsigned int		uncaughtExceptions;
};

__cxa_eh_globals* __cxa_get_globals() noexcept;
__cxa_eh_globals* __cxa_get_globals_fast() noexcept;


#ifdef _SHOW_UNWIND_INTERNAL
void* __cxa_allocate_exception(size_t) noexcept;
void __cxa_free_exception(void*) noexcept;
void __cxa_throw(void*, const std::type_info*, void (*)(void*)) noexcept
                __attribute__((__noreturn__));
#endif


#undef Struct

_cdecl_end

} /* namespace abi */

#endif /* _ABI_EH_H_ */
