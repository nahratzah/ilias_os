namespace __cxxabiv1 {
namespace ext {


inline c_string_ptr::~c_string_ptr() noexcept { if (data_) std::free(data_); }
inline c_string_ptr::c_string_ptr(c_string_ptr&& other) noexcept { swap(other); }

inline c_string_ptr::size_type c_string_ptr::size() const noexcept { return len_; }
inline c_string_ptr::size_type c_string_ptr::length() const noexcept { return size(); }
inline bool c_string_ptr::empty() const noexcept { return size() == 0U; }

inline char* c_string_ptr::release() noexcept {
  char* result = c_str();
  len_ = 0U;
  capacity_ = 0U;
  data_ = nullptr;
  return result;
}

inline char* c_string_ptr::c_str() const noexcept {
  assert(capacity_ == 0U || len_ < capacity_);
  if (data_) data_[len_] = '\0';
  return data_;
}

inline char* c_string_ptr::data() const noexcept {
  return data_;
}

inline void c_string_ptr::swap(c_string_ptr& other) noexcept {
  using std::swap;

  swap(data_, other.data_);
  swap(len_, other.len_);
  swap(capacity_, other.capacity_);
}

inline void swap(c_string_ptr& a, c_string_ptr& b) noexcept { a.swap(b); }

inline bool c_string_ptr::operator==(const c_string_ptr& other) const noexcept {
  if (empty() || size() != other.size()) return size() == other.size();
  return std::memcmp(data(), other.data(), size()) == 0;
}

inline bool c_string_ptr::operator==(const char* s) const noexcept {
  return strcmp(c_str(), s) == 0;
}

inline bool c_string_ptr::operator!=(const c_string_ptr& other) const noexcept {
  return !(*this == other);
}

inline bool c_string_ptr::operator!=(const char* s) const noexcept {
  return !(*this == s);
}

inline c_string_ptr& c_string_ptr::operator=(c_string_ptr&& other) noexcept {
  if (data_) std::free(data_);
  data_ = other.data_;
  len_ = other.len_;
  capacity_ = other.capacity_;
  other.data_ = nullptr;
  other.len_ = 0U;
  other.capacity_ = 0U;
  return *this;
}

inline c_string_ptr& c_string_ptr::operator+=(const c_string_ptr& other) {
  append(other);
  return *this;
}
inline c_string_ptr& c_string_ptr::operator+=(const char* s) {
  append(s);
  return *this;
}

inline void c_string_ptr::append(const c_string_ptr& other) {
  if (!other.empty()) append(other.data_, other.len_);
}
inline void c_string_ptr::append(const char* s) {
  append(s, strlen(s));
}

inline void c_string_ptr::clear() noexcept {
  len_ = 0U;
}

inline c_string_ptr::iterator c_string_ptr::begin() {
  return data();
}
inline c_string_ptr::iterator c_string_ptr::end() {
  return data() + size();
}
inline c_string_ptr::const_iterator c_string_ptr::begin() const {
  return data();
}
inline c_string_ptr::const_iterator c_string_ptr::end() const {
  return data() + size();
}
inline c_string_ptr::const_iterator c_string_ptr::cbegin() const {
  return data();
}
inline c_string_ptr::const_iterator c_string_ptr::cend() const {
  return data() + size();
}
inline char& c_string_ptr::operator[](size_type idx) {
  assert(idx < size());
  return data()[idx];
}
inline const char& c_string_ptr::operator[](size_type idx) const {
  assert(idx < size());
  return data()[idx];
}

inline c_string_ptr make_c_string_ptr(const char* s) {
  c_string_ptr result;
  result.append(s, strlen(s));
  return result;
}

inline c_string_ptr make_c_string_ptr(const char* s, c_string_ptr::size_type len) {
  c_string_ptr result;
  result.append(s, len);
  return result;
}

inline c_string_ptr make_c_string_ptr_buffer(char* buf, c_string_ptr::size_type len) noexcept {
  c_string_ptr result;
  if (buf) {
    assert(result.data_ == nullptr);
    result.data_ = buf;
    result.capacity_ = len;
  }
  return result;
}

inline c_string_ptr make_c_string_ptr_buffer(c_string_ptr::size_type len) {
  void* buf = nullptr;
  if (len > 0 && (buf = std::malloc(len)) == nullptr) throw std::bad_alloc();
  return make_c_string_ptr_buffer(static_cast<char*>(buf), len);
}

inline bool operator==(const char* a, const c_string_ptr& b) noexcept {
  return b == a;
}

inline bool operator!=(const char* a, const c_string_ptr& b) noexcept {
  return b != a;
}


}} /* namespace __cxxabiv1::ext */
