#ifndef _CSTDIO_FILE_INL_H_
#define _CSTDIO_FILE_INL_H_

#include "cstdio_file.h"
#include <cassert>

_namespace_begin(std)


inline file_lock::file_lock(FILE* f)
: file_lock(f, defer_lock)
{
  if (file_) {
    file_->lock();
    locked_ = true;
  }
}

inline file_lock::file_lock(FILE* f, defer_lock_t) noexcept
: file_(f)
{}

inline file_lock::file_lock(FILE* f, try_to_lock_t)
: file_(f),
  locked_(file_ && file_->try_lock())
{}

inline file_lock::file_lock(FILE* f, adopt_lock_t) noexcept
: file_(f),
  locked_(true)
{
  assert(f != nullptr);
}

inline file_lock::~file_lock() noexcept {
  assert(file_ != nullptr || !locked_);
  if (locked_) file_->unlock();
}

inline file_lock::file_lock(file_lock&& o) noexcept
: file_(exchange(o.file_, nullptr)),
  locked_(exchange(o.locked_, false))
{}

inline auto file_lock::operator=(file_lock&& o) noexcept -> file_lock& {
  file_lock(move(o)).swap(*this);
  return *this;
}

inline auto file_lock::lock() -> void {
  if (_predict_false(!file_))
    throw system_error(make_error_code(errc::operation_not_permitted));
  if (_predict_false(locked_))
    throw system_error(make_error_code(errc::resource_deadlock_would_occur));
  file_->lock();
  locked_ = true;
}

inline auto file_lock::try_lock() -> bool {
  if (_predict_false(!file_))
    throw system_error(make_error_code(errc::operation_not_permitted));
  if (_predict_false(locked_))
    throw system_error(make_error_code(errc::resource_deadlock_would_occur));
  return locked_ = file_->try_lock();
}

inline auto file_lock::unlock() -> void {
  if (_predict_false(!locked_))
    throw system_error(make_error_code(errc::operation_not_permitted));
  file_->unlock();
  locked_ = false;
}

inline auto file_lock::swap(file_lock& o) noexcept -> void {
  using _namespace(std)::swap;

  swap(file_, o.file_);
  swap(locked_, o.locked_);
}

inline auto file_lock::release() noexcept -> FILE* {
  locked_ = false;
  return exchange(file_, nullptr);
}

inline auto swap(file_lock& x, file_lock& y) noexcept -> void {
  x.swap(y);
}


_namespace_end(std)

#endif /* _CSTDIO_FILE_INL_H_ */
