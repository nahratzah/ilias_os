#ifndef _ILIAS_PMAP_PMAP_H_
#define _ILIAS_PMAP_PMAP_H_

#include <system_error>
#include <cstdint>
#include <ilias/arch.h>

namespace ilias {
namespace pmap {


template<arch> class pmap;  // Undefined.

class efault : public std::system_error {
 public:
  efault(uintptr_t) noexcept;
  ~efault() noexcept override;

  uintptr_t vaddr;  // Address for which the error occured.
};


}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_PMAP_H_ */
