#ifndef _STDIMPL_ALLOC_DELETER_H_
#define _STDIMPL_ALLOC_DELETER_H_

#include <cdecl.h>
#include <memory>

_namespace_begin(std)
namespace impl {


template<typename Alloc>
class alloc_deleter {
 public:
  alloc_deleter() noexcept = default;
  alloc_deleter(Alloc&) noexcept;
  alloc_deleter(const alloc_deleter&) noexcept = default;
  alloc_deleter& operator=(const alloc_deleter&) noexcept = default;

  void mark_constructed() noexcept;

  template<typename T> void operator()(T* v) const noexcept;

 private:
  Alloc* alloc_ = nullptr;
  bool destroy_ = false;
};

template<typename T, typename Alloc> using alloc_deleter_ptr =
    unique_ptr<T, alloc_deleter<Alloc>>;

template<typename T, typename Alloc, typename Hint, Args&&...>
auto new_alloc_deleter(Alloc&, Hint&&, Args&&...) ->
    alloc_deleter_ptr<T, Alloc>;

template<typename T, typename Alloc>
auto existing_alloc_deleter(Alloc&, T*) ->
    alloc_deleter_ptr<T, Alloc>;

template<typename Alloc>
auto alloc_deleter_visitor(Alloc&) noexcept ->
    alloc_deleter<Alloc>;


} /* namespace std::impl */
_namespace_end(std)

#include <stdimpl/alloc_deleter-inl.h>

#endif /* _STDIMPL_ALLOC_DELETER_H_ */
