_namespace_begin(std)
namespace impl {


inline long shared_ptr_ownership::get_shared_refcount() const noexcept {
  return shared_refcount_.load(memory_order_relaxed);
}

inline long shared_ptr_ownership::count_ptrs_to_me() const noexcept {
  return refcount_.load(memory_order_seq_cst);
}


inline void shared_ptr_ownership::acquire(shared_ptr_ownership* spd, size_t n) noexcept {
  auto old_refcount = spd->refcount_.fetch_add(n, memory_order_acquire);
  assert(old_refcount > 0);
}

inline void shared_ptr_ownership::release(shared_ptr_ownership* spd, size_t n) noexcept {
  if (spd->refcount_.fetch_sub(n, memory_order_release) == n)
    destroy_(spd);
}

inline void shared_ptr_ownership::shared_ptr_acquire_from_shared_ptr(size_t n)
    noexcept {
  auto old = shared_refcount_.fetch_add(n, memory_order_acquire);
  assert(old != 0);
}

inline void shared_ptr_ownership::shared_ptr_release(size_t n) noexcept {
  if (shared_refcount_.fetch_sub(n, memory_order_release) == n)
    release_pointee_();
}

template<typename T, typename U>
auto shared_ptr_ownership::weak_ptr_convert(U* ptr) noexcept ->
    enable_if_t<!is_same<T, U>::value, T*> {
  if (ptr != nullptr && shared_ptr_acquire_from_weak_ptr()) {
    T* rv = ptr;
    shared_ptr_release();
    return rv;
  }
  return nullptr;
}

template<typename T, typename U>
auto shared_ptr_ownership::weak_ptr_convert(U* ptr) noexcept ->
    enable_if_t<is_same<T, U>::value, T*> {
  return ptr;
}


template<typename T, typename A>
template<typename... Args>
placement_shared_ptr_ownership<T, A>::placement_shared_ptr_ownership(
    A alloc, offset_type off, Args&&... args)
: data_(nullptr, move_if_noexcept(alloc), off)
{
  void* placement_addr = &_namespace(std)::get<0>(data_).value_;
  new (placement_addr) T(forward<Args>(args)...);
}

template<typename T, typename A>
void* placement_shared_ptr_ownership<T, A>::get_deleter(const type_info&)
    noexcept {
  return nullptr;
}

template<typename T, typename A>
void placement_shared_ptr_ownership<T, A>::release_pointee() noexcept {
  _namespace(std)::get<0>(data_).value.~T();
}

template<typename T, typename A>
void placement_shared_ptr_ownership<T, A>::destroy_me() noexcept {
  /* Rescue the allocator, so it won't be destroyed before use. */
  A alloc = move_if_noexcept(_namespace(std)::get<1>(data_));

  /* Figure out the address and number of allocated allocator elements. */
  const offset_type off = _namespace(std)::get<2>(data_);
  typename allocator_traits<A>::value_type* addr =
      reinterpret_cast<typename allocator_traits<A>::value_type*>(this);
  addr -= off;
  const size_t n = n_elems();

  /* Destroy ourselves.  Access to this is no longer allowed from now on. */
  this->~placement_shared_ptr_ownership();
  /* Release memory, using the saved allocator. */
  alloc.deallocate(addr, n);
}

template<typename T, typename A, typename... Args>
auto create_placement_shared_ptr_ownership(A alloc_arg, Args&&... args) ->
    placement_shared_ptr_ownership<
        T, typename allocator_traits<A>::template rebind_alloc<uint64_t>> {
  using alloc_type =
      typename allocator_traits<A>::template rebind_alloc<uint64_t>;
  using impl = placement_shared_ptr_ownership<T, alloc_type>;
  using alloc_vt = typename allocator_traits<alloc_type>::value_type;

  struct temp_store_deleter {
   public:
    temp_store_deleter(alloc_type& alloc) noexcept : alloc_(alloc) {}

    void operator()(alloc_vt* p) { alloc_.deallocate(p, impl::n_items()); }

   private:
    alloc_type& alloc_;
  };

  alloc_type alloc = move_if_noexcept(alloc_arg);
  auto store = unique_ptr<alloc_vt, temp_store_deleter>(
      alloc.allocate(impl::n_items()), alloc);

  /* Figure out storage address and find a suitable address for impl. */
  uintptr_t store_addr = reinterpret_cast<uintptr_t>(store.get());
  uintptr_t impl_addr = (store_addr + alignof(impl) - 1U) &
                        ~uintptr_t(alignof(impl) - 1U);

  /* Calculate offset and assert it fits in the offset type. */
  typename impl::offset_type off = (impl_addr - store_addr) / sizeof(alloc_vt);
  assert(uintptr_t(off) * sizeof(alloc_vt) == impl_addr - store_addr);

  /* Assert object fits in allocated storage. */
  assert(impl_addr + sizeof(impl) <=
         store_addr + impl::n_items() * sizeof(alloc_vt));

  /* Run constructor and (if succesful) return impl addr. */
  new (reinterpret_cast<void*>(impl_addr)) impl(alloc, off,
                                                forward<Args>(args)...);
  store.release();
  return static_cast<impl*>(reinterpret_cast<void*>(impl_addr));
}


} /* namespace impl */
_namespace_end(std)
