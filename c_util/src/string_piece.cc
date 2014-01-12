#include <util/string_piece.h>
#include <string>

namespace util {


c_string_piece::operator std::string() const {
  return std::string(data(), size());
}

c_chord& c_chord::operator+=(c_string_piece other) {
  size_type add_len = other.length();
  if (add_len > 0) {
    data_.emplace_back(std::move(other));
    len_ += add_len;
  }
  return *this;
}

c_chord& c_chord::operator+=(const c_chord& other) {
  data_.reserve(data_.size() + other.data_.size());
  for (const auto& i : other.data_) data_.emplace_back(i);
  len_ += other.length();
  return *this;
}

bool c_chord::operator==(c_string_piece other) const noexcept {
  if (size() != other.size()) return false;
  for (c_string_piece item : data_) {
    assert(item.length() <= other.length());
    if (item != other.substr(0, item.length())) return false;
    other += item.length();
  }
  assert(other.empty());
  return true;
}

bool c_chord::operator==(const c_chord& other) const noexcept {
  using data_iter = data_t::const_iterator;

  if (size() != other.size()) return false;  // Quick rejection.

  data_iter this_i = data_.begin(), other_i = other.data_.begin();
  c_string_piece this_piece, other_piece;
  while (this_i != data_.end() && other_i != other.data_.end()) {
    if (this_piece.empty()) this_piece = *this_i++;
    if (other_piece.empty()) other_piece = *other_i++;

    size_type cmplen = std::min(this_piece.length(), other_piece.length());
    if (this_piece.substr(0, cmplen) != other_piece.substr(0, cmplen)) return false;
    this_piece += cmplen;
    other_piece += cmplen;
  }
  assert(this_piece.empty() && other_piece.empty());
  return true;
}

c_chord::operator c_string_ptr() const {
  c_string_ptr result;
  result.reserve(size());
  for (const auto& i : data_) result += i;
  return result;
}

c_chord::operator std::string() const {
  std::string result;
  result.reserve(size());
  for (const auto& i : data_) result.append(i.data(), i.size());
  return result;
}


} /* namespace util */
