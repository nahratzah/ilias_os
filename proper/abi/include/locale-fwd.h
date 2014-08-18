#ifndef _LOCALE_FWD_H_
#define _LOCALE_FWD_H_

#include <cdecl.h>
#ifdef __cplusplus
# include <iterator>
#endif

#ifdef __cplusplus
_namespace_begin(std)


class locale;
template<typename> class ctype;
template<typename, typename, typename> class codecvt;
template<typename Char, typename = istreambuf_iterator<Char>> class num_get;
template<typename Char, typename = ostreambuf_iterator<Char>> class num_put;
template<typename> class numpunct;
template<typename> class collate;
template<typename Char, typename = istreambuf_iterator<Char>> class time_get;
template<typename Char, typename = ostreambuf_iterator<Char>> class time_put;
template<typename Char, typename = istreambuf_iterator<Char>> class money_get;
template<typename Char, typename = ostreambuf_iterator<Char>> class money_put;
template<typename, bool = false> class moneypunct;
template<typename> class messages;


_namespace_end(std)
#endif /* __cplusplus */

#endif /* _LOCALE_FWD_H_ */
