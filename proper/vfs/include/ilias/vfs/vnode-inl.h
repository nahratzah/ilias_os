#ifndef _ILIAS_VFS_VNODE_INL_H_
#define _ILIAS_VFS_VNODE_INL_H_

#include "vnode.h"

namespace ilias {
namespace vfs {


inline vnode::vnode(refpointer<vfs> vfs, vnode_type type) noexcept
: vfs_(vfs),
  type(type)
{}

inline auto vnode::get_vfs() const noexcept -> vfs_ptr {
  return vfs_;
}

inline auto vnode::get_vfsmountedhere() const noexcept -> vfs_ptr {
  return vfsmountedhere_;
}


inline open_vnode::open_vnode(vnode_ptr owner) noexcept
: owner_(owner)
{}


}} /* namespace ilias::vfs */

#endif /* _ILIAS_VFS_VNODE_INL_H_ */
