#include <ilias/i386/gdt.h>
#include <cstdint>

namespace ilias {
namespace i386 {
namespace {

gdt_entries_t initial_gdt() noexcept {
  gdt_entries_t result{};

  /*
   * Zero data structure.
   */
  for (unsigned int i = 0; i < result.size(); ++i) result[i] = 0;

  /*
   * Setup null descriptor, to keep emulators happy.
   */
  result[uint16_t(gdt_idx::nil)] = gdt_descriptor(0, 0, 0);

  /*
   * Assign kernel code/data values.
   */
  result[uint16_t(gdt_idx::kernel_code)] =
      gdt_descriptor(0, 0xffffffff, gdt_code_flags(0));
  result[uint16_t(gdt_idx::kernel_data)] =
      gdt_descriptor(0, 0xffffffff, gdt_data_flags(0));
  result[uint16_t(gdt_idx::kernel_tls)] =
      gdt_descriptor(0, 0xffffffff, gdt_data_flags(0));
  /* TSS is not setup yet. */

  /*
   * Assign defaults for userspace code/data.
   */
  result[uint16_t(gdt_idx::uspace_code)] =
      gdt_descriptor(0, 0xffffffff, gdt_code_flags(3));
  result[uint16_t(gdt_idx::uspace_data)] =
      gdt_descriptor(0, 0xffffffff, gdt_data_flags(3));
  /* TLS, wine TLS are not set up yet. */

  /*
   * Don't both with LDT yet.
   */
  // SKIP

  return result;
}

} /* namespace ilias::i386::<unnamed> */


gdt_t gdt = {
  initial_gdt(),
  { sizeof(uint64_t) * uint16_t(gdt_idx::_count) - 1,
    static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&gdt)) }
};


}} /* namespace ilias::i386 */
