#pragma GCC system_header

_namespace_begin(std)


inline constexpr auto char_traits<char>::eq(char_type a, char_type b)
    noexcept -> bool {
  unsigned char a_ = a;
  unsigned char b_ = b;
  return a_ == b_;
}

inline constexpr auto char_traits<char>::lt(char_type a, char_type b)
    noexcept -> bool {
  unsigned char a_ = a;
  unsigned char b_ = b;
  return a_ < b_;
}

inline auto char_traits<char>::length(const char_type* s) noexcept -> size_t {
  return strlen(s);
}

inline auto char_traits<char>::assign(char_type& a, const char_type& b)
    noexcept -> void {
  a = b;
}

inline auto char_traits<char>::assign(char_type* s, size_t n, char_type c)
    noexcept -> char_type* {
  return static_cast<char_type*>(memset(s, c, n));
}

inline auto char_traits<char>::compare(const char_type* a, const char_type* b,
                                size_t n) noexcept -> int {
  return memcmp(a, b, n);
}

inline auto char_traits<char>::find(const char_type* s, size_t n,
                                    const char_type& c)
    noexcept -> const char_type* {
  return static_cast<const char_type*>(memchr(s, c, n));
}

inline auto char_traits<char>::move(char_type* d, const char_type* s, size_t n)
    noexcept -> char_type* {
  return static_cast<char_type*>(memmove(d, s, n));
}

inline auto char_traits<char>::copy(char_type* d, const char_type* s, size_t n)
    noexcept -> char_type* {
  return static_cast<char_type*>(memcpy(d, s, n));
}

inline constexpr auto char_traits<char>::eof() noexcept -> int_type {
  return EOF;
}

inline constexpr auto char_traits<char>::not_eof(int_type c) noexcept ->
    int_type {
  return (eq_int_type(c, eof()) ? 0 : c);
}

inline constexpr auto char_traits<char>::to_char_type(int_type c) noexcept ->
    char_type {
  return char_type(c);
}

inline constexpr auto char_traits<char>::to_int_type(char_type c) noexcept ->
    int_type {
  return c;
}

inline constexpr auto char_traits<char>::eq_int_type(int_type a, int_type b)
    noexcept -> bool {
  return a == b;
}

inline auto char_traits<char>::rfind(const char_type* s, size_t n,
                                     const char_type& c)
    noexcept -> const char_type* {
  return static_cast<const char_type*>(memrchr(s, c, n));
}





inline constexpr auto char_traits<wchar_t>::eq(char_type a, char_type b)
    noexcept -> bool {
  return a == b;
}

inline constexpr auto char_traits<wchar_t>::lt(char_type a, char_type b)
    noexcept -> bool {
  return a < b;
}

inline auto char_traits<wchar_t>::length(const wchar_t* s) noexcept -> size_t {
  return wcslen(s);
}

inline auto char_traits<wchar_t>::assign(char_type& a, const char_type& b)
    noexcept -> void {
  a = b;
}

inline auto char_traits<wchar_t>::assign(char_type* s, size_t n, char_type c)
    noexcept -> char_type* {
  return wmemset(s, c, n);
}

inline auto char_traits<wchar_t>::compare(const char_type* a,
                                          const char_type* b,
                                          size_t n) noexcept -> int {
  return wmemcmp(a, b, n);
}

inline auto char_traits<wchar_t>::find(const char_type* s, size_t n,
                                       const char_type& c)
    noexcept -> const char_type* {
  return wmemchr(s, c, n);
}

inline auto char_traits<wchar_t>::move(char_type* d, const char_type* s,
                                       size_t n) noexcept -> char_type* {
  return wmemmove(d, s, n);
}

inline auto char_traits<wchar_t>::copy(char_type* d, const char_type* s,
                                       size_t n) noexcept -> char_type* {
  return wmemcpy(d, s, n);
}

inline constexpr auto char_traits<wchar_t>::eof() noexcept -> int_type {
  return WEOF;
}

inline constexpr auto char_traits<wchar_t>::not_eof(int_type c) noexcept ->
    int_type {
  return (eq_int_type(c, eof()) ? 0 : c);
}

inline constexpr auto char_traits<wchar_t>::to_char_type(int_type c)
    noexcept -> char_type {
  return char_type(c);
}

inline constexpr auto char_traits<wchar_t>::to_int_type(char_type c)
    noexcept -> int_type {
  return c;
}

inline constexpr auto char_traits<wchar_t>::eq_int_type(int_type a, int_type b)
    noexcept -> bool {
  return a == b;
}

inline auto char_traits<wchar_t>::rfind(const char_type* s, size_t n,
                                        const char_type& c)
    noexcept -> const char_type* {
  return wmemrchr(s, c, n);
}





inline constexpr auto char_traits<char16_t>::eq(char_type a, char_type b)
    noexcept -> bool {
  return a == b;
}

inline constexpr auto char_traits<char16_t>::lt(char_type a, char_type b)
    noexcept -> bool {
  return a < b;
}

inline auto char_traits<char16_t>::assign(char_type& a, const char_type& b)
    noexcept -> void {
  a = b;
}

inline auto char_traits<char16_t>::assign(char_type* s, size_t n, char_type c)
    noexcept -> char_type* {
  char_type* rv = s;
  while (n-- > 0) *s++ = c;
  return rv;
}

inline auto char_traits<char16_t>::move(char_type* d, const char_type* s,
                                        size_t n) noexcept -> char_type* {
  return static_cast<char_type*>(memmove(d, s, sizeof(char_type) * n));
}

inline auto char_traits<char16_t>::copy(char_type* d, const char_type* s,
                                        size_t n) noexcept -> char_type* {
  return static_cast<char_type*>(memcpy(d, s, sizeof(char_type) * n));
}

inline constexpr auto char_traits<char16_t>::eof() noexcept -> int_type {
  return char_type(-1);
}

inline constexpr auto char_traits<char16_t>::not_eof(int_type c) noexcept ->
    int_type {
  return (eq_int_type(c, eof()) ? 0 : c);
}

inline constexpr auto char_traits<char16_t>::to_char_type(int_type c)
    noexcept -> char_type {
  return char_type(c);
}

inline constexpr auto char_traits<char16_t>::to_int_type(char_type c)
    noexcept -> int_type {
  return c;
}

inline constexpr auto char_traits<char16_t>::eq_int_type(
    int_type a, int_type b) noexcept -> bool {
  return a == b;
}





inline constexpr auto char_traits<char32_t>::eq(char_type a, char_type b)
    noexcept -> bool {
  return a == b;
}

inline constexpr auto char_traits<char32_t>::lt(char_type a, char_type b)
    noexcept -> bool {
  return a < b;
}

inline auto char_traits<char32_t>::assign(char_type& a, const char_type& b)
    noexcept -> void {
  a = b;
}

inline auto char_traits<char32_t>::assign(char_type* s, size_t n, char_type c)
    noexcept -> char_type* {
  char_type* rv = s;
  while (n-- > 0) *s++ = c;
  return rv;
}

inline auto char_traits<char32_t>::move(char_type* d, const char_type* s,
                                        size_t n) noexcept -> char_type* {
  return static_cast<char_type*>(memmove(d, s, sizeof(char_type) * n));
}

inline auto char_traits<char32_t>::copy(char_type* d, const char_type* s,
                                        size_t n) noexcept -> char_type* {
  return static_cast<char_type*>(memcpy(d, s, sizeof(char_type) * n));
}

inline constexpr auto char_traits<char32_t>::eof() noexcept -> int_type {
  return char_type(-1);
}

inline constexpr auto char_traits<char32_t>::not_eof(int_type c) noexcept ->
    int_type {
  return (eq_int_type(c, eof()) ? 0 : c);
}

inline constexpr auto char_traits<char32_t>::to_char_type(int_type c)
    noexcept -> char_type {
  return char_type(c);
}

inline constexpr auto char_traits<char32_t>::to_int_type(char_type c)
    noexcept -> int_type {
  return c;
}

inline constexpr auto char_traits<char32_t>::eq_int_type(
    int_type a, int_type b) noexcept -> bool {
  return a == b;
}







template<typename Char, typename Traits, typename Alloc>
basic_string<Char, Traits, Alloc>::basic_string(const allocator_type& alloc)
: impl::alloc_base<Alloc>(alloc)
{
  data_.immed[0] = 0;
}

template<typename Char, typename Traits, typename Alloc>
basic_string<Char, Traits, Alloc>::basic_string(const basic_string& s)
: basic_string(s, s.get_allocator())
{}

template<typename Char, typename Traits, typename Alloc>
basic_string<Char, Traits, Alloc>::basic_string(const basic_string& s,
                                                const allocator_type& alloc)
: basic_string(basic_string_ref<Char, Traits>(s), alloc)
{}

template<typename Char, typename Traits, typename Alloc>
basic_string<Char, Traits, Alloc>::basic_string(const basic_string& s,
                                                size_type pos, size_type len,
                                                const allocator_type& alloc)
: basic_string(basic_string_ref<Char, Traits>(s).substr(pos, len), alloc)
{}

template<typename Char, typename Traits, typename Alloc>
basic_string<Char, Traits, Alloc>::basic_string(const char_type* s,
                                                const allocator_type& alloc)
: basic_string(basic_string_ref<Char, Traits>(s), alloc)
{}

template<typename Char, typename Traits, typename Alloc>
basic_string<Char, Traits, Alloc>::basic_string(const char_type* s,
                                                size_type len,
                                                const allocator_type& alloc)
: basic_string(basic_string_ref<Char, Traits>(s, len), alloc)
{}

template<typename Char, typename Traits, typename Alloc>
basic_string<Char, Traits, Alloc>::basic_string(size_type n, char_type c,
                                                const allocator_type& alloc)
: basic_string(alloc)
{
  reserve(n);
  len_ = n;
  traits_type::assign(begin(), n, c);
}

template<typename Char, typename Traits, typename Alloc>
template<typename InputIter>
basic_string<Char, Traits, Alloc>::basic_string(InputIter b, InputIter e,
                                                const allocator_type& alloc)
: basic_string(alloc)
{
  while (b != e) {
    push_back(*b);
    ++b;
  }
}

template<typename Char, typename Traits, typename Alloc>
basic_string<Char, Traits, Alloc>::basic_string(initializer_list<char_type> il,
                                                const allocator_type& alloc)
: basic_string(basic_string_ref<Char, Traits>(il.begin(), il.size()), alloc)
{}

template<typename Char, typename Traits, typename Alloc>
basic_string<Char, Traits, Alloc>::basic_string(basic_string&& s) noexcept
: basic_string(s.get_allocator())
{
  swap(s);
}

template<typename Char, typename Traits, typename Alloc>
basic_string<Char, Traits, Alloc>::basic_string(basic_string&& s,
                                                const allocator_type& alloc)
: basic_string(alloc)
{
  // XXX
  swap(s);
}

template<typename Char, typename Traits, typename Alloc>
basic_string<Char, Traits, Alloc>::basic_string(
    basic_string_ref<Char, Traits> s, const allocator_type& alloc)
: basic_string()
{
  reserve(s.size());
  len_ = s.size();
  traits_type::copy(begin(), s.data(), s.size());
}

template<typename Char, typename Traits, typename Alloc>
basic_string<Char, Traits, Alloc>::~basic_string() noexcept {
  if (avail_ > immed_size)
    this->deallocate_dfl(data_.s, avail_);
}

template<typename Char, typename Traits, typename Alloc>
basic_string<Char, Traits, Alloc>::operator basic_string_ref<Char, Traits>()
    const noexcept {
  return basic_string_ref<Char, Traits>(data(), size());
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::operator=(const basic_string& s) ->
    basic_string& {
  return *this = basic_string_ref<Char, Traits>(s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::operator=(const char_type* s) ->
    basic_string& {
  return *this = basic_string_ref<Char, Traits>(s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::operator=(char_type c) ->
    basic_string& {
  reserve(1U);
  traits_type::assign((*this)[0], c);
  len_ = 1U;
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::operator=(
    initializer_list<char_type> il) -> basic_string& {
  return *this = basic_string(il, this->get_allocator());
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::operator=(basic_string&& s) noexcept ->
    basic_string& {
  swap(s);
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::operator=(
    basic_string_ref<Char, Traits> s) -> basic_string& {
  reserve(s.size());
  len_ = s.size();
  traits_type::copy(begin(), s.data(), s.size());
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::begin() noexcept -> iterator {
  return (avail_ > immed_size ? data_.s : &data_.immed[0]);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::end() noexcept -> iterator {
  return begin() + size();
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::begin() const noexcept ->
    const_iterator {
  return (avail_ > immed_size ? data_.s : &data_.immed[0]);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::end() const noexcept ->
    const_iterator {
  return begin() + size();
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::cbegin() const noexcept ->
    const_iterator {
  return begin();
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::cend() const noexcept ->
    const_iterator {
  return end();
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::rbegin() noexcept -> reverse_iterator {
  return reverse_iterator(end());
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::rend() noexcept -> reverse_iterator {
  return reverse_iterator(begin());
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::rbegin() const noexcept ->
    const_reverse_iterator {
  return const_reverse_iterator(end());
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::rend() const noexcept ->
    const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::crbegin() const noexcept ->
    const_reverse_iterator {
  return rbegin();
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::crend() const noexcept ->
    const_reverse_iterator {
  return rend();
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::size() const noexcept -> size_type {
  return len_;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::length() const noexcept -> size_type {
  return size();
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::max_size() const noexcept ->
    size_type {
  /* Subtract 1, since we need to keep a nul character at the end. */
  return allocator_traits<allocator_type>::max_size(this->get_allocator_()) -
         1U;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::resize(size_type sz) -> void {
  resize(sz, char_type());
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::resize(size_type sz, char_type c) ->
    void {
  if (len_ >= sz) {
    len_ = sz;
    return;
  }

  reserve(sz);
  traits_type::assign(begin() + len_, sz - len_, c);
  len_ = sz;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::capacity() const noexcept ->
    size_type {
  return avail_ - 1U;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::reserve(size_type sz) -> void {
  if (capacity() <= sz) {
    pointer s = this->allocate_dfl(sz + 1U, this);

    traits_type::copy(s, data(), size());
    if (avail_ > immed_size) this->deallocate_dfl(data_.s, avail_);
    data_.s = s;
    avail_ = sz + 1U;
  }
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::clear() noexcept -> void {
  len_ = 0U;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::empty() const noexcept -> bool {
  return len_ > 0U;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::shrink_to_fit() -> void {
  if (avail_ <= immed_size) return;

  if (size() < immed_size) {
    /* Switch over to storing string in immed. */
    const auto avail = avail_;
    const data_t tmp = data_;

    avail_ = immed_size;
    traits_type::copy(begin(), tmp.s, size());
    this->deallocate_dfl(tmp.s, avail);
  } else if (avail_ > size() + 1U) {
    /* Reduce allocated space to minimum. */
    pointer s;
    try {
      s = this->allocate_dfl(size() + 1U, this);
    } catch (const std::bad_alloc&) {
      return;
    }

    traits_type::copy(s, data(), size());
    this->deallocate_dfl(data_.s, avail_);
    avail_ = size() + 1U;
    data_.s = s;
  }
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::operator[](size_type i) noexcept ->
    reference {
  return begin()[i];
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::operator[](size_type i)
    const noexcept -> const_reference {
  return begin()[i];
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::at(size_type i) -> reference {
  if (i < 0 || i >= len_) throw out_of_range("basic_string::at");
  return (*this)[i];
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::at(size_type i) const ->
    const_reference {
  if (i < 0 || i >= len_) throw out_of_range("basic_string::at");
  return (*this)[i];
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::back() noexcept -> reference {
  return *rbegin();
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::back() const noexcept ->
    const_reference {
  return *rbegin();
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::front() noexcept -> reference {
  return *begin();
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::front() const noexcept ->
    const_reference {
  return *begin();
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::operator+=(const basic_string& s) ->
    basic_string& {
  return *this += basic_string_ref<Char, Traits>(s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::operator+=(const char_type* s) ->
    basic_string& {
  return *this += basic_string_ref<Char, Traits>(s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::operator+=(char_type c) ->
    basic_string& {
  reserve(len_ + 1U);
  traits_type::assign((*this)[len_++], c);
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::operator+=(
    initializer_list<char_type> il) -> basic_string& {
  return *this += basic_string_ref<Char, Traits>(&*il.begin(), il.size());
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::operator+=(
    basic_string_ref<Char, Traits> s) -> basic_string& {
  reserve(len_ + s.size());
  traits_type::copy(begin() + len_, s.data(), s.size());
  len_ += s.size();
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::append(const basic_string& s) ->
    basic_string& {
  return *this += basic_string_ref<Char, Traits>(s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::append(const basic_string& s,
                                               size_type pos, size_type len) ->
    basic_string& {
  return *this += basic_string_ref<Char, Traits>(s).substr(pos, len);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::append(const char_type* s) ->
    basic_string& {
  return *this += basic_string_ref<Char, Traits>(s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::append(const char_type* s,
                                               size_type len) ->
    basic_string& {
  return *this += basic_string_ref<Char, Traits>(s, len);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::append(size_type n, char_type c) ->
    basic_string& {
  reserve(size() + n);
  traits_type::assign(begin() + size(), n, c);
  len_ += n;
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
template<typename InputIter>
auto basic_string<Char, Traits, Alloc>::append(InputIter b, InputIter e) ->
    basic_string& {
  while (b != e) {
    append(*b);
    ++b;
  }
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::append(
    initializer_list<char_type> il) -> basic_string& {
  return *this += il;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::append(
    basic_string_ref<Char, Traits> s) -> basic_string& {
  return *this += s;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::push_back(char_type c) -> void {
  *this += c;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::assign(const basic_string& s) ->
    basic_string& {
  return assign(basic_string_ref<Char, Traits>(s));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::assign(const basic_string& s,
                                               size_type pos, size_type len) ->
    basic_string& {
  return assign(basic_string_ref<Char, Traits>(s).substr(pos, len));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::assign(const char_type* s) ->
    basic_string& {
  return assign(basic_string_ref<Char, Traits>(s));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::assign(const char_type* s,
                                               size_type len) ->
    basic_string& {
  return assign(basic_string_ref<Char, Traits>(s, len));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::assign(size_type n, char_type c) ->
    basic_string& {
  reserve(n);
  traits_type::assign(begin(), n, c);
  len_ = n;
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
template<typename InputIter>
auto basic_string<Char, Traits, Alloc>::assign(InputIter b, InputIter e) ->
    basic_string& {
  return *this = basic_string(b, e, this->get_allocator());
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::assign(
    initializer_list<char_type> il) -> basic_string& {
  return assign(basic_string_ref<Char, Traits>(il.begin(), il.size()));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::assign(
    basic_string_ref<Char, Traits> s) -> basic_string& {
  reserve(s.size());
  traits_type::copy(begin(), s.data(), s.size());
  len_ = s.size();
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::assign(basic_string&& s) noexcept ->
    basic_string& {
  swap(s);
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::insert(size_type pos,
                                               const basic_string& s) ->
    basic_string& {
  return insert(pos, basic_string_ref<Char, Traits>(s));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::insert(
    size_type pos, const basic_string& s, size_type s_pos, size_type s_len) ->
    basic_string& {
  return insert(pos, basic_string_ref<Char, Traits>(s).substr(s_pos, s_len));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::insert(size_type pos,
                                               const char_type* s) ->
    basic_string& {
  return insert(pos, basic_string_ref<Char, Traits>(s));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::insert(
    size_type pos, basic_string_ref<Char, Traits> s) -> basic_string& {
  if (pos > size()) throw out_of_range("basic_string::insert");
  if (s.size() > max_size() - size())
    throw length_error("basic_string::insert");

  reserve(size() + s.size());
  traits_type::move(begin() + pos + s.size(), begin() + pos,
                    size() - pos);
  traits_type::copy(begin() + pos, s.data(), s.size());
  len_ += s.size();
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::insert(size_type pos,
                                               const char_type* s,
                                               size_type len) ->
    basic_string& {
  return insert(pos, basic_string_ref<Char, Traits>(s, len));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::insert(size_type pos,
                                               size_type n, char_type c) ->
    basic_string& {
  if (pos > size()) throw out_of_range("basic_string::insert");
  if (n > max_size() - size()) throw length_error("basic_string::insert");

  reserve(size() + n);
  traits_type::move(begin() + pos + n, begin() + pos, size() - pos);
  traits_type::assign(begin() + pos, n, c);
  len_ += n;
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::insert(const_iterator p,
                                               size_type n, char_type c) ->
    iterator {
  assert(p >= begin() && p <= end());

  reserve(size() + n);
  traits_type::move(begin() + (p - begin()) + n, p, end() - p);
  traits_type::assign(begin() + (p - begin()), n, c);
  len_ += n;
  return begin() + (p - begin());
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::insert(const_iterator p,
                                               char_type c) ->
    iterator {
  return insert(p, 1U, c);
}

template<typename Char, typename Traits, typename Alloc>
template<typename InputIter>
auto basic_string<Char, Traits, Alloc>::insert(const_iterator p,
                                               InputIter b, InputIter e) ->
    iterator {
  return insert(p, basic_string(b, e));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::insert(
    const_iterator p, initializer_list<char_type> il) -> basic_string& {
  insert(p, basic_string_ref<Char, Traits>(il.begin(), il.size()));
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::insert(
    const_iterator p, basic_string_ref<Char, Traits> s) -> iterator {
  assert(p >= begin() && p <= end());

  reserve(size() + s.size());
  traits_type::move(begin() + (p - begin()) + s.size(), p, end() - p);
  traits_type::copy(begin() + (p - begin()), s.data(), s.size());
  len_ += s.size();
  return begin() + (p - begin());
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::erase(size_type pos, size_type len) ->
    basic_string& {
  if (pos > size()) throw out_of_range("basic_string::erase");

  len = min(size() - pos, len);
  size_type pl = pos + len;
  traits_type::move(begin() + pos, begin() + pl, size() - pl);
  len_ -= len;
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::erase(const_iterator p) -> iterator {
  assert(p >= begin() && p < end());

  traits_type::move(begin() + (p - begin()), p + 1U, end() - (p + 1U));
  --len_;
  return begin() + (p - begin());
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::erase(const_iterator b,
                                              const_iterator e) -> iterator {
  assert(begin() <= b && b <= e && e <= end());

  traits_type::move(begin() + (b - begin()), e, end() - e);
  len_ -= (e - b);
  return begin() + (b - begin());
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::replace(size_type pos, size_type len,
                                                const basic_string& s) -> basic_string& {
  return replace(pos, len, basic_string_ref<Char, Traits>(s));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::replace(const_iterator b,
                                                const_iterator e,
                                                const basic_string& s) ->
    basic_string& {
  return replace(b, e, basic_string_ref<Char, Traits>(s));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::replace(size_type pos, size_type len,
                                                const basic_string& s,
                                                size_type s_pos,
                                                size_type s_len) ->
    basic_string& {
  return replace(pos, len,
                 basic_string_ref<Char, Traits>(s).substr(s_pos, s_len));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::replace(size_type pos, size_type len,
                                                const char_type* s) ->
    basic_string& {
  return replace(pos, len, basic_string_ref<Char, Traits>(s));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::replace(const_iterator b,
                                                const_iterator e,
                                                const char_type* s) ->
    basic_string& {
  return replace(b, e, basic_string_ref<Char, Traits>(s));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::replace(
    size_type pos, size_type len, basic_string_ref<Char, Traits> s) ->
    basic_string& {
  if (pos < 0 || pos > size()) throw out_of_range("basic_string::replace");
  if (len > size() - pos) len = size() - pos;
  return replace(begin() + pos, begin() + pos + len, s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::replace(
    const_iterator b, const_iterator e, basic_string_ref<Char, Traits> s) ->
    basic_string& {
  assert(begin() <= b && b <= e && e <= end());
  if (max_size() - size() + (e - b) < s.size())
    throw length_error("basic_string::replace");

  reserve(size() - (e - b) + s.size());
  traits_type::move(begin() + (b - begin()) + s.size(), e, end() - e);
  traits_type::copy(begin() + (b - begin()), s.data(), s.size());
  len_ = len_ - (e - b) + s.size();
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::replace(size_type pos, size_type len,
                                                const char_type* s,
                                                size_type s_len) ->
    basic_string& {
  return replace(pos, len, basic_string_ref<Char, Traits>(s, s_len));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::replace(const_iterator b,
                                                const_iterator e,
                                                const char_type* s,
                                                size_type s_len) ->
    basic_string& {
  return replace(b, e, basic_string_ref<Char, Traits>(s, s_len));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::replace(size_type pos, size_type len,
                                                size_type n, char_type c) ->
    basic_string& {
  if (pos < 0 || pos > size()) throw out_of_range("basic_string::replace");
  if (len > size() - pos) len = size() - pos;
  return replace(begin() + pos, begin() + pos + len, n, c);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::replace(const_iterator b,
                                                const_iterator e,
                                                size_type n, char_type c) ->
    basic_string& {
  assert(begin() <= b && b <= e && e <= end());
  if (max_size() - size() + (e - b) < 1)
    throw length_error("basic_string::replace");

  reserve(size() - (e - b) + n);
  traits_type::move(begin() + (b - begin()) + n, e, end() - e);
  traits_type::assign(begin() + (b - begin()), n, c);
  len_ = len_ - (e - b) + n;
  return *this;
}

template<typename Char, typename Traits, typename Alloc>
template<typename InputIter>
auto basic_string<Char, Traits, Alloc>::replace(const_iterator b,
                                                const_iterator e,
                                                InputIter ib, InputIter ie) ->
    basic_string& {
  return replace(b, e, basic_string(ib, ie));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::replace(
    const_iterator b, const_iterator e, initializer_list<char_type> il) ->
    basic_string& {
  return replace(b, e, basic_string_ref<Char, Traits>(il.begin(), il.size()));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::swap(basic_string& s) -> void {
  using std::swap;

  this->impl::alloc_base<Alloc>::swap_(
    static_cast<impl::alloc_base<Alloc>&>(s));
  swap(len_, s.len_);
  swap(avail_, s.avail_);
  swap(data_, s.data_);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::pop_back() noexcept -> void {
  assert(size() > 0);
  if (size() > 0) --len_;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::c_str() const noexcept ->
    const char_type* {
  traits_type::assign(const_cast<char_type&>(*end()), char_type());
  return data();
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::data() const noexcept ->
    const char_type* {
  return (avail_ > immed_size ? data_.s : &data_.immed[0]);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::copy(char_type* s, size_type len,
                                             size_type pos) const ->
    size_type {
  if (pos > size()) throw out_of_range("basic_string::copy");

  if (len > size() - pos) len = size() - pos;
  traits_type::copy(s, data() + pos, len);
  return len;
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find(
    const basic_string& s, size_type pos) const noexcept -> size_type {
  return find(basic_string_ref<Char, Traits>(s), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find(
    const char_type* s, size_type pos) const noexcept -> size_type {
  return find(basic_string_ref<Char, Traits>(s), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find(
    basic_string_ref<Char, Traits> s, size_type pos) const noexcept ->
    size_type {
  if (pos >= size()) return npos;
  return basic_string_ref<Char, Traits>(*this).substr(pos).find(s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find(const char_type* s, size_type pos,
                                             size_type n) const noexcept ->
    size_type {
  return find(basic_string_ref<Char, Traits>(s, n), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find(char_type c,
                                             size_type pos) const noexcept ->
    size_type {
  if (pos >= size()) return npos;
  return basic_string_ref<Char, Traits>(*this).substr(pos).find(c);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::rfind(const basic_string& s,
                                              size_type pos) const noexcept ->
    size_type {
  return rfind(basic_string_ref<Char, Traits>(s), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::rfind(const char_type* s,
                                              size_type pos) const noexcept ->
    size_type {
  return rfind(basic_string_ref<Char, Traits>(s), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::rfind(basic_string_ref<Char, Traits> s,
                                              size_type pos) const noexcept ->
    size_type {
  if (pos >= size())
    pos = size();
  else
    ++pos;
  return basic_string_ref<Char, Traits>(*this).substr(0, pos).rfind(s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::rfind(const char_type* s,
                                              size_type pos, size_type n)
    const noexcept -> size_type {
  return rfind(basic_string_ref<Char, Traits>(s, n), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::rfind(char_type c, size_type pos)
    const noexcept -> size_type {
  if (pos >= size())
    pos = size();
  else
    ++pos;
  return basic_string_ref<Char, Traits>(*this).substr(0, pos).rfind(c);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_first_of(
    const basic_string& s, size_type pos) const noexcept -> size_type {
  return find_first_of(basic_string_ref<Char, Traits>(s), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_first_of(
    const char_type* s, size_type pos) const noexcept -> size_type {
  return find_first_of(basic_string_ref<Char, Traits>(s), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_first_of(
    basic_string_ref<Char, Traits> s, size_type pos) const noexcept ->
    size_type {
  if (pos >= size()) return npos;
  return basic_string_ref<Char, Traits>(*this).substr(pos).find_first_of(s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_first_of(
    const char_type* s, size_type len, size_type pos) const noexcept ->
    size_type {
  return find_first_of(basic_string_ref<Char, Traits>(s, len), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_first_of(
    char_type c, size_type pos) const noexcept -> size_type {
  if (pos >= size()) return npos;
  return basic_string_ref<Char, Traits>(*this).substr(0, pos).find_first_of(c);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_last_of(
    const basic_string& s, size_type pos) const noexcept -> size_type {
  return find_last_of(basic_string_ref<Char, Traits>(s), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_last_of(
    const char_type* s, size_type pos) const noexcept -> size_type {
  return find_last_of(basic_string_ref<Char, Traits>(s), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_last_of(
    basic_string_ref<Char, Traits> s, size_type pos) const noexcept ->
    size_type {
  if (pos >= size())
    pos = size();
  else
    ++pos;
  return basic_string_ref<Char, Traits>(*this).substr(0, pos).find_last_of(s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_last_of(
    const char_type* s, size_type len, size_type pos) const noexcept ->
    size_type {
  return find_last_of(basic_string_ref<Char, Traits>(s, len), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_last_of(
    char_type c, size_type pos) const noexcept -> size_type {
  if (pos >= size())
    pos = size();
  else
    ++pos;
  return basic_string_ref<Char, Traits>(*this).substr(0, pos).find_last_of(c);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_first_not_of(
    const basic_string& s, size_type pos) const noexcept -> size_type {
  return find_first_not_of(basic_string_ref<Char, Traits>(s), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_first_not_of(
    const char_type* s, size_type pos) const noexcept -> size_type {
  return find_first_not_of(basic_string_ref<Char, Traits>(s), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_first_not_of(
    basic_string_ref<Char, Traits> s, size_type pos) const noexcept ->
    size_type {
  if (pos >= size()) return npos;
  return basic_string_ref<Char, Traits>(*this).substr(pos).
      find_first_not_of(s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_first_not_of(
    const char_type* s, size_type len, size_type pos) const noexcept ->
    size_type {
  return find_first_not_of(basic_string_ref<Char, Traits>(s, len), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_first_not_of(
    char_type c, size_type pos) const noexcept -> size_type {
  if (pos >= size()) return npos;
  return basic_string_ref<Char, Traits>(*this).substr(0, pos).
      find_first_not_of(c);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_last_not_of(
    const basic_string& s, size_type pos) const noexcept -> size_type {
  return find_last_not_of(basic_string_ref<Char, Traits>(s), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_last_not_of(
    const char_type* s, size_type pos) const noexcept -> size_type {
  return find_last_not_of(basic_string_ref<Char, Traits>(s), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_last_not_of(
    basic_string_ref<Char, Traits> s, size_type pos) const noexcept ->
    size_type {
  if (pos >= size())
    pos = size();
  else
    ++pos;
  return basic_string_ref<Char, Traits>(*this).substr(0, pos).
      find_last_not_of(s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_last_not_of(
    const char_type* s, size_type len, size_type pos) const noexcept ->
    size_type {
  return find_last_not_of(basic_string_ref<Char, Traits>(s, len), pos);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::find_last_not_of(
    char_type c, size_type pos) const noexcept -> size_type {
  if (pos >= size())
    pos = size();
  else
    ++pos;
  return basic_string_ref<Char, Traits>(*this).substr(0, pos).
      find_last_not_of(c);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::substr(size_type pos, size_type len)
    const -> basic_string {
  if (pos > size()) throw out_of_range("basic_string::substr");
  if (len > size() - pos) len = size() - pos;
  return basic_string(data() + pos, len, this->get_allocator());
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::compare(const basic_string& s)
    const noexcept -> int {
  return compare(basic_string_ref<Char, Traits>(s));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::compare(size_type pos1, size_type len1,
                                                const basic_string& s)
    const -> int {
  return compare(pos1, len1, basic_string_ref<Char, Traits>(s));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::compare(size_type pos1, size_type len1,
                                                const basic_string& s,
                                                size_type pos2, size_type len2)
    const -> int {
  if (pos2 > s.size()) throw out_of_range("basic_string::compare");
  return compare(pos1, len1,
                 basic_string_ref<Char, Traits>(s).substr(pos2, len2));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::compare(const char_type* s)
    const noexcept -> int {
  return compare(basic_string_ref<Char, Traits>(s));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::compare(
    basic_string_ref<Char, Traits> s) const noexcept -> int {
  return compare(0, npos, s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::compare(size_type pos, size_type len,
                                                const char_type* s) const ->
    int {
  return compare(pos, len, basic_string_ref<Char, Traits>(s));
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::compare(
    size_type pos, size_type len, basic_string_ref<Char, Traits> s)
    const -> int {
  if (pos > size()) throw out_of_range("basic_string::compare");
  return basic_string_ref<Char, Traits>(*this).substr(pos, len).compare(s);
}

template<typename Char, typename Traits, typename Alloc>
auto basic_string<Char, Traits, Alloc>::compare(
    size_type pos, size_type len, const char_type* s, size_type slen)
    const -> int {
  return compare(pos, len, basic_string_ref<Char, Traits>(s, slen));
}



template<typename Char, typename Traits>
basic_string_ref<Char, Traits>::basic_string_ref(const_pointer s)
: basic_string_ref(s, traits_type::length(s))
{}

template<typename Char, typename Traits>
constexpr basic_string_ref<Char, Traits>::basic_string_ref(const_pointer s,
                                                           size_type len)
: s_(s),
  len_(len)
{}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::begin() const noexcept ->
    const_iterator {
  return s_;
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::end() const noexcept ->
    const_iterator {
  return begin() + len_;
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::cbegin() const noexcept ->
    const_iterator {
  return begin();
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::cend() const noexcept ->
    const_iterator {
  return end();
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::rbegin() const noexcept ->
    const_reverse_iterator {
  return const_reverse_iterator(end());
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::rend() const noexcept ->
    const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::crbegin() const noexcept ->
    const_reverse_iterator {
  return const_reverse_iterator(end());
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::crend() const noexcept ->
    const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::size() const noexcept ->
    size_type {
  return len_;
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::length() const noexcept ->
    size_type {
  return size();
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::max_size() const noexcept ->
    size_type {
  return SIZE_MAX;
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::empty() const noexcept -> bool {
  return size() == 0U;
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::operator[](size_type i)
    const -> const_reference {
  return begin()[i];
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::at(size_type i) const ->
    const_reference {
  if (i < 0 || i >= size()) throw out_of_range("basic_string_ref::at");
  return (*this)[i];
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::front() const ->
    const_reference {
  return begin()[0];
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::back() const ->
    const_reference {
  return rbegin()[0];
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::data() const noexcept ->
    const_pointer {
  return s_;
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::clear() noexcept -> void {
  *this = basic_string_ref();
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::remove_prefix(size_type n) -> void {
  if (n >= size()) throw out_of_range("basic_string_ref::remove_prefix");
  s_ += n;
  len_ -= n;
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::remove_suffix(size_type n) -> void {
  len_ -= (n >= len_ ? len_ : n);
}

template<typename Char, typename Traits>
constexpr auto basic_string_ref<Char, Traits>::substr(size_type pos,
                                                      size_type n)
    const -> basic_string_ref {
  if (pos < 0 || pos >= size()) throw out_of_range("basic_string_ref::substr");
  basic_string_ref copy = *this;
  copy.s_ += pos;
  copy.len_ -= pos;
  if (copy.len_ > n) copy.len_ = n;
  return copy;
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::compare(basic_string_ref o)
    const noexcept -> int {
  int cmp = traits_type::compare(data(), o.data(),
                                 min(size(), o.size()));
  return (cmp != 0 ? cmp :
          (size() < o.size() ? -1 : (size() > o.size())));
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::compare(const_pointer s)
    const noexcept -> int {
  return compare(basic_string_ref(s));
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::starts_with(
    basic_string_ref prefix) const noexcept -> bool {
  return size() >= prefix.size() && substr(0, prefix.size()) == prefix;
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::starts_with(char_type c)
    const noexcept -> bool {
  return starts_with(basic_string_ref(&c, 1));
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::starts_with(const_pointer s)
    const noexcept -> bool {
  return starts_with(basic_string_ref(s));
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::ends_with(
    basic_string_ref suffix) const noexcept -> bool {
  return size() >= suffix.size() && substr(size() - suffix.size()) == suffix;
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::ends_with(char_type c)
    const noexcept -> bool {
  return ends_with(basic_string_ref(&c, 1));
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::ends_with(const_pointer s)
    const noexcept -> bool {
  return ends_with(basic_string_ref(s));
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find(basic_string_ref needle)
    const noexcept -> size_type {
  auto rv = impl::strfind<traits_type>(data(), size(),
                                       needle.data(), needle.size());
  return (rv == nullptr ? npos : rv - data());
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find(char_type c)
    const noexcept -> size_type {
  auto p = traits_type::find(data(), size(), c);
  return (p == nullptr ? npos : p - begin());
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find(const_pointer s)
    const noexcept -> size_type {
  return find(basic_string_ref(s));
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::rfind(basic_string_ref needle)
    const noexcept -> size_type {
  auto rv = impl::strrfind<traits_type>(data(), size(),
                                        needle.data(), needle.size());
  return (rv == nullptr ? npos : rv - data());
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::rfind(char_type c)
    const noexcept -> size_type {
  auto p = impl::rfind<traits_type>(data(), size(), c);
  return (p == nullptr ? npos : p - begin());
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::rfind(const_pointer s)
    const noexcept -> size_type {
  return rfind(basic_string_ref(s));
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find_first_of(basic_string_ref s)
    const noexcept -> size_type {
  auto p = impl::strcspn<traits_type>(data(), size(), s.data(), s.size());
  return (p == nullptr ? npos : p - begin());
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find_first_of(char_type c)
    const noexcept -> size_type {
  return find(c);
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find_first_of(const_pointer s)
    const noexcept -> size_type {
  return find_first_of(basic_string_ref(s));
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find_last_of(basic_string_ref s)
    const noexcept -> size_type {
  auto p = impl::strrcspn<traits_type>(data(), size(), s.data(), s.size());
  return (p == nullptr ? npos : p - begin());
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find_last_of(char_type c)
    const noexcept -> size_type {
  return rfind(c);
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find_last_of(const_pointer s)
    const noexcept -> size_type {
  return find_last_of(basic_string_ref(s));
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find_first_not_of(basic_string_ref s)
    const noexcept -> size_type {
  auto p = impl::strspn<traits_type>(data(), size(), s.data(), s.size());
  return (p == nullptr ? npos : p - begin());
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find_first_not_of(char_type c)
    const noexcept -> size_type {
  auto p = impl::spn<traits_type>(data(), size(), c);
  return (p == nullptr ? npos : p - begin());
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find_first_not_of(const_pointer s)
    const noexcept -> size_type {
  return find_first_not_of(basic_string_ref(s));
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find_last_not_of(basic_string_ref s)
    const noexcept -> size_type {
  auto p = impl::strrspn<traits_type>(data(), size(), s.data(), s.size());
  return (p == nullptr ? npos : p - begin());
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find_last_not_of(char_type c)
    const noexcept -> size_type {
  auto p = impl::rspn<traits_type>(data(), size(), c);
  return (p == nullptr ? npos : p - begin());
}

template<typename Char, typename Traits>
auto basic_string_ref<Char, Traits>::find_last_not_of(const_pointer s)
    const noexcept -> size_type {
  return find_last_not_of(basic_string_ref(s));
}


template<typename Char, typename Traits>
auto operator==(basic_string_ref<Char, Traits> a,
                basic_string_ref<Char, Traits> b) noexcept -> bool {
  return a.compare(b) == 0U;
}

template<typename Char, typename Traits>
auto operator!=(basic_string_ref<Char, Traits> a,
                basic_string_ref<Char, Traits> b) noexcept -> bool {
  return a.compare(b) != 0U;
}

template<typename Char, typename Traits>
auto operator<(basic_string_ref<Char, Traits> a,
               basic_string_ref<Char, Traits> b) noexcept -> bool {
  return a.compare(b) < 0U;
}

template<typename Char, typename Traits>
auto operator>(basic_string_ref<Char, Traits> a,
               basic_string_ref<Char, Traits> b) noexcept -> bool {
  return a.compare(b) > 0U;
}

template<typename Char, typename Traits>
auto operator<=(basic_string_ref<Char, Traits> a,
                basic_string_ref<Char, Traits> b) noexcept -> bool {
  return a.compare(b) <= 0U;
}

template<typename Char, typename Traits>
auto operator>=(basic_string_ref<Char, Traits> a,
                basic_string_ref<Char, Traits> b) noexcept -> bool {
  return a.compare(b) >= 0U;
}

template<typename Char, typename Traits>
auto operator==(basic_string_ref<Char, Traits> a,
                typename basic_string_ref<Char, Traits>::__identity b)
    noexcept -> bool {
  return a == b;
}

template<typename Char, typename Traits>
auto operator!=(basic_string_ref<Char, Traits> a,
                typename basic_string_ref<Char, Traits>::__identity b)
    noexcept -> bool {
  return a != b;
}

template<typename Char, typename Traits>
auto operator<(basic_string_ref<Char, Traits> a,
               typename basic_string_ref<Char, Traits>::__identity b)
    noexcept -> bool {
  return a < b;
}

template<typename Char, typename Traits>
auto operator>(basic_string_ref<Char, Traits> a,
               typename basic_string_ref<Char, Traits>::__identity b)
    noexcept -> bool {
  return a > b;
}

template<typename Char, typename Traits>
auto operator<=(basic_string_ref<Char, Traits> a,
                typename basic_string_ref<Char, Traits>::__identity b)
    noexcept -> bool {
  return a <= b;
}

template<typename Char, typename Traits>
auto operator>=(basic_string_ref<Char, Traits> a,
                typename basic_string_ref<Char, Traits>::__identity b)
    noexcept -> bool {
  return a >= b;
}

template<typename Char, typename Traits>
auto operator==(typename basic_string_ref<Char, Traits>::__identity a,
                basic_string_ref<Char, Traits> b) noexcept -> bool {
  return a == b;
}

template<typename Char, typename Traits>
auto operator!=(typename basic_string_ref<Char, Traits>::__identity a,
                basic_string_ref<Char, Traits> b) noexcept -> bool {
  return a != b;
}

template<typename Char, typename Traits>
auto operator<(typename basic_string_ref<Char, Traits>::__identity a,
               basic_string_ref<Char, Traits> b) noexcept -> bool {
  return a < b;
}

template<typename Char, typename Traits>
auto operator>(typename basic_string_ref<Char, Traits>::__identity a,
               basic_string_ref<Char, Traits> b) noexcept -> bool {
  return a > b;
}

template<typename Char, typename Traits>
auto operator<=(typename basic_string_ref<Char, Traits>::__identity a,
                basic_string_ref<Char, Traits> b) noexcept -> bool {
  return a <= b;
}

template<typename Char, typename Traits>
auto operator>=(typename basic_string_ref<Char, Traits>::__identity a,
                basic_string_ref<Char, Traits> b) noexcept -> bool {
  return a >= b;
}

template<typename Char, typename Traits, typename Allocator>
basic_string<Char, Traits, Allocator> to_string(
    basic_string_ref<Char, Traits> s, const Allocator& a) {
  return basic_string<Char, Traits, Allocator>(s, a);
}


_namespace_end(std)
inline namespace literals {
inline namespace string_literals {


inline std::string operator"" s(const char* s, size_t l) {
  return std::string(s, l);
}

inline std::wstring operator"" s(const wchar_t* s, size_t l) {
  return std::wstring(s, l);
}

inline std::u16string operator"" s(const char16_t* s, size_t l) {
  return std::u16string(s, l);
}

inline std::u32string operator"" s(const char32_t* s, size_t l) {
  return std::u32string(s, l);
}


}} /* namespace literals::string_literals */
