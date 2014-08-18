#ifndef _LIST_INL_H_
#define _LIST_INL_H_

#include <algorithm>
#include <functional>
#include <vector>
#include <stdimpl/heap_array.h>

_namespace_begin(std)
namespace impl {

template<typename T, typename Tag>
template<typename... Args>
list_elem<T, Tag>::list_elem(Args&&... args)
: value(forward<Args>(args)...)
{}

} /* namespace std::impl */


template<typename T, typename A>
list<T, A>::list(const allocator_type& alloc)
: alloc_base(alloc)
{}

template<typename T, typename A>
list<T, A>::list(size_type n, const allocator_type& alloc)
: list(alloc)
{
  while (n-- > 0) emplace_back();
}

template<typename T, typename A>
list<T, A>::list(size_type n, const_reference v, const allocator_type& alloc)
: list(alloc)
{
  fill_n(back_inserter(*this), n, v);
}

template<typename T, typename A>
template<typename InputIterator>
list<T, A>::list(InputIterator b, InputIterator e, const allocator_type& alloc)
: list(alloc)
{
  copy(b, e, back_inserter(*this));
}

template<typename T, typename A>
list<T, A>::list(const list& other)
: list(other.begin(), other.end(), other.get_allocator_for_copy_())
{}

template<typename T, typename A>
list<T, A>::list(list&& other)
: alloc_base(other)
{
  using _namespace(std)::swap;

  swap(data_, other.data_);
  swap(size_, other.size_);
}

template<typename T, typename A>
list<T, A>::list(const list& other, const allocator_type& alloc)
: list(other.begin(), other.end(), alloc)
{}

template<typename T, typename A>
list<T, A>::list(list&& other, const allocator_type& alloc)
: list(alloc)
{
  if (this->get_allocator_() == other.get_allocator_()) {
    using _namespace(std)::swap;

    swap(data_, other.data_);
    swap(size_, other.size_);
  } else {
    move(other.begin(), other.end(), back_inserter(*this));
  }
}

template<typename T, typename A>
list<T, A>::list(initializer_list<T> il, const allocator_type& alloc)
: list(il.begin(), il.end(), alloc)
{}

template<typename T, typename A>
list<T, A>::~list() noexcept {
  clear();
}

template<typename T, typename A>
auto list<T, A>::operator=(const list& other) -> list& {
  if (&other != this) assign(other.begin(), other.end());
  return *this;
}

template<typename T, typename A>
auto list<T, A>::operator=(list&& other) -> list& {
  assert(&other != this);

  if (this->get_allocator_() == other.get_allocator_()) {
    using _namespace(std)::swap;

    swap(data_, other.data_);
    swap(size_, other.size_);
    other.clear();
  } else {
    clear();
    move(other.begin(), other.end(), back_inserter(*this));
  }
  return *this;
}

template<typename T, typename A>
auto list<T, A>::operator=(initializer_list<T> il) -> list& {
  assign(il);
  return *this;
}

template<typename T, typename A>
template<typename InputIterator>
auto list<T, A>::assign(InputIterator b, InputIterator e) -> void {
  clear();
  copy(b, e, back_inserter(*this));
}

template<typename T, typename A>
auto list<T, A>::assign(initializer_list<T> il) -> void {
  assign(il.begin(), il.end());
}

template<typename T, typename A>
auto list<T, A>::get_allocator() const noexcept -> allocator_type {
  return this->alloc_base::get_allocator();
}

template<typename T, typename A>
auto list<T, A>::begin() noexcept -> iterator {
  return iterator(data_.begin());
}

template<typename T, typename A>
auto list<T, A>::begin() const noexcept -> const_iterator {
  return const_iterator(data_.begin());
}

template<typename T, typename A>
auto list<T, A>::end() noexcept -> iterator {
  return iterator(data_.end());
}

template<typename T, typename A>
auto list<T, A>::end() const noexcept -> const_iterator {
  return const_iterator(data_.end());
}

template<typename T, typename A>
auto list<T, A>::rbegin() noexcept -> reverse_iterator {
  return reverse_iterator(end());
}

template<typename T, typename A>
auto list<T, A>::rbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(end());
}

template<typename T, typename A>
auto list<T, A>::rend() noexcept -> reverse_iterator {
  return reverse_iterator(begin());
}

template<typename T, typename A>
auto list<T, A>::rend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator(begin());
}

template<typename T, typename A>
auto list<T, A>::cbegin() const noexcept -> const_iterator {
  return begin();
}

template<typename T, typename A>
auto list<T, A>::cend() const noexcept -> const_iterator {
  return end();
}

template<typename T, typename A>
auto list<T, A>::crbegin() const noexcept -> const_reverse_iterator {
  return rbegin();
}

template<typename T, typename A>
auto list<T, A>::crend() const noexcept -> const_reverse_iterator {
  return rend();
}

template<typename T, typename A>
auto list<T, A>::empty() const noexcept -> bool {
  return data_.empty();
}

template<typename T, typename A>
auto list<T, A>::size() const noexcept -> size_type {
  return size_;
}

template<typename T, typename A>
auto list<T, A>::max_size() const noexcept -> size_type {
  return allocator_traits<typename alloc_base::allocator_type>::max_size(
      this->get_allocator_());
}

template<typename T, typename A>
auto list<T, A>::resize(size_type nsz) -> void {
  if (size() < nsz) {
    for (size_type i = size(); i != nsz; ++i)
      emplace_back();
  } else if (size() > nsz) {
    do {
      pop_back();
    } while (size() > nsz);
  }
}

template<typename T, typename A>
auto list<T, A>::resize(size_type nsz, const_reference v) -> void {
  if (size() < nsz)
    insert(end(), nsz - size(), v);
  else if (size() > nsz) {
    do {
      pop_back();
    } while (size() > nsz);
  }
}

template<typename T, typename A>
auto list<T, A>::front() -> reference {
  assert_msg(!empty(),
             "std::list: attempt to dereference front() of empty list");
  return data_.front().value;
}

template<typename T, typename A>
auto list<T, A>::front() const -> const_reference {
  assert_msg(!empty(),
             "std::list: attempt to dereference front() of empty list");
  return data_.front().value;
}

template<typename T, typename A>
auto list<T, A>::back() -> reference {
  assert_msg(!empty(),
             "std::list: attempt to dereference back() of empty list");
  return data_.back().value;
}

template<typename T, typename A>
auto list<T, A>::back() const -> const_reference {
  assert_msg(!empty(),
             "std::list: attempt to dereference back() of empty list");
  return data_.back().value;
}

template<typename T, typename A>
template<typename... Args>
auto list<T, A>::emplace_front(Args&&... args) -> void {
  auto ptr = impl::new_alloc_deleter<T>(this->get_allocator_(),
                                        (empty() ? this : &front()),
                                        forward<Args>(args)...);
  data_.link_front(ptr.release());
  ++size_;
}

template<typename T, typename A>
auto list<T, A>::push_front(const_reference v) -> void {
  emplace_front(v);
}

template<typename T, typename A>
auto list<T, A>::push_front(value_type&& v) -> void {
  emplace_front(move(v));
}

template<typename T, typename A>
auto list<T, A>::pop_front() -> void {
  auto ptr = impl::existing_alloc_deleter(this->get_allocator_(),
                                          data_.unlink_front());
  if (ptr) --size_;
}

template<typename T, typename A>
template<typename... Args>
auto list<T, A>::emplace_back(Args&&... args) -> void {
  auto ptr = impl::new_alloc_deleter<T>(this->get_allocator_(),
                                        (empty() ? this : &back()),
                                        forward<Args>(args)...);
  data_.link_back(ptr.release());
  ++size_;
}

template<typename T, typename A>
auto list<T, A>::push_back(const_reference v) -> void {
  emplace_back(v);
}

template<typename T, typename A>
auto list<T, A>::push_back(value_type&& v) -> void {
  emplace_back(move(v));
}

template<typename T, typename A>
auto list<T, A>::pop_back() -> void {
  auto ptr = impl::existing_alloc_deleter(this->get_allocator_(),
                                          data_.unlink_back());
  if (ptr) --size_;
}


template<typename T, typename A>
template<typename... Args>
auto list<T, A>::emplace(const_iterator i, Args&&... args) -> iterator {
  auto hint = (i != end() ? &*i : (empty() ? this : &back()));
  auto ptr = impl::new_alloc_deleter<T>(this->get_allocator_(),
                                        hint,
                                        forward<Args>(args)...);
  ++size_;
  return iterator(data_.link(i.impl_, ptr.release()));
}

template<typename T, typename A>
auto list<T, A>::insert(const_iterator i, const_reference v) -> iterator {
  return emplace(i, v);
}

template<typename T, typename A>
auto list<T, A>::insert(const_iterator i, value_type&& v) -> iterator {
  return emplace(i, move(v));
}

template<typename T, typename A>
auto list<T, A>::insert(const_iterator i, size_type n, const_reference v) ->
    iterator {
  typename data_type::iterator i_ = data_.nonconst_iterator(i.impl_);

  /* Use temporary storage to create all elements. */
  auto hint = (i != end() ? &*i : (empty() ? this : &back()));
  auto gen_fn = [this, &v, hint&]() {
                  auto rv = impl::new_alloc_deleter<T>(this->get_allocator_(),
                                                       hint, v);
                  hint = rv.get();
                  return rv;
                };
  using gen_t = impl::alloc_deleter_ptr<elem,
                                        typename alloc_base::allocator_type>;
  auto array = impl::heap_array<gen_t>(n);
  generate_n(back_inserter(array), n, move(gen_fn));

  /* Now that all elements have been constructed, link them in. */
  iterator rv = iterator(i_);
  bool first = true;
  for (auto& ptr : array) {
    i_ = data_.link(i_, ptr.release());
    if (exchange(first, false)) rv = iterator(i_);
    ++i_;
  }
  size_ += array.size();
  return rv;
}

template<typename T, typename A>
template<typename InputIterator>
auto list<T, A>::insert(const_iterator i, InputIterator b, InputIterator e) ->
    enable_if_t<is_base_of<forward_iterator_tag,
                           typename iterator_traits<
                               InputIterator>::iterator_category
                          >::value,
               iterator> {
  size_type n = distance(b, e);
  typename data_type::iterator i_ = data_.nonconst_iterator(i.impl_);

  /* Use temporary storage to create all elements. */
  auto hint = (i != end() ? &*i : (empty() ? this : &back()));
  auto gen_fn = [this, &v, hint&]() {
                  auto rv = impl::new_alloc_deleter<T>(this->get_allocator_(),
                                                       hint, move(v));
                  hint = rv.get();
                  return rv;
                };
  using gen_t = impl::alloc_deleter_ptr<elem,
                                        typename alloc_base::allocator_type>;
  using ii_value_type = typename iterator_traits<InputIterator>::value_type;
  auto array = impl::heap_array<gen_t>(n);
  transform(b, e, back_inserter(array), move(gen_fn));

  /* Now that all elements have been constructed, link them in. */
  iterator rv = iterator(i_);
  bool first = true;
  for (auto& ptr : array) {
    i_ = data_.link(i_, ptr.release());
    if (exchange(first, false)) rv = iterator(i_);
    ++i_;
  }
  size_ += array.size();
  return rv;
}

template<typename T, typename A>
template<typename InputIterator>
auto list<T, A>::insert(const_iterator i, InputIterator b, InputIterator e) ->
    enable_if_t<!is_base_of<forward_iterator_tag,
                            typename iterator_traits<
                                InputIterator>::iterator_category
                           >::value,
               iterator> {
  typename data_type::iterator i_ = data_.nonconst_iterator(i.impl_);

  /* Use temporary storage to create all elements. */
  auto hint = (i != end() ? &*i : (empty() ? this : &back()));
  auto gen_fn = [this, &v, hint&]() {
                  auto rv = impl::new_alloc_deleter<T>(this->get_allocator_(),
                                                       hint, move(v));
                  hint = rv.get();
                  return rv;
                };
  using gen_t = impl::alloc_deleter_ptr<elem,
                                        typename alloc_base::allocator_type>;
  using ii_value_type = typename iterator_traits<InputIterator>::value_type;
  auto array = vector<gen_t, temporary_buffer_allocator<gen_t>>();
  transform(b, e, back_inserter(array), move(gen_fn));

  /* Now that all elements have been constructed, link them in. */
  iterator rv = iterator(i_);
  bool first = true;
  for (auto& ptr : array) {
    i_ = data_.link(i_, ptr.release());
    ++size_;

    if (exchange(first, false)) rv = iterator(i_);
    ++i_;
  }
  return rv;
}

template<typename T, typename A>
auto list<T, A>::insert(const_iterator i, initializer_list<T> il) -> iterator {
  return insert(i, il.begin(), il.end());
}

template<typename T, typename A>
auto list<T, A>::erase(const_iterator i) -> iterator {
  assert(i != end());
  typename data_type::iterator i_ = data_.nonconst_iterator(i.impl_);
  iterator rv = iterator(next(i_));

  auto ptr = impl::existing_alloc_deleter(this->get_allocator_(),
                                          data_.unlink(i_));
  if (ptr) --size_;
  return rv;
}

template<typename T, typename A>
auto list<T, A>::erase(const_iterator b, const_iterator e) -> iterator {
  iterator b_ = iterator(data_.nonconst_iterator(b.impl_));
  while (b_ != e) b_ = erase(b_);
  return b_;
}

template<typename T, typename A>
auto list<T, A>::swap(list& other) -> void {
  using _namespace(std)::swap;

  this->alloc_base::swap_(other);
  data_.swap(other.data_);
  swap(size_, other.size_);
}

template<typename T, typename A>
auto list<T, A>::clear() noexcept -> void {
  while (!empty()) pop_back();
}

template<typename T, typename A>
auto list<T, A>::splice(const_iterator i, list& other) noexcept -> void {
  assert(this != &other);
  assert(this->get_allocator_() == other.get_allocator_());

  data_type::splice(i.impl_, other.data_);
  size_ += exchange(other.size_, 0);
}

template<typename T, typename A>
auto list<T, A>::splice(const_iterator i, list&& other) noexcept -> void {
  splice(i, other);
}

template<typename T, typename A>
auto list<T, A>::splice(const_iterator i, list& other, const_iterator o_i)
    noexcept -> void {
  if (i == o_i) return;
  assert(this->get_allocator_() == other.get_allocator_());

  data_.link(i.impl_, other.data_.unlink(o_i.impl_));
  if (&other != this) {
    ++size_;
    --other.size_;
  }
}

template<typename T, typename A>
auto list<T, A>::splice(const_iterator i, list&& other, const_iterator o_i)
    noexcept -> void {
  splice(i, other, o_i);
}

template<typename T, typename A>
auto list<T, A>::splice(const_iterator i, list& other,
                        const_iterator o_b, const_iterator o_e)
    noexcept -> void {
  assert(this->get_allocator_() == other.get_allocator_());

  data_type::splice(i.impl_, o_b.impl_, o_e.impl_);
  if (&other != this) {
    const size_type delta = distance(o_b, o_e);
    size_ += delta;
    other.size_ -= delta;
  }
}

template<typename T, typename A>
auto list<T, A>::splice(const_iterator i, list&& other,
                        const_iterator o_b, const_iterator o_e)
    noexcept -> void {
  splice(i, other, o_b, o_e);
}

template<typename T, typename A>
auto list<T, A>::remove(const_reference v) -> void {
  using placeholders::_1;

  remove_if(bind(equal_to<value_type>(), _1, ref(v)));
}

template<typename T, typename A>
template<typename Predicate>
auto list<T, A>::remove_if(Predicate predicate) -> void {
  if (empty()) return;

  const_iterator i = begin();
  while (i != end()) {
    if (predicate(*i))
      i = erase(i);
    else
      ++i;
  }
}

template<typename T, typename A>
auto list<T, A>::unique() -> void {
  unique(equal_to<value_type>());
}

template<typename T, typename A>
template<typename Predicate>
auto list<T, A>::unique(Predicate predicate) -> void {
  using placeholders::_1;

  const_iterator b = begin();
  while (b != end()) {
    const_iterator e = find_if_not(next(b), cend(),
                                   bind(ref(predicate), ref(*b), _1));
    b = erase(next(b), e);
  }
}

template<typename T, typename A>
auto list<T, A>::merge(list& other) -> void {
  merge(other, less<value_type>());
}

template<typename T, typename A>
auto list<T, A>::merge(list&& other) -> void {
  merge(move(other), less<value_type>());
}

template<typename T, typename A>
template<typename Compare>
auto list<T, A>::merge(list& other, Compare compare) -> void {
  class size_fixer {
   public:
    size_fixer(list& self, list& victim) : self_(self), victim_(victim) {}
    size_fixer(const size_fixer&) = delete;
    size_fixer& operator=(const size_fixer&) = delete;

    ~size_fixer() noexcept {
      if (commit_) {
        self_.size_ += exchange(victim_.size_, 0);
      } else {
        /* Aborted somewhere halfway... */
        self_.size_ = distance(self_.begin(), self_.end());
        victim_.size_ = distance(victim_.begin(), victim_.end());
      }
    }

    void commit() noexcept { commit_ = true; }

   private:
    list& self_;
    list& victim_;
    bool commit_ = false;
  };

  if (&other == this) return;
  assert(this->get_allocator_() == other.get_allocator_());

  size_fixer fixer{ *this, other };
  data_type::merge(data_.cbegin(), data_.cend(),
                   other.data_.cbegin(), other.data_.cend(),
                   [&compare](const elem& a, const elem& b) {
                     return compare(a.value, b.value);
                   });

  /*
   * Since merge_ does not update the size
   * (performance win, since it means we won't call distance()),
   * we need to fix the size now.
   * Fixer will do this for us in its destructor.
   */
  fixer.commit();
}

template<typename T, typename A>
template<typename Compare>
auto list<T, A>::merge(list&& other, Compare compare) -> void {
  merge(other, ref(compare));
}

template<typename T, typename A>
auto list<T, A>::sort() -> void {
  sort(less<value_type>());
}

template<typename T, typename A>
template<typename Compare>
auto list<T, A>::sort(Compare compare) -> void {
  assert(size() == size_type(distance(begin(), end())));
  data_type::sort(data_.cbegin(), data_.cend(),
                  [&compare](const elem& a, const elem& b) {
                    return compare(a.value, b.value);
                  },
                  size());
}

template<typename T, typename A>
auto list<T, A>::reverse() noexcept -> void {
  data_.reverse();
}


template<typename T, typename A>
list<T, A>::iterator::iterator(typename data_type::iterator impl) noexcept
: impl_(impl)
{}

template<typename T, typename A>
auto list<T, A>::iterator::operator*() const noexcept -> T& {
  return impl_->value;
}

template<typename T, typename A>
auto list<T, A>::iterator::operator->() const noexcept -> T* {
  return &impl_->value;
}

template<typename T, typename A>
auto list<T, A>::iterator::operator++() noexcept -> iterator& {
  ++impl_;
  return *this;
}

template<typename T, typename A>
auto list<T, A>::iterator::operator++(int) noexcept -> iterator {
  iterator tmp = *this;
  ++impl_;
  return tmp;
}

template<typename T, typename A>
auto list<T, A>::iterator::operator--() noexcept -> iterator& {
  --impl_;
  return *this;
}

template<typename T, typename A>
auto list<T, A>::iterator::operator--(int) noexcept -> iterator {
  iterator tmp = *this;
  --impl_;
  return tmp;
}

template<typename T, typename A>
auto list<T, A>::iterator::operator==(const iterator& o)
    const noexcept -> bool {
  return impl_ == o.impl_;
}

template<typename T, typename A>
auto list<T, A>::iterator::operator!=(const iterator& o)
    const noexcept -> bool {
  return !(*this == o);
}

template<typename T, typename A>
auto list<T, A>::iterator::operator==(const const_iterator& o)
    const noexcept -> bool {
  return impl_ == o.impl_;
}

template<typename T, typename A>
auto list<T, A>::iterator::operator!=(const const_iterator& o)
    const noexcept -> bool {
  return !(*this == o);
}


template<typename T, typename A>
list<T, A>::const_iterator::const_iterator(
    typename data_type::const_iterator impl) noexcept
: impl_(impl)
{}

template<typename T, typename A>
list<T, A>::const_iterator::const_iterator(const iterator& i) noexcept
: impl_(i.impl_)
{}

template<typename T, typename A>
auto list<T, A>::const_iterator::operator*() const noexcept -> const T& {
  return impl_->value;
}

template<typename T, typename A>
auto list<T, A>::const_iterator::operator->() const noexcept -> const T* {
  return &impl_->value;
}

template<typename T, typename A>
auto list<T, A>::const_iterator::operator++() noexcept -> const_iterator& {
  ++impl_;
  return *this;
}

template<typename T, typename A>
auto list<T, A>::const_iterator::operator++(int) noexcept -> const_iterator {
  const_iterator tmp = *this;
  ++impl_;
  return tmp;
}

template<typename T, typename A>
auto list<T, A>::const_iterator::operator--() noexcept -> const_iterator& {
  --impl_;
  return *this;
}

template<typename T, typename A>
auto list<T, A>::const_iterator::operator--(int) noexcept -> const_iterator {
  const_iterator tmp = *this;
  --impl_;
  return tmp;
}

template<typename T, typename A>
auto list<T, A>::const_iterator::operator==(const iterator& o)
    const noexcept -> bool {
  return impl_ == o.impl_;
}

template<typename T, typename A>
auto list<T, A>::const_iterator::operator!=(const iterator& o)
    const noexcept -> bool {
  return !(*this == o);
}

template<typename T, typename A>
auto list<T, A>::const_iterator::operator==(const const_iterator& o)
    const noexcept -> bool {
  return impl_ == o.impl_;
}

template<typename T, typename A>
auto list<T, A>::const_iterator::operator!=(const const_iterator& o)
    const noexcept -> bool {
  return !(*this == o);
}


template<typename T, typename A>
bool operator==(const list<T, A>& a, const list<T, A>& b) {
  return a.size() == b.size() && equal(a.begin(), a.end(), b.begin(), b.end());
}

template<typename T, typename A>
bool operator!=(const list<T, A>& a, const list<T, A>& b) {
  return !(a == b);
}

template<typename T, typename A>
bool operator<(const list<T, A>& a, const list<T, A>& b) {
  return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template<typename T, typename A>
bool operator>(const list<T, A>& a, const list<T, A>& b) {
  return b < a;
}

template<typename T, typename A>
bool operator<=(const list<T, A>& a, const list<T, A>& b) {
  return !(b < a);
}

template<typename T, typename A>
bool operator>=(const list<T, A>& a, const list<T, A>& b) {
  return !(a < b);
}

template<typename T, typename A>
void swap(list<T, A>& a, list<T, A>& b) {
  a.swap(b);
}


_namespace_end(std)

#endif /* _LIST_INL_H_ */
