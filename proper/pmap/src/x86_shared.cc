#include <ilias/pmap/x86_shared.h>

namespace ilias {
namespace pmap {
namespace x86_shared {


constexpr unsigned int flags::AVL_COUNT;

constexpr flags pml4_record::FLAGS_MASK;
constexpr unsigned int pml4_record::PAGE_SHIFT;
constexpr uint64_t pml4_record::PAGE_MASK;
constexpr uint64_t pml4_record::PT_P;

constexpr flags pdpe_record<arch::i386>::FLAGS_MASK;
constexpr unsigned int pdpe_record<arch::i386>::PAGE_SHIFT;
constexpr uint64_t pdpe_record<arch::i386>::PAGE_MASK;
constexpr uint64_t pdpe_record<arch::i386>::PT_P;

constexpr flags pdpe_record<arch::amd64>::FLAGS_MASK_NPS;
constexpr flags pdpe_record<arch::amd64>::FLAGS_MASK_PS;
constexpr unsigned int pdpe_record<arch::amd64>::PAGE_SHIFT;
constexpr uint64_t pdpe_record<arch::amd64>::PAGE_MASK_NPS;
constexpr uint64_t pdpe_record<arch::amd64>::PAGE_MASK_PS;
constexpr uint64_t pdpe_record<arch::amd64>::PT_P;
constexpr uint64_t pdpe_record<arch::amd64>::PT_PS;
constexpr uint64_t pdpe_record<arch::amd64>::PT_PAT_;

constexpr flags pdp_record::FLAGS_MASK_NPS;
constexpr flags pdp_record::FLAGS_MASK_PS;
constexpr unsigned int pdp_record::PAGE_SHIFT;
constexpr uint64_t pdp_record::PAGE_MASK_NPS;
constexpr uint64_t pdp_record::PAGE_MASK_PS;
constexpr uint64_t pdp_record::PT_P;
constexpr uint64_t pdp_record::PT_PS;
constexpr uint64_t pdp_record::PT_PAT_;

constexpr flags pte_record::FLAGS_MASK;
constexpr unsigned int pte_record::PAGE_SHIFT;
constexpr uint64_t pte_record::PAGE_MASK;
constexpr uint64_t pte_record::PT_P;


}}} /* namespace ilias::pmap */
