namespace __cxxabiv1 {
namespace ext {


/* Check if s is a power-of-2. */
constexpr bool is_pow2(uintmax_t s) {
  return (s & (s - 1U)) == 0U;
}

/* Returns the largest number x, such that 1 << x <= s. */
constexpr unsigned int __log2_down(uintmax_t s, unsigned int bits) {
  return (s <= 1U ? 0U :
          (s < (uintmax_t(1) << (bits / 2U)) ?
           __log2_down(s, bits / 2U) :
           __log2_down(s >> (bits / 2U), bits / 2U) + bits / 2U));
#if 0
  return (s <= 0U ?
          0U :
          ((s >> (bits / 2U)) != 0U ?
           bits + __log2_down(s >> (bits / 2U), bits / 2U) :
           __log2_down(s, bits / 2U)));
#endif
}

/* Returns the largest number x, such that 1 << x <= s. */
constexpr unsigned int log2_down(uintmax_t s) {
  return __log2_down(s, CHAR_BIT * sizeof(uintmax_t));
}

/* Returns the smallest number x, such that 1 << x >= s. */
constexpr unsigned int log2_up(uintmax_t s) {
  return log2_down(s) + (is_pow2(s) ? 0U : 1U);
}

static_assert(log2_down(0x00000000U) ==  0U, "log2 bug");
static_assert(log2_down(0x00000001U) ==  0U, "log2 bug");
static_assert(log2_down(0x00000002U) ==  1U, "log2 bug");
static_assert(log2_down(0x00000003U) ==  1U, "log2 bug");
static_assert(log2_down(0x00000004U) ==  2U, "log2 bug");
static_assert(log2_down(0x00000005U) ==  2U, "log2 bug");
static_assert(log2_down(0x00000006U) ==  2U, "log2 bug");
static_assert(log2_down(0x00000007U) ==  2U, "log2 bug");
static_assert(log2_down(0x00000008U) ==  3U, "log2 bug");
static_assert(log2_down(0x00000009U) ==  3U, "log2 bug");
static_assert(log2_down(0x0000000aU) ==  3U, "log2 bug");
static_assert(log2_down(0x0000000bU) ==  3U, "log2 bug");
static_assert(log2_down(0x0000000cU) ==  3U, "log2 bug");
static_assert(log2_down(0x0000000dU) ==  3U, "log2 bug");
static_assert(log2_down(0x0000000eU) ==  3U, "log2 bug");
static_assert(log2_down(0x0000000fU) ==  3U, "log2 bug");
static_assert(log2_down(0xffffffffU) == 31U, "log2 bug");
static_assert(log2_down(UINTMAX_MAX) == sizeof(uintmax_t) * CHAR_BIT - 1U,
    "log2 bug at UINTMAX_MAX");

static_assert(log2_up(0x00000001U) ==  0U, "log2 bug");
static_assert(log2_up(0x00000002U) ==  1U, "log2 bug");
static_assert(log2_up(0x00000003U) ==  2U, "log2 bug");
static_assert(log2_up(0x00000004U) ==  2U, "log2 bug");
static_assert(log2_up(0x00000005U) ==  3U, "log2 bug");
static_assert(log2_up(0x00000007U) ==  3U, "log2 bug");
static_assert(log2_up(0x00000008U) ==  3U, "log2 bug");
static_assert(log2_up(0x00000009U) ==  4U, "log2 bug");
static_assert(log2_up(0x80000000U) == 31U, "log2 bug");
static_assert(log2_up(0xffffffffU) == 32U, "log2 bug");


}} /* namespace __cxxabi::ext */
