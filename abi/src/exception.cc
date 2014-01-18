#include <exception>
#include <c_util/refcount_ptr.h>
#include <utility>

namespace std {

using exc_ptr_t = util::refcount_ptr<void>;
/* Validate assumption for std::exception_ptr. */
static_assert(sizeof(void*) == sizeof(exc_ptr_t),
              "Exception pointer implementation bug: size mismatch");
static_assert(alignof(void*) == alignof(exc_ptr_t),
              "Exception pointer implementation bug: alignment mismatch");

namespace {

inline exc_ptr_t& exc_ptr(void*& p) noexcept {
  return reinterpret_cast<refcount_ptr<exception>&>(p);
}

inline const exc_ptr_t& exc_ptr(void*const& p) noexcept {
  return reinterpret_cast<refcount_ptr<exception>&>(p);
}

} /* namespace std::<unnamed> */


exception_ptr::exception_ptr() noexcept {
  new (exc_ptr(ptr_)) exc_ptr_t();
}

exception_ptr::exception_ptr(const exception_ptr& other) noexcept
: exception_ptr() {
  exc_ptr(ptr_) = exc_ptr(other.ptr_);
}

exception_ptr::exception_ptr(exception_ptr&& other) noexcept
: exception_ptr() {
  exc_ptr(ptr_) = std::move(exc_ptr(other.ptr_));
}

exception_ptr& exception_ptr::operator=(const exception_ptr& other) noexcept {
  exc_ptr(ptr_) = exc_ptr(other.ptr_);
  return *this;
}

exception_ptr& exception_ptr::operator=(exception_ptr&& other) noexcept {
  exc_ptr(ptr_) = std::move(exc_ptr(other.ptr_));
  return *this;
}

exception_ptr::~exception_ptr() noexcept {
  exc_ptr(ptr_).~exc_ptr_t();
}

void rethrow_exception(exception_ptr p) {
  /* XXX implement this */
  exc_ptr(p.ptr_)
}


} /* namespace std */
