#ifndef _ABI_ERRNO_H_
#define _ABI_ERRNO_H_

#include <cdecl.h>

_cdecl_begin

extern thread_local int errno;

_cdecl_end

#define _E2BIG			 1
#define _EACCES			 2
#define _EADDRINUSE		 3
#define _EADDRNOTAVAIL		 4
#define _EAFNOSUPPORT		 5
#define _EAGAIN			 6
#define _EALREADY		 7
#define _EBADF			 8
#define _EBADMSG		 9
#define _EBUSY			10
#define _ECANCELED		11
#define _ECHILD			12
#define _ECONNABORTED		13
#define _ECONNREFUSED		14
#define _ECONNRESET		15
#define _EDEADLK		16
#define _EDESTADDRREQ		17
#define _EDOM			18
#define _EDQUOT			19
#define _EEXIST			20
#define _EFAULT			21
#define _EFBIG			22
#define _EHOSTUNREACH		23
#define _EIDRM			24
#define _EILSEQ			25
#define _EINPROGRESS		26
#define _EINTR			27
#define _EINVAL			28
#define _EIO			29
#define _EISCONN		30
#define _EISDIR			31
#define _ELOOP			32
#define _EMFILE			33
#define _EMLINK			34
#define _EMSGSIZE		35
#define _EMULTIHOP		36
#define _ENAMETOOLONG		37
#define _ENETDOWN		38
#define _ENETRESET		39
#define _ENETUNREACH		40
#define _ENFILE			41
#define _ENOBUFS		42
#define _ENODATA		43
#define _ENODEV			44
#define _ENOENT			45
#define _ENOEXEC		46
#define _ENOLCK			47
#define _ENOLINK		48
#define _ENOMEM			49
#define _ENOMSG			50
#define _ENOPROTOOPT		51
#define _ENOSPC			52
#define _ENOSR			53
#define _ENOSTR			54
#define _ENOSYS			55
#define _ENOTCONN		56
#define _ENOTDIR		57
#define _ENOTEMPTY		58
#define _ENOTRECOVERABLE	59
#define _ENOTSOCK		60
#define _ENOTSUP		61
#define _ENOTTY			62
#define _ENXIO			63
#define _EOPNOTSUPP		_ENOTSUP
#define _EOVERFLOW		65
#define _EOWNERDEAD		66
#define _EPERM			67
#define _EPIPE			68
#define _EPROTO			69
#define _EPROTONOSUPPORT	70
#define _EPROTOTYPE		71
#define _ERANGE			72
#define _EROFS			73
#define _ESPIPE			74
#define _ESRCH			75
#define _ESTALE			76
#define _ETIME			77
#define _ETIMEDOUT		78
#define _ETXTBSY		79
#define _EWOULDBLOCK		80
#define _EXDEV			81

#endif /* _ABI_ERRNO_H_ */