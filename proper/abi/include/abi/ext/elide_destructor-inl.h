namespace __cxxabiv1 {
namespace ext {


template<typename T>
template<typename... Args>
elide_destructor<T>::data_t::data_t(Args&&... args)
  noexcept(_namespace(std)::is_nothrow_constructible<
               element_type, Args...>::value) {
  using _namespace(std)::addressof;
  using _namespace(std)::forward;

  new (addressof(v)) element_type(forward<Args>(args)...);
}

template<typename T>
elide_destructor<T>::elide_destructor()
  noexcept(_namespace(std)::is_nothrow_default_constructible<data_t>::value)
{}

template<typename T>
template<typename... Args>
elide_destructor<T>::elide_destructor(Args&&... args)
noexcept(_namespace(std)::is_nothrow_constructible<
             element_type, Args...>::value)
: data_t(_namespace(std)::forward<Args>(args)...)
{}

template<typename T>
auto elide_destructor<T>::operator->() noexcept -> pointer {
  using _namespace(std)::addressof;

  return addressof(data_.v);
}

template<typename T>
auto elide_destructor<T>::operator->() const noexcept -> const_pointer {
  using _namespace(std)::addressof;

  return addressof(data_.v);
}

template<typename T>
auto elide_destructor<T>::operator*() noexcept -> reference {
  return data_.v;
}

template<typename T>
auto elide_destructor<T>::operator*() const noexcept -> const_reference {
  return data_.v;
}

template<typename T>
elide_destructor<T>::operator bool() const noexcept {
  return true;
}

template<typename T>
auto elide_destructor<T>::operator!() const noexcept -> bool {
  return false;
}


}} /* namespace __cxxabiv1::ext */
