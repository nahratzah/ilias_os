#ifndef _FENV_H_
#define _FENV_H_

#if defined(__x86_64__)
#include <stdimpl/fenv_amd64.h>
#elif defined(__i386__)
#include <stdimpl/fenv_i386.h>
#else
#error "Unsupported platform"
#endif

#if !defined(TEST) && defined(__cplusplus)
using std::feclearexcept;
using std::fegetexceptflag;
using std::fesetexceptflag;
using std::feraiseexcept;
using std::fetestexcept;
using std::fegetround;
using std::fesetround;
using std::fegetenv;
using std::feholdexcept;
using std::fesetenv;
using std::feupdateenv;
using std::feenableexcept;
using std::fedisableexcept;
using std::fegetexcept;
#endif

#endif /* _FENV_H_ */
