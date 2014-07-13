namespace __cxxabiv1 {


inline ref_cstr::ref_cstr(const ref_cstr& o) noexcept
: head_(o.head_)
{
  if (head_) head_->fetch_add(1U, _namespace(std)::memory_order_acquire);
}

inline ref_cstr::ref_cstr(ref_cstr&& o) noexcept
: head_(o.head_)
{
  o.head_ = nullptr;
}

inline ref_cstr::~ref_cstr() noexcept {
  if (head_) reset();
}

inline ref_cstr& ref_cstr::operator=(ref_cstr o) noexcept {
  swap(o);
  return *this;
}

inline ref_cstr::operator const char*() const noexcept {
  return (head_ ? reinterpret_cast<const char*>(head_ + 1) : nullptr);
}

inline void ref_cstr::swap(ref_cstr& o) noexcept {
  using _namespace(std)::swap;

  swap(head_, o.head_);
}

inline ref_cstr::operator bool() const noexcept {
  return head_ != nullptr;
}

inline void swap(ref_cstr& a, ref_cstr& b) noexcept {
  a.swap(b);
}


} /* namespace __cxxabiv1 */
