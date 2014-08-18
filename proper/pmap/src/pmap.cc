#include <ilias/pmap/pmap.h>

namespace ilias {
namespace pmap {


efault::efault(uintptr_t vaddr) noexcept
: std::system_error(std::make_error_code(std::errc::bad_address)),
  vaddr(vaddr)
{}

efault::~efault() noexcept {}


}} /* namespace ilias::pmap */
