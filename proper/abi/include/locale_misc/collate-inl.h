#ifndef _LOCALE_MISC_COLLATE_INL_H_
#define _LOCALE_MISC_COLLATE_INL_H_

#include <locale_misc/collate.h>

_namespace_begin(std)


template<typename Char>
collate<Char>::collate(size_t refs)
: locale::facet(refs)
{}

template<typename Char>
collate<Char>::~collate() noexcept {}

template<typename Char>
auto collate<Char>::compare(const char_type* b1, const char_type* e1,
                      const char_type* b2, const char_type* e2) const -> int {
  return do_compare(b1, e1, b2, e2);
}

template<typename Char>
auto collate<Char>::transform(const char_type* b, const char_type* e) const ->
    string_type {
  return do_transform(b, e);
}

template<typename Char>
auto collate<Char>::hash(const char_type* b, const char_type* e) const ->
    long {
  return do_hash(b, e);
}

template<typename Char>
auto collate<Char>::do_compare(const char_type* b1, const char_type* e1,
                         const char_type* b2, const char_type* e2) const ->
    int {
  int rv = char_traits<Char>::compare(b1, e1, min(e1 - b1, e2 - b2));
  if (rv == 0) {
    if (e1 - b1 < e2 - b2) return -1;
    if (e1 - b1 > e2 - b2) return  1;
  }
  return 0;
}

template<typename Char>
auto collate<Char>::do_transform(const char_type* b, const char_type* e)
    const -> string_type {
  return string_type(b, e);
}

template<typename Char>
auto collate<Char>::do_hash(const char_type* b, const char_type* e) const ->
    long {
  using string = basic_string_ref<Char>;
  using hash_t = _namespace(std)::hash<string>;

  return hash_t()(string(b, typename string::size_type(e - b)));
}


template<typename Char>
collate_byname<Char>::collate_byname(const char* name, size_t refs)
: collate_byname(string_ref(name), refs)
{}

template<typename Char>
collate_byname<Char>::collate_byname(const string& name, size_t refs)
: collate_byname(string_ref(name), refs)
{}

template<typename Char>
collate_byname<Char>::collate_byname(string_ref name, size_t refs)
: collate<Char>(refs),
  impl::facet_ref<collate<Char>>(locale(name))
{}

template<typename Char>
auto collate_byname<Char>::do_compare(const char_type* b1, const char_type* e1,
                                      const char_type* b2, const char_type* e2)
    const -> int {
  return this->impl.compare(b1, e1, b2, e2);
}

template<typename Char>
auto collate_byname<Char>::do_transform(const char_type* b, const char_type* e)
    const -> string_type {
  return this->impl.transform(b, e);
}

template<typename Char>
auto collate_byname<Char>::do_hash(const char_type* b, const char_type* e)
    const -> long {
  return this->impl.hash(b, e);
}


_namespace_end(std)

#endif /* _LOCALE_MISC_COLLATE_INL_H_ */
