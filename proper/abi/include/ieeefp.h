#ifndef _IEEEFP_H_
#define _IEEEFP_H_

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
# include <stdimpl/ieeefp_amd64.h>
#else
# error "Unsupported platform."
#endif

#if !defined(TEST) && defined(__cplusplus)
using _namespace(std)::fp_rnd_t;
using _namespace(std)::fp_prec_t;
using _namespace(std)::fp_except_t;

using _namespace(std)::fpgetround;
using _namespace(std)::fpsetround;
using _namespace(std)::fpgetprec;
using _namespace(std)::fpsetprec;
using _namespace(std)::fpgetmask;
using _namespace(std)::fpsetmask;
using _namespace(std)::fpgetsticky;
#endif

#endif /* _IEEEFP_H_ */
