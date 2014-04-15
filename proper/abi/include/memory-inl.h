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


_namespace_end(std)
