namespace __cxxabiv1 {
namespace ext {


template<typename T> constexpr unsigned int log10(T v) {
  return (v >= 10 ? 1U + log10(v / 10U) : 0U);
}

static_assert(log10(0) == 0, "incorrect result: log10(0)");
static_assert(log10(1) == 0, "incorrect result: log10(1)");
static_assert(log10(2) == 0, "incorrect result: log10(2)");
static_assert(log10(5) == 0, "incorrect result: log10(5)");
static_assert(log10(10) == 1, "incorrect result: log10(10)");
static_assert(log10(11) == 1, "incorrect result: log10(11)");
static_assert(log10(19) == 1, "incorrect result: log10(19)");
static_assert(log10(99) == 1, "incorrect result: log10(99)");
static_assert(log10(100) == 2, "incorrect result: log10(100)");
static_assert(log10(1001) == 3, "incorrect result: log10(1001)");
static_assert(log10(123456789) == 8, "incorrect result: log10(123456789)");


template<typename T> constexpr unsigned int __log10_up_support(T v, bool r) {
  return (v >= 10 ?
          1U + __log10_up_support(v / 10U, r || (v % 10U != 0)) :
	  (r || (v > 1) ? 1 : 0));
}

template<typename T> constexpr unsigned int log10_up(T v) {
  return __log10_up_support(v, false);
}

static_assert(log10_up(0) == 0, "incorrect result: log10_up(0)");
static_assert(log10_up(1) == 0, "incorrect result: log10_up(1)");
static_assert(log10_up(2) == 1, "incorrect result: log10_up(2)");
static_assert(log10_up(5) == 1, "incorrect result: log10_up(5)");
static_assert(log10_up(9) == 1, "incorrect result: log10_up(9)");
static_assert(log10_up(10) == 1, "incorrect result: log10_up(10)");
static_assert(log10_up(11) == 2, "incorrect result: log10_up(11)");
static_assert(log10_up(99) == 2, "incorrect result: log10_up(99)");
static_assert(log10_up(100) == 2, "incorrect result: log10_up(100)");
static_assert(log10_up(1000) == 3, "incorrect result: log10_up(1000)");
static_assert(log10_up(1001) == 4, "incorrect result: log10_up(1001)");
static_assert(log10_up(123456789) == 9,
    "incorrect result: log10_up(123456789)");


}} /* namespace __cxxabiv1::ext */
