#ifndef _ABI_EXT_STRING_PIECE_H_
#define _ABI_EXT_STRING_PIECE_H_

#include <abi/abi.h>
#include <abi/ext/c_string_ptr.h>
#include <iosfwd>
#include <stdexcept>
#include <vector>

namespace __cxxabiv1 {
namespace ext {


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
  c_string_piece(c_string_piece&&) noexcept;
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
  inline c_string_piece& operator+=(size_type l) noexcept;
  inline c_string_piece operator+(size_type l) const noexcept;
  inline c_string_piece& operator++() noexcept;
  inline c_string_piece operator++(int) noexcept;

  inline bool operator==(const c_string_piece&) const noexcept;
  inline bool operator!=(const c_string_piece&) const noexcept;

  inline void swap(c_string_piece&) noexcept;
  friend inline void swap(c_string_piece&, c_string_piece&) noexcept;

  inline c_string_piece substr(size_type s) const noexcept;
  inline c_string_piece substr(size_type s, size_type len) const noexcept;

  inline operator c_string_ptr() const;
  operator std::string() const;
  inline ptrdiff_t operator-(const c_string_piece&) const noexcept;

  inline iterator begin() const;
  inline iterator end() const;
  inline const_iterator cbegin() const;
  inline const_iterator cend() const;

  iterator find(char) const noexcept;

 private:
  pointer data_ = nullptr;
  size_type len_ = 0U;
};

inline ptrdiff_t operator-(const char*, const c_string_piece&) noexcept;

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

  bool operator==(c_string_piece other) const noexcept;
  bool operator==(const c_chord& other) const noexcept;
  inline bool operator!=(c_string_piece other) const noexcept;
  inline bool operator!=(const c_chord& other) const noexcept;

  operator c_string_ptr() const;
  operator std::string() const;

 private:
  data_t data_;
  size_type len_ = 0U;
};

template<typename... T> c_chord chord_cat(T&&... t);


}} /* namespace __cxxabiv1::ext */

#include <abi/ext/string_piece-inl.h>

#endif /* _ABI_EXT_STRING_PIECE_H_ */
