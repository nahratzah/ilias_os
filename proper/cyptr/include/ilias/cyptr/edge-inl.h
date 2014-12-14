#ifndef _ILIAS_CYPTR_EDGE_INL_H_
#define _ILIAS_CYPTR_EDGE_INL_H_

#include <ilias/cyptr/edge.h>

namespace ilias {
namespace cyptr {


template<typename T>
edge<T>::edge(const node& n) noexcept
: edge_(n.obj_)
{}

template<typename T>
template<typename U, typename>
edge<T>::edge(const node& n, const cyptr<U>& ptr) noexcept
: edge_(n.obj_)
{
  edge_.reset(ptr.objptr_.get());
  ptr_ = ptr.get();
}

template<typename T>
auto edge<T>::operator=(const edge& ptr) noexcept -> edge& {
  edge_.reset(ptr.edge_.get());
  ptr_ = ptr.get();
  return *this;
}

template<typename T>
template<typename U, typename>
auto edge<T>::operator=(const edge<U>& ptr) noexcept -> edge& {
  edge_.reset(ptr.edge_.get());
  ptr_ = ptr.get();
  return *this;
}

template<typename T>
template<typename U, typename>
auto edge<T>::operator=(const cyptr<U>& ptr) noexcept -> edge& {
  edge_.reset(ptr.objptr_.get());
  ptr_ = ptr.get();
  return *this;
}

template<typename T>
edge<T>::~edge() noexcept {
  reset();
}

template<typename T>
edge<T>::operator bool() const noexcept {
  return ptr_ != nullptr && edge_.get() != nullptr;
}

template<typename T>
auto edge<T>::get() const noexcept -> pointer {
  return (*this ? ptr_ : nullptr);
}

template<typename T>
auto edge<T>::operator->() const noexcept -> pointer {
  return get();
}

template<typename T>
auto edge<T>::operator*() const noexcept ->
    typename std::conditional_t<is_void<T>::value,
                                std::identity<T>,
                                std::add_lvalue_reference<T>>::type {
  assert(*this);
  return *get();
}

template<typename T>
auto edge<T>::reset() noexcept -> void {
  edge_.reset();
  ptr_ = nullptr;
}

template<typename T>
auto edge<T>::reset(std::nullptr_t) noexcept -> void {
  edge_.reset(nullptr);
  ptr_ = nullptr;
}

template<typename T>
auto edge<T>::reset(const edge& ptr) noexcept -> void {
  *this = ptr;
}

template<typename T>
auto edge<T>::reset(const cyptr<T>& ptr) noexcept -> void {
  *this = ptr;
}


template<typename T>
cyptr<T>::cyptr() noexcept {}

template<typename T>
cyptr<T>::cyptr(const cyptr& ptr) noexcept
: ptr_(ptr.ptr_),
  objptr_(ptr.objptr_)
{}

template<typename T>
cyptr<T>::cyptr(cyptr&& ptr) noexcept
: ptr_(std::exchange(ptr.ptr_, nullptr)),
  objptr_(std::move(ptr.objptr_))
{}

template<typename T>
template<typename U, typename D, typename, typename>
cyptr<T>::cyptr(U* ptr, std::function<void (void*)> destructor,
                void* destructor_arg) noexcept
: ptr_(ptr),
  objptr_(impl::refcnt_initialize(const_cast<remove_cv_t<U>(ptr),
                                  std::move(destructor),
                                  std::move(destructor_arg)),
          false)
{}

template<typename T>
template<typename U, typename>
cyptr<T>::cyptr(const cyptr<U>& ptr) noexcept
: ptr_(ptr.ptr_),
  objptr_(ptr.objptr_)
{}

template<typename T>
template<typename U, typename>
cyptr<T>::cyptr(cyptr<U>&& ptr) noexcept
: ptr_(std::exchange(ptr.ptr_, nullptr)),
  objptr_(std::move(ptr.objptr_))
{}

template<typename T>
template<typename U, typename>
cyptr<T>::cyptr(const edge<U>& ptr) noexcept
: ptr_(ptr.get()),
  objptr_(ptr.edge_.get())
{}

template<typename T>
template<typename U>
cyptr<T>::cyptr(cyptr<U> ownership, pointer ptr) noexcept
: ptr_(ptr),
  objptr_(std::move(ownership.objptr_))
{}

template<typename T>
auto cyptr<T>::operator=(const cyptr& ptr) noexcept -> cyptr& {
  ptr_ = ptr.ptr_;
  objptr_ = ptr.objptr_;
  return *this;
}

template<typename T>
auto cyptr<T>::operator=(cyptr&& ptr) noexcept -> cyptr& {
  ptr_ = std::exchange(ptr.ptr_, nullptr);
  objptr_ = std::move(ptr.objptr_);
  return *this;
}

template<typename T>
template<typename U, typename>
auto cyptr<T>::operator=(const cyptr<U>& ptr) noexcept -> cyptr& {
  ptr_ = ptr.ptr_;
  objptr_ = ptr.objptr_;
  return *this;
}

template<typename T>
template<typename U, typename>
auto cyptr<T>::operator=(cyptr<U>&& ptr) noexcept -> cyptr& {
  ptr_ = std::exchange(ptr.ptr_, nullptr);
  objptr_ = std::move(ptr.objptr_);
  return *this;
}

template<typename T>
template<typename U, typename>
auto cyptr<T>::operator=(const edge<U>& ptr) noexcept -> cyptr& {
  ptr_ = ptr.get();
  objptr_.reset(ptr.edge_.get());
  return *this;
}

template<typename T>
cyptr<T>::operator bool() const noexcept {
  assert((ptr_ == nullptr) == (objptr_ == nullptr));
  return ptr_ != nullptr;
}

template<typename T>
auto cyptr<T>::get() const noexcept -> pointer {
  assert((ptr_ == nullptr) == (objptr_ == nullptr));
  return ptr_;
}

template<typename T>
auto cyptr<T>::operator->() const noexcept -> pointer {
  return get();
}

template<typename T>
auto cyptr<T>::operator*() const noexcept ->
    typename std::conditional_t<is_void<T>::value,
                                std::identity<T>,
                                std::add_lvalue_reference<T>>::type {
  return *get();
}

template<typename T>
auto cyptr<T>::reset() noexcept -> void {
  ptr_ = nullptr;
  objptr_.reset();
}

template<typename T>
auto cyptr<T>::reset(std::nullptr_t) noexcept -> void {
  reset();
}

template<typename T>
auto cyptr<T>::reset(const cyptr& ptr) noexcept -> void {
  *this = ptr;
}

template<typename T>
auto cyptr<T>::reset(cyptr&& ptr) noexcept -> void {
  *this = std::move(ptr);
}

template<typename T>
template<typename U, typename>
auto cyptr<T>::reset(const cyptr<U>& ptr) noexcept -> void {
  *this = ptr;
}

template<typename T>
template<typename U, typename>
auto cyptr<T>::reset(cyptr<U>&& ptr) noexcept -> void {
  *this = std::move(ptr);
}

template<typename T>
template<typename U, typename>
auto cyptr<T>::reset(const edge<U>& ptr) noexcept -> void {
  *this = ptr;
}


template<typename T, typename... Args>
auto make_cyptr(Args&&... args) -> cyptr<T> {
  std::function<void (void*)> fn = [](void* p) { delete static_cast<T*>(p); };
  T* ptr = new T(std::forward<Args>(args)...);
  return cyptr<T>(ptr, std::move(fn), ptr);
}


template<typename T>
void swap(edge<T>& x, edge<T>& y) noexcept {
  std::tie(x, y) = std::make_tuple(cyptr<T>(y), cyptr<T>(x));
}

template<typename T>
void swap(edge<T>& x, cyptr<T>& y) noexcept {
  std::tie(x, y) = std::make_tuple(std::move(y), cyptr<T>(x));
}

template<typename T>
void swap(cyptr<T>& x, edge<T>& y) noexcept {
  std::tie(x, y) = std::make_tuple(cyptr<T>(y), std::move(x));
}

template<typename T>
void swap(cyptr<T>& x, cyptr<T>& y) noexcept {
  std::tie(x, y) = std::make_tuple(std::move(y), std::move(x));
}


}} /* namespace ilias::cyptr */

#endif /* _ILIAS_CYPTR_EDGE_INL_H_ */
