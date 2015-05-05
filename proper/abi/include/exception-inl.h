#ifndef _EXCEPTION_INL_H_
#define _EXCEPTION_INL_H_

#include <exception>

_namespace_begin(std)


constexpr exception_ptr::exception_ptr(nullptr_t) noexcept {}

inline exception_ptr::exception_ptr(exception_ptr&& o) noexcept
: exception_ptr()
{
  swap(*this, o);
}

inline exception_ptr::~exception_ptr() noexcept {
  if (ptr_) reset();
}

inline exception_ptr& exception_ptr::operator=(exception_ptr o) noexcept {
  swap(*this, o);
  return *this;
}

inline exception_ptr& exception_ptr::operator=(nullptr_t) noexcept {
  if (ptr_) reset();
  return *this;
}

inline void swap(exception_ptr& a, exception_ptr& b) noexcept {
  void* p_ = a.ptr_;
  a.ptr_ = b.ptr_;
  b.ptr_ = p_;
}

inline exception_ptr::operator bool() const noexcept {
  return bool(ptr_);
}

inline bool exception_ptr::operator==(const exception_ptr& o) const noexcept {
  return ptr_ == o.ptr_;
}

inline bool exception_ptr::operator!=(const exception_ptr& o) const noexcept {
  return !(*this == o);
}


template<typename T> exception_ptr make_exception_ptr(T e) noexcept {
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

    combined(T&& t) : T(forward<T>(t)) {}
  };

 public:
  combined create(T&& t) {
    return combined(forward<T>(t));
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

  throw impl::create(forward<T>(t));
}

template<typename E> void rethrow_if_nested(const E& e) {
  const nested_exception* ne = dynamic_cast<const nested_exception*>(&e);
  if (ne) ne->rethrow_nested();
}


_namespace_end(std)

#endif /* _EXCEPTION_INL_H_ */
