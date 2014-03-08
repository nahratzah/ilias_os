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
  if (n >= max_size()) throw bad_alloc();
  return static_cast<pointer>(::operator new(n * sizeof(value_type)));
}

template<typename T>
auto allocator<T>::deallocate(pointer p, size_type n) -> void {
  ::operator delete(p);
}

template<typename T>
auto allocator<T>::max_size() const noexcept -> size_type {
  return numeric_limits<size_t>::max() / sizeof(value_type);
}

template<typename T>
template<typename U, typename... Args>
auto allocator<T>::construct(U* p, Args&&... args)
    noexcept(noexcept(::new (static_cast<void*>(p))
                            U(forward<Args>(args)...))) ->
    void {
  ::new (static_cast<void*>(p)) U(forward<Args>(args)...);
}

template<typename T>
template<typename U>
auto allocator<T>::destroy(U* p)
    noexcept(noexcept(p->~U())) ->
    void {
  p->~U();
}


template<typename T, typename U>
bool operator==(const allocator<T>&, const allocator<U>&) noexcept {
  return true;
}

template<typename T, typename U>
bool operator!=(const allocator<T>&, const allocator<U>&) noexcept {
  return false;
}


template<typename Alloc>
auto allocator_traits<Alloc>::allocate(Alloc& alloc,
                                       size_type n, const_void_pointer hint) ->
    pointer {
  return impl::alloc_traits<Alloc>::allocate(alloc, n, hint);
}

template<typename Alloc>
auto allocator_traits<Alloc>::deallocate(Alloc& alloc, pointer p, size_type n)
    noexcept(noexcept(impl::alloc_traits<Alloc>::deallocate(alloc, p, n))) ->
    void {
  impl::alloc_traits<Alloc>::deallocate(alloc, p, n);
}

template<typename Alloc>
template<typename T, typename... Args>
void allocator_traits<Alloc>::construct(
    Alloc& alloc, T* p, Args&&... args)
    noexcept(noexcept(impl::alloc_traits<Alloc>::construct(
        alloc, p, forward<Args>(args)...))) {
  impl::alloc_traits<Alloc>::construct(alloc, p, forward<Args>(args)...);
}

template<typename Alloc>
template<typename T>
void allocator_traits<Alloc>::destroy(Alloc& alloc, T* p)
    noexcept(noexcept(impl::alloc_traits<Alloc>::destroy(alloc, p))) {
  impl::alloc_traits<Alloc>::destroy(alloc, p);
}

template<typename Alloc>
auto allocator_traits<Alloc>::max_size(const Alloc& a)
    noexcept(noexcept(impl::alloc_traits<Alloc>::max_size(a))) ->
    size_type
{
  return impl::alloc_traits<Alloc>::max_size(a);
}


_namespace_end(std)
