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

#include <ilias/pmap/pmap-inl.h>

#endif /* _ILIAS_PMAP_PMAP_H_ */
