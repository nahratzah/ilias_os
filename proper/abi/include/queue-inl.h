#ifndef _QUEUE_INL_H_
#define _QUEUE_INL_H_

#include <queue>
#include <utility>
#include <algorithm>

_namespace_begin(std)


template<typename T, typename C>
queue<T, C>::queue(const container_type& c)
: c(c)
{}

template<typename T, typename C>
queue<T, C>::queue(container_type&& c)
: c(move(c))
{}

template<typename T, typename C>
queue<T, C>::queue(queue&& q)
: c(move(q.c))
{}

template<typename T, typename C>
template<typename Alloc, typename>
queue<T, C>::queue(const Alloc& a)
: c(a)
{}

template<typename T, typename C>
template<typename Alloc, typename>
queue<T, C>::queue(const container_type& c, const Alloc& a)
: c(c, a)
{}

template<typename T, typename C>
template<typename Alloc, typename>
queue<T, C>::queue(container_type&& c, const Alloc& a)
: c(move(c), a)
{}

template<typename T, typename C>
template<typename Alloc, typename>
queue<T, C>::queue(const queue& q, const Alloc& a)
: c(q.c, a)
{}

template<typename T, typename C>
template<typename Alloc, typename>
queue<T, C>::queue(queue&& q, const Alloc& a)
: c(move(q.c), a)
{}

template<typename T, typename C>
auto queue<T, C>::operator=(queue&& q) -> queue& {
  c = move(q.c);
  return *this;
}

template<typename T, typename C>
auto queue<T, C>::empty() const -> bool {
  return c.empty();
}

template<typename T, typename C>
auto queue<T, C>::size() const -> size_type {
  return c.size();
}

template<typename T, typename C>
auto queue<T, C>::front() -> reference {
  return c.front();
}

template<typename T, typename C>
auto queue<T, C>::front() const -> const_reference {
  return c.front();
}

template<typename T, typename C>
auto queue<T, C>::back() -> reference {
  return c.back();
}

template<typename T, typename C>
auto queue<T, C>::back() const -> const_reference {
  return c.back();
}

template<typename T, typename C>
auto queue<T, C>::push(const value_type& v) -> void {
  c.push_back(v);
}

template<typename T, typename C>
auto queue<T, C>::push(value_type&& v) -> void {
  c.push_back(move(v));
}

template<typename T, typename C>
template<typename... Args>
auto queue<T, C>::emplace(Args&&... args) -> void {
  c.emplace_back(forward<Args>(args)...);
}

template<typename T, typename C>
auto queue<T, C>::pop() -> void {
  c.pop_front();
}

template<typename T, typename C>
auto queue<T, C>::swap(queue& q) -> void {
  using _namespace(std)::swap;

  swap(c, q.c);
}


template<typename T, typename C>
bool operator==(const queue<T, C>& x, const queue<T, C>& y) {
  return x.c == y.c;
}

template<typename T, typename C>
bool operator!=(const queue<T, C>& x, const queue<T, C>& y) {
  return x.c != y.c;
}

template<typename T, typename C>
bool operator<(const queue<T, C>& x, const queue<T, C>& y) {
  return x.c < y.c;
}

template<typename T, typename C>
bool operator>(const queue<T, C>& x, const queue<T, C>& y) {
  return x.c > y.c;
}

template<typename T, typename C>
bool operator<=(const queue<T, C>& x, const queue<T, C>& y) {
  return x.c <= y.c;
}

template<typename T, typename C>
bool operator>=(const queue<T, C>& x, const queue<T, C>& y) {
  return x.c >= y.c;
}

template<typename T, typename C>
void swap(queue<T, C>& x, queue<T, C>& y) {
  x.swap(y);
}


template<typename T, typename C, typename Cmp>
priority_queue<T, C, Cmp>::priority_queue(const Cmp& cmp,
                                          const container_type& cc)
: c(cc),
  comp(cmp)
{
  make_heap(c.begin(), c.end(), comp);
}

template<typename T, typename C, typename Cmp>
priority_queue<T, C, Cmp>::priority_queue(const Cmp& cmp,
                                          container_type&& cc)
: c(move(cc)),
  comp(cmp)
{
  make_heap(c.begin(), c.end(), comp);
}

template<typename T, typename C, typename Cmp>
template<typename InputIter>
priority_queue<T, C, Cmp>::priority_queue(InputIter b, InputIter e,
                                          const Cmp& cmp,
                                          const container_type& cc)
: c(cc),
  comp(cmp)
{
  c.insert(c.begin(), b, e);
  make_heap(c.begin(), c.end(), comp);
}

template<typename T, typename C, typename Cmp>
template<typename InputIter>
priority_queue<T, C, Cmp>::priority_queue(InputIter b, InputIter e,
                                          const Cmp& cmp,
                                          container_type&& cc)
: c(move(cc)),
  comp(cmp)
{
  c.insert(c.begin(), b, e);
  make_heap(c.begin(), c.end(), comp);
}

template<typename T, typename C, typename Cmp>
priority_queue<T, C, Cmp>::priority_queue(priority_queue&& q)
: c(move(q.c)),
  comp(move(q.comp))
{}

template<typename T, typename C, typename Cmp>
template<typename Alloc, typename>
priority_queue<T, C, Cmp>::priority_queue(const Alloc& a)
: c(a),
  comp()
{}

template<typename T, typename C, typename Cmp>
template<typename Alloc, typename>
priority_queue<T, C, Cmp>::priority_queue(const Cmp& cmp, const Alloc& a)
: c(a),
  comp(cmp)
{}

template<typename T, typename C, typename Cmp>
template<typename Alloc, typename>
priority_queue<T, C, Cmp>::priority_queue(const Cmp& cmp,
                                          const container_type& cc,
                                          const Alloc& a)
: c(cc, a),
  comp(cmp)
{
  make_heap(c.begin(), c.end(), comp);
}

template<typename T, typename C, typename Cmp>
template<typename Alloc, typename>
priority_queue<T, C, Cmp>::priority_queue(const Cmp& cmp,
                                          container_type&& cc,
                                          const Alloc& a)
: c(move(cc), a),
  comp(cmp)
{
  make_heap(c.begin(), c.end(), comp);
}

template<typename T, typename C, typename Cmp>
template<typename Alloc, typename>
priority_queue<T, C, Cmp>::priority_queue(const priority_queue& q,
                                          const Alloc& a)
: c(q.c, a),
  comp(q.comp)
{}

template<typename T, typename C, typename Cmp>
template<typename Alloc, typename>
priority_queue<T, C, Cmp>::priority_queue(priority_queue&& q,
                                          const Alloc& a)
: c(move(q.c), a),
  comp(q.comp)
{}

template<typename T, typename C, typename Cmp>
auto priority_queue<T, C, Cmp>::operator=(priority_queue&& q) ->
    priority_queue& {
  c = move(q.c);
  comp = move(q.comp);
  return *this;
}

template<typename T, typename C, typename Cmp>
auto priority_queue<T, C, Cmp>::empty() const -> bool {
  return c.empty();
}

template<typename T, typename C, typename Cmp>
auto priority_queue<T, C, Cmp>::size() const -> size_type {
  return c.size();
}

template<typename T, typename C, typename Cmp>
auto priority_queue<T, C, Cmp>::top() const -> const_reference {
  return c.front();
}

template<typename T, typename C, typename Cmp>
auto priority_queue<T, C, Cmp>::push(const value_type& v) -> void {
  c.push_back(v);
  push_heap(c.begin(), c.end(), comp);
}

template<typename T, typename C, typename Cmp>
auto priority_queue<T, C, Cmp>::push(value_type&& v) -> void {
  c.push_back(move(v));
  push_heap(c.begin(), c.end(), comp);
}

template<typename T, typename C, typename Cmp>
template<typename... Args>
auto priority_queue<T, C, Cmp>::emplace(Args&&... args) -> void {
  c.emplace_back(forward<Args>(args)...);
  push_heap(c.begin(), c.end(), comp);
}

template<typename T, typename C, typename Cmp>
auto priority_queue<T, C, Cmp>::pop() -> void {
  pop_heap(c.begin(), c.end(), comp);
  c.pop_back();
}

template<typename T, typename C, typename Cmp>
auto priority_queue<T, C, Cmp>::swap(priority_queue& q) -> void {
  using _namespace(std)::swap;

  swap(c, q.c);
  swap(comp, q.comp);
}

template<typename T, typename C, typename Cmp>
void swap(priority_queue<T, C, Cmp>& x, priority_queue<T, C, Cmp>& y) {
  x.swap(y);
}


_namespace_end(std)

#endif /* _QUEUE_INL_H_ */
