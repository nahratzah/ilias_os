#ifndef _STDIMPL_COMBINE_ALIGN_H_
#define _STDIMPL_COMBINE_ALIGN_H_

_namespace_begin(std)
namespace impl {

template<typename...> struct combine_align;

template<> struct combine_align<> {
  using type = char;
};

template<typename T> struct combine_align<T> {
  using type = T;
};

template<typename A, typename B, typename... Tail>
struct combine_align<A, B, Tail...> {
  union type {
    A a;
    B b;
    typename combine_align<Tail...>::type other;
  };
};

} /* namespace impl */
_namespace_end(std)

#endif /* _STDIMPL_COMBINE_ALIGN_H_ */
