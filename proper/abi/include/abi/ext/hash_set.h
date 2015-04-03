#ifndef _ABI_EXT_HASH_SET_H_
#define _ABI_EXT_HASH_SET_H_

#include <abi/abi.h>
#include <abi/ext/list.h>

namespace __cxxabiv1 {
namespace ext {


template<typename T, size_t Buckets, typename Tag>
class hash_set {
 private:
  using list_t = list<T, Tag>;

 public:
  using bucket_idx = size_t;
  using value_type = typename list_t::value_type;
  using reference = typename list_t::reference;
  using const_reference = typename list_t::const_reference;
  using pointer = typename list_t::pointer;
  using const_pointer = typename list_t::const_pointer;
  using size_type = typename list_t::size_type;
  using difference_type = typename list_t::difference_type;

  using iterator = typename list_t::iterator;
  using const_iterator = typename list_t::const_iterator;
  using reverse_iterator = typename list_t::reverse_iterator;
  using const_reverse_iterator = typename list_t::const_reverse_iterator;
  class bucket;
  class const_bucket;

  hash_set() noexcept;
  hash_set(const hash_set&) = delete;
  hash_set(hash_set&&) noexcept;
  hash_set& operator=(const hash_set&) = delete;
  hash_set& operator=(hash_set&&) noexcept;
  friend void swap(hash_set&, hash_set&) noexcept;

  bool empty() const noexcept;
  bool empty(bucket_idx) const noexcept;
  void clear() noexcept;

  iterator iterator_to(pointer) noexcept;
  const_iterator iterator_to(const_pointer) const noexcept;

  static bool is_linked(const_pointer) noexcept;
  bool link_front(pointer) noexcept;
  bool link_back(pointer) noexcept;
  bool unlink(pointer) noexcept;
  bool unlink(const_iterator) noexcept;

  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  reverse_iterator rbegin() noexcept;
  reverse_iterator rend() noexcept;
  const_reverse_iterator rbegin() const noexcept;
  const_reverse_iterator rend() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;
  const_reverse_iterator crbegin() const noexcept;
  const_reverse_iterator crend() const noexcept;

  iterator begin(bucket_idx) noexcept;
  iterator end(bucket_idx) noexcept;
  const_iterator begin(bucket_idx) const noexcept;
  const_iterator end(bucket_idx) const noexcept;
  reverse_iterator rbegin(bucket_idx) noexcept;
  reverse_iterator rend(bucket_idx) noexcept;
  const_reverse_iterator rbegin(bucket_idx) const noexcept;
  const_reverse_iterator rend(bucket_idx) const noexcept;
  const_iterator cbegin(bucket_idx) const noexcept;
  const_iterator cend(bucket_idx) const noexcept;
  const_reverse_iterator crbegin(bucket_idx) const noexcept;
  const_reverse_iterator crend(bucket_idx) const noexcept;

  static constexpr bucket_idx hashcode_2_bucket(size_t) noexcept;
  static bucket_idx hash(const_reference) noexcept;

  bucket get_bucket(bucket_idx) noexcept;
  const_bucket get_bucket(bucket_idx) const noexcept;

 private:
  static bucket_idx hash(const_pointer) noexcept;

  list_t list_;
  typename list_t::iterator buckets_[Buckets + 1U];
};

template<typename T, size_t Buckets, typename Tag>
class hash_set<T, Buckets, Tag>::bucket {
  friend hash_set<T, Buckets, Tag>;
  friend hash_set<T, Buckets, Tag>::const_bucket;

 public:
  using bucket_idx = typename hash_set::bucket_idx;
  using iterator = typename hash_set::iterator;
  using const_iterator = typename hash_set::const_iterator;

  bucket() = default;
  bucket(const bucket&) = default;
  bucket& operator=(const bucket&) = default;

  iterator begin() const noexcept;
  iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  bucket_idx index() const noexcept;

 private:
  bucket(hash_set&, bucket_idx) noexcept;

  hash_set* hl_;
  bucket_idx idx_;
};

template<typename T, size_t Buckets, typename Tag>
class hash_set<T, Buckets, Tag>::const_bucket {
  friend hash_set<T, Buckets, Tag>;

 public:
  using bucket_idx = typename hash_set::bucket_idx;
  using iterator = typename hash_set::const_iterator;
  using const_iterator = typename hash_set::const_iterator;

  const_bucket() = default;
  const_bucket(const const_bucket&) = default;
  const_bucket& operator=(const const_bucket&) = default;
  const_bucket(const bucket&) noexcept;

  iterator begin() const noexcept;
  iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  bucket_idx index() const noexcept;

 private:
  const_bucket(const hash_set&, bucket_idx) noexcept;

  const hash_set* hl_;
  bucket_idx idx_;
};


}} /* namespace __cxxabiv1::ext */

#include <abi/ext/hash_set-inl.h>

#endif /* _ABI_EXT_HASH_SET_H_ */
