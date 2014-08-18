#ifndef _LOCALE_MISC_MESSAGES_INL_H_
#define _LOCALE_MISC_MESSAGES_INL_H_

#include <locale_misc/messages.h>

_namespace_begin(std)


template<typename Char>
messages<Char>::messages(size_t refs)
: locale::facet(refs)
{}

template<typename Char>
auto messages<Char>::open(const string& name, const locale& loc) const ->
    catalog {
  return do_open(name, loc);
}

template<typename Char>
auto messages<Char>::get(catalog cat, int set, int msgid,
                         const string_type& dfl) const -> string_type {
  return do_get(cat, set, msgid, dfl);
}

template<typename Char>
auto messages<Char>::close(catalog cat) const -> void {
  do_close(cat);
}

template<typename Char>
messages<Char>::~messages() noexcept {}


template<typename Char>
messages_byname<Char>::messages_byname(const char* name, size_t refs)
: messages_byname(string_ref(name), refs)
{}

template<typename Char>
messages_byname<Char>::messages_byname(const string& name, size_t refs)
: messages_byname(string_ref(name), refs)
{}

template<typename Char>
messages_byname<Char>::messages_byname(string_ref name, size_t refs)
: messages<Char>(refs),
  impl::facet_ref<messages<Char>>(locale(name))
{}

template<typename Char>
auto messages_byname<Char>::do_open(const string& name, const locale& loc)
    const -> catalog {
  return this->impl.open(name, loc);
}

template<typename Char>
auto messages_byname<Char>::do_get(catalog cat, int set, int msgid,
                                   const string_type& dfl) const ->
    string_type {
  return this->impl.get(cat, set, msgid, dfl);
}

template<typename Char>
auto messages_byname<Char>::do_close(catalog cat) const -> void {
  return this->impl.close(cat);
}


_namespace_end(std)

#endif /* _LOCALE_MISC_MESSAGES_INL_H_ */
