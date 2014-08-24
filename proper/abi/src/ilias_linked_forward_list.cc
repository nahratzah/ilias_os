#include <ilias/linked_forward_list.h>
#include <cassert>
#include <utility>

_namespace_begin(ilias)


auto basic_linked_forward_list::unlink(element* e) noexcept -> element* {
  using _namespace(std)::exchange;

  assert(e != nullptr);

  iterator pred;
  for (pred = before_begin(); pred->succ_ != e; ++pred);
  pred->succ_ = exchange(e->succ_, nullptr);
  return e;
}

auto basic_linked_forward_list::splice_after(
    iterator i, basic_linked_forward_list& o) noexcept -> void {
  using _namespace(std)::exchange;

  assert(this != &o);

  if (!o.empty()) {
    if (i->succ_ == nullptr) {
      i->succ_ = exchange(o.before_.succ_, nullptr);
    } else {
      element*& o_end_ptr = o.find_succ_for_(nullptr);
      o_end_ptr = exchange(i->succ_, exchange(o.before_.succ_, nullptr));
    }
  }
}

auto basic_linked_forward_list::splice_after(
    iterator i,
    basic_linked_forward_list& o, iterator o_b, iterator o_e)
    noexcept -> void {
  using _namespace(std)::exchange;

  if (o_b != o_e) {
    /* Find succ_ pointing at o_b.elem_. */
    element*& o_b_ptr = o.find_succ_for_(o_b.elem_);

    if (o_e.elem_ == nullptr && i->succ_ == nullptr) {
      i->succ_ = exchange(o_b_ptr, nullptr);
    } else {
      /* Find succ_ pointing at o_e.elem_. */
      element*& o_e_ptr = o.find_succ_for_(o_e.elem_);

      o_b_ptr = o_e.elem_;  // Unlink [o_b..o_e)
      o_e_ptr = exchange(i->succ_, o_b.elem_);  // Link [o_b..o_e) after i.
    }
  }
}

auto basic_linked_forward_list::splice(ptr_iterator i,
                                       ptr_iterator o_b, ptr_iterator o_e)
    noexcept -> _namespace(std)::tuple<ptr_iterator, ptr_iterator> {
  using _namespace(std)::exchange;
  using _namespace(std)::make_tuple;

  if (o_b != o_e) {
    /* Find succ_ pointing at o_b.elem_. */
    element*& o_b_ptr = o_b.get_ptr();

    if (o_e.get_ptr() == nullptr && i.get_ptr() == nullptr) {
      i.get_ptr() = exchange(o_b_ptr, nullptr);
    } else {
      /* Find succ_ pointing at o_e.elem_. */
      element*& o_e_ptr = o_e.get_ptr();

      o_e_ptr = exchange(i.get_ptr(), exchange(o_b_ptr, o_e.get_ptr()));
    }

    return make_tuple(o_e, o_b);
  } else {
    return make_tuple(i, o_b);
  }
}

auto basic_linked_forward_list::find_succ_for_(element* e) noexcept ->
    element*& {
  element** ptr = &before_.succ_;
  while (*ptr != e) {
    assert_msg(*ptr != nullptr, "Element not in linked_forward_list!");
    ptr = &(*ptr)->succ_;
  }
  return *ptr;
}


_namespace_end(ilias)
