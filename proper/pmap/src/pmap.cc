#include <ilias/pmap/pmap.h>

namespace ilias {
namespace pmap {


efault::efault(uintptr_t vaddr) noexcept
: std::system_error(std::make_error_code(std::errc::bad_address)),
  vaddr(vaddr)
{}

efault::~efault() noexcept {}


/* Verify permission logic: permission::READ. */
static_assert(permission::READ().read == true,
              "READ permission requires 'read' bit set.");
static_assert(permission::READ().write == false,
              "READ permission requires 'write' bit cleared.");
static_assert(permission::READ().exec == false,
              "READ permission requires 'exec' bit cleared.");
static_assert(permission::READ().global == false,
              "READ permission requires 'global' bit cleared.");
static_assert(permission::READ().no_cache_read == false,
              "READ permission requires 'no_cache_read' bit cleared.");
static_assert(permission::READ().no_cache_write == false,
              "READ permission requires 'no_cache_write' bit cleared.");

/* Verify permission logic: permission::WRITE. */
static_assert(permission::WRITE().read == false,
              "WRITE permission requires 'read' bit cleared.");
static_assert(permission::WRITE().write == true,
              "WRITE permission requires 'write' bit set.");
static_assert(permission::WRITE().exec == false,
              "WRITE permission requires 'exec' bit cleared.");
static_assert(permission::WRITE().global == false,
              "WRITE permission requires 'global' bit cleared.");
static_assert(permission::WRITE().no_cache_read == false,
              "WRITE permission requires 'no_cache_read' bit cleared.");
static_assert(permission::WRITE().no_cache_write == false,
              "WRITE permission requires 'no_cache_write' bit cleared.");

/* Verify permission logic: permission::EXEC. */
static_assert(permission::EXEC().read == false,
              "EXEC permission requires 'read' bit cleared.");
static_assert(permission::EXEC().write == false,
              "EXEC permission requires 'write' bit cleared.");
static_assert(permission::EXEC().exec == true,
              "EXEC permission requires 'exec' bit set.");
static_assert(permission::EXEC().global == false,
              "EXEC permission requires 'global' bit cleared.");
static_assert(permission::EXEC().no_cache_read == false,
              "EXEC permission requires 'no_cache_read' bit cleared.");
static_assert(permission::EXEC().no_cache_write == false,
              "EXEC permission requires 'no_cache_write' bit cleared.");

/* Verify permission logic: permission::GLOBAL. */
static_assert(permission::GLOBAL().read == false,
              "GLOBAL permission requires 'read' bit cleared.");
static_assert(permission::GLOBAL().write == false,
              "GLOBAL permission requires 'write' bit cleared.");
static_assert(permission::GLOBAL().exec == false,
              "GLOBAL permission requires 'exec' bit cleared.");
static_assert(permission::GLOBAL().global == true,
              "GLOBAL permission requires 'global' bit set.");
static_assert(permission::GLOBAL().no_cache_read == false,
              "GLOBAL permission requires 'no_cache_read' bit cleared.");
static_assert(permission::GLOBAL().no_cache_write == false,
              "GLOBAL permission requires 'no_cache_write' bit cleared.");

/* Verify permission logic: permission::NO_CACHE_READ. */
static_assert(permission::NO_CACHE_READ().read == false,
              "NO_CACHE_READ permission requires 'read' bit cleared.");
static_assert(permission::NO_CACHE_READ().write == false,
              "NO_CACHE_READ permission requires 'write' bit cleared.");
static_assert(permission::NO_CACHE_READ().exec == false,
              "NO_CACHE_READ permission requires 'exec' bit cleared.");
static_assert(permission::NO_CACHE_READ().global == false,
              "NO_CACHE_READ permission requires 'global' bit cleared.");
static_assert(permission::NO_CACHE_READ().no_cache_read == true,
              "NO_CACHE_READ permission requires 'no_cache_read' bit set.");
static_assert(permission::NO_CACHE_READ().no_cache_write == false,
              "NO_CACHE_READ permission requires 'no_cache_write' bit "
              "cleared.");

/* Verify permission logic: permission::NO_CACHE_WRITE. */
static_assert(permission::NO_CACHE_WRITE().read == false,
              "NO_CACHE_WRITE permission requires 'read' bit cleared.");
static_assert(permission::NO_CACHE_WRITE().write == false,
              "NO_CACHE_WRITE permission requires 'write' bit cleared.");
static_assert(permission::NO_CACHE_WRITE().exec == false,
              "NO_CACHE_WRITE permission requires 'exec' bit cleared.");
static_assert(permission::NO_CACHE_WRITE().global == false,
              "NO_CACHE_WRITE permission requires 'global' bit cleared.");
static_assert(permission::NO_CACHE_WRITE().no_cache_read == false,
              "NO_CACHE_WRITE permission requires 'no_cache_read' bit "
              "cleared.");
static_assert(permission::NO_CACHE_WRITE().no_cache_write == true,
              "NO_CACHE_WRITE permission requires 'no_cache_write' bit set.");

/* Verify permission logic: permission::RO. */
static_assert(permission::RO().read == true,
              "RO permission requires 'read' bit set.");
static_assert(permission::RO().write == false,
              "RO permission requires 'write' bit cleared.");
static_assert(permission::RO().exec == false,
              "RO permission requires 'exec' bit cleared.");
static_assert(permission::RO().global == false,
              "RO permission requires 'global' bit cleared.");
static_assert(permission::RO().no_cache_read == false,
              "RO permission requires 'no_cache_read' bit cleared.");
static_assert(permission::RO().no_cache_write == false,
              "RO permission requires 'no_cache_write' bit cleared.");

/* Verify permission logic: permission::RW. */
static_assert(permission::RW().read == true,
              "RW permission requires 'read' bit set.");
static_assert(permission::RW().write == true,
              "RW permission requires 'write' bit set.");
static_assert(permission::RW().exec == false,
              "RW permission requires 'exec' bit cleared.");
static_assert(permission::RW().global == false,
              "RW permission requires 'global' bit cleared.");
static_assert(permission::RW().no_cache_read == false,
              "RW permission requires 'no_cache_read' bit cleared.");
static_assert(permission::RW().no_cache_write == false,
              "RW permission requires 'no_cache_write' bit cleared.");

/* Verify permission logic: permission::RX. */
static_assert(permission::RX().read == true,
              "RX permission requires 'read' bit set.");
static_assert(permission::RX().write == false,
              "RX permission requires 'write' bit cleared.");
static_assert(permission::RX().exec == true,
              "RX permission requires 'exec' bit set.");
static_assert(permission::RX().global == false,
              "RX permission requires 'global' bit cleared.");
static_assert(permission::RX().no_cache_read == false,
              "RX permission requires 'no_cache_read' bit cleared.");
static_assert(permission::RX().no_cache_write == false,
              "RX permission requires 'no_cache_write' bit cleared.");

/* Verify permission logic: permission::RWX. */
static_assert(permission::RWX().read == true,
              "RWX permission requires 'read' bit set.");
static_assert(permission::RWX().write == true,
              "RWX permission requires 'write' bit set.");
static_assert(permission::RWX().exec == true,
              "RWX permission requires 'exec' bit set.");
static_assert(permission::RWX().global == false,
              "RWX permission requires 'global' bit cleared.");
static_assert(permission::RWX().no_cache_read == false,
              "RWX permission requires 'no_cache_read' bit cleared.");
static_assert(permission::RWX().no_cache_write == false,
              "RWX permission requires 'no_cache_write' bit cleared.");

/* Verify permission logic: permission::UNCACHED. */
static_assert(permission::UNCACHED().read == false,
              "UNCACHED permission requires 'read' bit cleared.");
static_assert(permission::UNCACHED().write == false,
              "UNCACHED permission requires 'write' bit cleared.");
static_assert(permission::UNCACHED().exec == false,
              "UNCACHED permission requires 'exec' bit cleared.");
static_assert(permission::UNCACHED().global == false,
              "UNCACHED permission requires 'global' bit cleared.");
static_assert(permission::UNCACHED().no_cache_read == true,
              "UNCACHED permission requires 'no_cache_read' bit set.");
static_assert(permission::UNCACHED().no_cache_write == true,
              "UNCACHED permission requires 'no_cache_write' bit set.");


}} /* namespace ilias::pmap */
