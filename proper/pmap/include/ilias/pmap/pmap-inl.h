#ifndef _ILIAS_PMAP_PMAP_INL_H_
#define _ILIAS_PMAP_PMAP_INL_H_

#include <ilias/pmap/pmap.h>

namespace ilias {
namespace pmap {


constexpr permission::permission() noexcept
: read(false),
  write(false),
  exec(false),
  global(false),
  no_cache_read(false),
  no_cache_write(false)
{}

constexpr permission::permission(bool read, bool write, bool exec, bool global,
                                 bool no_cache_read, bool no_cache_write)
    noexcept
: read(read),
  write(write),
  exec(exec),
  global(global),
  no_cache_read(no_cache_read),
  no_cache_write(no_cache_write)
{}

inline auto permission::operator&=(const permission& other) noexcept ->
    permission& {
  return *this = (*this & other);
}

inline auto permission::operator|=(const permission& other) noexcept ->
    permission& {
  return *this = (*this | other);
}

constexpr auto permission::operator~() const noexcept -> permission {
  return permission(!read,
                    !write,
                    !exec,
                    !global,
                    !no_cache_read,
                    !no_cache_write);
}

constexpr auto permission::operator&(const permission& other) const noexcept ->
    permission {
  return permission(read           & other.read,
                    write          & other.write,
                    exec           & other.exec,
                    global         & other.global,
                    no_cache_read  & other.no_cache_read,
                    no_cache_write & other.no_cache_write);
}

constexpr auto permission::operator|(const permission& other) const noexcept ->
    permission {
  return permission(read           | other.read,
                    write          | other.write,
                    exec           | other.exec,
                    global         | other.global,
                    no_cache_read  | other.no_cache_read,
                    no_cache_write | other.no_cache_write);
}

constexpr auto permission::READ() noexcept -> permission {
  return permission(true, false, false, false, false, false);
}

constexpr auto permission::WRITE() noexcept -> permission {
  return permission(false, true, false, false, false, false);
}

constexpr auto permission::EXEC() noexcept -> permission {
  return permission(false, false, true, false, false, false);
}

constexpr auto permission::GLOBAL() noexcept -> permission {
  return permission(false, false, false, true, false, false);
}

constexpr auto permission::NO_CACHE_READ() noexcept -> permission {
  return permission(false, false, false, false, true, false);
}

constexpr auto permission::NO_CACHE_WRITE() noexcept -> permission {
  return permission(false, false, false, false, false, true);
}

constexpr auto permission::RO() noexcept -> permission {
  return permission::READ();
}

constexpr auto permission::RW() noexcept -> permission {
  return permission::READ() | permission::WRITE();
}

constexpr auto permission::RX() noexcept -> permission {
  return permission::READ() | permission::EXEC();
}

constexpr auto permission::RWX() noexcept -> permission {
  return permission::READ() | permission::EXEC();
}

constexpr auto permission::UNCACHED() noexcept -> permission {
  return permission::NO_CACHE_READ() | permission::NO_CACHE_WRITE();
}


constexpr auto operator==(const permission& a, const permission& b) noexcept ->
    bool {
  return a.read           == b.read          &&
         a.write          == b.write         &&
         a.exec           == b.exec          &&
         a.global         == b.global        &&
         a.no_cache_read  == b.no_cache_read &&
         a.no_cache_write == b.no_cache_write;
}

constexpr auto operator!=(const permission& a, const permission& b) noexcept ->
    bool {
  return !(a == b);
}


}} /* namespace ilias::pmap */

#endif /* _ILIAS_PMAP_PMAP_INL_H_ */
