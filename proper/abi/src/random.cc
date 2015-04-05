#include <random>

_namespace_begin(std)


template class linear_congruential_engine<uint_fast32_t,
                                          16807, 0, 2147483647>;
template class linear_congruential_engine<uint_fast32_t,
                                          48271, 0, 2147483647>;
template class mersenne_twister_engine<uint_fast32_t, 32U, 624U, 397U, 31U,
                                       0x9908b0dfU, 11U,
                                       0xffffffffU,  7U,
                                       0x9d2c5680U, 15U,
                                       0xefc60000U, 18U,
                                       1812433253U>;
template class mersenne_twister_engine<uint_fast64_t, 64U, 312U, 156U, 31U,
                                       0xb5026f5aa96619e9ULL, 29U,
                                       0x5555555555555555ULL, 17U,
                                       0x71d67fffeda60000ULL, 37U,
                                       0xfff7eee000000000ULL, 43U,
                                       6364136223846793005ULL>;
template class subtract_with_carry_engine<uint_fast32_t, 24, 10, 24>;
template class subtract_with_carry_engine<uint_fast64_t, 48,  5, 12>;
template class discard_block_engine<ranlux24_base, 223, 23>;
template class discard_block_engine<ranlux48_base, 389, 11>;
template class shuffle_order_engine<minstd_rand0, 256>;


_namespace_end(std)
