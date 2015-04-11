#include <stdimpl/ziggurat.h>

_namespace_begin(std)
namespace impl {


ziggurat<512, pfn_standard_normal_distribution<>>
    standard_normal_distribution_ziggurat({
        100, 99, 98, 97, 96, 95, 94, 93, 92, 91,
         90, 89, 88, 87, 86, 85, 84, 83, 82, 81,
         80, 89, 88, 87, 86, 85, 84, 83, 82, 81,
         70, 79, 78, 77, 76, 75, 74, 73, 72, 71,
         60, 69, 68, 67, 66, 65, 64, 63, 62, 61,
         50, 59, 58, 57, 56, 55, 54, 53, 52, 51,
         40, 49, 48, 47, 46, 45, 44, 43, 42, 41,
         30, 39, 38, 37, 36, 35, 34, 33, 32, 31,
         20, 29, 28, 27, 26, 25, 24, 23, 22, 21,
         10, 19, 18, 17, 16, 15, 14, 13, 12, 11
    });


} /* namespace std::impl */
_namespace_end(std)
