#include <ilias/stats.h>
#include <abi/linker.h>
#include <cassert>
#include <cstring>
#include <algorithm>

_namespace_begin(ilias)


basic_stats::~basic_stats() noexcept {}


stats_group::stats_group(_namespace(std)::string_ref name) noexcept
: basic_stats(nullptr, stats_type::group, name)
{}

stats_group::stats_group(stats_group& parent,
                         _namespace(std)::string_ref name) noexcept
: basic_stats(&parent, stats_type::group, name)
{
  parent.register_child(*this);
}

stats_group::~stats_group() noexcept {
  auto parent = this->parent();
  if (parent) parent->deregister_child(*this);
}

auto stats_group::register_child(basic_stats& child) const noexcept -> void {
  // XXX lock
  children_.link_back(&child);
}

auto stats_group::deregister_child(basic_stats& child) const noexcept -> void {
  // XXX lock
  children_.unlink(&child);
}


stats_leaf::~stats_leaf() noexcept {}

auto stats_leaf::path() const ->
    _namespace(std)::vector<_namespace(std)::string_ref> {
  _namespace(std)::vector<_namespace(std)::string_ref> rv;

  for (const basic_stats* i = this; i != nullptr; i = i->parent())
    rv.push_back(i->name());
  _namespace(std)::reverse(rv.begin(), rv.end());
  return rv;
}

auto stats_leaf::init() noexcept -> void {
  auto* p = parent();
  assert(p);
  p->register_child(*this);
}

auto stats_leaf::deinit() noexcept -> void {
  auto* p = parent();
  assert(p);
  p->deregister_child(*this);
}


stats_counter::~stats_counter() noexcept {
  deinit();
}


global_stats_group::operator stats_group&() noexcept {
  void* p = &data_;
  static_assert(sizeof(data_) == sizeof(stats_group), "Storage error.");
  call_once(init_,
            [](void* p, global_stats_group* parent, const char* name) {
              if (parent)
                new (p) stats_group(*parent, name);
              else
                new (p) stats_group(name);

              /* Register destructor for when module goes out of scope. */
              abi::__cxa_atexit([](void* p) {
                                   stats_group* p_ =
                                       static_cast<stats_group*>(p);
                                   p_->~stats_group();
                                 },
                                 p, &abi::__dso_handle);
            },
            p, parent_, name_);
  return *static_cast<stats_group*>(p);
}


_namespace_end(ilias)
