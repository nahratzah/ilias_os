namespace __cxxabiv1 {


inline __pbase_type_info::__pbase_type_info(const char* type_name,
                                            unsigned int flags,
                                            const std::type_info& pti) noexcept
: std::type_info(type_name),
  __flags(flags),
  __pointee(&pti)
{}

inline __pointer_to_member_type_info::__pointer_to_member_type_info(
  const char* type_name, unsigned int flags, const std::type_info& pti,
  const __class_type_info& ctx) noexcept
: __pbase_type_info(type_name, flags, pti),
  __context(&ctx)
{}


inline long __base_class_type_info::get_offset(
    const __base_class_type_info& bcti) noexcept {
  return bcti.__offset_flags >> __offset_shift;
}
inline long __base_class_type_info::get_flags(
    const __base_class_type_info& bcti) noexcept {
  return bcti.__offset_flags & ((1L << __offset_shift) - 1L);
}


} /* namespace __cxxabiv1 */
