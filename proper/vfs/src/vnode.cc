#include <ilias/vfs/vnode.h>
#include <ilias/vfs/vfs.h>

namespace ilias {
namespace vfs {


auto vnode_deleter::operator()(const vnode* vn) noexcept -> void {
  vfs_ptr vfs = vn->get_vfs();
  vfs->vn_inactive(const_cast<vnode*>(vn));
}

auto open_vnode_deleter::operator()(const open_vnode* ovn) noexcept -> void {
  vnode_ptr vnode = ovn->get_vnode();
  vnode->close(const_cast<open_vnode*>(ovn));
}


}} /* namespace ilias::vfs */
