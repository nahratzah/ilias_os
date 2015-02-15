#ifndef _STDIMPL_LOCALE_CATALOGS_H_
#define _STDIMPL_LOCALE_CATALOGS_H_

#include <cdecl.h>
#include <locale>
#include <string>

_namespace_begin(std)
namespace impl {


extern const string ilias_c_catalog_name;

enum class ilias_c_catalog_set : int {
  strerror = 0,
};


template<typename Char>
class catalog_handle {
 public:
  catalog_handle() = delete;
  catalog_handle(const catalog_handle&) = delete;
  catalog_handle& operator=(const catalog_handle&) = delete;

  catalog_handle(const messages<Char>&, typename messages<Char>::catalog)
      noexcept;
  catalog_handle(const messages<Char>&, const string&, const locale&);
  catalog_handle(const locale&, const string&);
  ~catalog_handle() noexcept;

  basic_string<Char> get(int set, int msgid, const basic_string<Char>& dfl);

 private:
  const messages<Char>& repo_;
  const typename messages<Char>::catalog cata_;
};


template<typename Char>
auto get_message(const string&, int, int, const basic_string<Char>&,
                 const locale& = locale()) -> basic_string<Char>;

template<typename Char>
auto get_message(ilias_c_catalog_set, int, const basic_string<Char>&,
                 const locale& = locale()) -> basic_string<Char>;

template<typename Char>
auto strerror_dfl(int, const locale& = locale()) -> basic_string<Char>;

template<typename Char>
auto strerror(int, const locale& = locale()) -> basic_string<Char>;


} /* namespace std::impl */
_namespace_end(std)

#include <stdimpl/locale_catalogs-inl.h>

#endif /* _STDIMPL_LOCALE_CATALOGS_H_ */
