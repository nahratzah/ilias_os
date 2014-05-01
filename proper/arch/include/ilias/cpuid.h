#ifndef _ILIAS_ARCH_CPUID_H_
#define _ILIAS_ARCH_CPUID_H_

#include <cassert>
#include <initializer_list>
#include <string>
#include <tuple>
#include <utility>

namespace ilias {

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
bool has_cpuid() noexcept;
#else
constexpr bool has_cpuid() noexcept { return false; }
#endif

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
inline auto __attribute__((pure)) cpuid(uint32_t function) noexcept ->
    std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> {
  assert(has_cpuid());

  uint32_t a, b, c, d;
  asm volatile ("cpuid" :
                "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(function), "c"(0));
  return std::make_tuple(a, b, c, d);
}
#endif

/*
 * Returns the cpu vendor from the cpu.
 * If cpuid is not supported, returns an empty string.
 */
std::string cpu_vendor();

/*
 * Find the highest supported cpuid function.
 * Returns 0 if cpuid is not supported.
 */
inline auto __attribute__((pure)) cpuid_max_fn() noexcept -> uint32_t {
  return (has_cpuid() ? std::get<0>(cpuid(0)) : 0);
}

/*
 * Find the highest supported cpuid extended function.
 * Returns 0 if cpuid is not supported.
 */
inline auto __attribute__((pure)) cpuid_max_ext_fn() noexcept -> uint32_t {
  return (has_cpuid() ? std::get<0>(cpuid(0x80000000)) : 0);
}

/*
 * Mask, field
 * - get<field>(cpuid(...)) & mask
 *   implies the feature is present.
 */
using cpuid_feature = std::pair<uint32_t, int>;

/* Tag, used on tuples to identify as cpuid feature tests. */
struct cpuid_feature_tag {};
using cpuid_feature_result =
    std::tuple<uint32_t, uint32_t, uint32_t, uint32_t,
               cpuid_feature_tag>;

inline auto __attribute__((pure)) cpuid_features() noexcept ->
    cpuid_feature_result {
  return std::tuple_cat(cpuid(1), std::make_tuple(cpuid_feature_tag()));
}

inline bool cpuid_feature_present(cpuid_feature f,
                                  cpuid_feature_result r = cpuid_features()) {
  uint32_t flags;
  switch (std::get<1>(f)) {
  default:
    assert_msg(false, "cpuid feature test invalid");
  case 0:
    flags = std::get<0>(r);
    break;
  case 1:
    flags = std::get<1>(r);
    break;
  case 2:
    flags = std::get<2>(r);
    break;
  case 3:
    flags = std::get<3>(r);
    break;
  }

  return (flags & std::get<0>(f)) != 0;
}

/* Generate a comma-separated list of features in argument. */
std::string to_string(cpuid_feature_result);

/*
 * cpuid feature constants.
 */
namespace cpuid_feature_const {

constexpr cpuid_feature sse3     = { 1UL <<  0, 2 };
constexpr cpuid_feature pclmul   = { 1UL <<  1, 2 };
constexpr cpuid_feature dtes64   = { 1UL <<  2, 2 };
constexpr cpuid_feature monitor  = { 1UL <<  3, 2 };
constexpr cpuid_feature ds_cpl   = { 1UL <<  4, 2 };
constexpr cpuid_feature vmx      = { 1UL <<  5, 2 };
constexpr cpuid_feature smx      = { 1UL <<  6, 2 };
constexpr cpuid_feature est      = { 1UL <<  7, 2 };
constexpr cpuid_feature tm2      = { 1UL <<  8, 2 };
constexpr cpuid_feature ssse3    = { 1UL <<  9, 2 };
constexpr cpuid_feature cid      = { 1UL << 10, 2 };
constexpr cpuid_feature fma      = { 1UL << 12, 2 };
constexpr cpuid_feature cx16     = { 1UL << 13, 2 };
constexpr cpuid_feature etprd    = { 1UL << 14, 2 };
constexpr cpuid_feature pdcm     = { 1UL << 15, 2 };
constexpr cpuid_feature dca      = { 1UL << 18, 2 };
constexpr cpuid_feature sse4_1   = { 1UL << 19, 2 };
constexpr cpuid_feature sse4_2   = { 1UL << 20, 2 };
constexpr cpuid_feature x2apic   = { 1UL << 21, 2 };
constexpr cpuid_feature movbe    = { 1UL << 22, 2 };
constexpr cpuid_feature popcnt   = { 1UL << 23, 2 };
// 24: reserved
constexpr cpuid_feature aes      = { 1UL << 25, 2 };
constexpr cpuid_feature xsave    = { 1UL << 26, 2 };
constexpr cpuid_feature osxsave  = { 1UL << 27, 2 };
constexpr cpuid_feature avx      = { 1UL << 28, 2 };
constexpr cpuid_feature f16c     = { 1UL << 30, 2 };
// 30: reserved
constexpr cpuid_feature raz      = { 1UL << 31, 2 };  // Guest indicator

constexpr cpuid_feature fpu      = { 1UL <<  0, 3 };
constexpr cpuid_feature vme      = { 1UL <<  1, 3 };
constexpr cpuid_feature de       = { 1UL <<  2, 3 };
constexpr cpuid_feature pse      = { 1UL <<  3, 3 };
constexpr cpuid_feature tsc      = { 1UL <<  4, 3 };
constexpr cpuid_feature msr      = { 1UL <<  5, 3 };
constexpr cpuid_feature pae      = { 1UL <<  6, 3 };
constexpr cpuid_feature mce      = { 1UL <<  7, 3 };
constexpr cpuid_feature cx8      = { 1UL <<  8, 3 };
constexpr cpuid_feature apic     = { 1UL <<  9, 3 };
constexpr cpuid_feature sep      = { 1UL << 11, 3 };
constexpr cpuid_feature mtrr     = { 1UL << 12, 3 };
constexpr cpuid_feature pge      = { 1UL << 13, 3 };
constexpr cpuid_feature mca      = { 1UL << 14, 3 };
constexpr cpuid_feature cmov     = { 1UL << 15, 3 };
constexpr cpuid_feature pat      = { 1UL << 16, 3 };
constexpr cpuid_feature pse36    = { 1UL << 17, 3 };
constexpr cpuid_feature psn      = { 1UL << 18, 3 };
constexpr cpuid_feature clf      = { 1UL << 19, 3 };
constexpr cpuid_feature dtes     = { 1UL << 21, 3 };
constexpr cpuid_feature acpi     = { 1UL << 22, 3 };
constexpr cpuid_feature mmx      = { 1UL << 23, 3 };
constexpr cpuid_feature fxsr     = { 1UL << 24, 3 };
constexpr cpuid_feature sse      = { 1UL << 25, 3 };
constexpr cpuid_feature sse2     = { 1UL << 26, 3 };
constexpr cpuid_feature ss       = { 1UL << 27, 3 };
constexpr cpuid_feature htt      = { 1UL << 28, 3 };
constexpr cpuid_feature tm1      = { 1UL << 29, 3 };
constexpr cpuid_feature ia64     = { 1UL << 30, 3 };
constexpr cpuid_feature pbe      = { 1UL << 31, 3 };

constexpr std::initializer_list<std::pair<cpuid_feature, const char*>>
    all_features = {
  { sse3     , "SSE3"    },
  { pclmul   , "PCLMUL"  },
  { dtes64   , "DTES64"  },
  { monitor  , "MONITOR" },
  { ds_cpl   , "DS_CPL"  },
  { vmx      , "VMX"     },
  { smx      , "SMX"     },
  { est      , "EST"     },
  { tm2      , "TM2"     },
  { ssse3    , "SSSE3"   },
  { cid      , "CID"     },
  { fma      , "FMA"     },
  { cx16     , "CX16"    },
  { etprd    , "ETPRD"   },
  { pdcm     , "PDCM"    },
  { dca      , "DCA"     },
  { sse4_1   , "SSE4.1"  },
  { sse4_2   , "SSE4.2"  },
  { x2apic   , "x2APIC"  },
  { movbe    , "MOVBE"   },
  { popcnt   , "POPCNT"  },
  { aes      , "AES"     },
  { xsave    , "XSAVE"   },
  { osxsave  , "OSXSAVE" },
  { avx      , "AVX"     },
  { f16c     , "F16C"    },
  { raz      , "RAZ"     },

  { fpu      , "FPU"     },
  { vme      , "VME"     },
  { de       , "DE"      },
  { pse      , "PSE"     },
  { tsc      , "TSC"     },
  { msr      , "MSR"     },
  { pae      , "PAE"     },
  { mce      , "MCE"     },
  { cx8      , "CX8"     },
  { apic     , "APIC"    },
  { sep      , "SEP"     },
  { mtrr     , "MTRR"    },
  { pge      , "PGE"     },
  { mca      , "MCA"     },
  { cmov     , "CMOV"    },
  { pat      , "PAT"     },
  { pse36    , "PSE36"   },
  { psn      , "PSN"     },
  { clf      , "CLF"     },
  { dtes     , "DTES"    },
  { acpi     , "ACPI"    },
  { mmx      , "MMX"     },
  { fxsr     , "FXSR"    },
  { sse      , "SSE"     },
  { sse2     , "SSE2"    },
  { ss       , "SS"      },
  { htt      , "HTT"     },
  { tm1      , "TM1"     },
  { ia64     , "IA64"    },
  { pbe      , "PBE"     },
};

} /* namespace ilias::cpuid_feature_const */

} /* namespace ilias */

#endif /* _ILIAS_ARCH_CPUID_H_ */
