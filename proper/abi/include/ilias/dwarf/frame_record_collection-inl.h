#ifndef _ILIAS_DWARF_FRAME_RECORD_COLLECTION_INL_H_
#define _ILIAS_DWARF_FRAME_RECORD_COLLECTION_INL_H_

#include <ilias/dwarf/frame_record_collection.h>
#include <cassert>

_namespace_begin(ilias)
namespace dwarf {


inline frame_record_collection::frame_record_collection(const void* begin,
                                                        const void* end)
    noexcept
: begin_(begin),
  end_(end)
{
  assert(begin_ <= end_);
}

inline auto frame_record_collection::begin() const noexcept -> const_iterator {
  return iterator(begin_, end_);
}

inline auto frame_record_collection::cbegin() const noexcept ->
    const_iterator {
  return begin();
}

inline auto frame_record_collection::end() const noexcept -> const_iterator {
  return const_iterator();
}

inline auto frame_record_collection::cend() const noexcept ->
    const_iterator {
  return end();
}

inline auto frame_record_collection::empty() const noexcept -> bool {
  return !(begin_ == end_);
}

inline auto frame_record_collection::operator==(
    const frame_record_collection& o) const noexcept -> bool {
  return (begin_ == o.begin_ && end_ == o.end_);
}

inline auto frame_record_collection::operator!=(
    const frame_record_collection& o) const noexcept -> bool {
  return !(*this == o);
}


inline frame_record_collection::iterator::iterator(const void* cur,
                                                   const void* end) noexcept
: cur_(cur == end ? frame_record() : frame_record(cur)),
  end_(cur == end ? nullptr : end)
{}

inline auto frame_record_collection::iterator::operator++() noexcept ->
    iterator& {
  assert(end_ != nullptr);

  if (_predict_false(cur_.succ_ptr() == end_)) {
    cur_ = frame_record();
    end_ = nullptr;
  } else {
    cur_ = cur_.succ();
  }
  return *this;
}

inline auto frame_record_collection::iterator::operator++(int) noexcept ->
    iterator {
  iterator copy = *this;
  ++*this;
  return copy;
}

inline auto frame_record_collection::iterator::operator==(const iterator& o)
    const noexcept -> bool {
  /* Check if both are end-iterators. */
  return cur_ == o.cur_;
}

inline auto frame_record_collection::iterator::operator!=(const iterator& o)
    const noexcept -> bool {
  return !(*this == o);
}

inline auto frame_record_collection::iterator::operator*() const noexcept ->
    const frame_record {
  assert(end_ != nullptr);
  return cur_;
}

inline auto frame_record_collection::iterator::operator->() const noexcept ->
    proxy {
  assert(end_ != nullptr);
  return proxy(cur_);
}


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#endif /* _ILIAS_DWARF_FRAME_RECORD_COLLECTION_INL_H_ */
