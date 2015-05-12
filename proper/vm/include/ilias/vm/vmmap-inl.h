#ifndef _ILIAS_VM_VMMAP_INL_H_
#define _ILIAS_VM_VMMAP_INL_H_

#include <ilias/vm/vmmap.h>
#include <ilias/vm/stats.h>

namespace ilias {
namespace vm {


inline constexpr auto vm_permission::operator==(const vm_permission& y)
    const noexcept -> bool {
  return perm_ == y.perm_;
}

inline constexpr auto vm_permission::operator!=(const vm_permission& y)
    const noexcept -> bool {
  return perm_ != y.perm_;
}

inline auto vm_permission::operator&=(const vm_permission& y) noexcept ->
    vm_permission& {
  return *this = (*this & y);
}

inline auto vm_permission::operator|=(const vm_permission& y) noexcept ->
    vm_permission& {
  return *this = (*this | y);
}

inline auto vm_permission::operator^=(const vm_permission& y) noexcept ->
    vm_permission& {
  return *this = (*this ^ y);
}

inline constexpr vm_permission::operator bool() const noexcept {
  return perm_ != perm_type::none;
}


template<typename Impl, typename... Args>
vmmap_entry_ptr make_vmmap_entry(Args&&... args) {
  return make_refpointer<Impl>(forward<Args>(args)...);
}


template<arch Arch>
vmmap_shard<Arch>::entry::entry(const entry& e)
: data_(get<0>(e.data_), get<1>(e.data_), get<2>(e.data_), get<3>(e.data_),
        get<4>(e.data_)->clone(), get<5>(e.data_))
{}

template<arch Arch>
vmmap_shard<Arch>::entry::entry(entry&& e) noexcept
: data_(get<0>(e.data_), get<1>(e.data_), get<2>(e.data_), get<3>(e.data_),
        move(get<4>(e.data_)), get<5>(e.data_))
{}

template<arch Arch>
vmmap_shard<Arch>::entry::entry(range r, vpage_no<Arch> free_end,
                                vm_permission perm,
                                vmmap_entry_ptr&& data,
                                fork_style fs) noexcept
: data_(get<0>(r), get<1>(r), free_end - (get<0>(r) + get<1>(r)), perm,
        move(data), fs)
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

    vmmap_entry_ptr d0, d1;
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
auto vmmap_shard<Arch>::entry::fault_read(monitor_token mt,
                                          shared_ptr<page_alloc> pga,
                                          vpage_no<Arch> pgno) noexcept ->
    cb_future<tuple<page_ptr, monitor_token>> {
  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  using std::placeholders::_4;

  assert(pgno >= get_addr_used() && pgno < get_addr_free());

  auto arch_off = pgno - get_addr_used();
  page_count<native_arch> off = page_count<native_arch>(arch_off.get());
  assert(off.get() == arch_off.get());  // Verify cast.
  return data().fault_read(move(mt), move(pga), move(off));
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::fault_write(monitor_token mt,
                                           shared_ptr<page_alloc> pga,
                                           vpage_no<Arch> pgno) noexcept ->
    cb_future<tuple<page_ptr, monitor_token>> {
  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  using std::placeholders::_4;

  assert(pgno >= get_addr_used() && pgno < get_addr_free());

  auto arch_off = pgno - get_addr_used();
  page_count<native_arch> off = page_count<native_arch>(arch_off.get());
  assert(off.get() == arch_off.get());  // Verify cast.
  return data().fault_write(move(mt), move(pga), move(off));
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::get_fork_style() const noexcept -> fork_style {
  return get<fork_style>(data_);
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::set_fork_style(fork_style fs) noexcept ->
    fork_style {
  return exchange(get<fork_style>(data_), fs);
}

template<arch Arch>
auto vmmap_shard<Arch>::entry::mincore() const -> vector<bool> {
  /* Only return the mincore values for in-use memory. */
  vector<bool> rv;
  if (!unused()) rv = data().mincore();
  return rv;
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
: entries_(move(rhs.entries_)),
  npg_free_(exchange(rhs.npg_free_, page_count<Arch>(0)))
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
auto vmmap_shard<Arch>::operator=(vmmap_shard&& o) noexcept -> vmmap_shard& {
  auto tmp = vmmap_shard(move(o));
  swap(*this, tmp);
  return *this;
}

template<arch Arch>
auto vmmap_shard<Arch>::operator=(const vmmap_shard& o) -> vmmap_shard& {
  auto tmp = vmmap_shard(o);
  swap(*this, tmp);
  return *this;
}

template<arch A>
auto swap(vmmap_shard<A>& x, vmmap_shard<A>& y) noexcept -> void {
  using std::swap;
  using ilias::swap;

  swap(x.entries_, y.entries_);
  swap(x.free_, y.free_);
  swap(x.free_list_, y.free_list_);
  swap(x.npg_free_, y.npg_free_);
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
                      vmmap_entry_ptr&& data) -> void {
  /* Validate request. */
  if (data == nullptr)
    throw invalid_argument("vmmap_shard::map");
  if (get<1>(pos) <= page_count<Arch>(0))
    throw invalid_argument("vmmap_shard::map");

  map_link_(make_unique<entry>(pos, get<0>(pos) + get<1>(pos),
                               perm, move(data)));
}

template<arch Arch>
auto vmmap_shard<Arch>::largest_free_size() const noexcept ->
    page_count<Arch> {
  return (free_list_.empty() ?
          page_count<Arch>(0) :
          get<1>(free_list_.back().get_range_free()));
}

template<arch Arch>
auto vmmap_shard<Arch>::mincore(vpage_no<Arch> b_addr, vpage_no<Arch> e_addr)
    const -> vector<bool> {
  typename entries_type::const_iterator b, e;
  tie(b, e) = entries_.equal_range(isect_(b_addr, e_addr));

  vector<bool> rv;

  /*
   * Special handling for first entry, to handle erasing the first few elements
   * from rv, before it is enourmous.
   */
  if (b != e) {
    if (b->get_addr_free() > b_addr) {
      rv = b->mincore();
      rv.erase(rv.begin(), rv.begin() + (b_addr - b->get_addr_free()).get());
    }

    ++b;
    rv.resize((b->get_addr_used() - b_addr).get());
  }

  /* Append mincore bits for the rest of the input. */
  for_each(b, e,
           [&rv](const entry& e) {
             vector<bool> e_mincore = e.mincore();
             rv.insert(rv.end(), e_mincore.begin(), e_mincore.end());
             rv.resize(rv.size() + get<1>(e.get_range_free()).get());
           });

  rv.resize((e_addr - b_addr).get());
  return rv;
}

template<arch Arch>
auto vmmap_shard<Arch>::merge(vmmap_shard&& rhs) noexcept -> void {
  /* Clear all collections in rhs, except for entries_. */
  while (!rhs.free_list_.empty()) rhs.free_list_.unlink_front();
  rhs.free_.unlink_all();
  rhs.npg_free_ = page_count<Arch>(0);

  /* Relink all entries from rhs into this. */
  rhs.entries_.unlink_all([this](entry* e) {
#ifndef NDEBUG
                            /* Verify there is no overlap. */
                            auto r = entries_.equal_range(
                                isect_(e->get_addr_used(), e->get_addr_end()));
                            assert(get<0>(r) == get<1>(r));
#endif

                            /* Link. */
                            auto inserted =
                                this->link_(unique_ptr<entry>(e));

                            /* Merge free space before. */
                            if (inserted->unused() &&
                                inserted != entries_.begin()) {
                              auto pred = prev(inserted);
                              this->free_update_(*pred,
                                                 [this, &pred, &inserted]() {
                                                   auto i =
                                                       this->unlink_(inserted);
                                                   pred->update_end(
                                                       i->get_addr_end());
                                                   inserted = pred;
                                                 });
                            }

                            /* Merge free space after. */
                            if (inserted != entries_.end()) {
                              auto succ = next(inserted);
                              if (succ != entries_.end() && succ->unused()) {
                                this->free_update_(*inserted,
                                                   [this, &succ, &inserted]() {
                                                     auto i =
                                                         this->unlink_(succ);
                                                     inserted->update_end(
                                                         i->get_addr_end());
                                                   });
                              }
                            }
                          });
}

template<arch Arch>
template<typename Iter>
auto vmmap_shard<Arch>::fanout(Iter b, Iter e) noexcept -> void {
  const page_count<Arch> npg_per_shard =
      max(free_size() / size_t(distance(b, e)),
          page_count<Arch>(1));
  const page_count<Arch> max_npg_per_shard =
      2 * npg_per_shard;

  typename entries_type::iterator i = entries_.begin();
  for (Iter out = b; i != entries_.end() && next(out) != e; ++out) {
    page_count<Arch> c = page_count<Arch>(0);
    while (i != entries_.end() && c < npg_per_shard) {
      auto i_free = get<1>(i->get_range_free());
      /* Split range if it would cause us to cross max_npg_per_shard limit. */
      if (c + i_free > max_npg_per_shard) {
        auto split = i->get_addr_free() + (npg_per_shard - c);
        auto p = make_unique<entry>(range(split, page_count<Arch>(0)),
                                    i->get_addr_end(), vm_perm_none, nullptr);
        this->free_update_(*i, [&i, split]() { i->update_end(split); });
        link_(move(p));

        /* Update i_free value. */
        i_free = get<1>(i->get_range_free());
      }

      /* Save iterator successor (since i will be invalidated below). */
      typename entries_type::iterator i_next = next(i);

      /* Move entry to destination. */
      if (&*out != this) out->link_(unlink_(i));
      c += i_free;

      /* Update for next step in while  loop. */
      i = i_next;
    }
  }
}

template<arch Arch>
auto vmmap_shard<Arch>::fault_read(monitor_token mt,
                                   shared_ptr<page_alloc> pga,
                                   vpage_no<Arch> pgno) ->
    cb_future<tuple<page_ptr, monitor_token>> {
  entry* e = find_entry_for_addr_(pgno);
  if (_predict_false(e == nullptr)) {
    cb_promise<tuple<page_ptr, monitor_token>> pgptr_promise;
    pgptr_promise.set_exception(make_exception_ptr(efault(pgno)));
    return pgptr_promise.get_future();
  }
  return e->fault_read(move(mt), move(pga), pgno);
}

template<arch Arch>
auto vmmap_shard<Arch>::fault_write(monitor_token mt,
                                    shared_ptr<page_alloc> pga,
                                    vpage_no<Arch> pgno) ->
    cb_future<tuple<page_ptr, monitor_token>> {
  entry* e = find_entry_for_addr_(pgno);
  if (_predict_false(e == nullptr)) {
    cb_promise<tuple<page_ptr, monitor_token>> pgptr_promise;
    pgptr_promise.set_exception(make_exception_ptr(efault(pgno)));
    return pgptr_promise.get_future();
  }
  return e->fault_write(move(mt), move(pga), pgno);
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
auto vmmap_shard<Arch>::link_(unique_ptr<entry>&& ptr) noexcept ->
    typename entries_type::iterator {
  typename entries_type::iterator rv;
  bool link_succes;

  tie(rv, link_succes) = entries_.link(ptr.get(), false);
  assert(link_succes);

  typename free_type::const_iterator free_pos;
  tie(free_pos, link_succes) = free_.link(ptr.get(), true);
  assert(link_succes);

  free_list_.link(free_list::iterator_to(*next(free_pos)), ptr.get());

  npg_free_ += get<1>(ptr->get_range_free());

  ptr.release();
  return rv;
}

template<arch Arch>
auto vmmap_shard<Arch>::unlink_(entry* e) noexcept -> unique_ptr<entry> {
  npg_free_ -= get<1>(e->get().get_range_free());
  free_list_.unlink(e);
  free_.unlink(e);
  return unique_ptr<entry>(entries_.unlink(e));
}

template<arch Arch>
auto vmmap_shard<Arch>::unlink_(typename entries_type::const_iterator i)
    noexcept -> unique_ptr<entry> {
  npg_free_ -= get<1>(i->get_range_free());
  free_list_.unlink(&*i);
  free_.unlink(&*i);
  return unique_ptr<entry>(entries_.unlink(i));
}

template<arch Arch>
template<typename Fn, typename... Args>
auto vmmap_shard<Arch>::free_update_(entry& e, Fn fn, Args&&... args)
    noexcept -> void {
  npg_free_ -= get<1>(e.get_range_free());
  free_list_.unlink(&e);
  free_.unlink(&e);

  fn(forward<Args>(args)...);

  typename free_type::const_iterator free_pos;
  bool link_succes;
  tie(free_pos, link_succes) = free_.link(&e, true);
  assert(link_succes);

  free_list_.link(free_list::iterator_to(*next(free_pos)), &e);
  npg_free_ += get<1>(e.get_range_free());
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

template<arch Arch>
auto vmmap_shard<Arch>::find_entry_for_addr_(vpage_no<Arch> pg) noexcept ->
    entry* {
  auto rv = entries_.find(isect_(pg, pg + page_count<Arch>(1)));
  if (_predict_false(rv == entries_.end()))
    return nullptr;

  vpage_no<Arch> start;
  page_count<Arch> len;
  tie(start, len) = rv->get_range_used();
  if (_predict_true(start <= pg && pg < start + len))
    return &*rv;

  return nullptr;
}

template<arch Arch>
template<typename T>
auto vmmap_shard<Arch>::efault_future_(vpage_no<Arch>) -> cb_future<T> {
  return async_lazy([]() -> T {
                      throw system_error(make_error_code(errc::bad_address));
                    });
}


template<arch Arch>
vmmap<Arch>::vmmap(pmap_support<Arch>& ps, shared_ptr<page_alloc> pga,
                   workq_service& wqs)
: pmap_(ps),
  avail_(1),
  pga_(pga),
  wq_(wqs.new_workq())
{}

template<arch Arch>
vmmap<Arch>::vmmap(pmap_support<Arch>& ps, shared_ptr<page_alloc> pga,
                   workq_service& wqs,
                   vpage_no<Arch> b, vpage_no<Arch> e)
: pmap_(ps),
  avail_(1, vmmap_shard<Arch>(b, e)),
  pga_(pga),
  wq_(wqs.new_workq())
{}

template<arch Arch>
auto vmmap<Arch>::reshard(size_t n_shards, size_t in_use) -> cb_future<void> {
  return async(wq_, &vmmap::reshard_,
               this, avail_guard_.queue(monitor_access::write),
               n_shards, in_use);
}

template<arch Arch>
auto vmmap<Arch>::fault_read(vpage_no<Arch> pgno) -> cb_future<void> {
  using std::placeholders::_1;

  auto mt_future = avail_guard_.queue(monitor_access::read).share();

  cb_future<typename shard_list::iterator> find_shard_future =
      async(wq_, launch::parallel,
            [this, pgno](monitor_token) {
              typename shard_list::iterator shard =
                  this->find_shard_locked_(pgno);
              if (_predict_false(shard == avail_.end()))
                throw efault(pgno);
              return shard;
            },
            mt_future);

  cb_future<tuple<page_ptr, monitor_token>> pgptr_future =
      async(wq_, launch::parallel | launch::aid,
            pass_promise<tuple<page_ptr, monitor_token>>(
                [](cb_promise<tuple<page_ptr, monitor_token>> pgptr_promise,
                   typename shard_list::iterator shard,
                   monitor_token mt,
                   shared_ptr<page_alloc> pga,
                   vpage_no<Arch> pgno) {
                  convert(move(pgptr_promise),
                          shard->fault_read(mt, move(pga), pgno));
                }),
            move(find_shard_future),
            mt_future, this->pga_, pgno);

  return async(wq_, launch::parallel | launch::aid,
               [this](monitor_token, tuple<page_ptr, monitor_token> pg,
                      vpage_no<Arch> pgno) {
                 const auto npa = page_no<native_arch>(get<0>(pg)->address());
                 const auto pa = page_no<Arch>(npa.get());
                 pmap_.map(pgno, pa, permission::RO());  // XXX Use permissions from entry.
               },
               mt_future, move(pgptr_future), pgno);
}

template<arch Arch>
auto vmmap<Arch>::fault_write(vpage_no<Arch> pgno) -> cb_future<void> {
  using std::placeholders::_1;

  auto mt_future = avail_guard_.queue(monitor_access::read).share();

  cb_future<typename shard_list::iterator> find_shard_future =
      async(wq_, launch::parallel,
            [this, pgno](monitor_token) {
              typename shard_list::iterator shard =
                  this->find_shard_locked_(pgno);
              if (_predict_false(shard == avail_.end()))
                throw efault(pgno);
              return shard;
            },
            mt_future);

  cb_future<tuple<page_ptr, monitor_token>> pgptr_future =
      async(wq_, launch::parallel | launch::aid,
            pass_promise<tuple<page_ptr, monitor_token>>(
                [](cb_promise<tuple<page_ptr, monitor_token>> pgptr_promise,
                   typename shard_list::iterator shard,
                   monitor_token mt,
                   shared_ptr<page_alloc> pga,
                   vpage_no<Arch> pgno) {
                  convert(move(pgptr_promise),
                          shard->fault_write(mt, move(pga), pgno));
                }),
            move(find_shard_future),
            mt_future, ref(this->pga_), pgno);

  return async(wq_, launch::parallel | launch::aid,
               [this](monitor_token, tuple<page_ptr, monitor_token> pg, vpage_no<Arch> pgno) {
                 const auto npa = page_no<native_arch>(get<0>(pg)->address());
                 const auto pa = page_no<Arch>(npa.get());
                 pmap_.map(pgno, pa, permission::RW());  // XXX Use permissions from entry.
               },
               mt_future, move(pgptr_future), pgno);
}

template<arch Arch>
auto vmmap<Arch>::mincore(vpage_no<Arch> addr_b, vpage_no<Arch> addr_e)
    const -> cb_future<vector<bool>> {
  auto mt_future = avail_guard_.queue(monitor_access::read);

  return async(wq_, launch::parallel,
               [this, addr_b, addr_e](monitor_token) {
                 vector<bool> rv((addr_e - addr_b).get());
                 for (const vmmap_shard<Arch>& shard : avail_) {
                   vector<bool> shard_rv = shard.mincore(addr_b, addr_e);
                   transform(shard_rv.begin(), shard_rv.end(), rv.begin(),
                             rv.begin(),
                             logical_or<bool>());
                 }
                 return rv;
               },
               move(mt_future));
}

template<arch Arch>
auto vmmap<Arch>::find_shard_locked_(vpage_no<Arch> pgno) noexcept ->
    typename shard_list::iterator {
  typename shard_list::iterator i = avail_.begin();
  for (auto end = avail_.end(); i != end; ++i) {
    vpage_no<Arch> start;
    page_count<Arch> len;
    tie(start, len) = i->get_range();

    if (start <= pgno && pgno < start + len) break;
  }
  return i;
}

template<arch Arch>
auto vmmap<Arch>::shard_free_less_(const vmmap_shard<Arch>& x,
                                   const vmmap_shard<Arch>& y)
    noexcept -> bool {
  return x.largest_free_size() < y.largest_free_size();
}

template<arch Arch>
auto vmmap<Arch>::heap_begin() noexcept -> typename shard_list::iterator {
  return avail_.begin();
}

template<arch Arch>
auto vmmap<Arch>::heap_begin() const noexcept ->
    typename shard_list::const_iterator {
  return avail_.begin();
}

template<arch Arch>
auto vmmap<Arch>::heap_end() noexcept -> typename shard_list::iterator {
  return avail_.end() - in_use_;
}

template<arch Arch>
auto vmmap<Arch>::heap_end() const noexcept ->
    typename shard_list::const_iterator {
  return avail_.end() - in_use_;
}

template<arch Arch>
auto vmmap<Arch>::heap_empty() const noexcept -> bool {
  assert(in_use_ <= avail_.size());
  return avail_.size() == in_use_;
}

template<arch Arch>
auto vmmap<Arch>::swap_slot_(size_t slot_idx) noexcept -> cb_future<void> {
  auto mt_future = avail_guard_.queue(monitor_access::write);

  return async(wq_,
               [this](monitor_token, size_t slot_idx) {
                 pop_heap(heap_begin(), heap_end(), &shard_free_less_);
                 iter_swap(prev(heap_end()), prev(avail_.end(),
                           slot_idx + 1U));
                 push_heap(heap_begin(), heap_end(), &shard_free_less_);
               },
               move(mt_future), slot_idx);
}

template<arch Arch>
auto vmmap<Arch>::reshard_(monitor::token mt, size_t n_shards,
                           size_t in_use) -> void {
  assert(mt.locked() && mt.access() == monitor_access::write);

  if (n_shards == 0) n_shards = 1;
  in_use_ = 0;  // Reset use counter.

  if (avail_.empty()) {
    avail_.resize(n_shards);
    return;
  }
  avail_.reserve(n_shards);

  /* Merge all shards together. */
  for_each(next(avail_.begin()), avail_.end(),
           [this](vmmap_shard<Arch>& shard) {
             this->avail_.front().merge(move(shard));
           });

  /* Allocate shards (all shards except the first are empty). */
  avail_.resize(n_shards);

  if (n_shards > 1U) {
    /* Distribute pages across shards. */
    avail_.front().fanout(avail_.begin(), avail_.end());

    /* Heap sort, so the range with the most free pages is at the front. */
    make_heap(avail_.begin(), avail_.end(), &shard_free_less_);
  }

  /* Assign in-use shards. */
  while (in_use_ < min(in_use, n_shards)) {
    pop_heap(avail_.begin(), avail_.end() - in_use_, &shard_free_less_);
    ++in_use_;
  }

  /* Try to reduce memory usage (not a big deal if this fails). */
  try {
    avail_.shrink_to_fit();
  } catch (...) {
    /* IGNORE */
  }
}


}} /* namespace ilias::vm */

#endif /* _ILIAS_VM_VMMAP_INL_H_ */
