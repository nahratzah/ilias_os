#ifndef _ILIAS_VM_VMMAP_INL_H_
#define _ILIAS_VM_VMMAP_INL_H_

#include <ilias/vm/vmmap.h>

namespace ilias {
namespace vm {


constexpr auto vm_permission::operator==(const vm_permission& y)
    const noexcept -> bool {
  return perm_ == y.perm_;
}

constexpr auto vm_permission::operator!=(const vm_permission& y)
    const noexcept -> bool {
  return perm_ != y.perm_;
}

auto vm_permission::operator&=(const vm_permission& y) noexcept ->
    vm_permission& {
  return *this = (*this & y);
}

auto vm_permission::operator|=(const vm_permission& y) noexcept ->
    vm_permission& {
  return *this = (*this | y);
}

auto vm_permission::operator^=(const vm_permission& y) noexcept ->
    vm_permission& {
  return *this = (*this ^ y);
}

constexpr vm_permission::operator bool() const noexcept {
  return perm_ != perm_type::none;
}


template<arch Arch>
vmmap_shard<Arch>::entry::entry(const entry& e)
: data_(get<0>(e.data_), get<1>(e.data_), get<2>(e.data_), get<3>(e.data_),
        get<4>(e.data_)->clone())
{}

template<arch Arch>
vmmap_shard<Arch>::entry::entry(entry&& e) noexcept
: data_(get<0>(e.data_), get<1>(e.data_), get<2>(e.data_), get<3>(e.data_),
        move(get<4>(e.data_)))
{}

template<arch Arch>
vmmap_shard<Arch>::entry::entry(range r, vpage_no<Arch> free_end,
                                vm_permission perm,
                                unique_ptr<vmmap_entry>&& data) noexcept
: data_(get<0>(r), get<1>(r), free_end - (get<0>(r) + get<1>(r)), perm,
        move(data))
{
  assert(free_end >= get<0>(r) + get<1>(r));
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::operator=(const entry& e) -> entry& {
  entry tmp{ e };
  swap(*this, tmp);
  return *this;
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::operator=(entry&& e) noexcept -> entry& {
  entry tmp{ move(e) };
  swap(*this, tmp);
  return *this;
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::get_range_used() const noexcept -> range {
  return make_tuple(get<0>(data_), get<1>(data_));
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::get_range_free() const noexcept -> range {
  auto used = get_range_used();
  return make_tuple(get<0>(used) + get<1>(used), get<2>(data_));
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::get_permission() const noexcept ->
    vm_permission {
  return get<3>(data_);
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::get_addr_used() const noexcept ->
    vpage_no<Arch> {
  return get<0>(get_range_used());
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::get_addr_free() const noexcept ->
    vpage_no<Arch> {
  return get<0>(get_range_free());
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::get_addr_end() const noexcept ->
    vpage_no<Arch> {
  range r = get_range_free();
  return get<0>(r) + get<1>(r);
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::update_end(vpage_no<Arch> e) noexcept -> void {
  assert(e >= get_addr_free());
  get<2>(data_) = e - (get<0>(data_) + get<1>(data_));
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::unused() const noexcept -> bool {
  return get<1>(data_) == page_count<Arch>(0);
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::split(vpage_no<Arch> at) const ->
    pair<unique_ptr<entry>, unique_ptr<entry>> {
  assert(at > get_addr_used() && at < get_addr_end());

  unique_ptr<entry> e0, e1;
  if (at >= get_addr_free()) {
    e0 = make_unique<entry>(get_range_used(), at, get_permission(),
                            data().clone());
    e0->update_end(at);
    e1 = make_unique<entry>(range(at, page_count<Arch>(0)), get_addr_end(),
                            get_permission(), nullptr);
  } else {
    page_count<Arch> npg_arch = at - get_addr_used();
    page_count<native_arch> npg_native =
        page_count<native_arch>(npg_arch.get());
    assert(npg_native.get() == npg_arch.get());  // Catch overflow.

    std::unique_ptr<vmmap_entry> d0, d1;
    tie(d0, d1) = data().split(npg_native);
    e0 = make_unique<entry>(range(get_addr_used(), at - get_addr_used()), at,
                            get_permission(), move(d0));
    e1 = make_unique<entry>(range(at, get_addr_free() - at), get_addr_end(),
                            get_permission(), move(d1));
  }

  return { move(e0), move(e1) };
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::data() const noexcept -> vmmap_entry& {
  const auto& ptr = get<4>(data_);
  assert(ptr != nullptr);
  return *ptr;
}


template<arch Arch>
auto vmmap_shard<Arch>::entry_before_::operator()(const entry& x,
                                                  const entry& y)
    const noexcept -> bool {
  return x.get_addr_used() < y.get_addr_used();
}

template<arch Arch>
auto vmmap_shard<Arch>::entry_before_::operator()(const entry& x,
                                                  const isect_& y)
    const noexcept -> bool {
  return x.get_addr_end() <= y.begin;
}

template<arch Arch>
auto vmmap_shard<Arch>::entry_before_::operator()(const isect_& x,
                                                  const entry& y)
    const noexcept -> bool {
  return x.end <= y.get_addr_used();
}


template<arch Arch>
auto vmmap_shard<Arch>::free_before_::unwrap_(const entry& e)
    noexcept -> page_count<Arch> {
  return get<1>(e.get_range_free());
}

template<arch Arch>
auto vmmap_shard<Arch>::free_before_::unwrap_(const page_count<Arch>& c)
    noexcept -> const page_count<Arch>& {
  return c;
}

template<arch Arch>
template<typename T, typename U>
auto vmmap_shard<Arch>::free_before_::operator()(const T& x, const U& y)
    const noexcept -> bool {
  return unwrap_(x) < unwrap_(y);
}


template<arch Arch>
vmmap_shard<Arch>::vmmap_shard(const vmmap_shard<Arch>& rhs)
: vmmap_shard()
{
  for (const auto& i : rhs.entries_)
    link_(make_unique<entry>(i));
}

template<arch Arch>
vmmap_shard<Arch>::vmmap_shard(vmmap_shard<Arch>&& rhs) noexcept
: entries_(move(rhs.entries_))
{}

template<arch Arch>
vmmap_shard<Arch>::vmmap_shard(range r)
: vmmap_shard()
{
  manage(r);
}

template<arch Arch>
vmmap_shard<Arch>::vmmap_shard(vpage_no<Arch> p, page_count<Arch> npg)
: vmmap_shard()
{
  manage(p, npg);
}

template<arch Arch>
vmmap_shard<Arch>::vmmap_shard(vpage_no<Arch> b, vpage_no<Arch> e)
: vmmap_shard()
{
  manage(b, e);
}

template<arch Arch>
vmmap_shard<Arch>::~vmmap_shard() noexcept {
  entries_.unlink_all(&entry_deletor_);
}

template<arch Arch>
auto vmmap_shard<Arch>::get_range() const noexcept -> range {
  if (_predict_false(entries_.empty()))
    return make_tuple(vpage_no<Arch>(0), page_count<Arch>(0));

  const vpage_no<Arch> start = entries_.begin()->get_addr_used();
  const range end_range = entries_.end()->get_range_free();
  const auto end = get<0>(end_range) + get<1>(end_range);
  return make_tuple(start, end - start);
}

template<arch Arch>
auto vmmap_shard<Arch>::manage(range r) -> void {
  manage(get<0>(r), get<1>(r));
}

template<arch Arch>
auto vmmap_shard<Arch>::manage(vpage_no<Arch> p, page_count<Arch> npg) ->
    void {
  manage(p, p + npg);
}

template<arch Arch>
auto vmmap_shard<Arch>::manage(vpage_no<Arch> b, vpage_no<Arch> e) -> void {
  auto b_present = entries_.lower_bound(isect_(b, b));
  if (b_present != entries_.end())
    b = min(b, b_present->get_addr_used());

  while (b_present != entries_.end() && b < e) {
    if (b_present->get_addr_used() != b) {
      auto new_e_end = min(b_present->get_addr_used(), e);
      b_present = link_(make_unique<entry>(range(b, page_count<Arch>(0)),
                                           new_e_end, vm_perm_none, nullptr));
    }

    b = b_present->get_addr_end();
    ++b_present;
  }

  if (b < e) {
    link_(make_unique<entry>(range(b, page_count<Arch>(0)), e,
                             vm_perm_none, nullptr));
  }
}

template<arch Arch>
auto vmmap_shard<Arch>::intersect(range x, range y) -> range {
  const auto x_begin = get<0>(x);
  const auto y_begin = get<0>(y);
  const auto x_end = x_begin + get<1>(x);
  const auto y_end = y_begin + get<1>(y);
  const auto begin = max(x_begin, y_begin);
  const auto end = min(x_end, y_end);

  if (_predict_false(begin > end))
    throw invalid_argument("vmmap_shard::intersect");
  return range(begin, end - begin);
}

template<arch Arch>
auto vmmap_shard<Arch>::map(range pos, vm_permission perm,
                      unique_ptr<vmmap_entry>&& data) -> void {
  /* Validate request. */
  if (data == nullptr)
    throw invalid_argument("vmmap_shard::map");
  if (get<1>(pos) <= page_count<Arch>(0))
    throw invalid_argument("vmmap_shard::map");

  map_link_(make_unique<entry>(pos, get<0>(pos) + get<1>(pos),
                               perm, move(data)));
}

template<arch Arch>
auto vmmap_shard<Arch>::map_link_(unique_ptr<entry>&& ptr) -> void {
  assert(ptr != nullptr);
  assert(!ptr->unused());

  /* Update the map to split across pos. */
  typename entries_type::iterator front, back;
  tie(front, back) = split_(ptr->get_range_used());

  /* Validate result from split_. */
  assert(front->get_addr_used() == ptr->get_addr_used() ||
         front->get_addr_free() <= ptr->get_addr_used());
  assert(front->get_addr_end() >= ptr->get_addr_used());
  assert(back->get_addr_end() >= ptr->get_addr_free() &&
         back->get_addr_free() <= ptr->get_addr_free());
  assert(front == back || front->get_addr_end() <= back->get_addr_used());

  /* Update end of new entry. */
  ptr->update_end(back->get_addr_end());

  /* Truncate front position. */
  typename entries_type::iterator unlink_start;
  typename entries_type::iterator unlink_end = next(back);
  if (front->get_addr_free() <= ptr->get_addr_used()) {
    assert(front != back);
    assert(get<1>(front->get_range_free()) > page_count<Arch>(0));

    free_update_(*front, [&]() { front->update_end(ptr->get_addr_used()); });
    unlink_start = next(front);
  } else {
    unlink_start = front;
  }

  /* Unlink entries that will be completely overwritten. */
  for (auto unlink_next = unlink_start;
       unlink_start != unlink_end;
       unlink_start = unlink_next) {
    unlink_next = next(unlink_start);
    unlink_(unlink_start);
  }

  /* Link new entry. */
  link_(move(ptr));
}

template<arch Arch>
auto vmmap_shard<Arch>::link_(unique_ptr<entry>&& ptr) ->
    typename entries_type::iterator {
  typename entries_type::iterator rv;
  bool link_succes;

  tie(rv, link_succes) = entries_.link(ptr.get(), false);
  assert(link_succes);

  typename free_type::const_iterator free_pos;
  tie(free_pos, link_succes) = free_.link(ptr.get(), true);
  assert(link_succes);

  free_list_.link(free_list::iterator_to(*next(free_pos)), ptr.get());

  ptr.release();
  return rv;
}

template<arch Arch>
auto vmmap_shard<Arch>::unlink_(entry* e) noexcept -> unique_ptr<entry> {
  free_list_.unlink(e);
  free_.unlink(e);
  return unique_ptr<entry>(entries_.unlink(e));
}

template<arch Arch>
auto vmmap_shard<Arch>::unlink_(typename entries_type::const_iterator i)
    noexcept -> unique_ptr<entry> {
  free_list_.unlink(&*i);
  free_.unlink(&*i);
  return unique_ptr<entry>(entries_.unlink(i));
}

template<arch Arch>
template<typename Fn, typename... Args>
auto vmmap_shard<Arch>::free_update_(entry& e, Fn fn, Args&&... args)
    noexcept -> void {
  free_list_.unlink(&e);
  free_.unlink(&e);

  fn(forward<Args>(args)...);

  typename free_type::const_iterator free_pos;
  bool link_succes;
  tie(free_pos, link_succes) = free_.link(&e, true);
  assert(link_succes);

  free_list_.link(free_list::iterator_to(*next(free_pos)), &e);
}

/*
 * Test that a given range has no gaps.
 */
template<arch Arch>
auto vmmap_shard<Arch>::ensure_no_gaps_(
    tuple<typename entries_type::iterator, typename entries_type::iterator> r)
    const -> void {
  if (get<0>(r) == get<1>(r)) return;
  assert_msg(get<0>(r) != entries_.end(), "bad range");
  auto addr = get<0>(r)->get_addr_end();

  while (get<0>(r) != get<1>(r)) {
    assert_msg(get<0>(r) != entries_.end(), "bad range");
    if (addr != get<0>(r)->get_addr_used())
      throw range_error("vmmap_shard::ensure_no_gaps_: detected a gap");

    addr = get<0>(r)->get_addr_end();
    ++get<0>(r);
  }
}

/*
 * We have to change the map, to make insertion of the new range possible.
 * However, since we can throw exceptions in this code path, we'll have to
 * prevent changing the map in a way that will leave it invalid.
 */
template<arch Arch>
auto vmmap_shard<Arch>::split_(range pos) ->
    pair<typename entries_type::iterator, typename entries_type::iterator> {
  /* Validate request. */
  if (_predict_false(intersect(pos, get_range()) != pos))
    throw invalid_argument("vmmap_shard::split_");
  if (_predict_false(get<1>(pos) == page_count<Arch>(0)))
    throw invalid_argument("vmmap_shard::split_");

  /* Find range. */
  auto r = entries_.equal_range(isect_(pos));
  assert(get<0>(r) != get<1>(r));

  /*
   * Ensure pos starts in the free space of get<0>(r)
   * or at the start of get<0>(r).
   */
  if (get<0>(r)->get_addr_used() < get<0>(pos) &&
      get<0>(r)->get_addr_free() > get<0>(pos)) {
    unique_ptr<entry> e0, e1;
    tie(e0, e1) = get<0>(r)->split(get<0>(pos));

    unlink_(get<0>(r));
    link_(move(e1));
    get<0>(r) = link_(move(e0));
  }

  /*
   * Transform range into inclusive range.
   * Note that we cannot do this earlier, because the predecessor of the end
   * of the range may be the newly created successor of get<0>(r)
   * in the if-block above.
   */
  advance(get<1>(r), -1);

  /*
   * Ensure pos ends in the free space of get<0>(r)
   * or at the end of get<0>(r).
   */
  if (get<0>(r)->get_addr_free() > get<0>(pos) + get<1>(pos) &&
      get<0>(r)->get_addr_end() > get<0>(pos) + get<1>(pos)) {
    unique_ptr<entry> e0, e1;
    tie(e0, e1) = get<0>(r)->split(get<0>(pos) + get<1>(pos));

    unlink_(get<1>(r));
    get<1>(r) = link_(move(e0));
    link_(move(e1));
  }

  return r;
}


}} /* namespace ilias::vm */

#endif /* _ILIAS_VM_VMMAP_INL_H_ */
