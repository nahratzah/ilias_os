#ifndef _STDIMPL_LOCALE_CATALOGS_INL_H_
#define _STDIMPL_LOCALE_CATALOGS_INL_H_

#include <stdimpl/locale_catalogs.h>
#include <locale>
#include <string>
#include <sstream>
#include <abi/errno.h>

_namespace_begin(std)
namespace impl {


template<typename Char>
catalog_handle<Char>::catalog_handle(const messages<Char>& repo,
                                     typename messages<Char>::catalog cata)
    noexcept
: repo_(repo),
  cata_(cata)
{}

template<typename Char>
catalog_handle<Char>::catalog_handle(const messages<Char>& repo,
                                     const string& name,
                                     const locale& loc)
: catalog_handle(repo, repo.open(name, loc))
{}

template<typename Char>
catalog_handle<Char>::catalog_handle(const locale& loc, const string& name)
: catalog_handle(use_facet<messages<Char>>(loc), name, loc)
{}

template<typename Char>
catalog_handle<Char>::~catalog_handle() noexcept {
  repo_.close(cata_);
}

template<typename Char>
auto catalog_handle<Char>::get(int set, int msgid,
                               const basic_string<Char>& dfl) ->
    basic_string<Char> {
  return repo_.get(cata_, set, msgid, dfl);
}


template<typename Char>
auto get_message(const string& name, int set, int msgid,
                 const basic_string<Char>& dfl,
                 const locale& loc) -> basic_string<Char> {
  return catalog_handle<Char>(loc, name).get(set, msgid, dfl);
}

template<typename Char>
auto get_message(ilias_c_catalog_set set, int msgid,
                 const basic_string<Char>& dfl,
                 const locale& loc) -> basic_string<Char> {
  return get_message(ilias_c_catalog_name, static_cast<int>(set), msgid, dfl,
                     loc);
}

template<typename Char>
auto strerror_dfl(int errnum, const locale& loc) ->
    basic_string<Char> {
  using abi::sys_nerr;
  using abi::sys_errlist;

  basic_string<Char> dfl;
  if (errnum < 0 || errnum >= sys_nerr) {
    basic_ostringstream<Char> oss;
    oss.imbue(loc);
    oss << string_ref("unknown error ") << errnum;
    dfl = oss.str();
  } else {
    dfl = string_ref(sys_errlist[errnum]);
  }

  return dfl;
}

template<typename Char>
auto strerror(int errnum, const locale& loc) -> basic_string<Char> {
  return get_message(ilias_c_catalog_set::strerror, errnum,
                     strerror_dfl<Char>(errnum, loc), loc);
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_LOCALE_CATALOGS_INL_H_ */
