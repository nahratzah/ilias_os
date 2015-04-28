#include <limits>

_namespace_begin(std)


#define IMPLEMENT_CONSTEXPR_VALUES(Type)                               \
constexpr bool numeric_limits<Type>::is_specialized;                   \
constexpr int numeric_limits<Type>::digits;                            \
constexpr int numeric_limits<Type>::digits10;                          \
constexpr int numeric_limits<Type>::max_digits10;                      \
constexpr bool numeric_limits<Type>::is_signed;                        \
constexpr bool numeric_limits<Type>::is_integer;                       \
constexpr bool numeric_limits<Type>::is_exact;                         \
constexpr int numeric_limits<Type>::radix;                             \
constexpr int numeric_limits<Type>::min_exponent;                      \
constexpr int numeric_limits<Type>::min_exponent10;                    \
constexpr int numeric_limits<Type>::max_exponent;                      \
constexpr int numeric_limits<Type>::max_exponent10;                    \
constexpr bool numeric_limits<Type>::has_infinity;                     \
constexpr bool numeric_limits<Type>::has_quiet_NaN;                    \
constexpr bool numeric_limits<Type>::has_signaling_NaN;                \
constexpr float_denorm_style numeric_limits<Type>::has_denorm;         \
constexpr bool numeric_limits<Type>::has_denorm_loss;                  \
constexpr bool numeric_limits<Type>::is_iec559;                        \
constexpr bool numeric_limits<Type>::is_bounded;                       \
constexpr bool numeric_limits<Type>::is_modulo;                        \
constexpr bool numeric_limits<Type>::traps;                            \
constexpr bool numeric_limits<Type>::tinyness_before;                  \
constexpr float_round_style numeric_limits<Type>::round_style;

IMPLEMENT_CONSTEXPR_VALUES(bool)
IMPLEMENT_CONSTEXPR_VALUES(char)
IMPLEMENT_CONSTEXPR_VALUES(signed char)
IMPLEMENT_CONSTEXPR_VALUES(unsigned char)
IMPLEMENT_CONSTEXPR_VALUES(char16_t)
IMPLEMENT_CONSTEXPR_VALUES(char32_t)
IMPLEMENT_CONSTEXPR_VALUES(wchar_t)
IMPLEMENT_CONSTEXPR_VALUES(short)
IMPLEMENT_CONSTEXPR_VALUES(unsigned short)
IMPLEMENT_CONSTEXPR_VALUES(int)
IMPLEMENT_CONSTEXPR_VALUES(unsigned int)
IMPLEMENT_CONSTEXPR_VALUES(long)
IMPLEMENT_CONSTEXPR_VALUES(unsigned long)
IMPLEMENT_CONSTEXPR_VALUES(long long)
IMPLEMENT_CONSTEXPR_VALUES(unsigned long long)
#if _USE_INT128
IMPLEMENT_CONSTEXPR_VALUES(int128_t)
IMPLEMENT_CONSTEXPR_VALUES(uint128_t)
#endif
IMPLEMENT_CONSTEXPR_VALUES(float)
IMPLEMENT_CONSTEXPR_VALUES(double)
IMPLEMENT_CONSTEXPR_VALUES(long double)


_namespace_end(std)
