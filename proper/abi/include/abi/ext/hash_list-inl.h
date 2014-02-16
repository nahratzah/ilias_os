namespace __cxxabiv1 {
namespace ext {


template<typename T, unsigned int Buckets, typename Tag>
hash_set<T, Buckets, Tag>::hash_set() noexcept {
  for (bucket_idx i = 0U; i <= Buckets; ++i) buckets_[i] = list_.end();
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::empty() const noexcept -> bool {
  return list_.empty();
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::empty(bucket_idx b) const noexcept -> bool {
  if (b >= Buckets) return true;
  return buckets_[b] == buckets_[b + 1U];
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::link_front(pointer e) noexcept -> bool {
  const bucket_idx b = hash(e);

  if (!list_.insert_before(e, buckets_[b])) return false;
  for (bucket_idx i = 0U; i <= b; ++i)
    if (buckets_[i] == buckets_[b]) buckets_[i] = e;
  return true;
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::link_back(pointer e) noexcept -> bool {
  const bucket_idx b = hash(e);

  if (!list_.insert_before(e, buckets_[b + 1U])) return false;
  for (bucket_idx i = 0U; i <= b; ++i)
    if (buckets_[i] == buckets_[b + 1U]) buckets_[i] = e;
  return true;
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::unlink(pointer ee) noexcept -> bool {
  if (!ee) return false;
  return unlink(iterator_to(ee));
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::unlink(const_iterator ee) noexcept -> bool {
  const typename list_t::const_iterator& e = ee;
  const bucket_idx b = hash(*e);
  const typename list_t::iterator s =
      ++list_t::iterator_to(const_cast<pointer>(&*ee));

  if (!unlink(e)) return false;

  for (bucket_idx i = 0U; i <= b; ++i) if (buckets_[i] == e) buckets_[i] = s;
  return true;
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::begin() noexcept -> iterator {
  return iterator{ list_.begin() };
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::end() noexcept -> iterator {
  return iterator{ list_.end() };
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::begin() const noexcept -> const_iterator {
  return const_iterator{ list_.begin() };
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::end() const noexcept -> const_iterator {
  return const_iterator{ list_.end() };
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rbegin() noexcept -> reverse_iterator {
  return reverse_iterator{ list_.rbegin() };
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rend() noexcept -> reverse_iterator {
  return reverse_iterator{ list_.rend() };
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rbegin() const noexcept ->
    const_reverse_iterator {
  return const_reverse_iterator{ list_.rbegin() };
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rend() const noexcept ->
    const_reverse_iterator {
  return const_reverse_iterator{ list_.rend() };
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::cbegin() const noexcept -> const_iterator {
  return begin();
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::cend() const noexcept -> const_iterator {
  return end();
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::crbegin() const noexcept ->
    const_reverse_iterator {
  return rbegin();
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::crend() const noexcept ->
    const_reverse_iterator {
  return rend();
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::begin(bucket_idx b) noexcept -> iterator {
  if (b >= Buckets) return end();
  return buckets_[b];
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::end(bucket_idx b) noexcept -> iterator {
  if (b >= Buckets) return end();
  return buckets_[b + 1U];
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::begin(bucket_idx b) const noexcept ->
    const_iterator {
  if (b >= Buckets) return end();
  return buckets_[b];
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::end(bucket_idx b) const noexcept ->
    const_iterator {
  if (b >= Buckets) return end();
  return buckets_[b + 1U];
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rbegin(bucket_idx b) noexcept ->
    reverse_iterator {
  if (b >= Buckets) return rend();
  auto i = buckets_[b + 1U];
  return --i;
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rend(bucket_idx b) noexcept ->
    reverse_iterator {
  if (b >= Buckets) return rend();
  auto i = buckets_[b];
  return --i;
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rbegin(bucket_idx b) const noexcept ->
    const_reverse_iterator {
  if (b >= Buckets) return rend();
  auto i = buckets_[b + 1U];
  return --i;
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::rend(bucket_idx b) const noexcept ->
    const_reverse_iterator {
  if (b >= Buckets) return rend();
  auto i = buckets_[b];
  return --i;
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::cbegin(bucket_idx b) const noexcept ->
    const_iterator {
  return begin(b);
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::cend(bucket_idx b) const noexcept ->
    const_iterator {
  return end(b);
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::crbegin(bucket_idx b) const noexcept ->
    const_reverse_iterator {
  return rbegin(b);
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::crend(bucket_idx b) const noexcept ->
    const_reverse_iterator {
  return rend(b);
}

template<typename T, unsigned int Buckets, typename Tag>
constexpr auto hash_set<T, Buckets, Tag>::hashcode_2_bucket(size_t h)
    noexcept -> bucket_idx {
  return h % Buckets;
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::hash(const_reference e) noexcept ->
    bucket_idx {
  return hashcode_2_bucket(hash_code(e));
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::hash(const_pointer e) noexcept ->
    bucket_idx {
  return (e ? hash(*e) : 0U);
}


template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::bucket::begin() const noexcept -> iterator {
  return hl_->begin(idx_);
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::bucket::end() const noexcept -> iterator {
  return hl_->end(idx_);
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::bucket::cbegin() const noexcept ->
    const_iterator {
  return hl_->cbegin(idx_);
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::bucket::cend() const noexcept ->
    const_iterator {
  return hl_->cend(idx_);
}

template<typename T, unsigned int Buckets, typename Tag>
hash_set<T, Buckets, Tag>::bucket::bucket(hash_set& hl, bucket_idx idx)
    noexcept
: hl_(&hl),
  idx_(idx)
{}


template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::const_bucket::begin() const noexcept ->
    iterator {
  return hl_->begin(idx_);
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::const_bucket::end() const noexcept ->
    iterator {
  return hl_->end(idx_);
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::const_bucket::cbegin() const noexcept ->
    const_iterator {
  return hl_->cbegin(idx_);
}

template<typename T, unsigned int Buckets, typename Tag>
auto hash_set<T, Buckets, Tag>::const_bucket::cend() const noexcept ->
    const_iterator {
  return hl_->cend(idx_);
}

template<typename T, unsigned int Buckets, typename Tag>
hash_set<T, Buckets, Tag>::const_bucket::const_bucket(
    const hash_set& hl, bucket_idx idx) noexcept
: hl_(&hl),
  idx_(idx)
{}

template<typename T, unsigned int Buckets, typename Tag>
hash_set<T, Buckets, Tag>::const_bucket::const_bucket(
    const bucket& sibling) noexcept
: hl_(sibling.hl_),
  idx_(sibling.idx_)
{}


}} /* namespace __cxxabiv1::ext */
