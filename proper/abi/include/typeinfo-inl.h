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


_namespace_end(std)
