#ifndef _ILIAS_PMAP_PMAP_H_
#define _ILIAS_PMAP_PMAP_H_

#include <system_error>
#include <cstdint>
#include <ilias/arch.h>
#include <ilias/pmap/page.h>
#include <ilias/pmap/pmap_page-fwd.h>

namespace ilias {
namespace pmap {


enum class reduce_permission_result {
  OK,
  UNMAPPED
};


class basic_pmap_support {
 public:
  basic_pmap_support(bool userspace) noexcept : userspace(userspace) {}
  virtual ~basic_pmap_support() noexcept {}

  /* Set if the pmap controls a userspace map. */
  const bool userspace;
};

template<arch Arch>
class pmap_support
: public basic_pmap_support
{
 public:
  pmap_support(bool userspace) noexcept : basic_pmap_support(userspace) {}
  virtual ~pmap_support() noexcept {}

  virtual vpage_no<native_arch> map_page(page_no<Arch>) = 0;
  virtual void unmap_page(vpage_no<native_arch>) noexcept = 0;
  virtual page_no<Arch> allocate_page() = 0;
  virtual void deallocate_page(page_no<Arch>) noexcept = 0;

#ifndef _LOADER
  virtual pmap_page& lookup_pmap_page(page_no<Arch>) noexcept = 0;
#endif
};

template<arch> class pmap;  // Undefined.
template<typename> class pmap_map;  // Undefined.

template<> class pmap<arch::i386>;
template<> class pmap<arch::amd64>;

class efault : public std::system_error {
 public:
  efault(uintptr_t) noexcept;
  ~efault() noexcept override;

  uintptr_t vaddr;  // Address for which the error occured.
};

struct permission {
  constexpr permission() noexcept;
  constexpr permission(bool, bool, bool, bool, bool, bool) noexcept;
  constexpr permission(const permission&) noexcept = default;
  permission& operator=(const permission&) noexcept = default;

  permission& operator&=(const permission&) noexcept;
  permission& operator|=(const permission&) noexcept;
  constexpr permission operator~() const noexcept;
  constexpr permission operator&(const permission&) const noexcept;
  constexpr permission operator|(const permission&) const noexcept;

  static constexpr permission READ() noexcept;
  static constexpr permission WRITE() noexcept;
  static constexpr permission EXEC() noexcept;
  static constexpr permission GLOBAL() noexcept;
  static constexpr permission NO_CACHE_READ() noexcept;
  static constexpr permission NO_CACHE_WRITE() noexcept;

  static constexpr permission RO() noexcept;
  static constexpr permission RW() noexcept;
  static constexpr permission RX() noexcept;
  static constexpr permission RWX() noexcept;
  static constexpr permission UNCACHED() noexcept;

  bool read : 1;
  bool write : 1;
  bool exec : 1;
  bool global : 1;  // Hint only, pmap may ignore this.
  bool no_cache_read : 1;
  bool no_cache_write : 1;
};

constexpr bool operator==(const permission&, const permission&) noexcept;
constexpr bool operator!=(const permission&, const permission&) noexcept;


}} /* namespace ilias::pmap */

#include "pmap-inl.h"

#endif /* _ILIAS_PMAP_PMAP_H_ */
