_namespace_begin(std)
namespace impl {


constexpr allocator_cloneable::size_align::size_align(size_t size,
                                                      size_t align) noexcept
: size(size),
  align(align)
{}

inline allocator_cloneable::allocator_cloneable(size_align constraints)
    noexcept
: constraints_(constraints)
{}

template<typename T, typename Alloc>
auto allocator_cloneable::clone(const T& v, Alloc&& a) ->
    pointer_type<T> {
  using copy_into_result = decltype(declval<T>().copy_into(declval<void*>()));
  static_assert(is_base_of<T, remove_pointer_t<copy_into_result>>::value,
                "The copy_into method must provide a return type of T.");

  return clone_impl(v, forward<Alloc>(a), &T::copy_into);
}

template<typename T, typename Alloc>
auto allocator_cloneable::clone(T&& v, Alloc&& a) ->
    pointer_type<T> {
  using move_into_result = decltype(declval<T>().move_into(declval<void*>()));
  static_assert(is_base_of<T, remove_pointer_t<move_into_result>>::value,
                "The copy_into method must provide a return type of T.");

  return clone_impl(forward<T>(v), forward<Alloc>(a), &T::move_into);
}

template<typename T, typename Alloc, typename CloneInto>
auto allocator_cloneable::clone_impl(T&& cloneable_t, Alloc&& alloc_arg,
                                     CloneInto clone_into) ->
    pointer_type<T> {
  /* Declare a tiny functor that will destroy clone in case of exceptions. */
  struct destroy_clone {
    void operator()(allocator_cloneable* p) noexcept {
      p->~allocator_cloneable();
    }
  };


  /*
   * Declare types:
   * - alloc_type    allocator type used internally
   * - alloc_traits  allocator_traits for alloc_type
   * - pointer       pointer for alloc_type
   * - value_type    opaque type used to control the allocator
   *                 (i.e. allocator_cloneable::alloc_type_).
   */
  using alloc_arg_traits =
      allocator_traits<remove_const_t<Alloc>>;
  using value_type = alloc_type_;
  using alloc_type =
      typename alloc_arg_traits::template rebind_alloc<value_type>;
  using alloc_traits = allocator_traits<alloc_type>;
  using pointer = typename alloc_traits::pointer;

  /*
   * Declare variables:
   * - cloneable    allocator_cloneable reference of cloneable_t.
   * - alloc        instance of alloc_type, used to control memory.
   * - store_sz     bytes required to hold allocator and cloned value
   *                (store_sz will be of sufficient size to accomodate
   *                 alignment requirements and rounded up to the size
   *                 of value_type).
   *                Note that store_sz will be changed during
   *                pointer assignment.
   * - store_items  number of items requested from allocator
   *                (since allocators don't think in bytes).
   */
  const allocator_cloneable& cloneable = cloneable_t;
  alloc_type alloc = alloc_type(forward<Alloc>(alloc_arg));
  size_t store_sz = cloneable.alloc_size_(sizeof(alloc_type),
                                          alignof(alloc_type));
  const size_t store_items = store_sz / sizeof(value_type);

  /*
   * Create storage_ptr, unique_ptr that takes care of releasing
   * storage if an exception occurs.
   */
  auto storage_deleter =
      [&alloc, store_items](pointer p) {
        alloc_traits::deallocate(alloc, p, store_items);
      };
  using storage_ptr = unique_ptr<value_type, decltype(storage_deleter)&>;

  /* Allocate memory. */
  storage_ptr store_ptr{ alloc_traits::allocate(alloc, store_items),
                         storage_deleter };
  void* store = addressof(*store_ptr);

  /* Decide where exactly to store everything. */
  void*const alloc_ptr = align(alignof(alloc_type), sizeof(alloc_type),
                               store, store_sz);
  void*const clone_ptr = align(cloneable.constraints_.align,
                               cloneable.constraints_.size,
                               store, store_sz);
  assert_msg(alloc_ptr != nullptr && clone_ptr != nullptr,
             "Programmer error: "
             "storage is incorrect or std::align is broken.");

  /*
   * Construct the cloneable.
   * Note that we use a unique_ptr to automatically destroy it,
   * if an exception occurs later on.
   */
  auto clone_uptr = unique_ptr<T, destroy_clone>(invoke(clone_into,
                                                        cloneable_t,
                                                        clone_ptr));

  /*
   * Move allocator into position.
   * This must be the last step, since it breaks the store_ptr above
   * (by invalidating the alloc variable).
   *
   * Note that this is the last step that can fail.
   */
  alloc_type* rv_alloc = new (alloc_ptr) alloc_type(move_if_noexcept(alloc));

  /*
   * Release temporary pointers into the allocator_cloneable deleter based
   * unique_ptr.
   */
  T* rv_clone = clone_uptr.release();
  return pointer_type<T>(rv_clone,
                         deleter(addressof(*store_ptr.release()), rv_alloc,
                                 rv_clone, store_items));
}


template<typename Alloc>
allocator_cloneable::deleter::deleter(const void* base, Alloc* alloc,
                                      const allocator_cloneable* ptr_arg,
                                      size_t store_items) noexcept
: cleanup_fn_(&cleanup_alloc_<remove_const_t<remove_reference_t<Alloc>>>),
  store_items_(store_items),
  base_off_(calc_off(base, ptr_arg)),
  alloc_off_(calc_off(alloc, ptr_arg))
{}

template<typename Alloc>
auto allocator_cloneable::deleter::cleanup_alloc_(void* base, void* void_alloc,
                                                  size_t store_items)
    noexcept -> void {
  using pointer = typename allocator_traits<Alloc>::pointer;

  /* Cast back to internal types. */
  Alloc* alloc_ptr = static_cast<Alloc*>(void_alloc);
  pointer base_ptr = pointer(base);

  /* Move alloc into stack storage, so it won't try to deallocate itself. */
  Alloc alloc = move_if_noexcept(*alloc_ptr);
  alloc_ptr->~Alloc();

  /* Deallocate memory. */
  allocator_traits<Alloc>::deallocate(alloc, move(base_ptr), store_items);
}


} /* namespace std::impl */
_namespace_end(std)
