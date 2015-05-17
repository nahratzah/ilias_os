#ifndef _ILIAS_VFS_VFS_FWD_H_
#define _ILIAS_VFS_VFS_FWD_H_

#include <chrono>
#include <cstdint>
#include <ios>
#include <ilias/refcnt.h>
#include <ugid.h>

namespace ilias {
namespace vfs {

using namespace std;


struct vnode_deleter;
struct open_vnode_deleter;

enum class vnode_type : uint8_t {
  VNON,
  VREG,
  VDIR,
  VBLK,
  VCHR,
  VLNK,
  VSOCK,
  VBAD
};

#if _USE_INT128
using statfs_filecount = uint128_t;
#else
using statfs_filecount = uint64_t;
#endif

struct statfs {
  long f_type;
  streamsize f_bsize;
  streamsize f_blocks;
  streamsize f_bfree;
  streamsize f_bavail;
  statfs_filecount f_files;
  statfs_filecount f_ffree;
#if 0  // XXX notyet
  fsid_t f_fsid;
#endif
};

struct vattr {
  vnode_type va_type;
  uint16_t va_mode;
  uid_t va_uid;
  gid_t va_gid;
#if 0  // XXX notyet
  fsid_t va_fsid;
  long va_nodeid;
#endif
  uint16_t va_nlink;
  streamsize va_size;
  streamsize va_blocksize;
  std::chrono::system_clock::time_point va_atime;
  std::chrono::system_clock::time_point va_mtime;
  std::chrono::system_clock::time_point va_ctime;
#if 0  // XXX notyet
  dev_t va_rdev;
#endif
  streamsize va_blocks;
};

class uio {
  // XXX
};

class vfs;
class vnode;
class open_vnode;
class credentials;

using vfs_ptr = refpointer<vfs>;
using vnode_ptr = refpointer<vnode>;
using open_vnode_ptr = refpointer<open_vnode>;


}} /* namespace ilias::vfs */

#endif /* _ILIAS_VFS_VFS_FWD_H_ */
