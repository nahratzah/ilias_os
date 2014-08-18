#include <abi/ref_cstr.h>
#include <abi/memory.h>
#include <new>

namespace __cxxabiv1 {


heap ref_cstr_heap{ "ref_cstr" };

ref_cstr::ref_cstr(const char* s) noexcept
: ref_cstr()
{
  if (s == nullptr) return;

  /* Determine strlen(s). */
  size_t len = 0;
  for (const char* i = s; *i; ++i, ++len);
  /* Allocate space for reference counter + str. */
  head_ = static_cast<atom_t*>(ref_cstr_heap.malloc(sizeof(*head_) +
                                                    len + 1U));
  if (!head_) _namespace(std)::__throw_bad_alloc();

  /* Initialize reference counter. */
  atomic_init(head_, 1U);
  /* Copy string. */
  char* hs = reinterpret_cast<char*>(head_ + 1);
  while ((*hs++ = *s++));
}

void ref_cstr::reset() noexcept {
  if (head_ == nullptr) return;
  if (head_->fetch_sub(1U, _namespace(std)::memory_order_release) == 1U)
    ref_cstr_heap.free(head_);
  head_ = nullptr;
}


} /* namespace __cxxabiv1 */
