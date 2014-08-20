#include <ilias/linked_unordered_set.h>
#include <algorithm>

_namespace_begin(ilias)
namespace impl {


uint64_t ceil_uls_(long double f, uint64_t max) noexcept {
  if (f >= static_cast<long double>(max)) return max;

  uint64_t truncated = static_cast<uint64_t>(f);
  if (static_cast<long double>(truncated) < f) ++truncated;
  return _namespace(std)::min(truncated, max);
}


} /* namespace ilias::impl */
_namespace_end(ilias)
