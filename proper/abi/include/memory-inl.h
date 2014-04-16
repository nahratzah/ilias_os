#include <stdimpl/shared_ptr_ownership.h>

_namespace_begin(std)
namespace impl {

_MEMBER_TYPE_CHECK(allocator_type);

} /* namespace std::impl */


template<typename T, typename Alloc>
struct uses_allocator
: conditional_t<impl::member_type_check_allocator_type<T>::value,
                is_same<typename T::allocator_type, Alloc>,
                false_type>
{};


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
  if (n >= max_size()) __throw_bad_alloc();
  return static_cast<pointer>(::operator new(n * sizeof(value_type)));
}

template<typename T>
auto allocator<T>::deallocate(pointer p, size_type n) -> void {
  ::operator delete(p, n * sizeof(value_type));
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


inline void declare_reachable(void*) { return; }
template<typename T> T* undeclare_reachable(T* t) { return t; }
inline void declare_no_pointers(char*, size_t) { return; }
inline void undeclare_no_pointers(char*, size_t) { return; }

inline pointer_safety get_pointer_safety() noexcept {
  return pointer_safety::relaxed;
}


template<typename OutputIterator, typename T>
raw_storage_iterator<OutputIterator, T>::raw_storage_iterator(OutputIterator x)
: iter_(x)
{}

template<typename OutputIterator, typename T>
auto raw_storage_iterator<OutputIterator, T>::operator*() ->
    raw_storage_iterator& {
  return *this;
}

template<typename OutputIterator, typename T>
auto raw_storage_iterator<OutputIterator, T>::operator=(const T& v) ->
    raw_storage_iterator& {
  new (addressof(*iter_)) T(v);
  return *this;
}

template<typename OutputIterator, typename T>
auto raw_storage_iterator<OutputIterator, T>::operator++() ->
    raw_storage_iterator& {
  ++iter_;
  return *this;
}

template<typename OutputIterator, typename T>
auto raw_storage_iterator<OutputIterator, T>::operator++(int) ->
    raw_storage_iterator {
  return raw_storage_iterator(iter_++);
}


namespace impl {

pair<void*, size_t> temporary_buffer_allocate(size_t, size_t);
void temporary_buffer_deallocate(const void*);

} /* namespace std::impl */

template<typename T>
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t n) noexcept {
  if (_predict_false(n <= 0)) n = 1;
  if (_predict_false(SIZE_MAX / sizeof(T) < n)) return make_pair(nullptr, 0);

  void* addr;
  size_t count;
  tie(addr, count) =
      impl::temporary_buffer_allocate(sizeof(T) * n, alignof(T));
  return make_pair(static_cast<T*>(addr), count);
}

template<typename T>
void return_temporary_buffer(T* p) {
  impl::temporary_buffer_deallocate(p);
}


namespace impl {

template<typename T>
auto _uninitialized_copy(const T* b, const T* e, T* out) noexcept ->
    enable_if_t<is_trivially_copy_constructible<T>::value, T*> {
  return _uninitialized_copy_n(b, e - b, out);
}

template<typename T, typename Size>
auto _uninitialized_copy_n(const T* b, Size n, T* out) noexcept ->
    enable_if_t<is_trivially_copy_constructible<T>::value, T*> {
  if (n <= 0) return out;
  memcpy(out, b, n * sizeof(T));
  return out + n;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator _uninitialized_copy(InputIterator b, InputIterator e,
                                   OutputIterator out)
    noexcept(is_nothrow_constructible<
        typename iterator_traits<OutputIterator>::value_type,
        typename iterator_traits<InputIterator>::value_type>::value) {
  using value_type = typename iterator_traits<OutputIterator>::value_type;

  while (b != e)
    new (static_cast<void*>(addressof(*out++))) value_type(*b++);
  return out;
}

template<typename InputIterator, typename Size, typename OutputIterator>
OutputIterator _uninitialized_copy_n(InputIterator b, Size n,
                                     OutputIterator out)
    noexcept(is_nothrow_constructible<
        typename iterator_traits<OutputIterator>::value_type,
        typename iterator_traits<InputIterator>::value_type>::value) {
  using value_type = typename iterator_traits<OutputIterator>::value_type;

  while (n > 0) {
    new (static_cast<void*>(addressof(*out++))) value_type(*b++);
    --n;
  }
  return out;
}

} /* namespace std::impl */

template<typename InputIterator, typename OutputIterator>
OutputIterator uninitialized_copy(InputIterator b, InputIterator e,
                                  OutputIterator out) {
  return impl::_uninitialized_copy(move(b), move(e), move(out));
}

template<typename InputIterator, typename Size, typename OutputIterator>
OutputIterator uninitialized_copy_n(InputIterator b, Size n,
                                    OutputIterator out) {
  return impl::_uninitialized_copy_n(move(b), move(n), move(out));
}

template<typename ForwardIterator, typename T>
void uninitialized_fill(ForwardIterator b, ForwardIterator e, const T& v) {
  using value_type = typename iterator_traits<ForwardIterator>::value_type;

  while (b != e)
    new (static_cast<void*>(addressof(*b++))) value_type(v);
}

template<typename ForwardIterator, typename Size, typename T>
void uninitialized_fill_n(ForwardIterator b, Size n, const T& v) {
  using value_type = typename iterator_traits<ForwardIterator>::value_type;

  while (n > 0) {
    new (static_cast<void*>(addressof(*b++))) value_type(v);
    --n;
  }
}


template<typename T>
void default_delete<T>::operator()(T* p) const {
  delete p;
}

template<typename T>
void default_delete<T[]>::operator()(T* p) const {
  delete[] p;
}


template<typename T, typename D>
constexpr unique_ptr<T, D>::unique_ptr() noexcept
: data_(nullptr, deleter_type())
{}

template<typename T, typename D>
unique_ptr<T, D>::unique_ptr(pointer p) noexcept
: data_(p, deleter_type())
{}

template<typename T, typename D>
unique_ptr<T, D>::unique_ptr(pointer p, deleter_copy_arg d) noexcept
: data_(p, d)
{}

template<typename T, typename D>
unique_ptr<T, D>::unique_ptr(pointer p, deleter_move_arg d) noexcept
: data_(p, move(d))
{
  static_assert(!D_is_reference,
                "May not create reference to temporary deleter.");
}

template<typename T, typename D>
unique_ptr<T, D>::unique_ptr(unique_ptr&& p) noexcept
: data_(p.release(), forward<D>(p.get_deleter()))
{}

template<typename T, typename D>
template<typename U, typename E>
unique_ptr<T, D>::unique_ptr(unique_ptr<U, E>&& p) noexcept
: data_(p.release(), forward<E>(p.get_deleter()))
{}

template<typename T, typename D>
template<typename U>
unique_ptr<T, D>::unique_ptr(auto_ptr<U>&& p) noexcept
: data_(p.release(), deleter_type())
{}

template<typename T, typename D>
unique_ptr<T, D>::~unique_ptr() noexcept {
  if (get() != nullptr)
    get_deleter()(get());
}

template<typename T, typename D>
auto unique_ptr<T, D>::operator=(unique_ptr&& p) noexcept -> unique_ptr& {
  /* Protect against self assignment. */
  if (_predict_false(p == *this)) return *this;

  /* Exchange current value for new value. */
  auto old =
      exchange(data_, data_type(p.release(), forward<D>(p.get_deleter())));
  /* Clean up old value. */
  if (_namespace(std)::get<0>(old) != nullptr)
    _namespace(std)::get<1>(old)(_namespace(std)::get<0>(old));
  return *this;
}

template<typename T, typename D>
template<typename U, typename E>
auto unique_ptr<T, D>::operator=(unique_ptr<U, E>&& p) noexcept ->
    unique_ptr& {
  /* Exchange current value for new value. */
  auto old =
      exchange(data_, data_type(p.release(), forward<E>(p.get_deleter())));
  /* Clean up old value. */
  if (_namespace(std)::get<0>(old) != nullptr)
    _namespace(std)::get<1>(old)(_namespace(std)::get<0>(old));
  return *this;
}

template<typename T, typename D>
auto unique_ptr<T, D>::operator=(nullptr_t) noexcept -> unique_ptr& {
  reset();
  return *this;
}

template<typename T, typename D>
auto unique_ptr<T, D>::operator*() const ->
    typename conditional_t<is_void<T>::value,
                           identity<T>,
                           add_lvalue_reference<T>>::type {
  assert_msg(get() != nullptr, "attempt to dereference unique_ptr to nullptr");
  return *get();
}

template<typename T, typename D>
auto unique_ptr<T, D>::operator->() const noexcept -> pointer {
  assert_msg(get() != nullptr, "attempt to dereference unique_ptr to nullptr");
  return get();
}

template<typename T, typename D>
auto unique_ptr<T, D>::get() const noexcept -> pointer {
  return _namespace(std)::get<0>(data_);
}

template<typename T, typename D>
auto unique_ptr<T, D>::get_deleter() noexcept -> deleter_type& {
  return _namespace(std)::get<1>(data_);
}

template<typename T, typename D>
auto unique_ptr<T, D>::get_deleter() const noexcept -> const deleter_type& {
  return _namespace(std)::get<1>(data_);
}

template<typename T, typename D>
unique_ptr<T, D>::operator bool() const noexcept {
  return get() != nullptr;
}

template<typename T, typename D>
auto unique_ptr<T, D>::release() noexcept -> pointer {
  return exchange(_namespace(std)::get<0>(data_), nullptr);
}

template<typename T, typename D>
auto unique_ptr<T, D>::reset(pointer p) noexcept -> void {
  pointer old_p = exchange(_namespace(std)::get<0>(data_), move(p));
  if (old_p != nullptr) get_deleter()(old_p);
}

template<typename T, typename D>
auto unique_ptr<T, D>::swap(unique_ptr& p) noexcept -> void {
  using _namespace(std)::swap;

  swap(data_, p.data_);
}


template<typename T, typename D>
constexpr unique_ptr<T[], D>::unique_ptr() noexcept
: data_(nullptr, deleter_type())
{}

template<typename T, typename D>
unique_ptr<T[], D>::unique_ptr(pointer p) noexcept
: data_(p, deleter_type())
{}

template<typename T, typename D>
unique_ptr<T[], D>::unique_ptr(pointer p, deleter_copy_arg d) noexcept
: data_(p, d)
{}

template<typename T, typename D>
unique_ptr<T[], D>::unique_ptr(pointer p, deleter_move_arg d) noexcept
: data_(p, move(d))
{
  static_assert(!D_is_reference,
                "May not create reference to temporary deleter.");
}

template<typename T, typename D>
unique_ptr<T[], D>::unique_ptr(unique_ptr&& p) noexcept
: data_(p.release(), forward<D>(p.get_deleter()))
{}

template<typename T, typename D>
unique_ptr<T[], D>::~unique_ptr() noexcept {
  if (get() != nullptr)
    get_deleter()(get());
}

template<typename T, typename D>
auto unique_ptr<T[], D>::operator=(unique_ptr&& p) noexcept -> unique_ptr& {
  /* Protect against self assignment. */
  if (_predict_false(p == *this)) return *this;

  /* Exchange current value for new value. */
  auto old =
      exchange(data_, data_type(p.release(), forward<D>(p.get_deleter())));
  /* Clean up old value. */
  if (_namespace(std)::get<0>(old) != nullptr)
    _namespace(std)::get<1>(old)(_namespace(std)::get<0>(old));
  return *this;
}

template<typename T, typename D>
auto unique_ptr<T[], D>::operator=(nullptr_t) noexcept -> unique_ptr& {
  reset();
  return *this;
}

template<typename T, typename D>
auto unique_ptr<T[], D>::get() const noexcept -> pointer {
  return _namespace(std)::get<0>(data_);
}

template<typename T, typename D>
auto unique_ptr<T[], D>::get_deleter() noexcept -> deleter_type& {
  return _namespace(std)::get<1>(data_);
}

template<typename T, typename D>
auto unique_ptr<T[], D>::get_deleter() const noexcept -> const deleter_type& {
  return _namespace(std)::get<1>(data_);
}

template<typename T, typename D>
unique_ptr<T[], D>::operator bool() const noexcept {
  return get() != nullptr;
}

template<typename T, typename D>
auto unique_ptr<T[], D>::release() noexcept -> pointer {
  return exchange(_namespace(std)::get<0>(data_), nullptr);
}

template<typename T, typename D>
auto unique_ptr<T[], D>::reset(pointer p) noexcept -> void {
  pointer old_p = exchange(_namespace(std)::get<0>(data_), move(p));
  if (old_p != nullptr) get_deleter()(old_p);
}

template<typename T, typename D>
auto unique_ptr<T[], D>::swap(unique_ptr& p) noexcept -> void {
  using _namespace(std)::swap;

  swap(data_, p.data_);
}

template<typename T, typename D>
auto unique_ptr<T[], D>::operator[](size_t i) const -> T& {
  assert_msg(get() != nullptr, "attempt to dereference unique_ptr to nullptr");
  return get()[i];
}


template<typename T, typename... Args>
auto make_unique(Args&&... args) ->
    enable_if_t<!is_array<T>::value, unique_ptr<T>> {
  return unique_ptr<T>(new T(forward<Args>(args)...));
}

template<typename T, typename... Args>
auto make_unique(size_t n) ->
    enable_if_t<is_array<T>::value && extent<T>::value == 0,
                unique_ptr<T>> {
  return unique_ptr<T>(new remove_extent_t<T>[n]);
}


template<typename T, typename D>
void swap(unique_ptr<T, D>& x, unique_ptr<T, D>& y) noexcept {
  x.swap(y);
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator==(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
  return x.get() == y.get();
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator!=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
  return x.get() != y.get();
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator<(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
  using ct = common_type_t<typename unique_ptr<T1, D1>::pointer,
                           typename unique_ptr<T2, D2>::pointer>;
  return less<ct>()(x.get(), y.get());
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator<=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
  return !(y < x);
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
  return y < x;
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator>=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
  return !(x < y);
}

template<typename T, typename D>
bool operator==(const unique_ptr<T, D>& x, nullptr_t) noexcept {
  return !x;
}
template<typename T, typename D>
bool operator==(nullptr_t, const unique_ptr<T, D>& x) noexcept {
  return !x;
}

template<typename T, typename D>
bool operator!=(const unique_ptr<T, D>& x, nullptr_t) noexcept {
  return bool(x);
}
template<typename T, typename D>
bool operator!=(nullptr_t, const unique_ptr<T, D>& x) noexcept{
  return bool(x);
}

template<typename T, typename D>
bool operator<(const unique_ptr<T, D>& x, nullptr_t) noexcept {
  return less<typename unique_ptr<T, D>::pointer>()(x, nullptr);
}
template<typename T, typename D>
bool operator<(nullptr_t, const unique_ptr<T, D>& x) noexcept {
  return less<typename unique_ptr<T, D>::pointer>()(nullptr, x);
}

template<typename T, typename D>
bool operator<=(const unique_ptr<T, D>& x, nullptr_t) noexcept {
  return !(nullptr < x);
}
template<typename T, typename D>
bool operator<=(nullptr_t, const unique_ptr<T, D>& x) noexcept {
  return !(x < nullptr);
}

template<typename T, typename D>
bool operator>(const unique_ptr<T, D>& x, nullptr_t) noexcept {
  return nullptr < x;
}
template<typename T, typename D>
bool operator>(nullptr_t, const unique_ptr<T, D>& x) noexcept {
  return x < nullptr;
}

template<typename T, typename D>
bool operator>=(const unique_ptr<T, D>& x, nullptr_t) noexcept {
  return !(x < nullptr);
}
template<typename T, typename D>
bool operator>=(nullptr_t, const unique_ptr<T, D>& x) noexcept {
  return !(nullptr < x);
}


template<typename T>
auto_ptr<T>::auto_ptr(T* p) noexcept
: ptr_(p)
{}

template<typename T>
auto_ptr<T>::auto_ptr(auto_ptr& x) noexcept
: ptr_(x.release())
{}

template<typename T>
template<typename U>
auto_ptr<T>::auto_ptr(auto_ptr<U>& x) noexcept
: ptr_(x.release())
{}

template<typename T>
auto auto_ptr<T>::operator=(auto_ptr& p) noexcept -> auto_ptr& {
  reset(p.release());
  return *this;
}

template<typename T>
template<typename U>
auto auto_ptr<T>::operator=(auto_ptr<U>& p) noexcept -> auto_ptr& {
  reset(p.release());
  return *this;
}

template<typename T>
auto auto_ptr<T>::operator=(auto_ptr_ref<T> p) noexcept -> auto_ptr& {
  reset(p.release());
}

template<typename T>
auto_ptr<T>::~auto_ptr() noexcept {
  delete release();
}

template<typename T>
T& auto_ptr<T>::operator*() const noexcept {
  assert_msg(get() != nullptr, "attempt to dereference auto_ptr to nullptr");
  return *get();
}

template<typename T>
T* auto_ptr<T>::operator->() const noexcept {
  assert_msg(get() != nullptr, "attempt to dereference auto_ptr to nullptr");
  return get();
}

template<typename T>
T* auto_ptr<T>::get() const noexcept {
  return ptr_;
}

template<typename T>
T* auto_ptr<T>::release() noexcept {
  return exchange(ptr_, nullptr);
}

template<typename T>
void auto_ptr<T>::reset(T* p) noexcept {
  if (_predict_true(get() != p))
    delete exchange(ptr_, p);
}

template<typename T>
auto_ptr<T>::auto_ptr(auto_ptr_ref<T> p) noexcept
: auto_ptr(p.release())
{}

template<typename T>
template<typename U>
auto_ptr<T>::operator auto_ptr_ref<U>() noexcept {
  return auto_ptr_ref<U>(*this);
}

template<typename T>
template<typename U>
auto_ptr<T>::operator auto_ptr<U>() noexcept {
  return auto_ptr<U>(release());
}


template<typename T>
template<typename U>
auto_ptr_ref<T>::auto_ptr_ref(auto_ptr<U>& p) noexcept
: ptr_(addressof(p)),
  fn_(&release_impl_<U>)
{}

template<typename T>
T* auto_ptr_ref<T>::release() const noexcept {
  if (!ptr_) return nullptr;
  return (*fn_)(ptr_);
}

template<typename T>
template<typename U>
T* auto_ptr_ref<T>::release_impl_(void* pp) noexcept {
  return static_cast<auto_ptr<U>*>(pp)->release();
}


/* Implementation for shared_ptr_ownership. */
namespace impl {


template<typename T, typename D, typename A>
class shared_ptr_ownership_impl
: public shared_ptr_ownership
{
 public:
  using allocator_type = typename
      allocator_traits<A>::template rebind_alloc<shared_ptr_ownership_impl>;

 private:
  using data_type = tuple<unique_ptr<T, D>, A>;

 public:
  shared_ptr_ownership_impl(unique_ptr<T, D>, A);

  void* get_deleter(const type_info& ti) noexcept override;

 private:
  void release_pointee() noexcept override;
  void destroy_me() noexcept override;

  data_type data_;
};


template<typename T, typename D, typename A>
shared_ptr_ownership_impl<T, D, A>::shared_ptr_ownership_impl(
    unique_ptr<T, D> ptr, A alloc)
: data_(move(ptr), move(alloc))
{}

template<typename T, typename D, typename A>
void* shared_ptr_ownership_impl<T, D, A>::get_deleter(const type_info& ti)
    noexcept {
  if (typeid(D) == ti)
    return &get<0>(data_).get_deleter();
  return nullptr;
}

template<typename T, typename D, typename A>
void shared_ptr_ownership_impl<T, D, A>::release_pointee() noexcept {
  get<0>(data_) = nullptr;
}

template<typename T, typename D, typename A>
void shared_ptr_ownership_impl<T, D, A>::destroy_me() noexcept {
  allocator_type alloc = get<1>(move(data_));
  allocator_traits<allocator_type>::destroy(alloc, this);
  allocator_traits<allocator_type>::deallocate(alloc, this, 1);
}


template<typename T, typename D, typename A>
shared_ptr_ownership* allocate_shared_ptr_ownership(
    unique_ptr<T, D> ptr, const A& alloc_arg = std::allocator<void>()) {
  using impl = shared_ptr_ownership_impl<T, D, A>;
  using allocator_type = typename impl::allocator_type;

  class uninitialized_deleter {
   public:
    uninitialized_deleter(allocator_type& alloc) noexcept : alloc_(alloc) {}

    void operator()(impl* p) const noexcept {
      allocator_traits<allocator_type>::deallocate(alloc_, p, 1);
    }

   private:
    allocator_type& alloc_;
  };

  allocator_type alloc = alloc_arg;
  auto storage_ptr = unique_ptr<impl*, uninitialized_deleter>(
      allocator_traits<allocator_type>::allocate(1), alloc);
  allocator_traits<allocator_type>::construct(storage_ptr.get(),
                                              move(ptr), alloc);
  return storage_ptr.release();
}


} /* namespace impl */

template<typename T>
template<typename Y>
shared_ptr<T>::shared_ptr(Y* ptr)
: shared_ptr(unique_ptr<Y>(ptr))
{}

template<typename T>
template<typename Y, typename D>
shared_ptr<T>::shared_ptr(Y* ptr, D deleter)
: shared_ptr(unique_ptr<Y, D>(ptr, move_if_noexcept(deleter)))
{}

template<typename T>
template<typename Y, typename D, typename A>
shared_ptr<T>::shared_ptr(Y* ptr, D deleter, A alloc)
: shared_ptr(unique_ptr<Y, D>(ptr, move_if_noexcept(deleter)),
             move_if_noexcept(alloc))
{}

template<typename T>
template<typename D>
shared_ptr<T>::shared_ptr(nullptr_t, D)
: shared_ptr()
{}

template<typename T>
template<typename D, typename A>
shared_ptr<T>::shared_ptr(nullptr_t, D, A)
: shared_ptr()
{}

template<typename T>
template<typename Y>
shared_ptr<T>::shared_ptr(const shared_ptr<Y>& owner, T* ptr) noexcept
: ptr_(ptr),
  ownership_(owner.ownership_)
{
  if (ownership_) {
    impl::shared_ptr_ownership::acquire(ownership_);
    ownership_->shared_ptr_acquire_from_shared_ptr();
  }
}

template<typename T>
shared_ptr<T>::shared_ptr(const shared_ptr& ptr) noexcept
: shared_ptr(ptr, (ptr.ownership_ ? ptr.get() : nullptr))
{}

template<typename T>
template<typename Y, typename>
shared_ptr<T>::shared_ptr(const shared_ptr<Y>& ptr) noexcept
: shared_ptr(ptr, (ptr.ownership_ ? ptr.get() : nullptr))
{}

template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr&& ptr) noexcept
: ptr_(exchange(ptr.ptr_, nullptr)),
  ownership_(exchange(ptr.ownership_, nullptr))
{}

template<typename T>
template<typename Y, typename>
shared_ptr<T>::shared_ptr(shared_ptr<Y>&& ptr) noexcept
: ptr_(exchange(ptr.ptr_, nullptr)),
  ownership_(exchange(ptr.ownership_, nullptr))
{}

template<typename T>
template<typename Y>
shared_ptr<T>::shared_ptr(const weak_ptr<Y>& wptr) {
  T* ptr = wptr.ptr_;
  impl::shared_ptr_ownership* ownership = wptr.ownership_;

  /* Try to acquire ownership. */
  if (ownership) {
    impl::shared_ptr_ownership::acquire(ownership);
    if (!ownership->shared_ptr_acquire_from_weak_ptr()) {
      impl::shared_ptr_ownership::release(ownership);
      throw bad_weak_ptr();
    }
  }

  /* Ownership acquisition succesful, assign to this. */
  ptr_ = ptr;
  ownership_ = ownership;
}

template<typename T>
template<typename Y>
shared_ptr<T>::shared_ptr(auto_ptr<Y>&& aptr)
: shared_ptr(unique_ptr<Y>(aptr))
{}

template<typename T>
template<typename Y, typename D>
shared_ptr<T>::shared_ptr(unique_ptr<Y, D>&& ptr)
: shared_ptr(move(ptr), allocator<void>())
{}

template<typename T>
template<typename Y, typename D, typename A>
shared_ptr<T>::shared_ptr(unique_ptr<Y, D>&& ptr, A alloc) {
  if (ptr) {
    T* p = ptr.get();
    /* ownership_ gets constructed with reference counters set to 1. */
    ownership_ = impl::allocate_shared_ptr_ownership(move(ptr),
                                                     move_if_noexcept(alloc));
    ptr_ = p;
  }
}

template<typename T>
shared_ptr<T>::~shared_ptr() noexcept {
  if (ownership_) {
    ownership_->shared_ptr_release();
    impl::shared_ptr_ownership::release(ownership_);
  } else if (ptr_) {
    delete ptr_;
  }

  ownership_ = nullptr;
  ptr_ = nullptr;
}

template<typename T>
auto shared_ptr<T>::operator=(const shared_ptr& ptr) noexcept -> shared_ptr& {
  if (ownership_ != nullptr && ownership_ == ptr.ownership_)
    ptr_ = ptr.ptr_;  // Don't modify the reference counters.
  else
    shared_ptr(ptr).swap(*this);
  return *this;
}

template<typename T>
template<typename Y>
auto shared_ptr<T>::operator=(const shared_ptr<Y>& ptr) noexcept ->
    shared_ptr& {
  if (ownership_ != nullptr && ownership_ == ptr.ownership_)
    ptr_ = ptr.ptr_;  // Don't modify the reference counters.
  else
    shared_ptr(ptr).swap(*this);
  return *this;
}

template<typename T>
auto shared_ptr<T>::operator=(shared_ptr&& ptr) noexcept -> shared_ptr& {
  shared_ptr(move(ptr)).swap(*this);
  return *this;
}

template<typename T>
template<typename Y>
auto shared_ptr<T>::operator=(auto_ptr<Y>&& ptr) -> shared_ptr& {
  shared_ptr(move(ptr)).swap(*this);
  return *this;
}

template<typename T>
template<typename Y, typename D>
auto shared_ptr<T>::operator=(unique_ptr<Y, D>&& ptr) -> shared_ptr& {
  shared_ptr(move(ptr)).swap(*this);
  return *this;
}

template<typename T>
auto shared_ptr<T>::swap(shared_ptr& other) noexcept -> void {
  using _namespace(std)::swap;

  swap(ptr_, other.ptr_);
  swap(ownership_, other.ownership_);
}

template<typename T>
auto shared_ptr<T>::reset() noexcept -> void {
  shared_ptr().swap(*this);
}

template<typename T>
template<typename Y>
auto shared_ptr<T>::reset(Y* p) -> void {
  shared_ptr(p).swap(*this);
}

template<typename T>
template<typename Y, typename D>
auto shared_ptr<T>::reset(Y* p, D d) -> void {
  shared_ptr(p, d).swap(*this);
}

template<typename T>
template<typename Y, typename D, typename A>
auto shared_ptr<T>::reset(Y* p, D d, A a) -> void {
  shared_ptr(p, d, a).swap(*this);
}

template<typename T>
auto shared_ptr<T>::get() const noexcept -> T* {
  return ptr_;
}

template<typename T>
auto shared_ptr<T>::operator*() const noexcept ->
    typename conditional_t<is_void<T>::value,
                           identity<T>,
                           add_lvalue_reference<T>>::type {
  assert_msg(get() != nullptr, "shared_ptr: dereference of nullptr");
  return *get();
}

template<typename T>
auto shared_ptr<T>::operator->() const noexcept -> T* {
  assert_msg(get() != nullptr, "shared_ptr: dereference of nullptr");
  return get();
}

template<typename T>
auto shared_ptr<T>::use_count() const noexcept -> long {
  return (ownership_ ? ownership_->get_shared_refcount() : 0);
}

template<typename T>
auto shared_ptr<T>::unique() const noexcept -> bool {
  /*
   * NOTE: this does not count weak pointers to the object.
   */
  return use_count() == 1;
}

template<typename T>
shared_ptr<T>::operator bool() const noexcept {
  return get() != nullptr;
}

template<typename T>
template<typename U>
auto shared_ptr<T>::owner_before(const shared_ptr<U>& other) const -> bool {
  return ownership_ < other.ownership_;
}

template<typename T>
template<typename U>
auto shared_ptr<T>::owner_before(const weak_ptr<U>& other) const -> bool {
  return ownership_ < other.ownership_;
}

template<typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args) {
  return allocate_shared<T>(std::allocator<void>(), forward<Args>(args)...);
}

template<typename T, typename A, typename... Args>
shared_ptr<T> allocate_shared(const A& alloc, Args&&... args) {
  shared_ptr<T> rv;

  auto ownership_impl = impl::create_placement_shared_ptr_ownership(
      alloc, forward<Args>(args)...);
  rv.ownership_ = ownership_impl;
  rv.ptr_ = ownership_impl.get();
  return rv;
}

template<typename T, typename U>
bool operator==(const shared_ptr<T>& a, const shared_ptr<U>& b) noexcept {
  return a.get() == b.get();
}

template<typename T, typename U>
bool operator!=(const shared_ptr<T>& a, const shared_ptr<U>& b) noexcept {
}

template<typename T, typename U>
bool operator<(const shared_ptr<T>& a, const shared_ptr<U>& b) noexcept {
  using ct = common_type_t<typename pointer_traits<shared_ptr<T>>::pointer,
                           typename pointer_traits<shared_ptr<U>>::pointer>;
  return less<ct>()(a.get(), b.get());
}

template<typename T, typename U>
bool operator<=(const shared_ptr<T>& a, const shared_ptr<U>& b) noexcept {
  return !(b < a);
}

template<typename T, typename U>
bool operator>(const shared_ptr<T>& a, const shared_ptr<U>& b) noexcept {
  return b < a;
}

template<typename T, typename U>
bool operator>=(const shared_ptr<T>& a, const shared_ptr<U>& b) noexcept {
  return !(a < b);
}

template<typename T>
bool operator==(const shared_ptr<T>& a, nullptr_t) noexcept {
  return !a;
}
template<typename T>
bool operator==(nullptr_t, const shared_ptr<T>& a) noexcept {
  return !a;
}

template<typename T>
bool operator!=(const shared_ptr<T>& a, nullptr_t) noexcept {
  return bool(a);
}
template<typename T>
bool operator!=(nullptr_t, const shared_ptr<T>& a) noexcept {
  return bool(a);
}

template<typename T>
bool operator<(const shared_ptr<T>& a, nullptr_t) noexcept {
  return less<T*>()(a.get(), nullptr);
}
template<typename T>
bool operator<(nullptr_t, const shared_ptr<T>& a) noexcept {
  return less<T*>()(nullptr, a.get());
}

template<typename T>
bool operator>(const shared_ptr<T>& a, nullptr_t) noexcept {
  return nullptr < a;
}
template<typename T>
bool operator>(nullptr_t, const shared_ptr<T>& a) noexcept {
  return a < nullptr;
}

template<typename T>
bool operator<=(const shared_ptr<T>& a, nullptr_t) noexcept {
  return !(nullptr < a);
}
template<typename T>
bool operator<=(nullptr_t, const shared_ptr<T>& a) noexcept {
  return !(a < nullptr);
}

template<typename T>
bool operator>=(const shared_ptr<T>& a, nullptr_t) noexcept {
  return !(a < nullptr);
}
template<typename T>
bool operator>=(nullptr_t, const shared_ptr<T>& a) noexcept {
  return !(nullptr < a);
}

template<typename T>
void swap(shared_ptr<T>& a, shared_ptr<T>& b) noexcept {
  a.swap(b);
}

template<typename T, typename U>
shared_ptr<T> static_pointer_cast(const shared_ptr<U>& r) noexcept {
  if (!r.use_count()) return nullptr;
  return shared_ptr<T>(r, static_cast<T*>(r.get()));
}

template<typename T, typename U>
shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U>& r) noexcept {
  if (!r.use_count()) return nullptr;
  return shared_ptr<T>(r, dynamic_cast<T*>(r.get()));
}

template<typename T, typename U>
shared_ptr<T> const_pointer_cast(const shared_ptr<U>& r) noexcept {
  if (!r.use_count()) return nullptr;
  return shared_ptr<T>(r, const_cast<T*>(r.get()));
}

template<typename D, typename T>
D* get_deleter(const shared_ptr<T>& ptr) noexcept {
  void* d = (ptr.ownership_ ?
             ptr.ownership_->get_deleter(typeid(D)) :
             nullptr);
  return (d ? static_cast<D*>(d) : nullptr);
}

template<typename E, typename T, typename Y>
basic_ostream<E, T>& operator<<(basic_ostream<E, T>& os,
                                const shared_ptr<Y>& p) {
  os << p.get();
  return os;
}


_namespace_end(std)
