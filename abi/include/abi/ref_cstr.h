#ifndef _ABI_REF_CSTR_H_
#define _ABI_REF_CSTR_H_

#include <atomic>
#include <utility>

namespace __cxxabiv1 {


class ref_cstr {
 public:
  ref_cstr() = default;
  inline ref_cstr(const ref_cstr&) noexcept;
  inline ref_cstr(ref_cstr&&) noexcept;
  inline ~ref_cstr() noexcept;
  ref_cstr(const char*) noexcept;
  ref_cstr& operator=(ref_cstr) noexcept;

  inline operator const char*() const noexcept;
  inline void swap(ref_cstr&) noexcept;
  explicit inline operator bool() const noexcept;

  void reset() noexcept;

 private:
  using atom_t = std::atomic<uint32_t>;

  atom_t* head_ = nullptr;
};

inline void swap(ref_cstr&, ref_cstr&) noexcept;


} /* namespace __cxxabiv1 */

#include <abi/ref_cstr-inl.h>

#endif /* _ABI_REF_CSTR_H_ */
