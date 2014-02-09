#ifndef _TIME_MISC_TM_H_
#define _TIME_MISC_TM_H_

#include <time_misc/tm_fwd.h>
#include <cdecl.h>

_namespace_begin(std)

struct tm {
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};

_namespace_end(std)

#endif /* _TIME_MISC_TM_H_ */
