#include <ilias/pmap/x86_shared.h>

namespace ilias {
namespace pmap {
namespace x86_shared {

constexpr flags pdpe_record::FLAGS_MASK;
constexpr flags pdp_record::FLAGS_MASK_NPS;
constexpr flags pdp_record::FLAGS_MASK_PS;
constexpr flags pte_record::FLAGS_MASK;

}}} /* namespace ilias::pmap */
