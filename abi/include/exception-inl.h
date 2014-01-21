namespace std {


inline exception_ptr::operator bool() const noexcept {
  return bool(ptr_);
}

inline bool exception_ptr::operator==(const exception_ptr& o) const noexcept {
  return ptr_ == o.ptr_;
}

inline bool exception_ptr::operator!=(const exception_ptr& o) const noexcept {
  return !(*this == o);
}


template<typename T> exception_ptr make_exception(T e) noexcept {
  try {
    throw e;
  } catch (const T& t) {
    return current_exception();
  }
}


inline exception_ptr nested_exception::nested_ptr() const noexcept {
  return nested_;
}


namespace {


template<typename T, bool Add> struct add_nested;

template<typename T> struct add_nested<T, true> {
 private:
  class combined : public T, public nested_exception {
   public:
    combined() = default;
    combined(const combined&) = default;
    combined& operator=(const combined&) = default;
    ~combined() = default;

    combined(T&& t) : T(std::forward<T>(t)) {}
  };

 public:
  combined create(T&& t) {
    return combined(std::forward<T>(t));
  }
};

template<typename T> struct add_nested<T, false> {
  T create(T&& t) {
    return t;
  }
};


} /* namespace std::<unnamed> */


template<typename T> void throw_with_nested(T&& t) {
  using U = typename remove_reference<T>::type;
  using impl = add_nested<U, is_base_of<nested_exception, U>::value>;

  throw impl::create(std::forward<T>(t));
}

template<typename E> void rethrow_if_nested(const E& e) {
  const nested_exception* ne = dynamic_cast<const nested_exception*>(&e);
  if (ne) ne->rethrow_nested();
}


} /* namespace std */
