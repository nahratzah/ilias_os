#ifndef _ILIAS_CYPTR_NODE_INL_H_
#define _ILIAS_CYPTR_NODE_INL_H_

#include <ilias/cyptr/node.h>

namespace ilias {
namespace cyptr {


inline cynode::cynode(const cynode& other) noexcept
: obj_(other.obj_)
{}

inline cynode::cynode(cynode&& other) noexcept
: obj_(std::move(other.obj_))
{}

inline cynode& cynode::operator=(const cynode& other) noexcept {
  obj_ = other.obj_;
  return *this;
}

inline cynode& cynode::operator=(cynode&& other) noexcept {
  obj_ = std::move(other.obj_);
  return *this;
}

inline cynode::~cynode() noexcept {}


}} /* namespace ilias::cyptr */

#endif /* _ILIAS_CYPTR_NODE_INL_H_ */
