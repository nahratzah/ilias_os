#ifndef _ILIAS_PMAP_RMAP_H_
#define _ILIAS_PMAP_RMAP_H_

#include <ilias/arch.h>
#include <ilias/pmap/page.h>
#include <ilias/linked_set.h>
#include <tuple>
#include <memory>

namespace ilias {
namespace pmap {

template<arch> class rmap_entry;
template<arch, arch> class rmap;

template<arch Arch>
class rmap_entry
: public linked_set_element<rmap_entry<Arch>, void>
{
 public:
  using tuple_type = std::tuple<pmap<Arch>&, vpage_no<Arch>>;
  struct hash;
  struct equality;
  struct less;

  rmap_entry(pmap<Arch>&, vpage_no<Arch>&) noexcept;

  pmap<Arch>& pmap_;
  const vpage_no<Arch> addr_;
};

template<arch PhysArch, arch VirtArch>
class rmap {
 private:
  using rmap_set =
      linked_set<rmap_entry<VirtArch>, void,
                 typename rmap_entry<VirtArch>::less>;

 public:
  rmap(const rmap&) = delete;
  rmap(rmap&&) = delete;
  rmap& operator=(const rmap&) = delete;
  rmap& operator=(rmap&&) = delete;

  rmap() noexcept;
  ~rmap() noexcept;

 private:
  void pmap_register(std::unique_ptr<rmap_entry<VirtArch>>) noexcept;
  std::unique_ptr<rmap_entry<VirtArch>> pmap_deregister(pmap<VirtArch>&,
                                                        vpage_no<VirtArch>&)
      noexcept;

 public:
  bool linked() const noexcept;
  void reduce_permissions(bool, permission) noexcept;
  void unmap(bool) noexcept;

 private:
  rmap_set rmap_;
};

template<arch Arch>
struct rmap_entry<Arch>::hash {
  size_t operator()(const pmap<Arch>* p) const noexcept;
  size_t operator()(const pmap<Arch>& p) const noexcept;
  size_t operator()(const rmap_entry& e) const noexcept;
};

template<arch Arch>
struct rmap_entry<Arch>::equality {
 private:
  static auto convert_(tuple_type) noexcept -> tuple_type;
  static auto convert_(const rmap_entry&) noexcept -> tuple_type;
  static bool compare_(const tuple_type&, const tuple_type&)
      noexcept;

 public:
  template<typename T, typename U> bool operator()(const T& t, const U& u)
      const noexcept;
};

template<arch Arch>
struct rmap_entry<Arch>::less {
 private:
  static auto convert_(tuple_type) noexcept -> tuple_type;
  static auto convert_(const rmap_entry&) noexcept -> tuple_type;
  static bool compare_(const tuple_type&, const tuple_type&) noexcept;

 public:
  template<typename T, typename U> bool operator()(const T& t, const U& u)
      const noexcept;
};


}} /* namespace ilias::pmap */

#include "rmap-inl.h"

#endif /* _ILIAS_PMAP_RMAP_H_ */
