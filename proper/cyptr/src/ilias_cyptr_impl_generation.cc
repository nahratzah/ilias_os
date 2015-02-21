#include <ilias/cyptr/impl/generation.h>
#include <ilias/cyptr/impl/background.h>
#include <vector>
#include <algorithm>
#include <bitset>
#include <tuple>
#include <vector>
#include <queue>

namespace ilias {
namespace cyptr {
namespace impl {


generation_ptr generation::new_generation() throw (std::bad_alloc) {
  return make_refpointer<generation>();
}

void generation::marksweep() noexcept {
  if (backgrounded_.exchange(true, std::memory_order_acquire)) return;

  background* bg = get_background();
  if (bg && bg->enqueue(generation_ptr(this))) return;

  marksweep_bg();
}

void generation::marksweep(std::unique_lock<generation> lck) noexcept {
  if (backgrounded_.exchange(true, std::memory_order_acquire)) return;

  background* bg = get_background();
  if (bg && bg->enqueue(generation_ptr(this))) return;

  marksweep_bg(std::move(lck));
}

void generation::marksweep_bg() noexcept {
  marksweep(std::unique_lock<generation>(*this));
}

void generation::marksweep_bg(std::unique_lock<generation> lck) noexcept {
  assert(lck.mutex() == this);
  if (!lck.owns_lock()) lck.lock();
  backgrounded_.exchange(false, std::memory_order_acquire);

  /* Run mark-sweep algorithm on graph contained in this generation. */
  marksweep_process_(marksweep_init_());
  linked_list<basic_obj, wavefront_tag> dead = marksweep_dead_();
  using iterator = linked_list<basic_obj, wavefront_tag>::iterator;

  /* Collect the dying and mark them as dead. */
  for (iterator dead_obj_iter = dead.begin(),
                dead_obj_iter_next;
       dead_obj_iter != dead.end();
       dead_obj_iter = dead_obj_iter_next) {
    basic_obj& dead_obj = *dead_obj_iter;
    dead_obj_iter_next = std::next(dead_obj_iter);

    obj_color expect = obj_color::dying;
    if (dead_obj.color_.compare_exchange_strong(expect,
                                                obj_color::dead,
                                                std::memory_order_acq_rel,
                                                std::memory_order_relaxed)) {
      /* Break edges. */
      for (edge& e : dead_obj.edge_list_) {
        std::unique_lock<edge> elck{ e };
        basic_obj* edst = std::get<0>(e.dst_.load(std::memory_order_relaxed));
        if (!edst || !edst->has_generation(*this))
          continue;

        e.dst_.store(std::make_tuple(nullptr, edge::UNLOCKED),
                     std::memory_order_relaxed);
        elck.release();  // Above unlocks edge.
      }
    } else {
      dead.unlink(dead_obj_iter);
    }
  }

  /* Destroy the dead. */
  lck.unlock();
  while (!dead.empty()) {
    basic_obj* dead_obj = dead.unlink_front();
    assert(dead_obj->color_.load(std::memory_order_acquire) ==
           obj_color::dead);

    /* Invoke destructor. */
    std::function<void (void*)> destructor = std::move(dead_obj->destructor_);
    void* destructor_arg = dead_obj->destructor_arg_;
    assert(destructor);
    destructor(destructor_arg);
  }
}

std::unique_lock<generation> generation::fix_relation(basic_obj& src,
                                                      basic_obj& dst)
    noexcept {
  basic_obj_lock lck = fix_relation_(src, dst);

  /* Verify post condition. */
  assert(lck);
  assert(src.has_generation(lck.get_generation()));
  assert(lck.get_generation().get_tstamp() <= get_generation_seq(dst));

  return std::unique_lock<generation>(*lck.release());
}


class generation::fix_relation_state {
 public:
  fix_relation_state() noexcept = default;
  fix_relation_state(const fix_relation_state&) noexcept = delete;
  fix_relation_state(fix_relation_state&&) noexcept;
  fix_relation_state& operator=(const fix_relation_state&) noexcept = delete;
  fix_relation_state& operator=(fix_relation_state&&) noexcept;

  explicit fix_relation_state(basic_obj&) noexcept;
  void update() noexcept;
  void lock() noexcept;
  bool try_lock() noexcept;
  bool lock_atbefore(const tstamp&) noexcept;
  tstamp get_tstamp() const noexcept { return gen->get_tstamp(); }
  bool owns_lock() const noexcept { return genlck.owns_lock(); }

  bool operator==(const fix_relation_state&) const noexcept;
  bool operator!=(const fix_relation_state&) const noexcept;
  bool operator<(const fix_relation_state&) const noexcept;
  bool operator>(const fix_relation_state&) const noexcept;
  bool operator<=(const fix_relation_state&) const noexcept;
  bool operator>=(const fix_relation_state&) const noexcept;

  basic_obj* obj;
  generation_ptr gen;
  std::unique_lock<generation> genlck;
};

generation::fix_relation_state::fix_relation_state(basic_obj& o) noexcept
: obj(&o),
  gen(obj->get_generation()),
  genlck(*gen, std::defer_lock)
{}

generation::fix_relation_state::fix_relation_state(fix_relation_state&& o)
    noexcept
: obj(std::exchange(o.obj, nullptr)),
  gen(std::move(o.gen)),
  genlck(std::move(o.genlck))
{}

auto generation::fix_relation_state::operator=(fix_relation_state&& o)
    noexcept -> fix_relation_state& {
  if (genlck.owns_lock()) genlck.unlock();
  obj = std::exchange(o.obj, nullptr);
  gen = std::move(o.gen);
  genlck = std::move(o.genlck);
  return *this;
}

auto generation::fix_relation_state::update() noexcept -> void {
  assert(obj != nullptr);
  assert(!genlck);

  gen = obj->get_generation();
  genlck = std::unique_lock<generation>(*gen, std::defer_lock);
}

auto generation::fix_relation_state::lock() noexcept -> void {
  assert(obj != nullptr);
  assert(!genlck);

  basic_obj_lock lck{ *obj };
  gen = lck.release();
  genlck = std::unique_lock<generation>(*gen, std::adopt_lock);
}

auto generation::fix_relation_state::try_lock() noexcept -> bool {
  assert(obj != nullptr);
  assert(!genlck);

  basic_obj_lock lck{ *obj, std::try_to_lock };
  if (lck) {
    gen = lck.release();
    genlck = std::unique_lock<generation>(*gen, std::adopt_lock);
    return true;
  } else {
    return false;
  }
}

auto generation::fix_relation_state::lock_atbefore(const tstamp& ts)
    noexcept -> bool {
  assert(obj != nullptr);
  assert(gen != nullptr);
  assert(!genlck);

  std::unique_lock<generation> lck;

  for (;;) {
    if (gen->get_tstamp() > ts)
      return false;
    lck = std::unique_lock<generation>{ *gen };
    if (obj->has_generation(*gen)) {
      genlck = std::move(lck);
      return true;
    }
    update();
  }
}

inline bool generation::fix_relation_state::operator==(
    const fix_relation_state& y) const noexcept {
  return gen == y.gen;
}

inline bool generation::fix_relation_state::operator!=(
    const fix_relation_state& y) const noexcept {
  return !(*this == y);
}

inline bool generation::fix_relation_state::operator<(
    const fix_relation_state& y) const noexcept {
  return gen->get_tstamp() < y.gen->get_tstamp();
}

inline bool generation::fix_relation_state::operator>(
    const fix_relation_state& y) const noexcept {
  return y < *this;
}

inline bool generation::fix_relation_state::operator<=(
    const fix_relation_state& y) const noexcept {
  return !(y < *this);
}

inline bool generation::fix_relation_state::operator>=(
    const fix_relation_state& y) const noexcept {
  return !(*this < y);
}


class generation::basic_obj_gcmp_elem_ {
 public:
  basic_obj_gcmp_elem_() = default;
  basic_obj_gcmp_elem_(const basic_obj_gcmp_elem_&) noexcept = default;
  basic_obj_gcmp_elem_& operator=(const basic_obj_gcmp_elem_&) noexcept =
      default;
  explicit basic_obj_gcmp_elem_(basic_obj*) noexcept;
  explicit basic_obj_gcmp_elem_(basic_obj*, const tstamp&) noexcept;
  ~basic_obj_gcmp_elem_() noexcept {}

  basic_obj* operator->() const noexcept { return obj_; }
  basic_obj& operator*() const noexcept { return *obj_; }
  tstamp get_tstamp() const noexcept { return ts_; }

 private:
  basic_obj* obj_ = nullptr;
  tstamp ts_;
};

inline generation::basic_obj_gcmp_elem_::basic_obj_gcmp_elem_(basic_obj* obj)
    noexcept
: obj_(obj)
{
  if (obj) ts_ = get_generation_seq(*obj);
}

inline generation::basic_obj_gcmp_elem_::basic_obj_gcmp_elem_(basic_obj* obj,
                                                              const tstamp& ts)
    noexcept
: obj_(obj),
  ts_(ts)
{}


struct generation::basic_obj_gencmp_ {
 public:
  bool operator()(const basic_obj_gcmp_elem_& x, const basic_obj_gcmp_elem_& y)
      const noexcept {
    return x.get_tstamp() > y.get_tstamp();
  }
};


basic_obj_lock generation::fix_relation_(basic_obj& src, basic_obj& dst)
    noexcept {
  /*
   * Optimisticly try to acquire src lock.
   * Common case, it's the only lock we need.
   */
  basic_obj_lock src_lck{ src, std::defer_lock };
  if (_predict_true(get_generation_seq(dst) >=
                    src_lck.get_generation().get_tstamp()))
    return src_lck;

  basic_obj_lock dst_lck{ dst, std::defer_lock };
  lock_2_(src_lck, dst_lck);
  if (_predict_false(!dst_lck))
    return src_lck;
  if (_predict_false(src_lck.get_generation().get_tstamp() <=
                     dst_lck.get_generation().get_tstamp()))
    return src_lck;

  try {
    src_lck.unlock();
    src_lck = fixrel_lockcomplete_(fixrel_lock_(src, std::move(dst_lck)), src);
  } catch (const std::bad_alloc&) {
    assert(!src_lck && !dst_lck);
    lock_2_(src_lck, dst_lck);
    if (_predict_false(!dst_lck))
      return src_lck;
    if (_predict_false(src_lck.get_generation().get_tstamp() <=
                       dst_lck.get_generation().get_tstamp()))
      return src_lck;

    src_lck.unlock();
    src_lck = fixrel_slow_(src, std::move(dst_lck));
  }

  /*
   * Verify postcondition:
   * - src is locked,
   * - generation age of src <= generation age of dst.
   */
  assert(src_lck);
  assert(src.has_generation(src_lck.get_generation()));
  assert(src_lck.get_generation().get_tstamp() <= get_generation_seq(dst));
  return src_lck;
}

/*
 * Lock two generations.
 */
void generation::lock_2_(basic_obj_lock& x, basic_obj_lock& y) noexcept {
  assert(!x && !y);

  for (;;) {
    /* Lock X, then lock Y. */
    x.lock();
    if (get_generation_seq(y.get_obj()) > x.get_generation().get_tstamp()) {
      y.lock();
      return;
    }
    if (get_generation_seq(y.get_obj()) == x.get_generation().get_tstamp())
      return;
    if (y.try_lock()) return;
    x.unlock();

    /* Lock Y, then lock X. */
    y.lock();
    if (get_generation_seq(x.get_obj()) > y.get_generation().get_tstamp()) {
      x.lock();
      return;
    }
    if (get_generation_seq(x.get_obj()) == y.get_generation().get_tstamp())
      return;
    if (x.try_lock()) return;
    y.unlock();
  }
}

/*
 * Lock all generations in order of age (from oldest to youngest).
 */
auto generation::fixrel_lock_(basic_obj& src, basic_obj_lock dst) ->
    fixrel_locks {
  assert(dst);

  /*
   * Build a queue with objects whose generations need to be locked.
   * Build return value, containing all locked generations (ordered).
   */
  fixrel_queue locks;
  fixrel_locks rv;
  {
    /* Initialize queue, by locking dst. */
    fix_relation_state s{ dst.get_obj() };
    assert(s.gen == &dst.get_generation());
    s.genlck = std::unique_lock<generation>(*dst.release());
    fixrel_lock_add_edges_(*s.gen, locks);
    rv.push_back(std::move(s));
  }

  bool seen_src = false;
  while (!locks.empty() && !seen_src) {
    fix_relation_state s{ *locks.top() };
    locks.pop();

    if (s.gen == rv.back().gen) {
      s.update();
      if (s.gen == rv.back().gen)
        continue;  // SKIP already locked generation
    }

    if (fixrel_lock_1_(s, locks)) {
      seen_src = src.has_generation(*s.gen);
      rv.push_back(std::move(s));
    } else {
      locks.emplace(s.obj, s.get_tstamp());
    }
  }

  /*
   * Ensure src generation is locked, if src not reachable from dst.
   */
  if (!seen_src) {
    fix_relation_state s{ src };
    s.lock();
    rv.push_back(std::move(s));
  }

  return rv;
}

auto generation::fixrel_lock_1_(fix_relation_state& s, fixrel_queue& locks) ->
    bool {
  assert(!s.owns_lock());

  /* Maybe acquire ownership of lock. */
  bool owns_lock;
  if (!locks.empty()) {
    owns_lock = s.lock_atbefore(get_generation_seq(*locks.top()));
  } else {
    s.lock();
    owns_lock = true;
  }

  /* Add edges. */
  if (owns_lock)
    fixrel_lock_add_edges_(*s.gen, locks);

  return owns_lock;
}

auto generation::fixrel_lock_add_edges_(generation& gen,
                                        fixrel_queue& locks) -> void {
  for (basic_obj& obj : gen.obj_) {
    for (edge& e : obj.edge_list_) {
      std::lock_guard<edge> elck{ e };

      /* Skip nullptr edges. */
      basic_obj* e_dst =
          std::get<0>(e.dst_.load(std::memory_order_relaxed));
      if (!e_dst) continue;

      /* Skip edges in the same generation. */
      if (e_dst->has_generation(gen)) continue;

      /* Add edge destination to queue. */
      locks.emplace(e_dst);
    }
  }
}

auto generation::fixrel_lockcomplete_(fixrel_locks locks, basic_obj& src)
    noexcept -> basic_obj_lock {
  basic_obj_lock rv;
  generation_ptr srcgen = src.get_generation();

  for (fix_relation_state& s : locks) {
    assert(s.owns_lock());

    if (srcgen == s.gen) {
      rv = basic_obj_lock(src, std::adopt_lock);
      s.genlck.release();
    } else {
      fixrel_splice_(*s.gen, *srcgen);
      s.genlck.unlock();
    }
  }

  assert(rv);
  return rv;
}

/*
 * Slow algorithm for moving dst towards the same generation as src.
 *
 * This algorithm functions by locking the lowest generation after dst and
 * splicing all object into that generation.
 */
basic_obj_lock generation::fixrel_slow_1_(basic_obj& src,
                                          basic_obj_lock dstlck)
    noexcept {
  assert(dstlck);

  generation_ptr nextgen;
  std::unique_lock<generation> nextgenlck;

  /* Double for loop: for each object reachable from dstlck-generation... */
  for (basic_obj& obj : dstlck.get_generation().obj_) {
    for (edge& out : obj.edge_list_) {
      /* Lock edge. */
      std::lock_guard<edge> edgelck{ out };
      basic_obj* out_ptr =
          std::get<0>(out.dst_.load(std::memory_order_relaxed));
      if (!out_ptr) continue;  // SKIP: nullptr.
      basic_obj& next = *out_ptr;

      if (next.has_generation(dstlck.get_generation()))
        continue;  // SKIP: already locked (same generation as dst).
      if (nextgen != nullptr && next.has_generation(*nextgen))
        continue;  // SKIP: already locked (same generation as next).

      if (!nextgen) {
        /* Always lock first eligible generation. */
        basic_obj_lock nextlck{ next };
        if (nextlck.get_generation().get_tstamp() <= get_generation_seq(src)) {
          /* Only lock if nextgen at/before srcgen. */
          nextgen = nextlck.release();
          nextgenlck = std::unique_lock<generation>(*nextgen, std::adopt_lock);
        }
        continue;
      }

      /* SKIP: locked next generation is older. */
      if (get_generation_seq(next) >= nextgen->get_tstamp())
        continue;

      /*
       * Try to lock without unlocking previously locked generation.
       *
       * We try to lock both the current and eligible generation.
       * If that succeeds, we are guaranteed that the 'next' object will
       * remain reachable directly from dstgen.
       *
       * If that fails, we lock it unconditionally, but may end up locking
       * a generation that the object was moved out of.
       */
      basic_obj_lock nextlck{ next, std::try_to_lock };
      if (nextlck) {
        if (nextlck.get_generation().get_tstamp() < nextgen->get_tstamp()) {
          /* This generation is older, use it instead. */
          nextgenlck.unlock();
          nextgen = nextlck.release();
          nextgenlck = std::unique_lock<generation>(*nextgen, std::adopt_lock);
        }
      } else {
        generation_ptr gen = next.get_generation();
        if (gen->get_tstamp() < nextgen->get_tstamp()) {
          /* This generation is older, use it instead. */
          nextgenlck.unlock();
          nextgen = std::move(gen);
          nextgenlck = std::unique_lock<generation>(*nextgen);
        }
      }
    }
  }

  /* nextgen == nullptr  ==>  there is no generation between src and dst. */
  if (nextgen == nullptr) {
    basic_obj_lock srclck{ src };
    nextgen = srclck.release();
    nextgenlck = std::unique_lock<generation>(*nextgen, std::adopt_lock);
  }

  fixrel_splice_(dstlck.get_generation(), *nextgen);

  /* Return updated lock on dst object. */
  nextgenlck.release();
  basic_obj_lock rv = basic_obj_lock(dstlck.get_obj(), std::adopt_lock);
  assert(&rv.get_generation() == nextgen);
  return rv;
}

/*
 * Slow algorithm for fixing relation between src and dst.
 */
basic_obj_lock generation::fixrel_slow_(basic_obj& src, basic_obj_lock dstlck)
    noexcept {
  assert(dstlck);

  if (_predict_true(get_generation_seq(src) >
                    dstlck.get_generation().get_tstamp())) {
    while (!src.has_generation(dstlck.get_generation()))
      dstlck = fixrel_slow_1_(src, std::move(dstlck));
  }
  return dstlck;
}

/*
 * Splice all objects in src generation onto dst generation.
 */
void generation::fixrel_splice_(generation& src, generation& dst) noexcept {
  using flags_type = std::bitset<0>;

  assert(&src != &dst);
  assert(src.get_tstamp() < dst.get_tstamp());

  for (basic_obj& obj : src.obj_) {
    /* Update generation pointer. */
    assert(obj.has_generation(src));
    obj.gen_.store(std::make_tuple(generation_ptr(&dst), flags_type()),
                   std::memory_order_relaxed);

    /* Update objects pointed to by edges. */
    for (edge& e : obj.edge_list_) {
      std::lock_guard<edge> lck{ e };

      basic_obj* e_dst = std::get<0>(e.dst_.load(std::memory_order_relaxed));
      if (e_dst != nullptr && get_generation_seq(*e_dst) < dst.get_tstamp())
        e_dst->refcnt_.fetch_sub(1U, std::memory_order_relaxed);
    }
  }

  dst.obj_.splice(dst.obj_.begin(), src.obj_);
}

/*
 * Build the wavefront and mark any object not on the wavefront as
 * maybe dead.
 */
linked_list<basic_obj, wavefront_tag> generation::marksweep_init_() noexcept {
  linked_list<basic_obj, wavefront_tag> wavefront;
  for (basic_obj& o : obj_) {
    obj_color expect = obj_color::linked;

    if (o.refcnt_.load(std::memory_order_relaxed) != 0U) {
      /* Add to wavefront below. */ ;
    } else if (o.color_.compare_exchange_strong(expect,
                                                obj_color::maybe_dying,
                                                std::memory_order_acq_rel,
                                                std::memory_order_acquire)) {
      continue;
    } else if (expect == obj_color::unlinked) {
      /* SKIP: Add to wavefront below. */
    } else {
      continue;
    }

    wavefront.link_back(&o);
  }

  return wavefront;
}

/*
 * Process the wavefront.
 */
void generation::marksweep_process_(
    linked_list<basic_obj, wavefront_tag>&& wavefront) noexcept {
  while (!wavefront.empty()) {
    basic_obj& o = wavefront.front();

    /* Process all objects for each edge. */
    for (edge& e : o.edge_list_) {
      /*
       * Read the destination pointer and acquire the lock on edge.
       */
      std::lock_guard<edge> edgeptr_lock{ e };
      basic_obj* dst_ptr = std::get<0>(e.dst_.load(std::memory_order_relaxed));

      /* Skip nullptr. */
      if (dst_ptr == nullptr)
        continue;

      basic_obj& dst = *dst_ptr;
      /* Skip edges in different generations. */
      if (std::get<0>(dst.gen_.load_no_acquire(std::memory_order_relaxed)) !=
          this)
        continue;
      /* Skip edges already declared reachable. */
      assert(dst.color_.load() != obj_color::linked);
      if (dst.color_.load(std::memory_order_relaxed) != obj_color::maybe_dying)
        continue;

      /* Add dst object to wavefront, since it is reachable. */
      dst.color_.store(obj_color::linked, std::memory_order_relaxed);
      wavefront.link_back(&dst);
    }

    /* Done processing front of wavefront. */
    wavefront.unlink_front();
  }
}

linked_list<basic_obj, wavefront_tag> generation::marksweep_dead_() noexcept {
  linked_list<basic_obj, wavefront_tag> rv;

  for (basic_obj o : obj_) {
    obj_color expect = obj_color::maybe_dying;
    if (o.color_.compare_exchange_strong(expect,
                                         obj_color::dying,
                                         std::memory_order_acq_rel,
                                         std::memory_order_relaxed))
      rv.link_back(&o);
  }

  return rv;
}


}}} /* namespace ilias::cyptr::impl */
