namespace std {


inline constexpr type_info::type_info(const char* type_name)
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
  _TYPES(uintptr_t) v = reinterpret_cast<_TYPES(uintptr_t)>(name());
  return v | (v >> 8) | (v >> 16) | (v >> 24);
}


} /* namespace std */
