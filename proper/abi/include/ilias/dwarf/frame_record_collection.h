#ifndef _ILIAS_DWARF_FRAME_RECORD_COLLECTION_H_
#define _ILIAS_DWARF_FRAME_RECORD_COLLECTION_H_

#include <cdecl.h>
#include <cstddef>
#include <iterator>
#include <ilias/dwarf/frame_record.h>

_namespace_begin(ilias)
namespace dwarf {


class frame_record_collection {
 private:
  class proxy;

 public:
  using value_type = const frame_record;
  using difference_type = _namespace(std)::ptrdiff_t;
  using size_type = _namespace(std)::size_t;
  using pointer = value_type*;
  using const_pointer = value_type*;
  using reference = value_type&;
  using const_reference = value_type&;

  class iterator;
  using const_iterator = iterator;

  frame_record_collection(const void*, const void*) noexcept;

  const_iterator begin() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cend() const noexcept;

  bool empty() const noexcept;
  size_type size() const noexcept;

  bool operator==(const frame_record_collection&) const noexcept;
  bool operator!=(const frame_record_collection&) const noexcept;

 private:
  const void* begin_ = nullptr;
  const void* end_ = nullptr;
};

class frame_record_collection::proxy {
  friend frame_record_collection::iterator;

 public:
  proxy() = delete;
  proxy(const proxy&) = default;
  proxy& operator=(const proxy&) = delete;

  const frame_record& operator*() const noexcept { return impl_; }
  const frame_record* operator->() const noexcept { return &impl_; }

  bool operator==(const proxy& o) noexcept { return impl_ == o.impl_; }
  bool operator!=(const proxy& o) noexcept { return !(*this == o); }

 private:
  explicit proxy(const frame_record& fr) noexcept : impl_(fr) {}

  const frame_record impl_;
};

class frame_record_collection::iterator
: public _namespace(std)::iterator<_namespace(std)::forward_iterator_tag,
                                   const frame_record,
                                   frame_record_collection::difference_type,
                                   proxy,
                                   const frame_record>
{
  friend frame_record_collection;

 private:
  iterator(const void*, const void*) noexcept;

 public:
  iterator() noexcept = default;
  iterator(const iterator&) noexcept = default;
  iterator& operator=(const iterator&) noexcept = default;

  iterator& operator++() noexcept;
  iterator operator++(int) noexcept;

  bool operator==(const iterator&) const noexcept;
  bool operator!=(const iterator&) const noexcept;

  const frame_record operator*() const noexcept;
  proxy operator->() const noexcept;

 private:
  frame_record cur_;
  const void* end_ = nullptr;
};


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#include <ilias/dwarf/frame_record_collection-inl.h>

#endif /* _ILIAS_DWARF_FRAME_RECORD_COLLECTION_H_ */
