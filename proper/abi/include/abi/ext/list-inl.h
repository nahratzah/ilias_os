namespace __cxxabiv1 {
namespace ext {


template<typename Tag> constexpr list_elem<Tag>::list_elem() {}

template<typename Tag> constexpr list_elem<Tag>::list_elem(list_elem* init)
: succ_{ init },
  pred_{ init }
{}

template<typename Tag> constexpr list_elem<Tag>::list_elem(const list_elem&) {}

template<typename Tag> list_elem<Tag>& list_elem<Tag>::operator=(
    const list_elem&) noexcept {
  return *this;
}


template<typename T, typename Tag>
list<T, Tag>::list(list&& o) noexcept {
  head_.succ_ = o.head_.succ_;
  head_.pred_ = o.head_.pred_;
  o.head_.succ_ = &o.head_;
  o.head_.pred_ = &o.head_;
  head_.succ_.pred_ = &head_;
  head_.pred_.succ_ = &head_;
}

template<typename T, typename Tag>
auto list<T, Tag>::operator=(list&& o) noexcept -> list& {
  swap(o);
}

template<typename T, typename Tag>
auto list<T, Tag>::swap(list& o) noexcept -> void {
  using std::swap;

  swap(head_.succ_, o.head_.succ_);
  swap(head_.pred_, o.head_.pred_);
  head_.succ_.pred_ = head_.pred_.succ_ = &head_;
  o.head_.succ_.pred_ = o.head_.pred_.succ_ = &o.head_;
}

template<typename T, typename Tag>
auto list<T, Tag>::empty() const noexcept -> bool {
  return head_.succ_ != &head_;
}

template<typename T, typename Tag>
auto list<T, Tag>::is_linked(const_pointer p) noexcept -> bool {
  if (!p) return false;
  const list_elem<Tag>* p_ = p;
  return p_->succ_ != nullptr;
}

template<typename T, typename Tag>
auto list<T, Tag>::link_front(const_pointer p) noexcept -> bool {
  return link_after_(p, &head_);
}

template<typename T, typename Tag>
auto list<T, Tag>::link_back(const_pointer p) noexcept -> bool {
  return link_before_(p, &head_);
}

template<typename T, typename Tag>
auto list<T, Tag>::unlink(const_pointer e_ptr) noexcept -> bool {
  if (!is_linked(e_ptr)) return false;
  const list_elem<Tag>* e = e_ptr;
  list_elem<Tag>*const s = e->succ_;
  list_elem<Tag>*const p = e->pred_;

  s->pred_ = p;
  p->succ_ = s;
  e->succ_ = nullptr;
  e->pred_ = nullptr;
  return true;
}

template<typename T, typename Tag>
auto list<T, Tag>::link_before(const_pointer e, const_iterator pos) noexcept ->
    bool {
  const list_elem<Tag>* p = &*pos;
  return link_before_(e, const_cast<list_elem<Tag>*>(p));
}

template<typename T, typename Tag>
auto list<T, Tag>::link_after(const_pointer e, const_iterator pos) noexcept ->
    bool {
  const list_elem<Tag>* p = &*pos;
  return link_after_(e, const_cast<list_elem<Tag>*>(p));
}

template<typename T, typename Tag>
auto list<T, Tag>::unlink(const_iterator pos) noexcept -> bool {
  return unlink(&*pos);
}

template<typename T, typename Tag>
auto list<T, Tag>::begin() noexcept -> iterator {
  return iterator{ succ(&head_) };
}

template<typename T, typename Tag>
auto list<T, Tag>::end() noexcept -> iterator {
  return iterator{ &head_ };
}

template<typename T, typename Tag>
auto list<T, Tag>::begin() const noexcept -> const_iterator {
  return const_iterator{ succ(&head_) };
}

template<typename T, typename Tag>
auto list<T, Tag>::end() const noexcept -> const_iterator {
  return const_iterator{ &head_ };
}

template<typename T, typename Tag>
auto list<T, Tag>::rbegin() noexcept -> reverse_iterator {
  return reverse_iterator{ pred(&head_) };
}

template<typename T, typename Tag>
auto list<T, Tag>::rend() noexcept -> reverse_iterator {
  return reverse_iterator{ &head_ };
}

template<typename T, typename Tag>
auto list<T, Tag>::rbegin() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator{ pred(&head_) };
}

template<typename T, typename Tag>
auto list<T, Tag>::rend() const noexcept -> const_reverse_iterator {
  return const_reverse_iterator{ &head_ };
}

template<typename T, typename Tag>
auto list<T, Tag>::cbegin() const noexcept -> const_iterator {
  return begin();
}

template<typename T, typename Tag>
auto list<T, Tag>::cend() const noexcept -> const_iterator {
  return end();
}

template<typename T, typename Tag>
auto list<T, Tag>::crbegin() const noexcept -> const_reverse_iterator {
  return rbegin();
}

template<typename T, typename Tag>
auto list<T, Tag>::crend() const noexcept -> const_reverse_iterator {
  return rend();
}

template<typename T, typename Tag>
auto list<T, Tag>::iterator_to(pointer p) noexcept -> iterator {
  if (!p) return end();
  return iterator{ p };
}

template<typename T, typename Tag>
auto list<T, Tag>::iterator_to(const_pointer p) const noexcept ->
    const_iterator {
  if (!p) return end();
  return const_iterator{ p };
}

template<typename T, typename Tag>
auto list<T, Tag>::link_after_(const_pointer e_ptr, const list_elem<Tag>* p)
    noexcept -> bool {
  if (!e_ptr || is_linked(e_ptr)) return false;
  const list_elem<Tag>* e = e_ptr;
  const list_elem<Tag>* s = p->succ_;

  p->succ_ = const_cast<list_elem<Tag>*>(e);
  s->pred_ = const_cast<list_elem<Tag>*>(e);
  e->pred_ = const_cast<list_elem<Tag>*>(p);
  e->succ_ = const_cast<list_elem<Tag>*>(s);
  return true;
}

template<typename T, typename Tag>
auto list<T, Tag>::link_before_(const_pointer e, const list_elem<Tag>* s)
    noexcept -> bool {
  return link_after_(e, s->pred_);
}

template<typename T, typename Tag>
auto list<T, Tag>::pred(list_elem<Tag>* e) noexcept -> list_elem<Tag>* {
  return (e ? e->pred_ : nullptr);
}

template<typename T, typename Tag>
auto list<T, Tag>::pred(const list_elem<Tag>* e) noexcept ->
    const list_elem<Tag>* {
  return (e ? e->pred_ : nullptr);
}

template<typename T, typename Tag>
auto list<T, Tag>::succ(list_elem<Tag>* e) noexcept -> list_elem<Tag>* {
  return (e ? e->succ_ : nullptr);
}

template<typename T, typename Tag>
auto list<T, Tag>::succ(const list_elem<Tag>* e) noexcept ->
    const list_elem<Tag>* {
  return (e ? e->succ_ : nullptr);
}

template<typename T, typename Tag>
auto list<T, Tag>::down_cast(list_elem<Tag>* e) noexcept -> T* {
  return static_cast<T*>(e);
}

template<typename T, typename Tag>
auto list<T, Tag>::down_cast(const list_elem<Tag>* e) noexcept -> const T* {
  return static_cast<const T*>(e);
}


template<typename Tag>
auto iterator_order<true, Tag>::pred(elem_t* e) noexcept -> elem_t* {
  return (e ? e->pred_ : nullptr);
}
template<typename Tag>
auto iterator_order<true, Tag>::pred(const_elem_t* e) noexcept ->
    const_elem_t* {
  return (e ? e->pred_ : nullptr);
}
template<typename Tag>
auto iterator_order<true, Tag>::succ(elem_t* e) noexcept -> elem_t* {
  return (e ? e->succ_ : nullptr);
}
template<typename Tag>
auto iterator_order<true, Tag>::succ(const_elem_t* e) noexcept ->
    const_elem_t* {
  return (e ? e->succ_ : nullptr);
}


template<typename Tag>
auto iterator_order<false, Tag>::pred(elem_t* e) noexcept -> elem_t* {
  return (e ? e->succ_ : nullptr);
}
template<typename Tag>
auto iterator_order<false, Tag>::pred(const_elem_t* e) noexcept ->
    const_elem_t* {
  return (e ? e->succ_ : nullptr);
}
template<typename Tag>
auto iterator_order<false, Tag>::succ(elem_t* e) noexcept -> elem_t* {
  return (e ? e->pred_ : nullptr);
}
template<typename Tag>
auto iterator_order<false, Tag>::succ(const_elem_t* e) noexcept ->
    const_elem_t* {
  return (e ? e->pred_ : nullptr);
}


template<typename T, typename Tag>
template<typename IT, typename IE, bool Order>
constexpr list<T, Tag>::iterator_tmpl<IT, IE, Order>::iterator_tmpl(IE* elem)
: elem_(elem)
{}

template<typename T, typename Tag>
template<typename IT, typename IE, bool Order>
auto list<T, Tag>::iterator_tmpl<IT, IE, Order>::operator*() const noexcept ->
    reference {
  return *down_cast(elem_);
}

template<typename T, typename Tag>
template<typename IT, typename IE, bool Order>
auto list<T, Tag>::iterator_tmpl<IT, IE, Order>::operator->() const noexcept ->
    pointer {
  return down_cast(elem_);
}

template<typename T, typename Tag>
template<typename IT, typename IE, bool Order>
auto list<T, Tag>::iterator_tmpl<IT, IE, Order>::operator++() noexcept ->
    iterator_tmpl& {
  elem_ = this->succ(elem_);
  return *this;
}

template<typename T, typename Tag>
template<typename IT, typename IE, bool Order>
auto list<T, Tag>::iterator_tmpl<IT, IE, Order>::operator--() noexcept ->
    iterator_tmpl& {
  elem_ = this->pred(elem_);
  return *this;
}

template<typename T, typename Tag>
template<typename IT, typename IE, bool Order>
auto list<T, Tag>::iterator_tmpl<IT, IE, Order>::operator++(int) noexcept ->
    iterator_tmpl {
  iterator_tmpl rv = *this;
  ++(*this);
  return rv;
}

template<typename T, typename Tag>
template<typename IT, typename IE, bool Order>
auto list<T, Tag>::iterator_tmpl<IT, IE, Order>::operator--(int) noexcept ->
    iterator_tmpl {
  iterator_tmpl rv = *this;
  --(*this);
  return rv;
}

template<typename T, typename Tag>
template<typename IT, typename IE, bool Order>
template<typename O_IT, typename O_IE, bool O_Order>
list<T, Tag>::iterator_tmpl<IT, IE, Order>::iterator_tmpl(
    const iterator_tmpl<O_IT, O_IE, O_Order>& o) noexcept
: iterator_tmpl()
{
  *this = o;
}

template<typename T, typename Tag>
template<typename IT, typename IE, bool Order>
template<typename O_IT, typename O_IE, bool O_Order>
auto list<T, Tag>::iterator_tmpl<IT, IE, Order>::operator=(
    const iterator_tmpl<O_IT, O_IE, O_Order>& o) noexcept -> iterator_tmpl& {
  elem_ = &*o;
  return *this;
}


template<typename T, typename Tag>
auto swap(list<T, Tag>& a, list<T, Tag>& b) noexcept -> void {
  return a.swap(b);
}

template<typename Tag, typename T>
auto unlink(T* e) noexcept -> bool {
  return list<T, Tag>::unlink(e);
}


}} /* namespace __cxxabiv1::ext */
