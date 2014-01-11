#ifndef UTIL_STRING_PIECE_H
#define UTIL_STRING_PIECE_H

#include <util/c_string_ptr.h>

namespace util {


class c_string_piece {
 public:
  using value_type = const char;
  using pointer = value_type*;
  using reference = value_type&;
  using size_type = std::size_t;
  using iterator = pointer;
  using const_iterator = pointer;

  c_string_piece() = default;
  c_string_piece(const c_string_piece&) = default;
  c_string_piece(c_string_piece&&);
  inline constexpr c_string_piece(const char*) noexcept;
  inline constexpr c_string_piece(pointer, size_type) noexcept;
  inline constexpr c_string_piece(const c_string_ptr&) noexcept;

  inline bool empty() const noexcept;
  inline size_type size() const noexcept;
  inline size_type length() const noexcept;
  inline pointer data() const noexcept;
  inline reference operator*() const noexcept;
  inline pointer operator->() const noexcept;
  inline reference operator[](size_type) const noexcept;

  inline c_string_piece& operator=(const c_string_piece&) = default;
  inline c_string_piece& operator=(c_string_piece&&) noexcept;
  inline c_string_piece operator+=(size_type l);
  inline c_string_piece operator+(size_type l) const;

  inline bool operator==(const c_string_piece&) const noexcept;
  inline bool operator!=(const c_string_piece&) const noexcept;

  inline void swap(c_string_piece&) noexcept;
  friend inline void swap(c_string_piece&, c_string_piece&) noexcept;

  inline c_string_piece substr(size_type s) const;
  inline c_string_piece substr(size_type s, size_type len) const;

  inline operator c_string_ptr() const;

  inline iterator begin() const;
  inline iterator end() const;
  inline const_iterator cbegin() const;
  inline const_iterator cend() const;

 private:
  pointer data_ = nullptr;
  size_type len_ = 0U;
};

class c_chord {
 private:
  using data_t = std::vector<c_string_piece>;

 public:
  using size_type = c_string_piece::size_type;

  inline bool empty() const noexcept;
  inline size_type size() const noexcept;
  inline size_type length() const noexcept;

  inline c_chord& operator+=(const c_string_ptr&);
  c_chord& operator+=(c_string_piece);
  c_chord& operator+=(const c_chord&);
  inline c_chord& operator+=(const char*);

  bool operator==(const c_string_piece& other) const noexcept;
  bool operator==(const c_chord& other) const noexcept;
  inline bool operator!=(const c_string_piece& other) const noexcept;
  inline bool operator!=(const c_chord& other) const noexcept;

  operator c_string_ptr() const;

 private:
  data_t data_;
  size_type len_ = 0U;
};

template<typename... T> c_chord chord_cat(T&&... t);


} /* namespace util */

#include <util/string_piece-inl.h>

#endif /* UTIL_STRING_PIECE_H */
