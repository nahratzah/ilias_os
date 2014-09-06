#ifndef _ILIAS_STATS_H_
#define _ILIAS_STATS_H_

#include <ilias/stats-fwd.h>
#include <cdecl.h>
#include <atomic>
#include <cstddef>
#include <string>
#include <mutex>
#include <vector>
#include <ilias/linked_list.h>

_namespace_begin(ilias)


enum class stats_type {
  leaf,
  group
};

class stats_group;

class basic_stats
: public linked_list_element<>
{
 public:
  basic_stats() = delete;
  basic_stats(const basic_stats&) = delete;
  basic_stats& operator=(const basic_stats&) = delete;

  basic_stats(stats_group*, stats_type, _namespace(std)::string_ref) noexcept;
  virtual ~basic_stats() noexcept;

  _namespace(std)::string_ref name() const noexcept;
  const stats_group* parent() const noexcept;

  const stats_type type;

 private:
  stats_group*const parent_;
  const _namespace(std)::string_ref name_;
};

/*
 * Statistics group.
 *
 * Acts as a parent to related statistics.
 *
 * In global storage, don't use the statistics group,
 * instead use the global_stats_group, which is safe wrt
 * reordering of global constructors.
 */
class stats_group
: public basic_stats
{
 public:
  stats_group(_namespace(std)::string_ref) noexcept;
  stats_group(stats_group&, _namespace(std)::string_ref) noexcept;
  ~stats_group() noexcept override;

  void register_child(basic_stats&) const noexcept;
  void deregister_child(basic_stats&) const noexcept;

 private:
  mutable linked_list<basic_stats> children_;
  mutable _namespace(std)::mutex m_;
};

/*
 * Globally declared stats group.
 *
 * Usage:
 * static global_stats_group my_group{ &my_group_parent, "name", {}, {} };
 *
 * This class uses lazy construction to prevent global constructor re-ordering
 * from messing up the initialization order of stats.
 */
struct global_stats_group final {
  operator stats_group&() noexcept;

  global_stats_group*const parent_;
  const char*const name_;

  _namespace(std)::aligned_storage_t<sizeof(stats_group), alignof(stats_group)>
      data_;
  _namespace(std)::once_flag init_;
};

class stats_leaf
: public basic_stats
{
 public:
  stats_leaf(stats_group&, _namespace(std)::string_ref) noexcept;
  ~stats_leaf() noexcept override;

  _namespace(std)::vector<_namespace(std)::string_ref> path() const;

 protected:
  void init() noexcept;
  void deinit() noexcept;
};

/*
 * Unsigned 64-bit counter.
 */
class stats_counter final
: public stats_leaf
{
 public:
  stats_counter(stats_group&, _namespace(std)::string_ref) noexcept;
  ~stats_counter() noexcept override;

  void add(uint64_t n = 1) noexcept;
  void sub(uint64_t n = 1) noexcept;

 private:
  _namespace(std)::atomic<uint64_t> counter_;
};


_namespace_end(ilias)

#include <ilias/stats-inl.h>

#endif /* _ILIAS_STATS_H_ */
