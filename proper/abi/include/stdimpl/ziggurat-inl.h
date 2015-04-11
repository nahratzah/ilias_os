#ifndef _STDIMPL_ZIGGURAT_INL_H_
#define _STDIMPL_ZIGGURAT_INL_H_

#include "ziggurat.h"
#include <algorithm>
#include <cassert>
#include <utility>
#include <random>

_namespace_begin(std)
namespace impl {


template<size_t N, typename ProbabilityFn, typename RealType>
ziggurat<N, ProbabilityFn, RealType>::ziggurat(const ProbabilityFn& pfn)
: ziggurat(numeric_limits<result_type>::max() / 2, pfn)
{}

template<size_t N, typename ProbabilityFn, typename RealType>
ziggurat<N, ProbabilityFn, RealType>::ziggurat(result_type i,
                                               const ProbabilityFn& pfn)
: pfn_(pfn),
  zero_y_(pfn_(static_cast<result_type>(0)))
{
  const result_type dist = i / layers_.size();
  for (coord& c : layers_) {
    c.x = i;
    c.y = pfn_(i);
    i -= dist;
  }

  validate_();
}

template<size_t N, typename ProbabilityFn, typename RealType>
ziggurat<N, ProbabilityFn, RealType>::ziggurat(
    initializer_list<result_type> il, const ProbabilityFn& pfn)
: ziggurat(il.begin(), il.end(), pfn)
{}

template<size_t N, typename ProbabilityFn, typename RealType>
template<typename Iter>
ziggurat<N, ProbabilityFn, RealType>::ziggurat(
    Iter b, Iter e, const ProbabilityFn& pfn)
: pfn_(pfn),
  zero_y_(pfn_(static_cast<result_type>(0)))
{
  auto i = layers_.begin();
  result_type v = numeric_limits<result_type>::max();
  while (b != e && i != layers_.end()) {
    v = i->x = *b;
    i->y = pfn_(v);
    ++b;
    ++i;
  }
  reverse(layers_.begin(), i);

  const result_type dist = v / ((layers_.end() - i) + 1U);
  while (i != layers_.end()) {
    v -= dist;
    i->x = v;
    i->y = pfn_(v);
    ++i;
  }

  validate_();
}

template<size_t N, typename ProbabilityFn, typename RealType>
template<typename URNG>
auto ziggurat<N, ProbabilityFn, RealType>::operator()(URNG& g) const ->
    result_type {
  for (;;) {
    // Select a layer.
    const layer_index layer_idx = select_layer_(g);
    const auto layer = layers_.begin() + layer_idx;

    // Select a value within the layer.
    const result_type v = select_layer_value_(g, layer_idx);

    /* Skip probability generation (below) if the probability of this value
     * in the current layer is p=1. */
    if ((layer != layers_.end() && v <= layer->x) || v == 0)
      return v;

    // Apply the chance of this value being a good value.
    if (select_y_value_(layer_idx) < pfn_(v))
      return v;
  }
}

template<size_t N, typename ProbabilityFn, typename RealType>
template<typename URNG>
auto ziggurat<N, ProbabilityFn, RealType>::select_layer_(URNG& g) const ->
    layer_index {
  using distribution =
      uniform_int_distribution<typename layer_list::size_type>;

  return distribution(0, layers_.size())(g);
}

template<size_t N, typename ProbabilityFn, typename RealType>
template<typename URNG>
auto ziggurat<N, ProbabilityFn, RealType>::select_layer_value_(
    URNG& g, layer_index layer) const -> result_type {
  using distribution = uniform_real_distribution<result_type>;

  auto hi = numeric_limits<result_type>::max();
  if (layer != 0) hi = layers_[layer - 1U].x;

  return distribution(0, hi)(g);
}

template<size_t N, typename ProbabilityFn, typename RealType>
template<typename URNG>
auto ziggurat<N, ProbabilityFn, RealType>::select_y_value_(URNG& g,
                                                           layer_index layer)
    const -> result_type {
  using distribution = uniform_real_distribution<result_type>;

  const auto lo = (layer == 0 ? 0 : layers_[layer - 1U].y);
  const auto hi = (layer == layers_.size() ? zero_y_ : layers_[layer].y);
  return distribution(lo, hi)(g);
}

template<size_t N, typename ProbabilityFn, typename RealType>
auto ziggurat<N, ProbabilityFn, RealType>::validate_() const noexcept -> void {
  auto n = layers_.begin();
  auto m = next(n);
  while (m != layers_.end()) {
    assert(n->x > m->x);
    assert(n->y <= m->y);

    n = m;
    ++m;
  }
  assert(layers_.back().y <= zero_y_);

  assert(layers_.back().x > 0);
  assert(layers_.front().x <= numeric_limits<result_type>::max());
}


template<typename RealType>
pfn_exponential_distribution<RealType>::pfn_exponential_distribution(
    result_type lambda)
: lambda_(lambda)
{}

template<typename RealType>
auto pfn_exponential_distribution<RealType>::operator()(result_type x) const ->
    result_type {
  return exp(-lambda_ * x);
}


template<typename RealType>
auto pfn_standard_normal_distribution<RealType>::operator()(result_type x)
    const -> result_type {
  return exp(-0.5 * pow(x, 2));
}


} /* namespace std::impl */
_namespace_end(std)

#endif /* _STDIMPL_ZIGGURAT_INL_H_ */
