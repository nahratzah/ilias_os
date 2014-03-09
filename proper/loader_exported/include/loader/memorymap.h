#ifndef _LOADER_MEMORYMAP_H_
#define _LOADER_MEMORYMAP_H_

#include <cdecl.h>
#include <abi/ext/relptr.h>
#include <vector>
#include <iosfwd>

namespace loader {


/*
 * Describe multiple ranges of memory.
 */
class memorymap {
 public:
  struct range {
    uint64_t addr;
    uint64_t len;
  };

 private:
  using data_t = std::vector<range, abi::ext::relptr_allocator<range>>;

 public:
  using iterator = typename data_t::iterator;
  using const_iterator = typename data_t::const_iterator;
  using reverse_iterator = typename data_t::reverse_iterator;
  using const_reverse_iterator = typename data_t::const_reverse_iterator;
  using size_type = size_t;

  iterator begin() noexcept { return data_.begin(); }
  iterator end() noexcept { return data_.end(); }
  const_iterator begin() const noexcept { return data_.begin(); }
  const_iterator end() const noexcept { return data_.end(); }
  const_iterator cbegin() const noexcept { return data_.cbegin(); }
  const_iterator cend() const noexcept { return data_.cend(); }

  reverse_iterator rbegin() noexcept { return data_.rbegin(); }
  reverse_iterator rend() noexcept { return data_.rend(); }
  const_reverse_iterator rbegin() const noexcept { return data_.rbegin(); }
  const_reverse_iterator rend() const noexcept { return data_.rend(); }
  const_reverse_iterator crbegin() const noexcept { return data_.crbegin(); }
  const_reverse_iterator crend() const noexcept { return data_.crend(); }

  void push_back(range r) { data_.push_back(r); }
  void pop_back() { data_.pop_back(); }

  range& front() noexcept { return data_.front(); }
  const range& front() const noexcept { return data_.front(); }
  range& back() noexcept { return data_.front(); }
  const range& back() const noexcept { return data_.front(); }

  size_type size() const noexcept { return data_.size(); }
  bool empty() const noexcept { return data_.empty(); }
  void shrink_to_fit() noexcept { data_.shrink_to_fit(); }

  std::string to_string() const;

 private:
  data_t data_;
};

static_assert(sizeof(memorymap) == 3 * 4,
              "Memory map may be inconsistent across botted architectures.");


} /* namespace loader */

#endif /* _LOADER_MEMORYMAP_H_ */
