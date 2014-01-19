namespace __cxxabiv1 {


inline ref_cstr::ref_cstr(const ref_cstr& o) noexcept
: head_(o)
{
  if (head_) head_->fetch_add(1U, std::memory_order_acquire);
}

inline ref_cstr::ref_cstr(ref_cstr& o) noexcept
: head_(o.head_)
{
  o.head_ = nullptr;
}

inline ref_cstr::~ref_cstr() noexcept {
  if (head_ && head_->fetch_sub(1U, std::memory_order_release) == 1U)
    free(head_);
}

inline ref_cstr::ref_cstr(const char* s) noexcept
: ref_cstr()
{
  size_t len = 0;
  for (const char* i = s; *i; ++i, ++len);
  head_ = static_cast<atom_t*>(malloc(len + sizeof(head_) + 1U,
                                      "abi/ref_cstr"));
  if (!head_) throw std::bad_alloc();

  atomic_init(&head_, 1U);
  for (char* hs = reinterpret_cast<char*>(h + 1); *s; ++hs, ++s) *hs = *s;
  *hs = '\0';
}

inline ref_cstr& ref_cstr::operator=(ref_cstr o) noexcept {
  swap(o);
  return *this;
}

inline ref_cstr::operator const char*() const noexcept {
  return (h ? reinterpret_cast<const char*>(h + 1) : nullptr);
}

inline void ref_cstr::swap(ref_cstr& o) noexcept {
  using std::swap;

  swap(head_, o.head_);
}

inline ref_cstr::operator bool() const noexcept {
  return head_ != nullptr;
}

inline void swap(ref_cstr& a, ref_cstr& b) noexcept {
  a.swap(b);
}


} /* namespace __cxxabiv1 */
