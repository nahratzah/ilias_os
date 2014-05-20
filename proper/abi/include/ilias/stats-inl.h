#ifndef _ILIAS_STATS_INL_H_
#define _ILIAS_STATS_INL_H_

#include <cdecl.h>
#include <ilias/stats.h>

_namespace_begin(ilias)


inline basic_stats::basic_stats(stats_group* parent, stats_type type,
                                _namespace(std)::string_ref name) noexcept
: type(type),
  parent_(parent),
  name_(name)
{}

inline auto basic_stats::parent() const noexcept -> const stats_group* {
  return parent_;
}

inline auto basic_stats::name() const noexcept -> _namespace(std)::string_ref {
  return name_;
}


inline stats_leaf::stats_leaf(stats_group& parent,
                              _namespace(std)::string_ref name) noexcept
: basic_stats(&parent, stats_type::leaf, name)
{}


inline stats_counter::stats_counter(stats_group& parent,
                                    _namespace(std)::string_ref name) noexcept
: stats_leaf(parent, name)
{
  init();
}

inline auto stats_counter::add(uint64_t n) noexcept -> void {
  counter_.fetch_add(n, _namespace(std)::memory_order_relaxed);
}

inline auto stats_counter::sub(uint64_t n) noexcept -> void {
  counter_.fetch_sub(n, _namespace(std)::memory_order_relaxed);
}


_namespace_end(ilias)

#endif /* _ILIAS_STATS_INL_H_ */
