#include <util/c_string_ptr.h>

namespace util {


inline constexpr c_string_piece::c_string_piece(const char* s) noexcept : data_{ s }, len_{ s ? strlen(s) : 0U } {}
inline constexpr c_string_piece::c_string_piece(pointer p, size_type l) noexcept : data_{ p }, len_{ l } {}
inline c_string_piece::c_string_piece(c_string_piece&& other) noexcept
: c_string_piece{}
{
  swap(other);
}
inline constexpr c_string_piece::c_string_piece(const c_string_ptr& csp) noexcept
: data_{ csp.data() },
  len_{ csp.length() }
{}

inline bool c_string_piece::empty() const noexcept { return size() == 0U; }
inline c_string_piece::size_type c_string_piece::size() const noexcept { return len_; }
inline c_string_piece::size_type c_string_piece::length() const noexcept { return size(); }
inline c_string_piece::pointer c_string_piece::data() const noexcept { return data_; }
inline c_string_piece::reference c_string_piece::operator*() const noexcept { return *data(); }
inline c_string_piece::pointer c_string_piece::operator->() const noexcept { return data(); }
inline c_string_piece::reference c_string_piece::operator[](size_type i) const noexcept {
  if (i >= len_) throw std::range_error("data not in string piece");
  return data()[i];
}

inline c_string_piece& c_string_piece::operator=(c_string_piece&& other) noexcept {
  using std::move;

  data_ = move(other.data_);
  len_ = other.len_;
  other.len_ = 0U;
  return *this;
}
inline c_string_piece& c_string_piece::operator+=(size_type l) {
  if (l > len_) throw std::range_error("string_piece shorter than added index");
  data_ += l;
  len_ -= l;
  return *this;
}
inline c_string_piece c_string_piece::operator+(size_type l) const {
  c_string_piece clone = *this;
  return clone += l;
}

inline bool c_string_piece::operator==(const c_string_piece& other) const noexcept {
  return size() == other.size() && memcmp(data(), other.data(), size()) == 0;
}
inline bool c_string_piece::operator!=(const c_string_piece& other) const noexcept {
  return !(*this == other);
}

inline void c_string_piece::swap(c_string_piece& other) noexcept {
  using std::swap;

  swap(data_, other.data_);
  swap(len_, other.len_);
}

inline void swap(c_string_piece& a, c_string_piece& b) noexcept { a.swap(b); }
inline c_string_piece c_string_piece::substr(size_type s) const { return *this + s; }

inline c_string_piece c_string_piece::substr(size_type s, size_type len) const {
  c_string_piece result = *this + s;
  if (result.len_ > len) result.len_ = len;
  return result;
}

inline c_string_piece::operator c_string_ptr() const {
  return make_c_string_ptr(data_, len_);
}

inline c_string_piece::iterator c_string_piece::begin() const { return data_; }
inline c_string_piece::iterator c_string_piece::end() const { return begin() + size(); }
inline c_string_piece::const_iterator c_string_piece::cbegin() const { return begin(); }
inline c_string_piece::const_iterator c_string_piece::cend() const { return end(); }


inline bool c_chord::empty() const noexcept { return size() == 0U; }
inline c_chord::size_type c_chord::size() const noexcept { return len_; }
inline c_chord::size_type c_chord::length() const noexcept { return size(); }

inline c_chord& c_chord::operator+=(const c_string_ptr& other) {
  return *this += c_string_piece(other);
}
inline c_chord& c_chord::operator+=(const char* s) {
  return *this += c_string_piece(s);
}

inline bool c_chord::operator!=(c_string_piece other) const noexcept { return !(*this == other); }
inline bool c_chord::operator!=(const c_chord& other) const noexcept { return !(*this == other); }


namespace chord_cat_detail {
  inline c_chord chord_cat_(c_chord&& out) {
    return out;
  }

  template<typename U, typename... T> c_chord chord_cat_(c_chord&& out, U&& u, T&&... t) {
    out += u;
    return chord_cat_(std::move(out), std::forward<T>(t)...);
  }
} /* namespace util::chord_cat_detail */

template<typename... T> c_chord chord_cat(T&&... t) {
  return chord_cat_detail::chord_cat_(c_chord(), std::forward<T>(t)...);
}


} /* namespace util */
