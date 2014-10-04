_namespace_begin(std)
namespace impl {


inline long shared_ptr_ownership::get_shared_refcount() const noexcept {
  return shared_refcount_.load(memory_order_relaxed);
}

inline long shared_ptr_ownership::count_ptrs_to_me() const noexcept {
  return refcount_.load(memory_order_seq_cst);
}


inline void shared_ptr_ownership::acquire(shared_ptr_ownership* spd, size_t n)
    noexcept {
  auto old_refcount = spd->refcount_.fetch_add(n, memory_order_acquire);
  assert(old_refcount > 0);
}

inline void shared_ptr_ownership::release(shared_ptr_ownership* spd, size_t n)
    noexcept {
  if (size_t(spd->refcount_.fetch_sub(n, memory_order_release)) == n)
    destroy_(spd);
}

inline void shared_ptr_ownership::shared_ptr_acquire_from_shared_ptr(size_t n)
    noexcept {
  auto old = shared_refcount_.fetch_add(n, memory_order_acquire);
  assert(old != 0);
}

inline void shared_ptr_ownership::shared_ptr_release(size_t n) noexcept {
  if (size_t(shared_refcount_.fetch_sub(n, memory_order_release)) == n)
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
    A alloc, Args&&... args)
: data_({}, move_if_noexcept(alloc))
{
  void* placement_addr = &_namespace(std)::get<0>(data_);
  new (placement_addr) T(forward<Args>(args)...);
}

template<typename T, typename A>
void* placement_shared_ptr_ownership<T, A>::get_deleter(const type_info&)
    noexcept {
  return nullptr;
}

template<typename T, typename A>
auto placement_shared_ptr_ownership<T, A>::get() noexcept -> T* {
  void* placement_addr = &_namespace(std)::get<0>(data_);
  return static_cast<T*>(placement_addr);
}

template<typename T, typename A>
void placement_shared_ptr_ownership<T, A>::release_pointee() noexcept {
  void* placement_addr = &_namespace(std)::get<0>(data_);
  T* t_ptr = static_cast<T*>(placement_addr);
  t_ptr->~T();
}

template<typename T, typename A>
void placement_shared_ptr_ownership<T, A>::destroy_me() noexcept {
  using impl = placement_shared_ptr_ownership<T, A>;
  using alloc_type =
      typename allocator_traits<A>::template rebind_alloc<impl>;
  using alloc_traits = allocator_traits<alloc_type>;

  /* Rescue the allocator, so it won't be destroyed before use. */
  alloc_type alloc = move_if_noexcept(_namespace(std)::get<1>(data_));

  /* Destroy ourselves.  Access to this is no longer allowed from now on. */
  this->~placement_shared_ptr_ownership();
  /* Release memory, using the saved allocator. */
  alloc_traits::deallocate(alloc, this, 1);
}

template<typename T, typename A, typename... Args>
auto create_placement_shared_ptr_ownership(A alloc_arg, Args&&... args) ->
    placement_shared_ptr_ownership<
        T, typename allocator_traits<A>::template rebind_alloc<void>>* {
  using impl = placement_shared_ptr_ownership<
      T, typename allocator_traits<A>::template rebind_alloc<void>>;
  using alloc_type =
      typename allocator_traits<A>::template rebind_alloc<impl>;
  using alloc_traits = allocator_traits<alloc_type>;

  struct temp_store_deleter {
   public:
    temp_store_deleter(alloc_type& alloc) noexcept : alloc_(alloc) {}

    void operator()(impl* p) {
      alloc_traits::deallocate(alloc_, p, 1);
    }

   private:
    alloc_type& alloc_;
  };

  using uptr_type = unique_ptr<impl, temp_store_deleter>;


  /* Allocate storage. */
  alloc_type alloc = alloc_arg;
  auto p = uptr_type(alloc_traits::allocate(alloc, 1),
                     temp_store_deleter(alloc));

  /* Run constructor and (if succesful) return impl addr. */
  void* void_p = reinterpret_cast<void*>(p.get());
  impl* rv = new (void_p) impl(alloc, forward<Args>(args)...);
  p.release();
  return rv;
}


} /* namespace impl */
_namespace_end(std)
