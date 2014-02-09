#ifndef _ABI_ERRNO_H_
#define _ABI_ERRNO_H_

#include <cdecl.h>
#include <abi/abi.h>

_namespace_begin(__cxxabiv1)
_cdecl_begin

#ifdef __cplusplus
extern thread_local int errno;
#else
extern _Thread_local int errno;
#endif

_cdecl_end
_namespace_end(__cxxabiv1)

#define _ABI_E2BIG			 1
#define _ABI_EACCES			 2
#define _ABI_EADDRINUSE			 3
#define _ABI_EADDRNOTAVAIL		 4
#define _ABI_EAFNOSUPPORT		 5
#define _ABI_EAGAIN			 6
#define _ABI_EALREADY			 7
#define _ABI_EBADF			 8
#define _ABI_EBADMSG			 9
#define _ABI_EBUSY			10
#define _ABI_ECANCELED			11
#define _ABI_ECHILD			12
#define _ABI_ECONNABORTED		13
#define _ABI_ECONNREFUSED		14
#define _ABI_ECONNRESET			15
#define _ABI_EDEADLK			16
#define _ABI_EDESTADDRREQ		17
#define _ABI_EDOM			18
#define _ABI_EDQUOT			19
#define _ABI_EEXIST			20
#define _ABI_EFAULT			21
#define _ABI_EFBIG			22
#define _ABI_EHOSTUNREACH		23
#define _ABI_EIDRM			24
#define _ABI_EILSEQ			25
#define _ABI_EINPROGRESS		26
#define _ABI_EINTR			27
#define _ABI_EINVAL			28
#define _ABI_EIO			29
#define _ABI_EISCONN			30
#define _ABI_EISDIR			31
#define _ABI_ELOOP			32
#define _ABI_EMFILE			33
#define _ABI_EMLINK			34
#define _ABI_EMSGSIZE			35
#define _ABI_EMULTIHOP			36
#define _ABI_ENAMETOOLONG		37
#define _ABI_ENETDOWN			38
#define _ABI_ENETRESET			39
#define _ABI_ENETUNREACH		40
#define _ABI_ENFILE			41
#define _ABI_ENOBUFS			42
#define _ABI_ENODATA			43
#define _ABI_ENODEV			44
#define _ABI_ENOENT			45
#define _ABI_ENOEXEC			46
#define _ABI_ENOLCK			47
#define _ABI_ENOLINK			48
#define _ABI_ENOMEM			49
#define _ABI_ENOMSG			50
#define _ABI_ENOPROTOOPT		51
#define _ABI_ENOSPC			52
#define _ABI_ENOSR			53
#define _ABI_ENOSTR			54
#define _ABI_ENOSYS			55
#define _ABI_ENOTCONN			56
#define _ABI_ENOTDIR			57
#define _ABI_ENOTEMPTY			58
#define _ABI_ENOTRECOVERABLE		59
#define _ABI_ENOTSOCK			60
#define _ABI_ENOTSUP			61
#define _ABI_ENOTTY			62
#define _ABI_ENXIO			63
#define _ABI_EOPNOTSUPP			_ABI_ENOTSUP  // Common alias.
#define _ABI_EOVERFLOW			65
#define _ABI_EOWNERDEAD			66
#define _ABI_EPERM			67
#define _ABI_EPIPE			68
#define _ABI_EPROTO			69
#define _ABI_EPROTONOSUPPORT		70
#define _ABI_EPROTOTYPE			71
#define _ABI_ERANGE			72
#define _ABI_EROFS			73
#define _ABI_ESPIPE			74
#define _ABI_ESRCH			75
#define _ABI_ESTALE			76
#define _ABI_ETIME			77
#define _ABI_ETIMEDOUT			78
#define _ABI_ETXTBSY			79
#define _ABI_EWOULDBLOCK		80
#define _ABI_EXDEV			81

#endif /* _ABI_ERRNO_H_ */
