#ifndef _STDIMPL_HEAP_SUPPORT_H_
#define _STDIMPL_HEAP_SUPPORT_H_

#include <cdecl.h>
#include <abi/ext/log2.h>

_namespace_begin(std)
namespace impl {
namespace heap {


/*
 * Calculate the level which a given index is at.
 * Index is 0-based.
 * Resulting level is 0-based.
 *
 * Example:
 * a[0] is at level 0
 * a[1..2] are at level 1
 * a[3..6] are at level 2
 * a[7..14] are at level 3
 * etc...
 */
template<typename T>
constexpr T level(T i) noexcept {
  return abi::ext::log2_down(i + 1);
}

/*
 * Calculate the number of elements at a given level.
 *
 * Example:
 * level 0 contains 1 element
 * level 1 contains 2 elements
 * level 2 contains 4 elements
 * etc...
 */
template<typename T>
constexpr T level_size(T lvl) noexcept {
  return T(1) << lvl;
}

/*
 * Calculate the index of the first element of a given level.
 *
 * Example:
 * level 0 starts at element 0
 * level 1 starts at element 1
 * level 2 starts at element 3
 * level 3 starts at element 7
 * etc...
 */
template<typename T>
constexpr T level_off(T lvl) noexcept {
  return level_size(lvl) - T(1);
}

/*
 * Calculate the element offset relative to the start of its level.
 *
 * Example:
 * a[0] has offset 0  (at level 0)
 * a[1] has offset 0  (at level 1)
 * a[2] has offset 1  (at level 1)
 * a[3] has offset 0  (at level 2)
 * a[4] has offset 1  (at level 2)
 * a[5] has offset 2  (at level 2)
 * a[6] has offset 3  (at level 2)
 * a[7] has offset 0  (at level 3)
 * etc...
 */
template<typename T>
constexpr T elem_level_off(T i) noexcept {
  return i - level_off(level(i));
}

/*
 * Calculate where the children of element i are located.
 *
 * Example:
 * a[0] has children at a[1..2]    (return = 1)
 * a[1] has children at a[3..4]    (return = 3)
 * a[2] has children at a[5..6]    (return = 5)
 * a[3] has children at a[7..8]    (return = 7)
 * a[4] has children at a[9..10]   (return = 9)
 * a[5] has children at a[11..12]  (return = 11)
 * a[6] has children at a[13..14]  (return = 13)
 * a[7] has children at a[15..16]  (return = 15)
 * etc...
 */
template<typename T>
constexpr T elem_child(T i) noexcept {
  return 2U * i + 1U;
}

/*
 * Calculate the parent of element i.
 *
 * Example:
 * a[0] has no parent  (result undefined)
 * a[1..2] has parent 0
 * a[3..4] has parent 1
 * a[4..5] has parent 2
 * a[6..7] has parent 3
 * a[8..9] has parent 4
 */
template<typename T>
constexpr T elem_parent(T i) noexcept {
  return (i - 1U) / 2U;
}


}} /* namespace std::impl::heap */
_namespace_end(std)

#endif /* _STDIMPL_HEAP_SUPPORT_H_ */
