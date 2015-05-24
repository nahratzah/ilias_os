#ifndef _ILIAS_ARCH_CPUID_H_
#define _ILIAS_ARCH_CPUID_H_

#include <cassert>
#include <initializer_list>
#include <string>
#include <tuple>
#include <utility>

namespace ilias {

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
bool has_cpuid() noexcept __attribute__((const));
#else
constexpr bool has_cpuid() noexcept { return false; }
#endif

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
inline auto __attribute__((const)) cpuid(uint32_t function) noexcept ->
    std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> {
  assert(has_cpuid());

  uint32_t a, b, c, d;
  asm ("cpuid"
  : "=a"(a), "=b"(b), "=c"(c), "=d"(d)
  : "a"(function), "b"(0), "c"(0), "d"(0));
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
inline auto __attribute__((const)) cpuid_max_fn() noexcept -> uint32_t {
  return (has_cpuid() ? std::get<0>(cpuid(0)) : 0);
}

/*
 * Find the highest supported cpuid extended function.
 * Returns 0 if cpuid is not supported.
 */
inline auto __attribute__((const)) cpuid_max_ext_fn() noexcept -> uint32_t {
  return (has_cpuid() ? std::get<0>(cpuid(0x80000000)) : 0);
}

/* Tag, used on tuples to identify as cpuid feature tests. */
struct cpuid_feature_tag {};
using cpuid_feature_result =
    std::tuple<uint32_t, uint32_t, uint32_t, uint32_t,
               cpuid_feature_tag>;

inline auto __attribute__((pure)) cpuid_features() noexcept ->
    cpuid_feature_result {
  if (_predict_false(cpuid_max_fn() <= 1))
    return std::make_tuple(0U, 0U, 0U, 0U, cpuid_feature_tag());
  return std::tuple_cat(cpuid(1), std::make_tuple(cpuid_feature_tag()));
}

/* Tag, used on tuples to identify as cpuid extended feature tests. */
struct cpuid_extfeature_tag {};
using cpuid_extfeature_result =
    std::tuple<uint32_t, uint32_t, uint32_t, uint32_t,
               cpuid_extfeature_tag>;

inline auto __attribute__((pure)) cpuid_extfeatures() noexcept ->
    cpuid_extfeature_result {
  if (_predict_false(cpuid_max_ext_fn() <= 0x80000001U))
    return std::make_tuple(0U, 0U, 0U, 0U, cpuid_extfeature_tag());
  return std::tuple_cat(cpuid(0x80000001),
                        std::make_tuple(cpuid_extfeature_tag()));
}

/*
 * Mask, field
 * - get<field>(cpuid(...)) & mask
 *   implies the feature is present.
 */
using cpuid_feature = std::tuple<uint32_t, int, cpuid_feature_tag>;
using cpuid_extfeature = std::tuple<uint32_t, int, cpuid_extfeature_tag>;

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

inline bool cpuid_feature_present(
    cpuid_extfeature f, cpuid_extfeature_result r = cpuid_extfeatures()) {
  uint32_t flags;
  switch (std::get<1>(f)) {
  default:
    assert_msg(false, "cpuid extfeature test invalid");
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
std::string to_string(cpuid_extfeature_result);

/*
 * cpuid feature constants.
 */
namespace cpuid_feature_const {

constexpr cpuid_feature sse3     = cpuid_feature(1UL <<  0, 2, cpuid_feature_tag());
constexpr cpuid_feature pclmul   = cpuid_feature(1UL <<  1, 2, cpuid_feature_tag());
constexpr cpuid_feature dtes64   = cpuid_feature(1UL <<  2, 2, cpuid_feature_tag());
constexpr cpuid_feature monitor  = cpuid_feature(1UL <<  3, 2, cpuid_feature_tag());
constexpr cpuid_feature ds_cpl   = cpuid_feature(1UL <<  4, 2, cpuid_feature_tag());
constexpr cpuid_feature vmx      = cpuid_feature(1UL <<  5, 2, cpuid_feature_tag());
constexpr cpuid_feature smx      = cpuid_feature(1UL <<  6, 2, cpuid_feature_tag());
constexpr cpuid_feature est      = cpuid_feature(1UL <<  7, 2, cpuid_feature_tag());
constexpr cpuid_feature tm2      = cpuid_feature(1UL <<  8, 2, cpuid_feature_tag());
constexpr cpuid_feature ssse3    = cpuid_feature(1UL <<  9, 2, cpuid_feature_tag());
constexpr cpuid_feature cid      = cpuid_feature(1UL << 10, 2, cpuid_feature_tag());
constexpr cpuid_feature fma      = cpuid_feature(1UL << 12, 2, cpuid_feature_tag());
constexpr cpuid_feature cx16     = cpuid_feature(1UL << 13, 2, cpuid_feature_tag());
constexpr cpuid_feature etprd    = cpuid_feature(1UL << 14, 2, cpuid_feature_tag());
constexpr cpuid_feature pdcm     = cpuid_feature(1UL << 15, 2, cpuid_feature_tag());
constexpr cpuid_feature dca      = cpuid_feature(1UL << 18, 2, cpuid_feature_tag());
constexpr cpuid_feature sse4_1   = cpuid_feature(1UL << 19, 2, cpuid_feature_tag());
constexpr cpuid_feature sse4_2   = cpuid_feature(1UL << 20, 2, cpuid_feature_tag());
constexpr cpuid_feature x2apic   = cpuid_feature(1UL << 21, 2, cpuid_feature_tag());
constexpr cpuid_feature movbe    = cpuid_feature(1UL << 22, 2, cpuid_feature_tag());
constexpr cpuid_feature popcnt   = cpuid_feature(1UL << 23, 2, cpuid_feature_tag());
// 24: reserved
constexpr cpuid_feature aes      = cpuid_feature(1UL << 25, 2, cpuid_feature_tag());
constexpr cpuid_feature xsave    = cpuid_feature(1UL << 26, 2, cpuid_feature_tag());
constexpr cpuid_feature osxsave  = cpuid_feature(1UL << 27, 2, cpuid_feature_tag());
constexpr cpuid_feature avx      = cpuid_feature(1UL << 28, 2, cpuid_feature_tag());
constexpr cpuid_feature f16c     = cpuid_feature(1UL << 30, 2, cpuid_feature_tag());
// 30: reserved
constexpr cpuid_feature raz      = cpuid_feature(1UL << 31, 2, cpuid_feature_tag());  // Guest indicator

constexpr cpuid_feature fpu      = cpuid_feature(1UL <<  0, 3, cpuid_feature_tag());
constexpr cpuid_feature vme      = cpuid_feature(1UL <<  1, 3, cpuid_feature_tag());
constexpr cpuid_feature de       = cpuid_feature(1UL <<  2, 3, cpuid_feature_tag());
constexpr cpuid_feature pse      = cpuid_feature(1UL <<  3, 3, cpuid_feature_tag());
constexpr cpuid_feature tsc      = cpuid_feature(1UL <<  4, 3, cpuid_feature_tag());
constexpr cpuid_feature msr      = cpuid_feature(1UL <<  5, 3, cpuid_feature_tag());
constexpr cpuid_feature pae      = cpuid_feature(1UL <<  6, 3, cpuid_feature_tag());
constexpr cpuid_feature mce      = cpuid_feature(1UL <<  7, 3, cpuid_feature_tag());
constexpr cpuid_feature cx8      = cpuid_feature(1UL <<  8, 3, cpuid_feature_tag());
constexpr cpuid_feature apic     = cpuid_feature(1UL <<  9, 3, cpuid_feature_tag());
constexpr cpuid_feature sep      = cpuid_feature(1UL << 11, 3, cpuid_feature_tag());
constexpr cpuid_feature mtrr     = cpuid_feature(1UL << 12, 3, cpuid_feature_tag());
constexpr cpuid_feature pge      = cpuid_feature(1UL << 13, 3, cpuid_feature_tag());
constexpr cpuid_feature mca      = cpuid_feature(1UL << 14, 3, cpuid_feature_tag());
constexpr cpuid_feature cmov     = cpuid_feature(1UL << 15, 3, cpuid_feature_tag());
constexpr cpuid_feature pat      = cpuid_feature(1UL << 16, 3, cpuid_feature_tag());
constexpr cpuid_feature pse36    = cpuid_feature(1UL << 17, 3, cpuid_feature_tag());
constexpr cpuid_feature psn      = cpuid_feature(1UL << 18, 3, cpuid_feature_tag());
constexpr cpuid_feature clf      = cpuid_feature(1UL << 19, 3, cpuid_feature_tag());
constexpr cpuid_feature dtes     = cpuid_feature(1UL << 21, 3, cpuid_feature_tag());
constexpr cpuid_feature acpi     = cpuid_feature(1UL << 22, 3, cpuid_feature_tag());
constexpr cpuid_feature mmx      = cpuid_feature(1UL << 23, 3, cpuid_feature_tag());
constexpr cpuid_feature fxsr     = cpuid_feature(1UL << 24, 3, cpuid_feature_tag());
constexpr cpuid_feature sse      = cpuid_feature(1UL << 25, 3, cpuid_feature_tag());
constexpr cpuid_feature sse2     = cpuid_feature(1UL << 26, 3, cpuid_feature_tag());
constexpr cpuid_feature ss       = cpuid_feature(1UL << 27, 3, cpuid_feature_tag());
constexpr cpuid_feature htt      = cpuid_feature(1UL << 28, 3, cpuid_feature_tag());
constexpr cpuid_feature tm1      = cpuid_feature(1UL << 29, 3, cpuid_feature_tag());
constexpr cpuid_feature ia64     = cpuid_feature(1UL << 30, 3, cpuid_feature_tag());
constexpr cpuid_feature pbe      = cpuid_feature(1UL << 31, 3, cpuid_feature_tag());

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
namespace cpuid_extfeature_const {


constexpr cpuid_extfeature lahfsahf       = cpuid_extfeature(1 <<  0, 2, cpuid_extfeature_tag());
constexpr cpuid_extfeature cmplegacy      = cpuid_extfeature(1 <<  1, 2, cpuid_extfeature_tag());
constexpr cpuid_extfeature svm            = cpuid_extfeature(1 <<  2, 2, cpuid_extfeature_tag());
constexpr cpuid_extfeature extapicspace   = cpuid_extfeature(1 <<  3, 2, cpuid_extfeature_tag());
constexpr cpuid_extfeature altmovcr8      = cpuid_extfeature(1 <<  4, 2, cpuid_extfeature_tag());
constexpr cpuid_extfeature abm            = cpuid_extfeature(1 <<  5, 2, cpuid_extfeature_tag());
constexpr cpuid_extfeature sse4a          = cpuid_extfeature(1 <<  6, 2, cpuid_extfeature_tag());
constexpr cpuid_extfeature misalignsse    = cpuid_extfeature(1 <<  7, 2, cpuid_extfeature_tag());
constexpr cpuid_extfeature _3dnowprefetch = cpuid_extfeature(1 <<  8, 2, cpuid_extfeature_tag());
constexpr cpuid_extfeature osvw           = cpuid_extfeature(1 <<  9, 2, cpuid_extfeature_tag());
constexpr cpuid_extfeature ibs            = cpuid_extfeature(1 << 10, 2, cpuid_extfeature_tag());
constexpr cpuid_extfeature sse5           = cpuid_extfeature(1 << 11, 2, cpuid_extfeature_tag());
constexpr cpuid_extfeature skinit         = cpuid_extfeature(1 << 12, 2, cpuid_extfeature_tag());
constexpr cpuid_extfeature wdt            = cpuid_extfeature(1 << 13, 2, cpuid_extfeature_tag());

constexpr cpuid_extfeature fpu            = cpuid_extfeature(1 <<  0, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature vme            = cpuid_extfeature(1 <<  1, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature de             = cpuid_extfeature(1 <<  2, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature pse            = cpuid_extfeature(1 <<  3, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature tsc            = cpuid_extfeature(1 <<  4, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature msr            = cpuid_extfeature(1 <<  5, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature pae            = cpuid_extfeature(1 <<  6, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature mce            = cpuid_extfeature(1 <<  7, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature cmpxchg8b      = cpuid_extfeature(1 <<  8, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature apic           = cpuid_extfeature(1 <<  9, 3, cpuid_extfeature_tag());
// 10: reserved
constexpr cpuid_extfeature syscallsysret  = cpuid_extfeature(1 << 11, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature mtrr           = cpuid_extfeature(1 << 12, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature pge            = cpuid_extfeature(1 << 13, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature mca            = cpuid_extfeature(1 << 14, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature cmov           = cpuid_extfeature(1 << 15, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature pat            = cpuid_extfeature(1 << 16, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature pse36          = cpuid_extfeature(1 << 17, 3, cpuid_extfeature_tag());
// 18: reserved
// 19: reserved
constexpr cpuid_extfeature nx             = cpuid_extfeature(1 << 20, 3, cpuid_extfeature_tag());
// 21: reserved
constexpr cpuid_extfeature mmxext         = cpuid_extfeature(1 << 22, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature mmx            = cpuid_extfeature(1 << 23, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature fxsr           = cpuid_extfeature(1 << 24, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature ffxsr          = cpuid_extfeature(1 << 25, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature page1gb        = cpuid_extfeature(1 << 26, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature rdtscp         = cpuid_extfeature(1 << 27, 3, cpuid_extfeature_tag());
// 28: reserved
constexpr cpuid_extfeature lm             = cpuid_extfeature(1 << 29, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature _3dnowext      = cpuid_extfeature(1 << 30, 3, cpuid_extfeature_tag());
constexpr cpuid_extfeature _3dnow         = cpuid_extfeature(1 << 31, 3, cpuid_extfeature_tag());

constexpr std::initializer_list<std::pair<cpuid_extfeature, const char*>>
    all_extfeatures = {
  { lahfsahf        , "LAHF/SAHF"      },
  { cmplegacy       , "CmpLegacy"      },
  { svm             , "SVM"            },
  { extapicspace    , "ExtApicSpace"   },
  { altmovcr8       , "AltMovCR8"      },
  { abm             , "ABM"            },
  { sse4a           , "SSE4A"          },
  { misalignsse     , "MisAlignSSE"    },
  { _3dnowprefetch  , "3DNowPrefetch"  },
  { osvw            , "OSVW"           },
  { ibs             , "IBS"            },
  { sse5            , "SSE5"           },
  { skinit          , "SKINIT"         },
  { wdt             , "WDT"            },

  { fpu             , "FPU"            },
  { vme             , "VME"            },
  { de              , "DE"             },
  { pse             , "PSE"            },
  { tsc             , "TSC"            },
  { msr             , "MSR"            },
  { pae             , "PAE"            },
  { mce             , "MCE"            },
  { cmpxchg8b       , "CMPXCHG8B"      },
  { apic            , "APIC"           },
  { syscallsysret   , "SysCall/SysRet" },
  { mtrr            , "MTRR"           },
  { pge             , "PGE"            },
  { mca             , "MCA"            },
  { cmov            , "CMOV"           },
  { pat             , "PAT"            },
  { pse36           , "PSE36"          },
  { nx              , "NX"             },
  { mmxext          , "MMXEXT"         },
  { mmx             , "MMX"            },
  { fxsr            , "FXSR"           },
  { ffxsr           , "FFXSR"          },
  { page1gb         , "PAGE1GB"        },
  { rdtscp          , "RSTSCP"         },
  { lm              , "LM"             },
  { _3dnowext       , "3DNowExt"       },
  { _3dnow          , "3DNow"          },
};


} /* namespace ilias::cpuid_extfeature_const */

} /* namespace ilias */

#endif /* _ILIAS_ARCH_CPUID_H_ */
