_namespace_begin(std)


template<typename T>
reference_wrapper<T>::reference_wrapper(type& v) noexcept
: ref_(addressof(v))
{}

template<typename T>
reference_wrapper<T>::operator type&() const noexcept {
  return get();
}

template<typename T>
auto reference_wrapper<T>::get() const noexcept -> type& {
  return *ref_;
}

template<typename T>
template<typename... ArgTypes>
auto reference_wrapper<T>::operator()(ArgTypes&&... args) const
    noexcept(noexcept(get()(forward<ArgTypes>(args)...))) ->
    result_of_t<type&(ArgTypes&&...)> {
  return impl::invoke(get(), forward<ArgTypes>(args)...);
}

template<typename T>
reference_wrapper<T> ref(T& v) noexcept {
  return reference_wrapper<T>(v);
}

template<typename T>
reference_wrapper<const T> cref(const T& v) noexcept {
  return reference_wrapper<const T>(v);
}

template<typename T>
reference_wrapper<T> ref(reference_wrapper<T> rw) noexcept {
  return reference_wrapper<T>(rw.get());
}

template<typename T>
reference_wrapper<const T> cref(reference_wrapper<T> rw) noexcept {
  return reference_wrapper<const T>(rw.get());
}


template<typename T, typename U>
constexpr auto plus<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) + forward<U>(y)) {
  return forward<T>(x) + forward<U>(y);
}

template<typename T, typename U>
constexpr auto minus<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) - forward<U>(y)) {
  return forward<T>(x) - forward<U>(y);
}

template<typename T, typename U>
constexpr auto multiplies<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) * forward<U>(y)) {
  return forward<T>(x) * forward<U>(y);
}

template<typename T, typename U>
constexpr auto divides<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) / forward<U>(y)) {
  return forward<T>(x) / forward<U>(y);
}

template<typename T, typename U>
constexpr auto modulus<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) % forward<U>(y)) {
  return forward<T>(x) % forward<U>(y);
}

template<typename T>
constexpr auto negate<void>::operator()(T&& x) const ->
    decltype(-forward<T>(x)) {
  return -forward<T>(x);
}


template<typename T, typename U>
constexpr auto logical_and<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) && forward<U>(y)) {
  return forward<T>(x) && forward<U>(y);
}

template<typename T, typename U>
constexpr auto logical_or<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) || forward<U>(y)) {
  return forward<T>(x) || forward<U>(y);
}

template<typename T>
constexpr auto logical_not<void>::operator()(T&& x) const ->
    decltype(!forward<T>(x)) {
  return !forward<T>(x);
}


template<typename T, typename U>
constexpr auto bit_and<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) & forward<U>(y)) {
  return forward<T>(x) & forward<U>(y);
}

template<typename T, typename U>
constexpr auto bit_or<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) | forward<U>(y)) {
  return forward<T>(x) | forward<U>(y);
}

template<typename T, typename U>
constexpr auto bit_xor<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) ^ forward<U>(y)) {
  return forward<T>(x) ^ forward<U>(y);
}

template<typename T>
constexpr auto bit_not<void>::operator()(T&& x) const ->
    decltype(~forward<T>(x)) {
  return ~forward<T>(x);
}


template<typename Predicate>
constexpr unary_negate<Predicate>::unary_negate(const Predicate& pred)
: pred_(pred)
{}

template<typename Predicate>
constexpr bool unary_negate<Predicate>::operator()(const argument_type& arg)
    const {
  return !pred(arg);
}

template<typename Predicate>
constexpr unary_negate<Predicate> not1(const Predicate& pred) {
  return unary_negate<Predicate>(pred);
}


template<typename Predicate>
constexpr binary_negate<Predicate>::binary_negate(const Predicate& pred)
: pred_(pred)
{}

template<typename Predicate>
constexpr bool binary_negate<Predicate>::operator()(
    const first_argument_type& arg1, const second_argument_type& arg2) const {
  return !pred(arg1, arg2);
}

template<typename Predicate>
constexpr binary_negate<Predicate> not2(const Predicate& pred) {
  return binary_negate<Predicate>(pred);
}


namespace _bind {


using ::_namespace(std)::impl::invoke;


template<typename T> struct _result_type_impl {
  struct type { using result_type = T; };
};
struct _no_result_type { struct type {}; };

_MEMBER_TYPE_CHECK(result_type);

template<typename T, typename... Args>
struct _result_type_impl<T(Args...)> {
  using result_type = T;
};

template<typename T, typename Arg0, typename Arg1>
struct _result_type_impl<T(Arg0, Arg1)> {
  using result_type = T;
  using first_argument_type = Arg0;
  using second_argument_type = Arg1;
};

template<typename T, typename Arg>
struct _result_type_impl<T(Arg)> {
  using result_type = T;
  using argument_type = Arg;
};

template<typename T, typename C, typename... Args>
struct _result_type_impl<T (C::*)(Args...)>
: _result_type_impl<T(C*, Args...)> {};

template<typename T, typename C, typename... Args>
struct _result_type_impl<T (C::*)(Args...) const>
: _result_type_impl<T(const C*, Args...)> {};

template<typename T, typename C, typename... Args>
struct _result_type_impl<T (C::*)(Args...) volatile>
: _result_type_impl<T(volatile C*, Args...)> {};

template<typename T, typename C, typename... Args>
struct _result_type_impl<T (C::*)(Args...) const volatile>
: _result_type_impl<T(const volatile C*, Args...)> {};

template<typename T, typename... Args>
struct _result_type_impl<T (*)(Args...)>
: _result_type_impl<T(Args...)> {};

template<typename R, typename T>
struct _result_type_impl<R T::*>
: _result_type_impl<R(T)> {};

/* Base class, used to add the result_type member type. */
template<typename T> using _result_type =
    typename conditional_t<member_type_check_result_type<T>::value,
                           _result_type_impl<T>,
                           _no_result_type>::type;


/*
 * Resolve argument:  case for non-bind-expression, non-placeholder.
 */
template<typename T, typename Args, typename ArgIndices>
auto resolve_argument(T& v, Args, ArgIndices) ->
    enable_if_t<!is_bind_expression<T>::value &&
                !is_placeholder<T>::value,
                T&> {
  return v;
}

/*
 * Resolve argument:  case for placeholder.
 */
template<typename PlaceHolder>
struct placeholder_type {
  template<typename Tuple> using type =
      typename tuple_element<is_placeholder<PlaceHolder>::value - 1,
                             Tuple>::type;
};
template<typename T, typename Args, size_t... ArgIndices>
auto resolve_argument(const T& v, Args args,
                      index_sequence<ArgIndices...>) ->
    typename enable_if_t<is_placeholder<T>::value,
                         placeholder_type<T>
                        >::template type<Args> {
  constexpr int index = is_placeholder<T>::value - 1;
  using result = typename tuple_element<index, Args>::type;
  static_assert(is_reference<result>::value,
                "Argument is not a reference...");

  return static_cast<result>(get<index>(args));
}

/*
 * Resolve argument:  case for bind expression.
 */
template<typename T, typename Args, size_t... ArgIndices>
auto resolve_argument(T& v, Args args,
                      index_sequence<ArgIndices...>) ->
    enable_if_t<is_bind_expression<T>::value,
                decltype(v(get<ArgIndices>(args)...))> {
  static_assert(!is_void<decltype(v(get<ArgIndices>(args)...))>::value,
                "Nested bind expression returning void.");
  return v(get<ArgIndices>(args)...);
}


/*
 * Invoke tuple of f, bound_args... using tuple of args...
 */
template<typename FTuple, typename ArgTuple,
         size_t... FTIndices, size_t... ArgIndices>
auto invoke_tuple(FTuple& ft, ArgTuple args,
                  index_sequence<FTIndices...>,
                  index_sequence<ArgIndices...> arg_indices) ->
    decltype(invoke(get<0>(ft), resolve_argument(get<FTIndices + 1>(ft),
                                                 args, arg_indices)...)) {
  return invoke(get<0>(ft), resolve_argument(get<FTIndices + 1>(ft),
                                             args, arg_indices)...);
}


template<typename F, typename... BoundArgs>
class expression
: public _result_type<F>
{
 private:
  using data_type = tuple<F, BoundArgs...>;

 public:
  expression() = delete;
  expression(const expression&) = default;
  expression& operator=(const expression&) = default;

  expression(expression&& other)
      noexcept(is_nothrow_move_constructible<data_type>::value)
  : data_(move(other.data_))
  {}

  template<typename F_, typename... Args_>
  expression(F_&& f, Args_&&... args)
      noexcept(is_nothrow_constructible<data_type, F_, Args_...>::value)
  : data_(forward<F_>(f), forward<Args_>(args)...)
  {}

  expression& operator=(expression&& other)
      noexcept(is_nothrow_move_assignable<data_type>::value) {
    data_ = move(other.data_);
    return *this;
  }

  template<typename... Args>
  auto operator()(Args&&... args) ->
      decltype(invoke_tuple(declval<expression::data_type&>(),
                            forward_as_tuple(forward<Args>(args)...),
                            index_sequence_for<BoundArgs...>(),
                            index_sequence_for<Args...>())) {
    return invoke_tuple(data_,
                        forward_as_tuple(forward<Args>(args)...),
                        index_sequence_for<BoundArgs...>(),
                        index_sequence_for<Args...>());
  }

  template<typename... Args>
  auto operator()(Args&&... args) const ->
      decltype(invoke_tuple(declval<const expression::data_type&>(),
                            forward_as_tuple(forward<Args>(args)...),
                            index_sequence_for<BoundArgs...>(),
                            index_sequence_for<Args...>())) {
    return invoke_tuple(data_,
                        forward_as_tuple(forward<Args>(args)...),
                        index_sequence_for<BoundArgs...>(),
                        index_sequence_for<Args...>());
  }

 private:
  data_type data_;
};

template<typename R, typename F, typename... BoundArgs>
class expression_r
: public _result_type<F>
{
 private:
  using impl_type = expression<F, BoundArgs...>;

 public:
  using result_type = R;

  expression_r() = delete;
  expression_r(const expression_r&) = default;
  expression_r& operator=(const expression_r&) = default;

  expression_r(expression_r&& other)
      noexcept(is_nothrow_move_constructible<impl_type>::value)
  : impl_(move(other.impl_))
  {}

  template<typename F_, typename... Args_>
  expression_r(F_&& f, Args_&&... args)
      noexcept(is_nothrow_constructible<impl_type, F_, Args_...>::value)
  : impl_(forward<F_>(f), forward<Args_>(args)...)
  {}

  expression_r& operator=(expression_r&& other)
      noexcept(is_nothrow_move_assignable<impl_type>::value) {
    impl_ = move(other.impl_);
    return *this;
  }

  template<typename... Args>
  auto operator()(Args&&... args) -> result_type {
    return impl_(forward<Args>(args)...);
  }

  template<typename... Args>
  auto operator()(Args&&... args) const -> result_type {
    return impl_(forward<Args>(args)...);
  }

 private:
  impl_type impl_;
};
/* Specialization for void return type. */
template<typename F, typename... BoundArgs>
class expression_r<void, F, BoundArgs...>
: public _result_type<F>
{
 private:
  using impl_type = expression<F, BoundArgs...>;

 public:
  using result_type = void;

  expression_r() = delete;
  expression_r(const expression_r&) = default;
  expression_r& operator=(const expression_r&) = default;

  expression_r(expression_r&& other)
      noexcept(is_nothrow_move_constructible<impl_type>::value)
  : impl_(move(other.impl_))
  {}

  template<typename F_, typename... Args_>
  expression_r(F_&& f, Args_&&... args)
      noexcept(is_nothrow_constructible<impl_type, F_, Args_...>::value)
  : impl_(forward<F_>(f), forward<Args_>(args)...)
  {}

  expression_r& operator=(expression_r&& other)
      noexcept(is_nothrow_move_assignable<impl_type>::value) {
    impl_ = move(other.impl_);
    return *this;
  }

  template<typename... Args>
  auto operator()(Args&&... args) -> result_type {
    impl_(forward<Args>(args)...);
  }

  template<typename... Args>
  auto operator()(Args&&... args) const -> result_type {
    impl_(forward<Args>(args)...);
  }

 private:
  impl_type impl_;
};


template<typename F>
class mem_fn
: public _result_type<F>
{
 public:
  constexpr mem_fn(F fn) noexcept
  : fn_(fn)
  {}

  template<typename... Args>
  typename _result_type<F>::result_type operator()(Args&&... args) const
      noexcept(noexcept(invoke(this->fn_, forward<Args>(args)...))) {
    return invoke(fn_, forward<Args>(args)...);
  }

 private:
  F fn_ = nullptr;
};


} /* namespace std::_bind */


template<typename F, typename... BoundArgs>
struct is_bind_expression<_bind::expression<F, BoundArgs...>>
: true_type {};
template<typename R, typename F, typename... BoundArgs>
struct is_bind_expression<_bind::expression_r<R, F, BoundArgs...>>
: true_type {};

template<typename T> struct is_bind_expression<const T>
: is_bind_expression<T> {};
template<typename T> struct is_bind_expression<volatile T>
: is_bind_expression<T> {};
template<typename T> struct is_bind_expression<const volatile T>
: is_bind_expression<T> {};
template<typename T> struct is_bind_expression<T&>
: is_bind_expression<T> {};
template<typename T> struct is_bind_expression<T&&>
: is_bind_expression<T> {};

template<typename T> struct is_placeholder<const T>
: is_placeholder<T> {};
template<typename T> struct is_placeholder<volatile T>
: is_placeholder<T> {};
template<typename T> struct is_placeholder<const volatile T>
: is_placeholder<T> {};
template<typename T> struct is_placeholder<T&>
: is_placeholder<T> {};
template<typename T> struct is_placeholder<T&&>
: is_placeholder<T> {};


template<typename F, typename... BoundArgs>
auto bind(F&& f, BoundArgs&&... bound_args)
    noexcept(noexcept(_bind::expression<decay_t<F>, decay_t<BoundArgs>...>(
        forward<F>(f), forward<BoundArgs>(bound_args)...))) ->
    _bind::expression<decay_t<F>, decay_t<BoundArgs>...> {
  static_assert(is_bind_expression<_bind::expression<decay_t<F>, decay_t<BoundArgs>...>>::value, "GRMBL");
  return _bind::expression<decay_t<F>, decay_t<BoundArgs>...>(
      forward<F>(f), forward<BoundArgs>(bound_args)...);
}

template<typename R, typename F, typename... BoundArgs>
auto bind(F&& f, BoundArgs&&... bound_args)
    noexcept(noexcept(
        _bind::expression_r<R, decay_t<F>, decay_t<BoundArgs>...>(
            forward<F>(f), forward<BoundArgs>(bound_args)...))) ->
    _bind::expression_r<R, decay_t<F>, decay_t<BoundArgs>...> {
  static_assert(is_bind_expression<_bind::expression_r<R, decay_t<F>, decay_t<BoundArgs>...>>::value, "GRMBL");
  return _bind::expression_r<R, decay_t<F>, decay_t<BoundArgs>...>(
      forward<F>(f), forward<BoundArgs>(bound_args)...);
}


template<typename R, typename T>
_bind::mem_fn<R T::*> mem_fn(R T::*fn) {
  return _bind::mem_fn<R T::*>(fn);
}


template<typename R, typename... ArgTypes>
function<R(ArgTypes...)>::function() noexcept
: fn_(&bad_function_call_fn),
  data_()
{}

template<typename R, typename... ArgTypes>
function<R(ArgTypes...)>::function(nullptr_t) noexcept
: function()
{}

template<typename R, typename... ArgTypes>
function<R(ArgTypes...)>::function(const function& other)
: function(allocator_arg_t(), allocator<void>(), other)
{}

template<typename R, typename... ArgTypes>
function<R(ArgTypes...)>::function(function&& other) noexcept
: function()
{
  swap(other);
}

template<typename R, typename... ArgTypes>
template<typename F>
function<R(ArgTypes...)>::function(F f)
: function()
{
  fn_ = data_.assign(is_special(), forward<F>(f));
}

template<typename R, typename... ArgTypes>
template<typename A>
function<R(ArgTypes...)>::function(allocator_arg_t, const A&) noexcept
: function()
{}

template<typename R, typename... ArgTypes>
template<typename A>
function<R(ArgTypes...)>::function(allocator_arg_t, const A&, nullptr_t)
    noexcept
: function()
{}

template<typename R, typename... ArgTypes>
template<typename A>
function<R(ArgTypes...)>::function(allocator_arg_t, const A& alloc,
    const function& other)
: function()
{
  if (other.is_special()) {
    fn_ = data_.assign(is_special(), other.data_.special);
  } else {
    fn_ = data_.assign(is_special(),
                       functor_wrapper_t::clone(*other.data_.functor, alloc));
  }
}

template<typename R, typename... ArgTypes>
template<typename A>
function<R(ArgTypes...)>::function(allocator_arg_t, const A& alloc,
    function&& other)
: function()
{
  if (other.is_special()) {
    fn_ = data_.assign(is_special(), move(other.data_.special));
  } else {
    fn_ = data_.assign(is_special(),
                       functor_wrapper_t::clone(move(*other.data_.functor),
                                                alloc));
  }
  other.reset();
}

template<typename R, typename... ArgTypes>
template<typename A, typename F>
function<R(ArgTypes...)>::function(allocator_arg_t, const A& alloc, F f)
: function()
{
  fn_ = data_.assign(is_special(), forward<F>(f), alloc);
}

template<typename R, typename... ArgTypes>
auto function<R(ArgTypes...)>::operator=(const function& other) -> function& {
  if (other.is_special()) {
    fn_ = data_.assign(is_special(), other.data_.special);
  } else {
    fn_ = data_.assign(is_special(),
                       functor_wrapper_t::clone(*other.data_.functor,
                                                allocator<void>()));
  }
  return *this;
}

template<typename R, typename... ArgTypes>
auto function<R(ArgTypes...)>::operator=(function&& other) -> function& {
  function(forward<function>(other)).swap(*this);
  return *this;
}

template<typename R, typename... ArgTypes>
auto function<R(ArgTypes...)>::operator=(nullptr_t) -> function& {
  fn_ = data_.assign(is_special(), nullptr);
  return *this;
}

template<typename R, typename... ArgTypes>
template<typename F>
auto function<R(ArgTypes...)>::operator=(F&& f) -> function& {
  fn_ = data_.assign(is_special(), forward<F>(f));
  return *this;
}

template<typename R, typename... ArgTypes>
template<typename F>
auto function<R(ArgTypes...)>::operator=(reference_wrapper<F>& f) noexcept ->
    function& {
  function(f).swap(*this);
  return *this;
}

template<typename R, typename... ArgTypes>
function<R(ArgTypes...)>::~function() noexcept {
  fn_ = data_.assign(is_special(), nullptr);
}

template<typename R, typename... ArgTypes>
void function<R(ArgTypes...)>::swap(function& other) noexcept {
  using _namespace(std)::swap;

  const fn_type my_fn = fn_;
  const fn_type other_fn = other.fn_;

  if (other.is_special()) {
    special_ptr tmp = other.data_.special;
    if (is_special())
      other.fn_ = other.data_.assign(true, move(data_.functor));
    else
      other.fn_ = other.data_.assign(true, data_.special, my_fn);
    fn_ = data_.assign(is_special(), tmp, other_fn);
  } else if (!is_special()) {
    swap(data_.functor, other.data_.functor);
    /* fn_ is null for both. */
  } else {
    /* this is special, other is not special. */
    special_ptr tmp = data_.special;
    fn_ = data_.assign(true, move(other.data_.functor));
    other.fn_ = other.data_.assign(false, tmp, my_fn);
  }
}

template<typename R, typename... ArgTypes>
template<typename F, typename A>
void function<R(ArgTypes...)>::assign(F&& f, const A& a) {
  function(allocator_arg_t(), a, forward<F>(f)).swap(*this);
}

template<typename R, typename... ArgTypes>
function<R(ArgTypes...)>::operator bool() const noexcept {
  return !is_special() || data_.special.ti != nullptr;
}

template<typename R, typename... ArgTypes>
auto function<R(ArgTypes...)>::operator()(ArgTypes... args) const -> R {
  if (is_special())
    return impl::invoke(fn_, data_.special, forward<ArgTypes>(args)...);
  else
    return impl::invoke(*data_.functor, forward<ArgTypes>(args)...);
}

template<typename R, typename... ArgTypes>
auto function<R(ArgTypes...)>::target_type() const noexcept ->
    const type_info& {
  if (is_special())
    return (data_.special.ti ? *data_.special.ti : typeid(void));
  return data_.functor->ti;
}

template<typename R, typename... ArgTypes>
template<typename T>
auto function<R(ArgTypes...)>::target() noexcept -> T* {
  if (is_special()) {
    return (target_type() == typeid(T) ?
            reinterpret_cast<T*>(&data_.special.p) :
            nullptr);
  }
  return static_cast<T*>(data_.functor->get_impl_pointer(typeid(T)));
}

template<typename R, typename... ArgTypes>
template<typename T>
auto function<R(ArgTypes...)>::target() const noexcept -> const T* {
  if (is_special()) {
    return (target_type() == typeid(T) ?
            reinterpret_cast<const T*>(&data_.special.p) :
            nullptr);
  }
  return static_cast<T*>(data_.functor->get_impl_pointer(typeid(T)));
}

template<typename R, typename... ArgTypes>
auto function<R(ArgTypes...)>::bad_function_call_fn(
    const special_ptr_p_type&,
    impl::function_transform_to_ref_t<ArgTypes>...) -> R {
  throw bad_function_call();
}

template<typename R, typename... ArgTypes>
template<typename Functor>
auto function<R(ArgTypes...)>::simple_function_call(
    const special_ptr_p_type& f,
    impl::function_transform_to_ref_t<ArgTypes>... args) -> R {
  return impl::invoke(*reinterpret_cast<const Functor*>(&f),
                      forward<ArgTypes>(args)...);
}

template<typename R, typename... ArgTypes>
function<R(ArgTypes...)>::data_type::data_type() noexcept {
  special.ti = nullptr;
}

template<typename R, typename... ArgTypes>
auto function<R(ArgTypes...)>::data_type::assign(bool is_special, nullptr_t)
    noexcept -> fn_type {
  special_ptr sp;
  sp.ti = nullptr;
  memset(&sp.p, 0, sizeof(sp.p));
  return assign(is_special, sp, &bad_function_call_fn);
}

template<typename R, typename... ArgTypes>
auto function<R(ArgTypes...)>::data_type::assign(bool is_special,
                                                 functor_wrapper_ptr p)
    noexcept -> fn_type {
  if (is_special)
    new (&functor) functor_wrapper_ptr(move(p));
  else
    functor = move(p);
  return nullptr;
}

template<typename R, typename... ArgTypes>
auto function<R(ArgTypes...)>::data_type::assign(bool is_special,
                                                 special_ptr p, fn_type fn)
    noexcept -> fn_type {
  if (!is_special) functor.~functor_wrapper_ptr();
  special = move(p);
  return fn;
}

template<typename R, typename... ArgTypes>
template<typename F>
auto function<R(ArgTypes...)>::data_type::assign(bool is_special,
                                                 F&& f) -> fn_type {
  return assign(is_special, forward<F>(f), std::allocator<void>());
}

template<typename R, typename... ArgTypes>
template<typename F, typename Alloc>
auto function<R(ArgTypes...)>::data_type::assign(bool is_special,
                                                 F&& f, Alloc&& alloc) ->
    enable_if_t<!(is_trivially_copyable<decay_t<F>>::value &&
                  is_trivially_destructible<decay_t<F>>::value &&
                  sizeof(decay_t<F>) <= sizeof(special_ptr_p_type) &&
                  alignof(special_ptr_p_type) % alignof(F) == 0),
                fn_type> {
  using wrapper = impl::functor_wrapper_impl<
      R(impl::function_transform_to_ref_t<ArgTypes>...), remove_const_t<F>>;

  /* Result from clone is too specialized for us. */
  functor_wrapper_ptr p = functor_wrapper_t::clone(wrapper(forward<F>(f)),
                                                   forward<Alloc>(alloc));
  return assign(is_special, move(p));
}

template<typename R, typename... ArgTypes>
template<typename F, typename Alloc>
auto function<R(ArgTypes...)>::data_type::assign(bool is_special,
                                                 F&& f,
                                                 Alloc&& alloc) noexcept ->
    enable_if_t<(is_trivially_copyable<decay_t<F>>::value &&
                 is_trivially_destructible<decay_t<F>>::value &&
                 sizeof(decay_t<F>) <= sizeof(special_ptr_p_type) &&
                 alignof(special_ptr_p_type) % alignof(F) == 0),
                fn_type> {
  special_ptr sp;
  sp.ti = &typeid(f);
  memcpy(&sp.p, &f, sizeof(f));
  return assign(is_special, move(sp), &simple_function_call<decay_t<F>>);
}


_namespace_end(std)
