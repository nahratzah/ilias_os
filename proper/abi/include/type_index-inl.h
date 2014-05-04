#ifndef _TYPE_INDEX_INL_H_
#define _TYPE_INDEX_INL_H_

#include <type_index>
#include <cdecl.h>
#include <cassert>
#include <cstddef>
#include <typeinfo>
#include <hash-fwd.h>

_namespace_begin(std)


inline type_index::type_index(const type_info& t) noexcept
: target_(&t)
{}

inline bool type_index::operator==(const type_index& o) const noexcept {
  assert(target_ != nullptr && o.target_ != nullptr);
  return *target_ == *o.target_;
}

inline bool type_index::operator!=(const type_index& o) const noexcept {
  assert(target_ != nullptr && o.target_ != nullptr);
  return *target_ != *o.target_;
}

inline bool type_index::operator<(const type_index& o) const noexcept {
  assert(target_ != nullptr && o.target_ != nullptr);
  return target_->before(*o.target_);
}

inline bool type_index::operator<=(const type_index& o) const noexcept {
  assert(target_ != nullptr && o.target_ != nullptr);
  return !o.target_->before(*target_);
}

inline bool type_index::operator>(const type_index& o) const noexcept {
  assert(target_ != nullptr && o.target_ != nullptr);
  return o.target_->before(*target_);
}

inline bool type_index::operator>=(const type_index& o) const noexcept {
  assert(target_ != nullptr && o.target_ != nullptr);
  return !target_->before(*o.target_);
}

inline size_t type_index::hash_code() const noexcept {
  assert(target_ != nullptr);
  return target_->hash_code();
}

inline const char* type_index::name() const noexcept {
  assert(target_ != nullptr);
  return target_->name();
}


inline size_t hash<type_index>::operator()(const type_index& ti) const
    noexcept {
  return ti.hash_code();
}


_namespace_end(std)

#endif /* _TYPE_INDEX_INL_H_ */
