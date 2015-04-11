#ifndef _STDIMPL_ZIGGURAT_H_
#define _STDIMPL_ZIGGURAT_H_

#include <cdecl.h>
#include "ziggurat-fwd.h"
#include <array>
#include <cassert>
#include <limits>
#include <initializer_list>

_namespace_begin(std)
namespace impl {


template<size_t N, typename ProbabilityFn, typename RealType>
class ziggurat {
 public:
  using result_type = RealType;
  static constexpr size_t layers = N + 1U;

 private:
  struct coord {
    result_type x, y;
  };

  using layer_list = array<coord, N>;
  using layer_index = typename layer_list::size_type;

 public:
  explicit ziggurat(const ProbabilityFn& = ProbabilityFn());
  explicit ziggurat(result_type, const ProbabilityFn& = ProbabilityFn());
  explicit ziggurat(initializer_list<result_type>,
                    const ProbabilityFn& = ProbabilityFn());
  template<typename Iter> explicit ziggurat(
      Iter b, Iter e, const ProbabilityFn& = ProbabilityFn());

  template<typename URNG> result_type operator()(URNG&) const;

 private:
  template<typename URNG> layer_index select_layer_(URNG&) const;
  template<typename URNG> result_type select_layer_value_(URNG&, layer_index)
      const;
  template<typename URNG> result_type select_y_value_(URNG&, layer_index)
      const;
  void validate_() const noexcept;

  ProbabilityFn pfn_;
  layer_list layers_;
  result_type zero_y_;
};

template<typename RealType>
class pfn_exponential_distribution {
 public:
  using result_type = RealType;

  pfn_exponential_distribution(result_type);
  result_type operator()(result_type) const;

  result_type lambda() const { return lambda_; }

 private:
  result_type lambda_;
};

template<typename RealType = long double>
class pfn_standard_normal_distribution {
 public:
  using result_type = RealType;

  result_type operator()(result_type) const;
};


extern const ziggurat<512, pfn_standard_normal_distribution<>>
    standard_normal_distribution_ziggurat;


} /* namespace std::impl */
_namespace_end(std)

#include "ziggurat-inl.h"

#endif /* _STDIMPL_ZIGGURAT_H_ */
