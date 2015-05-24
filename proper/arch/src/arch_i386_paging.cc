#include <ilias/i386/paging.h>
#include <ilias/i386/gdt.h>
#include <ilias/cpuid.h>
#include <ilias/pmap/pmap_i386.h>

namespace ilias {
namespace i386 {


void enable_paging(const gdt_t& gdt, pmap::pmap<native_arch>& p) {
  uintptr_t pmap_ptr = reinterpret_cast<uintptr_t>(p.get_pmap_ptr());
  const void* gdt_ptr = gdt.get_gdt_ptr();

  assert((pmap_ptr % (1U << 5)) == 0);
  constexpr uint32_t cr4_pae_flag = 1U << 5;
  constexpr uint32_t cr4_pse_flag = 1U << 4;  // Actually ignored in PAE.
  constexpr uint32_t cr4_pge_flag = 1U << 7;  // Page-global enable.
  constexpr uint32_t cr4_flags = cr4_pae_flag | cr4_pse_flag | cr4_pge_flag;

  /* Enable NX bit, if supported. */
  if (cpuid_feature_present(cpuid_extfeature_const::nx)) {
    asm volatile(
          "mov $0xc0000080, %%ecx\n"  // specify which MSR we want to read (EFER)
        "\trdmsr\n"  // edx:eax == 64-bit msr register.
        "\tor $0x8, %%ah\n"  // msr |= bit[11] (the NX-enable bit)
        "\twrmsr\n"  // write modified value back
    :
    :
    : "eax", "ecx", "edx");
  }

  asm volatile(
        "cli\n"  // Disable interrupts.
      "\tlgdt (%%eax)\n"  // Load our gdt.
      "\tjmp 0f\n"  // Clear prefetch cache.
      "\tnop\n"
      "0:\n"

      /* Set Protection Enable (0x1) bit in CR0. */
      "\tmovl %%cr0, %%eax\n"
      "\tor $1, %%al\n"
      "\tmovl %%eax, %%cr0\n"

      /* Update segment descriptors. */
      "\tmov %4, %%eax\n"
      "\tmov %%eax, %%ds\n"
      "\tmov %%eax, %%es\n"
      "\tmov %%eax, %%ss\n"
      "\tmov %%eax, %%fs\n"

      /*
       * Push jump to label 0 on the stack, in order to use lret instruction
       * to fix up the code segment.
       */
      "\tjmpl %3, $1f\n"
      "\tnop\n"
      "1:\n"

      /* Set up paging (cr4), load pmap (cr3). */
      "\tmovl %%cr4, %%eax\n"
      "\tor %2, %%eax\n"
      "\tmovl %%eax, %%cr4\n"
      "\tmovl %0, %%cr3\n"

      /* Enable paging. */
      "\tmovl %%cr0, %%eax\n"
      "\torl $0x80000000, %%eax\n"
      "\tmovl %%eax, %%cr0\n"
  :
  :   "r"(pmap_ptr), "a"(gdt_ptr), "r"(cr4_flags),
      "i"(uint16_t(gdt_idx::kernel_code) << 3),
      "i"(uint16_t(gdt_idx::kernel_data) << 3)
  :   "eax");
}


}} /* namespace ilias::i386 */
