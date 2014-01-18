namespace __cxxabiv1 {


inline constexpr __pbase_type_info::__pbase_type_info(
  const char* type_name, unsigned int flags, const std::type_info& pti)
: std::type_info(type_name),
  __flags(flags),
  __pointee(&pti)
{}

inline constexpr __pointer_to_member_type_info::__pointer_to_member_type_info(
  const char* type_name, unsigned int flags, const std::type_info& pti,
  const __class_type_info& ctx)
: __pbase_type_info(type_name, flags, pti),
  __context(&ctx)
{}


inline __base_class_type_info::get_offset(const __base_class_type_info& bcti) noexcept {
  return bcti.__offset_flags >> __offset_shift;
}
inline __base_class_type_info::get_flags(const __base_class_type_info& bcti) noexcept {
  return bcti.__offset_flags & ((long(1) << __offset_shift) - long(1));
}


} /* namespace __cxxabiv1 */
