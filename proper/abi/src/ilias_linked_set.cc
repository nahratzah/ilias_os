#include <ilias/linked_set.h>
#include <algorithm>
#include <cassert>

_namespace_begin(ilias)


auto basic_linked_set::element::sp_(unsigned int i) const noexcept ->
    element* {
  assert(i < children_.size());
  assert_msg(children_.size() == 2, "Not sure what kind of tree this is, "
                                   "but it is certainly not binary!");

  {
    auto sel = child(i);
    if (sel) {
      element* sel_child;
      while ((sel_child = sel->child(1 - i)) != nullptr) sel = sel_child;
      return sel;
    }
  }

  auto sel = this;
  auto p = sel->parent();
  while (p && sel->pidx_() == i) {
    sel = p;
    p = sel->parent();
  }
  return p;
}


auto basic_linked_set::unlink_all() noexcept -> void {
  unlink_all([](element*) { /* SKIP */ });
}

auto basic_linked_set::begin() const noexcept -> iterator {
  element* first = root_;
  if (first) {
    while (first->left()) first = first->left();
  }
  return iterator(this, first);
}

auto basic_linked_set::unlink_to_leaf_(element* e) noexcept -> void {
  using _namespace(std)::any_of;
  using _namespace(std)::all_of;

  element* s = e;
  while (s->left()) s = s->left();
  assert(any_of(s->children_.begin(), s->children_.end(),
                [](element* p) { return p == nullptr; }));
  node_swap_(e, s);
  if (e->right()) {
    node_swap_(e, e->right());
    assert(all_of(s->children_.begin(), s->children_.end(),
                  [](element* p) { return p == nullptr; }));
  }
}

auto basic_linked_set::rotate_(element* q, unsigned int i)
    noexcept -> element* {
  using _namespace(std)::find;

  assert(q != nullptr);
  assert(i < q->children_.size());

  element** parent_ptr;
  element*const p = q->child(1 - i);
  element*const b = p->child(i);
  element*const q_parent = q->parent();
  assert(p != nullptr);

  /* Figure out the parent pointer that needs updating. */
  if (q_parent) {
    auto c = find(q_parent->children_.begin(), q_parent->children_.end(), q);
    assert(c != q_parent->children_.end());
    parent_ptr = &*c;
  } else {
    parent_ptr = &root_;
  }

  *parent_ptr = p;
  p->children_[i] = q;
  q->children_[1 - i] = b;
  p->parent_ = p->rb_flag_() | (q->parent_ & PTR_MASK);
  q->parent_ = q->rb_flag_() | reinterpret_cast<uintptr_t>(p);
  b->parent_ = b->rb_flag_() | reinterpret_cast<uintptr_t>(q);

  return p;
}


_namespace_end(ilias)
