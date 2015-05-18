#ifndef _ILIAS_VFS_CREDENTIALS_H_
#define _ILIAS_VFS_CREDENTIALS_H_

#include <ilias/vfs/vfs-fwd.h>
#include <ugid.h>

namespace ilias {
namespace vfs {


struct credentials {
  uid_t uid;
  gid_t gid;
};


}} /* namespace ilias::vfs */

#endif /* _ILIAS_VFS_CREDENTIALS_H_ */
