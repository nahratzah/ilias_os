#ifndef _STACK_INL_H_
#define _STACK_INL_H_

#include <stack>

_namespace_begin(std)


template<typename T, typename C>
stack<T, C>::stack(const container_type& c)
: c(c)
{}

template<typename T, typename C>
stack<T, C>::stack(container_type&& c)
: c(move(c))
{}

template<typename T, typename C>
stack<T, C>::stack(stack&& s)
: c(move(s.c))
{}

template<typename T, typename C>
template<typename Alloc, typename>
stack<T, C>::stack(const Alloc& a)
: c(a)
{}

template<typename T, typename C>
template<typename Alloc, typename>
stack<T, C>::stack(const container_type& c, const Alloc& a)
: c(c, a)
{}

template<typename T, typename C>
template<typename Alloc, typename>
stack<T, C>::stack(container_type&& c, const Alloc& a)
: c(move(c), a)
{}

template<typename T, typename C>
template<typename Alloc, typename>
stack<T, C>::stack(const stack& s, const Alloc& a)
: c(s.c, a)
{}

template<typename T, typename C>
template<typename Alloc, typename>
stack<T, C>::stack(stack&& s, const Alloc& a)
: c(move(s.c), a)
{}

template<typename T, typename C>
auto stack<T, C>::operator=(stack&& s) -> stack& {
  c = move(s.c);
  return *this;
}

template<typename T, typename C>
auto stack<T, C>::empty() const -> bool {
  return c.empty();
}

template<typename T, typename C>
auto stack<T, C>::size() const -> size_type {
  return c.size();
}

template<typename T, typename C>
auto stack<T, C>::top() -> reference {
  return c.back();
}

template<typename T, typename C>
auto stack<T, C>::top() const -> const_reference {
  return c.back();
}

template<typename T, typename C>
auto stack<T, C>::push(const value_type& v) -> void {
  return c.push_back(v);
}

template<typename T, typename C>
auto stack<T, C>::push(value_type&& v) -> void {
  return c.push_back(move(v));
}

template<typename T, typename C>
template<typename... Args>
auto stack<T, C>::emplace(Args&&... args) -> void {
  return c.emplace_back(forward<Args>(args)...);
}

template<typename T, typename C>
auto stack<T, C>::pop() -> void {
  return c.pop_back();
}

template<typename T, typename C>
auto stack<T, C>::swap(stack& s) -> void {
  using _namespace(std)::swap;

  swap(c, s.c);
}

template<typename T, typename C>
bool operator==(const stack<T, C>& x, const stack<T, C>& y) {
  return x.c == y.c;
}

template<typename T, typename C>
bool operator!=(const stack<T, C>& x, const stack<T, C>& y) {
  return x.c != y.c;
}

template<typename T, typename C>
bool operator<(const stack<T, C>& x, const stack<T, C>& y) {
  return x.c < y.c;
}

template<typename T, typename C>
bool operator>(const stack<T, C>& x, const stack<T, C>& y) {
  return x.c > y.c;
}

template<typename T, typename C>
bool operator<=(const stack<T, C>& x, const stack<T, C>& y) {
  return x.c <= y.c;
}

template<typename T, typename C>
bool operator>=(const stack<T, C>& x, const stack<T, C>& y) {
  return x.c >= y.c;
}

template<typename T, typename C>
void swap(stack<T, C>& x, stack<T, C>& y) {
  x.swap(y);
}


_namespace_end(std)

#endif /* _STACK_INL_H_ */
