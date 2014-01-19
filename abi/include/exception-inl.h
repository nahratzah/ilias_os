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


}
