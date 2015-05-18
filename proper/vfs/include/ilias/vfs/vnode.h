#ifndef _ILIAS_VFS_VNODE_H_
#define _ILIAS_VFS_VNODE_H_

#include <ilias/vfs/vfs-fwd.h>
#include <ilias/vfs/credentials.h>
#include <ilias/refcnt.h>
#include <ilias/monitor.h>
#include <cstdint>

namespace ilias {
namespace vfs {


struct vnode_deleter {
  void operator()(const vnode*) noexcept;
};

struct open_vnode_deleter {
  void operator()(const open_vnode*) noexcept;
};

class vnode
: public refcount_base<vnode, vnode_deleter>
{
  friend vnode_deleter;
  friend open_vnode_deleter;

 public:
  vnode() = delete;
  vnode(const vnode&) = delete;
  vnode(vnode&&) = delete;
  vnode& operator=(const vnode&) = delete;
  vnode& operator=(vnode&&) = delete;

  using flags = int;  // XXX temporarily

 protected:
  vnode(vfs_ptr, vnode_type) noexcept;
  ~vnode() noexcept = default;

 public:
  const vfs_ptr& get_vfs() const noexcept;
  const vfs_ptr& get_vfsmountedhere() const noexcept;
  virtual open_vnode_ptr vnopen(flags, credentials) = 0;
#if 0  // XXX notyet
  virtual cb_future<void> ioctl(ioctl_command, data, credentials) = 0;
  virtual cb_future<void> select(readwrite_dir, credentials) = 0;
#endif
  virtual cb_future<vattr> getattr(credentials) = 0;
  virtual cb_future<void> setattr(vattr, credentials) = 0;
  virtual cb_future<void> access(uint16_t, credentials) = 0;
  virtual cb_future<vnode_ptr> lookup(wstring_ref, credentials) = 0;
  virtual cb_future<open_vnode_ptr> create(wstring_ref, bool,
                                           ios_base::openmode,
                                           credentials) = 0;
  virtual cb_future<void> remove(wstring_ref, credentials) = 0;
  virtual cb_future<void> link(vnode_ptr, wstring_ref, credentials) = 0;
  virtual cb_future<void> rename(wstring_ref, vnode_ptr, wstring_ref,
                                 credentials) = 0;
  virtual cb_future<vnode_ptr> mkdir(wstring_ref, vattr, credentials) = 0;
  virtual cb_future<void> rmdir(wstring_ref, credentials) = 0;
  virtual cb_future<uio> readdir(uio, credentials) = 0;
  virtual cb_future<void> symlink(wstring_ref, vattr, credentials) = 0;
  virtual cb_future<uio> readlink(uio, credentials) = 0;
#if 0  // XXX notyet
  cb_future<optional<tuple<vnode_ptr, off_t>>> bmap(off_t) = 0;
  cb_future<refpointer<vnode_buffer>> bread(off_t) = 0;
#endif

 private:
  virtual void close(open_vnode*) noexcept = 0;

 private:
  const vfs_ptr vfs_;

 public:
  const vnode_type type;

 private:
  vfs_ptr vfsmountedhere_;
};

class open_vnode
: public refcount_base<open_vnode, open_vnode_deleter>
{
  friend open_vnode_deleter;

 public:
  open_vnode() = delete;
  open_vnode(const open_vnode&) = delete;
  open_vnode(open_vnode&&) = delete;
  open_vnode& operator=(const open_vnode&) = delete;
  open_vnode& operator=(open_vnode&&) = delete;

 protected:
  open_vnode(vnode_ptr) noexcept;
  ~open_vnode() noexcept = default;

 public:
  const vnode_ptr& get_vnode() const noexcept;
  virtual cb_future<uio> read(uio, credentials) = 0;
  virtual cb_future<uio> write(uio, credentials) = 0;
  virtual cb_future<void> fsync(credentials) = 0;

 private:
  const vnode_ptr owner_;
};


}} /* namespace ilias::vfs */

#include "vnode-inl.h"

#endif /* _ILIAS_VFS_VNODE_H_ */
