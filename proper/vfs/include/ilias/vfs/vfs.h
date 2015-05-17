#ifndef _ILIAS_VFS_VFS_H_
#define _ILIAS_VFS_VFS_H_

#include <ilias/vfs/vfs-fwd.h>
#include <ilias/future.h>
#include <ilias/refcnt.h>
#include <ilias/monitor.h>

namespace ilias {
namespace vfs {


class vfs
: public refcount_base<vfs>
{
  friend vnode_deleter;

 public:
  vfs(const vfs&) noexcept = delete;
  vfs(vfs&&) noexcept = delete;
  vfs& operator=(const vfs&) noexcept = delete;
  vfs& operator=(vfs&&) noexcept = delete;

 protected:
  vfs() noexcept = default;

 public:
  virtual ~vfs() noexcept;

  virtual cb_future<vnode_ptr> root() = 0;
  virtual cb_future<statfs> statfs() = 0;
  virtual cb_future<void> sync() = 0;

 private:
  virtual void vn_inactive(vnode*) noexcept = 0;

  vnode_ptr covered_;
};


}} /* namespace ilias::vfs */

#endif /* _ILIAS_VFS_VFS_H_ */
