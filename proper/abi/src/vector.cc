#include <vector>
#include <abi/misc_int.h>

_namespace_begin(std)


template class vector<bool>;
template class vector<char>;
template class vector<signed char>;
template class vector<unsigned char>;
template class vector<short>;
template class vector<unsigned short>;
template class vector<int>;
template class vector<unsigned int>;
template class vector<long>;
template class vector<unsigned long>;
template class vector<long long>;
template class vector<unsigned long long>;

template bool operator==(const vector<bool>&,
                         const vector<bool>&);
template bool operator==(const vector<char>&,
                         const vector<char>&);
template bool operator==(const vector<signed char>&,
                         const vector<signed char>&);
template bool operator==(const vector<unsigned char>&,
                         const vector<unsigned char>&);
template bool operator==(const vector<short>&,
                         const vector<short>&);
template bool operator==(const vector<unsigned short>&,
                         const vector<unsigned short>&);
template bool operator==(const vector<int>&,
                         const vector<int>&);
template bool operator==(const vector<unsigned int>&,
                         const vector<unsigned int>&);
template bool operator==(const vector<long>&,
                         const vector<long>&);
template bool operator==(const vector<unsigned long>&,
                         const vector<unsigned long>&);
template bool operator==(const vector<long long>&,
                         const vector<long long>&);
template bool operator==(const vector<unsigned long long>&,
                         const vector<unsigned long long>&);

template bool operator<(const vector<bool>&,
                        const vector<bool>&);
template bool operator<(const vector<char>&,
                        const vector<char>&);
template bool operator<(const vector<signed char>&,
                        const vector<signed char>&);
template bool operator<(const vector<unsigned char>&,
                        const vector<unsigned char>&);
template bool operator<(const vector<short>&,
                        const vector<short>&);
template bool operator<(const vector<unsigned short>&,
                        const vector<unsigned short>&);
template bool operator<(const vector<int>&,
                        const vector<int>&);
template bool operator<(const vector<unsigned int>&,
                        const vector<unsigned int>&);
template bool operator<(const vector<long>&,
                        const vector<long>&);
template bool operator<(const vector<unsigned long>&,
                        const vector<unsigned long>&);
template bool operator<(const vector<long long>&,
                        const vector<long long>&);
template bool operator<(const vector<unsigned long long>&,
                        const vector<unsigned long long>&);


namespace impl {


auto _vector_bool_iterator_algorithms::bitwise_count(
    _vector_bool_iterator<true> b, _vector_bool_iterator<true> e, bool v)
    noexcept -> _vector_bool_iterator<true>::difference_type {
  using elem = _vector_bool_iterator<true>::elem;
  using abi::popcount;
  constexpr unsigned int digits = numeric_limits<elem>::digits;
  assert(b <= e);

  _vector_bool_iterator<true>::difference_type count_true = 0,
                                               dist = 0;

  /* Handle partial begin pointer. */
  if (b.shift_ != 0) {
    elem msk = ~((elem(1) << b.shift_) - 1U);
    if (b.p_ == e.p_) {
      msk &= ~((elem(1) << e.shift_) - 1U);
      dist += e.shift_ - b.shift_;
    } else {
      dist += digits - b.shift_;
    }
    count_true += popcount(*b.p_ & msk);

    /* Early exit. (Note that msk above has been fixed above, in this case.) */
    if (b.p_ == e.p_)
      return (v ? count_true : dist - count_true);

    ++b.p_;
    b.shift_ = 0;
  }

  /* Count whole ranges. */
  dist += digits * (e.p_ - b.p_);
  while (b.p_ != e.p_) {
    count_true += popcount(*b.p_);
    ++b.p_;
  }

  /* Handle partial end pointer.
   * Note that if e.shift_ == 0, msk will be 0. */
  elem msk = ((elem(1) << e.shift_) - 1U);
  count_true += popcount(*b.p_ & msk);
  dist += e.shift_;

  return (v ? count_true : dist - count_true);
}


} /* namespace std::impl */
_namespace_end(std)
