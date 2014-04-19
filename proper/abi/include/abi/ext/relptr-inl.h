namespace __cxxabiv1 {
namespace ext {


static_assert(sizeof(relptr<void>) == 4, "Relative pointer must be 4 bytes.");
static_assert(sizeof(relptr<const void>) == 4,
              "Relative pointer must be 4 bytes.");
static_assert(sizeof(relptr<volatile void>) == 4,
              "Relative pointer must be 4 bytes.");
static_assert(sizeof(relptr<int>) == 4, "Relative pointer must be 4 bytes.");


template<typename T>
relptr<T>::relptr(const relptr& o) {
  assign_(o.get());
}

template<typename T>
template<typename U>
relptr<T>::relptr(const relptr<U>& o) {
  assign_(o.get());
}

template<typename T>
relptr<T>::relptr(pointer p) {
  assign_(p);
}

template<typename T>
auto relptr<T>::operator=(decltype(nullptr)) noexcept -> relptr& {
  rel_ = invalid;
  return *this;
}

template<typename T>
auto relptr<T>::operator=(const relptr& o) -> relptr& {
  assign_(o.get());
  return *this;
}

template<typename T>
template<typename U>
auto relptr<T>::operator=(const relptr<U>& o) -> relptr& {
  assign_(o.get());
  return *this;
}

template<typename T>
auto relptr<T>::operator=(pointer p) -> relptr& {
  assign_(p);
  return *this;
}

template<typename T>
relptr<T>::operator bool() const noexcept {
  return rel_ != invalid;
}

template<typename T>
auto relptr<T>::operator->() const noexcept -> pointer {
  return get();
}

template<typename T>
auto relptr<T>::operator*() const noexcept -> reference {
  return *get();
}

template<typename T>
auto relptr<T>::operator+=(ptrdiff_t delta) -> relptr& {
  return *this = get() + delta;
}

template<typename T>
auto relptr<T>::operator-=(ptrdiff_t delta) -> relptr& {
  return *this = get() - delta;
}

template<typename T>
auto relptr<T>::operator+(ptrdiff_t delta) const -> pointer {
  return get() + delta;
}

template<typename T>
auto relptr<T>::operator-(ptrdiff_t delta) const -> pointer {
  return get() - delta;
}

template<typename T>
auto relptr<T>::assign_(pointer p) -> void {
  if (p == nullptr) {
    rel_ = invalid;
    return;
  }

  const ptrdiff_t diff = reinterpret_cast<const uint8_t*>(p) -
                         reinterpret_cast<uint8_t*>(this);
  uint32_t rel = diff;  // May lose high bits...
  if (diff < 0 || rel != size_t(diff))
    throw std::range_error("relptr: delta too large");
  rel_ = rel;
  return;
}

template<typename T>
auto relptr<T>::get() const noexcept -> pointer {
  if (rel_ == invalid) return nullptr;
  const uint8_t*const addr = reinterpret_cast<const uint8_t*>(this);
  return reinterpret_cast<T*>(const_cast<uint8_t*>(addr + rel_));
}


template<typename T, typename U>
bool operator==(const relptr<T>& a, const relptr<U>& b)
    noexcept(noexcept(a.get() == b.get())) {
  return a.get() == b.get();
}

template<typename T, typename U>
bool operator!=(const relptr<T>& a, const relptr<U>& b)
    noexcept(noexcept(!(a == b))) {
  return !(a == b);
}

template<typename T, typename U>
bool operator<(const relptr<T>& a, const relptr<U>& b)
    noexcept(noexcept(a.get() < b.get())) {
  return a.get() < b.get();
}

template<typename T, typename U>
bool operator>(const relptr<T>& a, const relptr<U>& b)
    noexcept(noexcept(b < a)) {
  return b < a;
}

template<typename T, typename U>
bool operator<=(const relptr<T>& a, const relptr<U>& b)
    noexcept(noexcept(!(b < a))) {
  return !(b < a);
}

template<typename T, typename U>
bool operator>=(const relptr<T>& a, const relptr<U>& b)
    noexcept(noexcept(!(a < b))) {
  return !(a < b);
}


template<typename T>
auto relptr_allocator<T>::allocate(size_type n) -> pointer {
  if (n > max_size()) throw std::bad_alloc();
  const size_type bytes = n * sizeof(value_type);
  return pointer(static_cast<value_type*>(::operator new(bytes)));
}

template<typename T>
auto relptr_allocator<T>::deallocate(pointer p, size_type) -> void {
  ::operator delete(p.get());
}

template<typename T>
constexpr auto relptr_allocator<T>::max_size() const noexcept -> size_type {
  return INT_MAX / sizeof(value_type);
}

template<typename T, typename U>
bool operator==(const relptr_allocator<T>&, const relptr_allocator<U>&)
    noexcept {
  return true;
}

template<typename T, typename U>
bool operator!=(const relptr_allocator<T>&, const relptr_allocator<U>&)
    noexcept {
  return false;
}


}} /* namespace __cxxabiv1::ext */
