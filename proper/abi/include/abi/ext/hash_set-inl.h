namespace __cxxabiv1 {
namespace ext {


template<typename T, size_t Buckets, typename Tag>
hash_set<T, Buckets, Tag>::hash_set() noexcept {
  for (bucket_idx i = 0U; i <= Buckets; ++i) buckets_[i] = list_.end();
}

template<typename T, size_t Buckets, typename Tag>
hash_set<T, Buckets, Tag>::hash_set(hash_set&& o) noexcept
: hash_set()
{
  swap(*this, o);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::operator=(hash_set&& o) noexcept -> hash_set& {
  clear();
  swap(*this, o);
  return *this;
}

template<typename T, size_t Buckets, typename Tag>
auto swap(hash_set<T, Buckets, Tag>& a, hash_set<T, Buckets, Tag>& b)
    noexcept -> void {
  typedef typename hash_set<T, Buckets, Tag>::bucket_idx bucket_idx;

  for (bucket_idx i = 0U; i <= Buckets; ++i) {
    const auto ai = a.buckets_[i];
    const auto bi = b.buckets_[i];

    b.buckets_[i] = (ai == a.list_.end() ? b.list_.end() : ai);
    a.buckets_[i] = (bi == b.list_.end() ? a.list_.end() : bi);
  }

  swap(a.list_, b.list_);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::empty() const noexcept -> bool {
  return list_.empty();
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::empty(bucket_idx b) const noexcept -> bool {
  if (b >= Buckets) return true;
  return buckets_[b] == buckets_[b + 1U];
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::clear() noexcept -> void {
  list_.clear();
  for (bucket_idx i = 0U; i <= Buckets; ++i) buckets_[i] = list_.end();
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::iterator_to(pointer e) noexcept -> iterator {
  return list_.iterator_to(e);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::iterator_to(const_pointer e) const noexcept ->
    const_iterator {
  return list_.iterator_to(e);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::is_linked(const_pointer e) noexcept -> bool {
  return list_t::is_linked(e);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::link_front(pointer e) noexcept -> bool {
  const bucket_idx b = hash(*e);

  if (!list_.link_before(e, buckets_[b])) return false;
  for (bucket_idx i = 0U; i < b; ++i)
    if (buckets_[i] == buckets_[b]) buckets_[i] = iterator_to(e);
  buckets_[b] = iterator_to(e);
  return true;
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::link_back(pointer e) noexcept -> bool {
  const bucket_idx b = hash(e);

  if (!list_.link_before(e, buckets_[b + 1U])) return false;
  if (buckets_[b] == list_.end()) {
    for (bucket_idx i = 0U; i < b; ++i)
      if (buckets_[i] == buckets_[b + 1U]) buckets_[i] = iterator_to(e);
    buckets_[b] = iterator_to(e);
  }
  return true;
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::unlink(pointer ee) noexcept -> bool {
  if (!ee) return false;
  return unlink(iterator_to(ee));
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::unlink(const_iterator e) noexcept -> bool {
  const bucket_idx b = hash(*e);
  iterator s = iterator_to(const_cast<pointer>(&*e));
  ++s;

  if (!unlink(e)) return false;

  for (bucket_idx i = 0U; i <= b; ++i) if (buckets_[i] == e) buckets_[i] = s;
  return true;
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::begin() noexcept -> iterator {
  return list_.begin();
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::end() noexcept -> iterator {
  return list_.end();
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::begin() const noexcept -> const_iterator {
  return list_.begin();
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::end() const noexcept -> const_iterator {
  return list_.end();
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rbegin() noexcept -> reverse_iterator {
  return list_.rbegin();
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rend() noexcept -> reverse_iterator {
  return list_.rend();
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rbegin() const noexcept ->
    const_reverse_iterator {
  return list_.rbegin();
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rend() const noexcept ->
    const_reverse_iterator {
  return list_.rend();
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::cbegin() const noexcept -> const_iterator {
  return begin();
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::cend() const noexcept -> const_iterator {
  return end();
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::crbegin() const noexcept ->
    const_reverse_iterator {
  return rbegin();
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::crend() const noexcept ->
    const_reverse_iterator {
  return rend();
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::begin(bucket_idx b) noexcept -> iterator {
  if (b >= Buckets) return end();
  return buckets_[b];
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::end(bucket_idx b) noexcept -> iterator {
  if (b >= Buckets) return end();
  return buckets_[b + 1U];
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::begin(bucket_idx b) const noexcept ->
    const_iterator {
  if (b >= Buckets) return end();
  return buckets_[b];
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::end(bucket_idx b) const noexcept ->
    const_iterator {
  if (b >= Buckets) return end();
  return buckets_[b + 1U];
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rbegin(bucket_idx b) noexcept ->
    reverse_iterator {
  if (b >= Buckets) return rend();
  auto i = buckets_[b + 1U];
  return --i;
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rend(bucket_idx b) noexcept ->
    reverse_iterator {
  if (b >= Buckets) return rend();
  auto i = buckets_[b];
  return --i;
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rbegin(bucket_idx b) const noexcept ->
    const_reverse_iterator {
  if (b >= Buckets) return rend();
  auto i = buckets_[b + 1U];
  return --i;
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rend(bucket_idx b) const noexcept ->
    const_reverse_iterator {
  if (b >= Buckets) return rend();
  auto i = buckets_[b];
  return --i;
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::cbegin(bucket_idx b) const noexcept ->
    const_iterator {
  return begin(b);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::cend(bucket_idx b) const noexcept ->
    const_iterator {
  return end(b);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::crbegin(bucket_idx b) const noexcept ->
    const_reverse_iterator {
  return rbegin(b);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::crend(bucket_idx b) const noexcept ->
    const_reverse_iterator {
  return rend(b);
}

template<typename T, size_t Buckets, typename Tag>
constexpr auto hash_set<T, Buckets, Tag>::hashcode_2_bucket(size_t h)
    noexcept -> bucket_idx {
  return h % Buckets;
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::hash(const_reference e) noexcept ->
    bucket_idx {
  return hashcode_2_bucket(hash_code(e));
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::hash(const_pointer e) noexcept ->
    bucket_idx {
  return (e ? hash(*e) : 0U);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::get_bucket(bucket_idx idx) noexcept -> bucket {
  return bucket{ *this, idx };
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::get_bucket(bucket_idx idx) const noexcept ->
    const_bucket {
  return const_bucket{ *this, idx };
}


template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::bucket::begin() const noexcept -> iterator {
  return hl_->begin(idx_);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::bucket::end() const noexcept -> iterator {
  return hl_->end(idx_);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::bucket::cbegin() const noexcept ->
    const_iterator {
  return hl_->cbegin(idx_);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::bucket::cend() const noexcept ->
    const_iterator {
  return hl_->cend(idx_);
}

template<typename T, size_t Buckets, typename Tag>
hash_set<T, Buckets, Tag>::bucket::bucket(hash_set& hl, bucket_idx idx)
    noexcept
: hl_(&hl),
  idx_(idx)
{}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::bucket::index() const noexcept -> bucket_idx {
  return idx_;
}


template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::const_bucket::begin() const noexcept ->
    iterator {
  return hl_->begin(idx_);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::const_bucket::end() const noexcept ->
    iterator {
  return hl_->end(idx_);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::const_bucket::cbegin() const noexcept ->
    const_iterator {
  return hl_->cbegin(idx_);
}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::const_bucket::cend() const noexcept ->
    const_iterator {
  return hl_->cend(idx_);
}

template<typename T, size_t Buckets, typename Tag>
hash_set<T, Buckets, Tag>::const_bucket::const_bucket(
    const hash_set& hl, bucket_idx idx) noexcept
: hl_(&hl),
  idx_(idx)
{}

template<typename T, size_t Buckets, typename Tag>
hash_set<T, Buckets, Tag>::const_bucket::const_bucket(
    const bucket& sibling) noexcept
: hl_(sibling.hl_),
  idx_(sibling.idx_)
{}

template<typename T, size_t Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::const_bucket::index() const noexcept ->
    bucket_idx {
  return idx_;
}


}} /* namespace __cxxabiv1::ext */
