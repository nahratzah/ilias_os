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
auto equal_to<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) == forward<U>(y)) {
  return forward<T>(x) == forward<U>(y);
}

template<typename T, typename U>
auto not_equal_to<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) != forward<U>(y)) {
  return forward<T>(x) != forward<U>(y);
}

template<typename T, typename U>
auto greater<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) > forward<U>(y)) {
  return forward<T>(x) > forward<U>(y);
}

template<typename T, typename U>
auto less<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) < forward<U>(y)) {
  return forward<T>(x) < forward<U>(y);
}

template<typename T, typename U>
auto greater_equal<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) >= forward<U>(y)) {
  return forward<T>(x) >= forward<U>(y);
}

template<typename T, typename U>
auto less_equal<void>::operator()(T&& x, U&& y) const ->
    decltype(forward<T>(x) <= forward<U>(y)) {
  return forward<T>(x) <= forward<U>(y);
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

/* Base class, used to add the result_type member type. */
template<typename T> using _result_type =
    typename conditional_t<member_type_check_result_type<T>::value,
                           _result_type_impl<T>,
                           _no_result_type>::type;


/* Resolve placement argument. */
template<int I, typename T>
auto resolve_placement(T&& t) noexcept ->
    decltype(get<I - 1>(t)) {
  return get<I - 1>(forward<T>(t));
}


/*
 * Resolve argument based on T;
 * default is placeholder based, specializtion below gives bound argument.
 */
template<typename T,
         int Placeholder =
             is_placeholder<remove_cv_t<remove_reference_t<T>>>::value>
struct resolve_argument_t {
  template<typename T_, typename Args>
  constexpr auto operator()(T_&&, Args&& args) const noexcept ->
      decltype(resolve_placement<Placeholder>(forward<Args>(args))) {
    return resolve_placement<Placeholder>(forward<Args>(args));
  }
};

struct resolve_bind_expression {
  template<typename T_, typename Args,
           size_t... Indices,
           index_sequence<Indices...> =
               make_index_sequence<tuple_size<Args>::value>()>
  auto operator()(T_&& expr, Args&& args) const
      noexcept(noexcept(invoke(forward<T_>(expr),
                               get<Indices>(forward<Args>(args))...))) ->
      decltype(invoke(forward<T_>(expr),
                      get<Indices>(forward<Args>(args))...)) {
    return invoke(forward<T_>(expr),
                  get<Indices>(forward<Args>(args))...);
  }
};

struct resolve_bound_argument {
  template<typename T_, typename Args>
  constexpr T_& operator()(reference_wrapper<T_> rw, Args&&) const noexcept {
    return rw.get();
  }

  template<typename T_, typename Args>
  constexpr auto operator()(T_&& v, Args&&) const noexcept ->
      decltype(forward<T_>(v)) {
    return forward<T_>(v);
  }
};

template<typename T>
struct resolve_argument_t<T, 0>
: conditional_t<is_bind_expression<remove_cv_t<remove_reference_t<T>>>::value,
                resolve_bind_expression,
                resolve_bound_argument>
{};

template<typename T, typename Args>
auto resolve_argument(T&& v, Args&& args)
    noexcept(noexcept(resolve_argument_t<remove_cv_t<remove_reference_t<T>>>()(
          forward<T>(v), forward<Args>(args)))) ->
    decltype(resolve_argument_t<remove_cv_t<remove_reference_t<T>>>()(
          forward<T>(v), forward<Args>(args))) {
  return resolve_argument_t<remove_cv_t<remove_reference_t<T>>>()(
      forward<T>(v), forward<Args>(args));
}

/*
 * Invoke tuple of f, bound_args... using tuple of args...
 */
template<typename FTuple, typename ArgTuple,
         size_t Index0,
         size_t... Indices,
         index_sequence<Index0, Indices...> =
             make_index_sequence<tuple_size<ArgTuple>::value>()>
auto invoke_tuple(FTuple&& ft, ArgTuple&& args)
    noexcept(noexcept(invoke(get<0>(forward<FTuple>(ft)),
                      resolve_argument(get<Indices>(forward<FTuple>(ft)),
                                       forward<ArgTuple>(args))...))) ->
    decltype(invoke(get<0>(forward<FTuple>(ft)),
             resolve_argument(get<Indices>(forward<FTuple>(ft)),
                              forward<ArgTuple>(args))...)) {
  static_assert(Index0 == 0,
                "Uh oh, there's something very wrong with "
                "the index_sequence generation or its use here...");
  return invoke(get<0>(forward<FTuple>(ft)),
                resolve_argument(get<Indices>(forward<FTuple>(ft)),
                                 forward<ArgTuple>(args))...);
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

  expression& operator=(expression&& other)
      noexcept(is_nothrow_move_assignable<data_type>::value) {
    data_ = move(other.data_);
    return *this;
  }

  template<typename... Args>
  auto operator()(Args&&... args)
      noexcept(noexcept(invoke_tuple(
          this->data_, forward_as_tuple(forward<Args>(args)...)))) ->
      decltype(invoke_tuple(this->data_,
                            forward_as_tuple(forward<Args>(args)...))) {
    return invoke_tuple(data_, forward_as_tuple(forward<Args>(args)...));
  }

  template<typename... Args>
  auto operator()(Args&&... args) const
      noexcept(noexcept(invoke_tuple(
          this->data_, forward_as_tuple(forward<Args>(args)...)))) ->
      decltype(invoke_tuple(this->data_,
                            forward_as_tuple(forward<Args>(args)...))) {
    return invoke_tuple(data_, forward_as_tuple(forward<Args>(args)...));
  }

  template<typename... Args>
  auto operator()(Args&&... args) volatile
      noexcept(noexcept(invoke_tuple(
          this->data_, forward_as_tuple(forward<Args>(args)...)))) ->
      decltype(invoke_tuple(this->data_,
                            forward_as_tuple(forward<Args>(args)...))) {
    return invoke_tuple(data_, forward_as_tuple(forward<Args>(args)...));
  }

  template<typename... Args>
  auto operator()(Args&&... args) const volatile
      noexcept(noexcept(invoke_tuple(
          this->data_, forward_as_tuple(forward<Args>(args)...)))) ->
      decltype(invoke_tuple(this->data_,
                            forward_as_tuple(forward<Args>(args)...))) {
    return invoke_tuple(data_, forward_as_tuple(forward<Args>(args)...));
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

  expression_r& operator=(expression_r&& other)
      noexcept(is_nothrow_move_assignable<impl_type>::value) {
    impl_ = move(other.impl_);
    return *this;
  }

  template<typename... Args>
  auto operator()(Args&&... args)
      noexcept(noexcept(invoke(this->impl_, forward<Args>(args)...))) ->
      result_type {
    return invoke(this->impl_, forward<Args>(args)...);
  }

  template<typename... Args>
  auto operator()(Args&&... args) const
      noexcept(noexcept(invoke(this->impl_, forward<Args>(args)...))) ->
      result_type {
    return invoke(this->impl_, forward<Args>(args)...);
  }

  template<typename... Args>
  auto operator()(Args&&... args) volatile
      noexcept(noexcept(invoke(this->impl_, forward<Args>(args)...))) ->
      result_type {
    return invoke(this->impl_, forward<Args>(args)...);
  }

  template<typename... Args>
  auto operator()(Args&&... args) const volatile
      noexcept(noexcept(invoke(this->impl_, forward<Args>(args)...))) ->
      result_type {
    return invoke(this->impl_, forward<Args>(args)...);
  }

 private:
  impl_type impl_;
};


} /* namespace std::_bind */


template<typename F, typename... BoundArgs>
auto bind(F&& f, BoundArgs&&... bound_args)
    noexcept(noexcept(_bind::expression<decay_t<F>, decay_t<BoundArgs>...>(
        forward<F>(f), forward<BoundArgs>(bound_args)...))) ->
    _bind::expression<decay_t<F>, decay_t<BoundArgs>...> {
  return _bind::expression<decay_t<F>, decay_t<BoundArgs>...>(
      forward<F>(f), forward<BoundArgs>(bound_args)...);
}

template<typename R, typename F, typename... BoundArgs>
auto bind(F&& f, BoundArgs&&... bound_args)
    noexcept(noexcept(
        _bind::expression_r<R, decay_t<F>, decay_t<BoundArgs>...>(
            forward<F>(f), forward<BoundArgs>(bound_args)...))) ->
    _bind::expression_r<R, decay_t<F>, decay_t<BoundArgs>...> {
  return _bind::expression_r<R, decay_t<F>, decay_t<BoundArgs>...>(
      forward<F>(f), forward<BoundArgs>(bound_args)...);
}


_namespace_end(std)
