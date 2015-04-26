#ifndef _IEEEFP_H_
#define _IEEEFP_H_

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
# include <stdimpl/ieeefp_amd64.h>
#else
# error "Unsupported platform."
#endif

#if !defined(TEST) && defined(__cplusplus)
using std::fp_rnd_t;
using std::fp_prec_t;
using std::fp_except_t;

using std::fpgetround;
using std::fpsetround;
using std::fpgetprec;
using std::fpsetprec;
using std::fpgetmask;
using std::fpsetmask;
using std::fpgetsticky;
#endif

#endif /* _IEEEFP_H_ */
