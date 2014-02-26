_namespace_begin(std)


inline type_info::type_info(const char* type_name)
: __type_name(type_name)
{}

inline bool type_info::operator==(const type_info& o) const noexcept {
  return __type_name == o.__type_name;
}

inline bool type_info::operator!=(const type_info& o) const noexcept {
  return __type_name != o.__type_name;
}

inline bool type_info::before(const type_info& o) const noexcept {
  return __type_name < o.__type_name;
}

inline const char* type_info::name() const noexcept {
  return __type_name;
}

inline _TYPES(size_t) type_info::hash_code() const noexcept {
  const void* name_ptr = name();
  return abi::hash_code(name_ptr);
}


_namespace_end(std)
