#include <ilias/i386/sse.h>

namespace ilias {
namespace i386 {


void enable_sse() noexcept {
  asm volatile(
        "mov %%cr0, %%eax\n"
      "\tand $0xfb, %%al\n"  // Clear CR0.EM
      "\tor $0x2, %%al\n"  // Set CR0.MP
      "\tmov %%eax, %%cr0\n"
      "\tmov %%cr4, %%eax\n"
      "\tor $0x600, %%ax\n"  // Set CR4.OSFXSR, CR4.OSXMMEXCP
      "\tmov %%eax, %%cr4\n"
    :
    :
    : "eax");
}


}} /* namespace ilias::i386 */
