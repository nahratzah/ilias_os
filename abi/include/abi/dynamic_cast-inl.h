namespace __cxxabiv1 {


constexpr __dyn_cast_response::__dyn_cast_response(
    const void* target, __has_base_result subj_visited)
: target_(target),
  subj_visited_(subj_visited)
{}

inline bool __dyn_cast_response::return_now(
    const __dyn_cast_request& req, const void* v) const noexcept {
  if (!target_) return false;
  if (target_ == v) return true;  // Type cannot inherit itself.

  /*
   * Stop the search if:
   * - target located and subject is a non-virtual base class of target
   *   (we'll never find another target having subject as a base, due to
   *   subject being a non-virtual base)
   * - multiple targets located with subject as base (virtual inheritance)
   *   (we'll never find anything that inherits from subject, but is better
   *   than what we've found)
   * - multiple targets located and subject is not a base of target-type
   *   (compiler guarantees cross cast, which has been proven to be ambiguous).
   */
  if (subj_visited_ == __has_base_non_virtual) return true;
  if (subj_visited_ == __has_base_virtual && ambiguous_) return true;
  if (ambiguous_ && req.is_cross_cast_) return true;

  /* Default: we are not sure, therefore continue search. */
  return false;
}

inline const void* __dyn_cast_response::resolution() const noexcept {
  return (ambiguous_ ? nullptr : target_);
}


} /* namespace __cxxabiv1 */
