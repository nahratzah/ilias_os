#ifndef _STDIMPL_ALLOC_DELETER_INL_H_
#define _STDIMPL_ALLOC_DELETER_INL_H_

#include <stdimpl/alloc_deleter.h>
#include <cdecl.h>
#include <memory>
#include <utility>
#include <type_traits>

_namespace_begin(std)
namespace impl {


template<typename Alloc>
alloc_deleter<Alloc>::alloc_deleter(Alloc& alloc) noexcept
: alloc_(&alloc)
{}

template<typename Alloc>
auto alloc_deleter<Alloc>::mark_constructed() noexcept -> void {
  destroy_ = true;
}

template<typename Alloc>
template<typename T>
auto alloc_deleter<Alloc>::operator()(T* v) const noexcept -> void {
  using alloc_traits = allocator_traits<Alloc>;

  assert(v != nullptr);
  assert(alloc_ != nullptr);

  if (destroy_) alloc_traits::destroy(*alloc_, v);
  alloc_traits::deallocate(*alloc_, v, 1);
}


template<typename T, typename Alloc, typename Hint, typename... Args>
auto new_alloc_deleter(Alloc& alloc, Hint&& hint, Args&&... args) ->
    alloc_deleter_ptr<T, Alloc> {
  using alloc_traits = allocator_traits<Alloc>;
  using ptr_t = alloc_deleter_ptr<T, Alloc>;

  ptr_t ptr = ptr_t(alloc_traits::allocate(alloc, 1, forward<Hint>(hint)),
                    alloc);
  alloc_traits::construct(alloc, ptr.get(), forward<Args>(args)...);
  ptr.get_deleter().mark_constructed();
  return ptr;
}

template<typename T, typename Alloc>
auto existing_alloc_deleter(Alloc& alloc, T* p) ->
    alloc_deleter_ptr<T, Alloc> {
  return alloc_deleter_ptr<T, Alloc>(p, alloc_deleter_visitor(alloc));
}

template<typename Alloc>
auto alloc_deleter_visitor(Alloc& alloc) noexcept ->
    alloc_deleter<Alloc> {
  auto rv = alloc_deleter<Alloc>(alloc);
  rv.mark_constructed();
  return rv;
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_ALLOC_DELETER_INL_H_ */
