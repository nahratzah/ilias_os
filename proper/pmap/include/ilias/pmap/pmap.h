#ifndef _ILIAS_PMAP_PMAP_H_
#define _ILIAS_PMAP_PMAP_H_

#include <system_error>
#include <cstdint>
#include <ilias/arch.h>
#include <ilias/pmap/page.h>

namespace ilias {
namespace pmap {


template<arch Arch> class pmap_support {
 public:
  virtual ~pmap_support() noexcept {}

  virtual vpage_no<Arch> map_page(page_no<Arch>) = 0;
  virtual void unmap_page(vpage_no<Arch>) noexcept = 0;
  virtual page_no<Arch> allocate_page() = 0;
  virtual void deallocate_page(page_no<Arch>) noexcept = 0;
};

template<arch> class pmap;  // Undefined.

class efault : public std::system_error {
 public:
  efault(uintptr_t) noexcept;
  ~efault() noexcept override;

  uintptr_t vaddr;  // Address for which the error occured.
};


}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_PMAP_H_ */
