_namespace_begin(std)


template<typename T> T* addressof(T& r) noexcept {
  return __builtin_addressof(r);
}


template<typename T>
template<typename U>
allocator<T>::allocator(const allocator<U>& other) noexcept
: allocator()
{}

template<typename U>
allocator<void>::allocator(const allocator<U>& other) noexcept
: allocator()
{}

template<typename T>
auto allocator<T>::address(reference x) const noexcept -> pointer {
  return addressof(x);
}

template<typename T>
auto allocator<T>::address(const_reference x) const noexcept -> const_pointer {
  return addressof(x);
}

template<typename T>
auto allocator<T>::allocate(size_type n,
                            allocator<void>::const_pointer hint) -> pointer {
  if (n >= max_size()) throw std::bad_alloc();
  return ::operator new(n * sizeof(value_type));
}

template<typename T>
auto allocator<T>::deallocate(pointer p, size_type n) -> void {
  ::operator delete(p, n * sizeof(value_type));
}

template<typename T>
auto allocator<T>::max_size() const noexcept -> size_type {
  return std::numeric_limits<size_t>::max() / sizeof(value_type);
}

template<typename T>
template<typename U, typename... Args>
auto allocator<T>::construct(U* p, Args&&... args)
    noexcept(noexcept(::new (static_cast<void*>(p))
                            U(std::forward<Args>(args)...))) ->
    void {
  ::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
}

template<typename T>
template<typename U>
auto allocator<T>::destroy(U* p)
    noexcept(noexcept(p->~U())) ->
    void {
  p->~U();
}


template<typename Alloc>
auto allocator_traits<Alloc>::allocate(Alloc& alloc,
                                       size_type n, const_void_pointer hint) ->
    pointer {
  return impl::alloc_traits<Alloc>::allocate(alloc, n, hint);
}

template<typename Alloc>
auto allocator_traits<Alloc>::deallocate(Alloc& alloc, pointer p, size_type n)
    noexcept(noexcept(alloc.deallocate(p, n))) -> void {
  alloc.deallocate(p, n);
}

template<typename Alloc>
template<typename T, typename... Args>
void allocator_traits<Alloc>::construct(
    Alloc& alloc, T* p, Args&&... args)
    noexcept(noexcept(impl::alloc_traits<Alloc>::construct(
        alloc, p, std::forward<Args>(args)...))) {
  impl::alloc_traits<Alloc>::construct(alloc, p, std::forward<Args>(args)...);
}

template<typename Alloc>
template<typename T>
void allocator_traits<Alloc>::destroy(Alloc& alloc, T* p)
    noexcept(noexcept(impl::alloc_traits<Alloc>::destroy(alloc, p))) {
  impl::alloc_traits<Alloc>::destroy(alloc, p);
}


_namespace_end(std)
