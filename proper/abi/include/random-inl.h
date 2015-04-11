#ifndef _RANDOM_INL_H_
#define _RANDOM_INL_H_

#include <random>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <utility>
#include <abi/ext/log2.h>
#include <stdimpl/ziggurat.h>

_namespace_begin(std)
namespace impl {


template<typename T>
constexpr auto rnd_mod_(T x, T y) noexcept -> T {
  return (y == 0 ? x : x % y);
}

template<typename T>
constexpr auto rnd_rshift_(T x, size_t n) noexcept -> T {
  return (n >= numeric_limits<T>::digits ? 0 : x >> n);
}

template<typename T>
constexpr auto rnd_lshift_(T x, size_t n, T modulus) noexcept -> T {
  return (n >= numeric_limits<T>::digits ? 0 : rnd_mod_(x << n, modulus));
}


} /* namespace std::impl */


template<typename UIntType, UIntType A, UIntType C, UIntType M>
linear_congruential_engine<UIntType, A, C, M>::linear_congruential_engine(
    result_type s)
: x_((impl::rnd_mod_(increment, modulus) == 0 &&
      impl::rnd_mod_(s, modulus) == 0) ?
     1U :
     impl::rnd_mod_(s, modulus))
{}

template<typename UIntType, UIntType A, UIntType C, UIntType M>
template<typename Sseq>
linear_congruential_engine<UIntType, A, C, M>::linear_congruential_engine(
    Sseq& q)
: linear_congruential_engine(process_sseq_(q))
{}

template<typename UIntType, UIntType A, UIntType C, UIntType M>
auto linear_congruential_engine<UIntType, A, C, M>::seed(result_type s) ->
    void {
  x_ = ((impl::rnd_mod_(increment, modulus) == 0 &&
         impl::rnd_mod_(s, modulus) == 0) ?
        1U :
        impl::rnd_mod_(s, modulus));
}

template<typename UIntType, UIntType A, UIntType C, UIntType M>
template<typename Sseq>
auto linear_congruential_engine<UIntType, A, C, M>::seed(Sseq& q) ->
    void {
  seed(process_sseq_(q));
}

template<typename UIntType, UIntType A, UIntType C, UIntType M>
auto linear_congruential_engine<UIntType, A, C, M>::operator()() ->
    result_type {
  using impl::rnd_mod_;

  return x_ = rnd_mod_(multiplier * x_ + increment, modulus);
}

template<typename UIntType, UIntType A, UIntType C, UIntType M>
auto linear_congruential_engine<UIntType, A, C, M>::discard(
    unsigned long long n) -> void {
  while (n--) (*this)();
}

template<typename UIntType, UIntType A, UIntType C, UIntType M>
template<typename Sseq>
auto linear_congruential_engine<UIntType, A, C, M>::process_sseq_(Sseq& q) ->
    result_type {
  using impl::rnd_mod_;

  static constexpr unsigned int k = (abi::ext::log2_up(modulus) + 31U) / 32U;
  array<UIntType, k + 3> a;
  q.generate(a.begin(), a.end());

  result_type sum = 0U;
  result_type mul = 1U;
  for (unsigned int j = 0; j != k; ++j) {
    sum = rnd_mod_(sum + rnd_mod_(rnd_mod_(a[j + 3], modulus) * mul,
                                  modulus),
                   modulus);
    mul = (sizeof(mul) > 4 ? rnd_mod_(mul << 32U, modulus) : 0U);
  }
  return sum;
}


template<typename UIntType, size_t W, size_t N, size_t M, size_t R,
         UIntType A, size_t U,
         UIntType D, size_t S,
         UIntType B, size_t T,
         UIntType C, size_t L,
         UIntType F>
mersenne_twister_engine<UIntType, W, N, M, R,
                        A, U, D, S, B, T, C, L, F>::mersenne_twister_engine(
    result_type s) {
  seed(s);
}

template<typename UIntType, size_t W, size_t N, size_t M, size_t R,
         UIntType A, size_t U,
         UIntType D, size_t S,
         UIntType B, size_t T,
         UIntType C, size_t L,
         UIntType F>
template<typename Sseq>
mersenne_twister_engine<UIntType, W, N, M, R,
                        A, U, D, S, B, T, C, L, F>::mersenne_twister_engine(
    Sseq& q) {
  seed(q);
}

template<typename UIntType, size_t W, size_t N, size_t M, size_t R,
         UIntType A, size_t U,
         UIntType D, size_t S,
         UIntType B, size_t T,
         UIntType C, size_t L,
         UIntType F>
auto mersenne_twister_engine<UIntType, W, N, M, R,
                             A, U, D, S, B, T, C, L, F>::seed(result_type s) ->
    void {
  using impl::rnd_mod_;

  x_[0] = rnd_mod_(s, modulus);
  for (unsigned int i = 1; i < x_.size(); ++i) {
    auto pred = (x_[i - 1] ^ (x_[i - 1] >> (word_size - 2U)));
    x_[i] = rnd_mod_(initialization_multiplier * rnd_mod_(pred, modulus) + i,
                     modulus);
  }
}

template<typename UIntType, size_t W, size_t N, size_t M, size_t R,
         UIntType A, size_t U,
         UIntType D, size_t S,
         UIntType B, size_t T,
         UIntType C, size_t L,
         UIntType F>
template<typename Sseq>
auto mersenne_twister_engine<UIntType, W, N, M, R,
                             A, U, D, S, B, T, C, L, F>::seed(Sseq& q) ->
    void {
  using impl::rnd_mod_;

  const size_t k = (word_size + 31U) / 32U;

  array<result_type, state_size * k> a;
  q.generate(a.begin(), a.end());

  for (size_t i = 0; i != x_.size(); ++i) {
    result_type sum = 0U;
    result_type mul = 1U;
    for (size_t j = 0; j != k; ++j) {
      sum = a[k * i + j] * mul;
      mul = (sizeof(mul) > 4 ? mul << 32U : 0U);
    }
    x_[i] = rnd_mod_(sum, modulus);
  }

  if (word_size != mask_bits) {
    const auto shift = numeric_limits<result_type>::digits -
                       (word_size - mask_bits);
    const result_type mask = static_cast<result_type>(0U) -
                             (static_cast<result_type>(1U) << shift);
    if (((x_[0] & mask) == static_cast<result_type>(0U)) &&
        all_of(x_.begin() + 1, x_.end(),
               [](result_type c) {
                 return c == static_cast<result_type>(0U);
               }))
      x_[0] = modulus - 1U;
  }
}

template<typename UIntType, size_t W, size_t N, size_t M, size_t R,
         UIntType A, size_t U,
         UIntType D, size_t S,
         UIntType B, size_t T,
         UIntType C, size_t L,
         UIntType F>
auto mersenne_twister_engine<UIntType, W, N, M, R,
                             A, U, D, S, B, T, C, L, F>::operator()() ->
    result_type {
  using impl::rnd_mod_;
  using impl::rnd_rshift_;
  using impl::rnd_lshift_;

  const result_type x_i = transition_();
  result_type z1 = x_i ^ (rnd_rshift_(x_i, tempering_u) & tempering_d);
  result_type z2 = z1 ^ (rnd_lshift_(z1, tempering_s, modulus) & tempering_b);
  result_type z3 = z2 ^ (rnd_lshift_(z2, tempering_t, modulus) & tempering_c);
  result_type z4 = z3 ^ (rnd_rshift_(z3, tempering_l));

  *move(x_.begin() + 1, x_.end(), x_.begin()) = x_i;
  return z4;
}

template<typename UIntType, size_t W, size_t N, size_t M, size_t R,
         UIntType A, size_t U,
         UIntType D, size_t S,
         UIntType B, size_t T,
         UIntType C, size_t L,
         UIntType F>
auto mersenne_twister_engine<UIntType, W, N, M, R,
                             A, U, D, S, B, T, C, L, F>::discard(
    unsigned long long n) -> void {
  while (n--) {
    result_type x_i = transition_();
    *move(x_.begin() + 1, x_.end(), x_.begin()) = x_i;
  }
}

template<typename UIntType, size_t W, size_t N, size_t M, size_t R,
         UIntType A, size_t U,
         UIntType D, size_t S,
         UIntType B, size_t T,
         UIntType C, size_t L,
         UIntType F>
auto mersenne_twister_engine<UIntType, W, N, M, R,
                             A, U, D, S, B, T, C, L, F>::transition_() const ->
    result_type {
  using impl::rnd_rshift_;

  constexpr result_type mod_mask = modulus - 1U;
  constexpr result_type low_mask = mod_mask >> (word_size - mask_bits);
  constexpr result_type high_mask = mod_mask ^ low_mask;

  const result_type y = (x_[0] & high_mask) | (x_[1] & low_mask);
  const result_type alpha = (y & 0x1 ? xor_mask : static_cast<result_type>(0));

  return x_[shift_size - 1] ^ rnd_rshift_(y, 1) ^ alpha;
}


template<typename UIntType, size_t W, size_t S, size_t R>
subtract_with_carry_engine<UIntType, W, S, R>::subtract_with_carry_engine(
    result_type s) {
  seed(s);
}

template<typename UIntType, size_t W, size_t S, size_t R>
template<typename Sseq>
subtract_with_carry_engine<UIntType, W, S, R>::subtract_with_carry_engine(
    Sseq& q) {
  seed(q);
}

template<typename UIntType, size_t W, size_t S, size_t R>
auto subtract_with_carry_engine<UIntType, W, S, R>::seed(result_type s) ->
    void {
  using impl::rnd_mod_;
  using lce = linear_congruential_engine<result_type, 40014U, 0U, 2147483563U>;

  auto e = lce(s == 0U ? default_seed : s);
  array<result_type, (word_size + 31U) / 32U> a;

  for (result_type& x : x_) {
    generate(a.begin(), a.end(), ref(e));
    result_type sum = 0U;
    result_type mul = 1U;
    for (result_type aval : a) {
      sum += aval * mul;
      mul = (sizeof(mul) > 4 ? mul << 32U : 0U);
    }
    x = rnd_mod_(sum, modulus);
  }

  c_ = (x_.back() == 0 ?
        static_cast<result_type>(1U) :
        static_cast<result_type>(0U));
}

template<typename UIntType, size_t W, size_t S, size_t R>
template<typename Sseq>
auto subtract_with_carry_engine<UIntType, W, S, R>::seed(Sseq& q) ->
    void {
  using impl::rnd_mod_;

  array<result_type, (word_size + 31U) / 32U> a;

  for (result_type& x : x_) {
    q.generate(a.begin(), a.end());
    result_type sum = 0U;
    result_type mul = 1U;
    for (result_type aval : a) {
      sum += aval * mul;
      mul = (sizeof(mul) > 4 ? mul << 32U : 0U);
    }
    x = rnd_mod_(sum, modulus);
  }

  c_ = (x_.back() == 0 ?
        static_cast<result_type>(1U) :
        static_cast<result_type>(0U));
}

template<typename UIntType, size_t W, size_t S, size_t R>
auto subtract_with_carry_engine<UIntType, W, S, R>::operator()() ->
    result_type {
  using impl::rnd_mod_;

  result_type x_slag = x_[x_.size() - short_lag];
  result_type x_llag = x_[x_.size() - long_lag];
  const result_type Y = x_slag - x_llag - c_;
  const result_type x_i = rnd_mod_(Y, modulus);

  *move(x_.begin() + 1, x_.end(), x_.begin()) = x_i;
  c_ = (Y < 0 || x_llag + c_ > x_slag ? 1 : 0);
  return x_i;
}

template<typename UIntType, size_t W, size_t S, size_t R>
auto subtract_with_carry_engine<UIntType, W, S, R>::discard(
    unsigned long long n) -> void {
  while (n--) (*this)();
}


template<typename Engine, size_t P, size_t R>
discard_block_engine<Engine, P, R>::discard_block_engine(const Engine& e)
: e_(e)
{}

template<typename Engine, size_t P, size_t R>
discard_block_engine<Engine, P, R>::discard_block_engine(Engine&& e)
: e_(move(e))
{}

template<typename Engine, size_t P, size_t R>
discard_block_engine<Engine, P, R>::discard_block_engine(result_type s)
: e_(s)
{}

template<typename Engine, size_t P, size_t R>
template<typename Sseq>
discard_block_engine<Engine, P, R>::discard_block_engine(Sseq& q)
: e_(q)
{}

template<typename Engine, size_t P, size_t R>
auto discard_block_engine<Engine, P, R>::seed() -> void {
  e_.seed();
}

template<typename Engine, size_t P, size_t R>
auto discard_block_engine<Engine, P, R>::seed(result_type s) -> void {
  e_.seed(s);
}

template<typename Engine, size_t P, size_t R>
template<typename Sseq>
auto discard_block_engine<Engine, P, R>::seed(Sseq& q) -> void {
  e_.seed(q);
}

template<typename Engine, size_t P, size_t R>
auto discard_block_engine<Engine, P, R>::operator()() -> result_type {
  if (n_ >= used_block) {
    e_.discard(block_size - used_block);
    n_ = 0;
  }
  return e_();
}

template<typename Engine, size_t P, size_t R>
auto discard_block_engine<Engine, P, R>::discard(unsigned long long n) ->
    void {
  unsigned long long skip = 0;
  if (n_ >= used_block) {
    skip += block_size - used_block;
    n_ = 0;
  } else if (n_ != 0) {
    auto avail = used_block - n_;
    if (avail <= n) {
      skip += avail + (block_size - used_block);
      n -= avail;
      n_ = 0;
    } else {
      n_ += n;
      e_.discard(n);
      return;
    }
  }

  /*
   * Skip whole blocks.
   * # blocks is determined by the number of entries it would yield.
   */
  skip += n / used_block * block_size;
  n %= used_block;

  /*
   * Skip part of a block.
   */
  skip += n;  // n < used_block && n_ == 0
  n_ += n;

  e_.discard(skip);
}


template<typename Engine, size_t W, typename UIntType>
constexpr auto independent_bits_engine<Engine, W, UIntType>::max() ->
    result_type {
  return modulus - 1U;
}

template<typename Engine, size_t W, typename UIntType>
independent_bits_engine<Engine, W, UIntType>::independent_bits_engine(
    const Engine& e)
: e_(e)
{}

template<typename Engine, size_t W, typename UIntType>
independent_bits_engine<Engine, W, UIntType>::independent_bits_engine(
    Engine&& e)
: e_(move(e))
{}

template<typename Engine, size_t W, typename UIntType>
independent_bits_engine<Engine, W, UIntType>::independent_bits_engine(
    result_type s)
: e_(static_cast<typename Engine::result_type>(s))
{}

template<typename Engine, size_t W, typename UIntType>
template<typename Sseq>
independent_bits_engine<Engine, W, UIntType>::independent_bits_engine(
    Sseq& q)
: e_(q)
{}

template<typename Engine, size_t W, typename UIntType>
auto independent_bits_engine<Engine, W, UIntType>::seed() -> void {
  e_.seed();
}

template<typename Engine, size_t W, typename UIntType>
auto independent_bits_engine<Engine, W, UIntType>::seed(result_type s) ->
    void {
  e_.seed(static_cast<typename Engine::result_type>(s));
}

template<typename Engine, size_t W, typename UIntType>
template<typename Sseq>
auto independent_bits_engine<Engine, W, UIntType>::seed(Sseq& q) -> void {
  e_.seed(q);
}

template<typename Engine, size_t W, typename UIntType>
auto independent_bits_engine<Engine, W, UIntType>::operator()() ->
    result_type {
  result_type S = 0;

  for (size_t k = 0; k != n0; ++k) {
    typename Engine::result_value u;
    do {
      u = e_() - Engine::min();
    } while (u >= y0);
    S = derive_two_pow_w0(n) * S +
        rnd_mod_(u, derive_two_pow_w0(n));
  }

  for (size_t k = n0; k != n; ++k) {
    typename Engine::result_value u;
    do {
      u = e_() - Engine::min();
    } while (u >= y1);
    S = (2U * derive_two_pow_w0(n)) * S +
        rnd_mod_(u, 2U * derive_two_pow_w0(n));
  }

  return S;
}

template<typename Engine, size_t W, typename UIntType>
auto independent_bits_engine<Engine, W, UIntType>::discard(
    unsigned long long n) -> void {
  while (n--) (*this)();
}


template<typename Engine, size_t K>
shuffle_order_engine<Engine, K>::shuffle_order_engine() {
  init();
}

template<typename Engine, size_t K>
shuffle_order_engine<Engine, K>::shuffle_order_engine(const Engine& e)
: e_(e)
{
  init();
}

template<typename Engine, size_t K>
shuffle_order_engine<Engine, K>::shuffle_order_engine(Engine&& e)
: e_(move(e))
{
  init();
}

template<typename Engine, size_t K>
shuffle_order_engine<Engine, K>::shuffle_order_engine(result_type s)
: e_(s)
{
  init();
}

template<typename Engine, size_t K>
template<typename Sseq>
shuffle_order_engine<Engine, K>::shuffle_order_engine(Sseq& q)
: e_(q)
{
  init();
}

template<typename Engine, size_t K>
auto shuffle_order_engine<Engine, K>::seed() -> void {
  e_.seed();
  init();
}

template<typename Engine, size_t K>
auto shuffle_order_engine<Engine, K>::seed(result_type s) -> void {
  e_.seed(s);
  init();
}

template<typename Engine, size_t K>
template<typename Sseq>
auto shuffle_order_engine<Engine, K>::seed(Sseq& q) -> void {
  e_.seed(q);
  init();
}

template<typename Engine, size_t K>
auto shuffle_order_engine<Engine, K>::operator()() -> result_type {
  constexpr result_type range = Engine::max() - Engine::min() + 1U;
  const auto jj = table_size * (Y_ - Engine::min());
  const size_t j = (range == 0U ? jj : jj / range);

  Y_ = exchange(V_[j], e_());
  return Y_;
}

template<typename Engine, size_t K>
auto shuffle_order_engine<Engine, K>::discard(unsigned long long n) -> void {
  while (n--) (*this)();
}

template<typename Engine, size_t K>
auto shuffle_order_engine<Engine, K>::init() -> void {
  generate(V_.begin(), V_.end(), ref(e_));
  Y_ = e_();
}


inline auto random_device::operator()() noexcept -> result_type {
  return arc4random();
}

inline auto random_device::entropy() const noexcept -> double {
  // sigma(0 .. 2^32 - 1 : 1 / 2^32 * log(1 / 2^32))
  return -22.1807097779182499013514278866616501784160042995281L;
}


template<typename U>
seed_seq::seed_seq(initializer_list<U> il)
: seed_seq(il.begin(), il.end())
{}

template<typename InputIter>
seed_seq::seed_seq(InputIter b, InputIter e) {
  transform(b, e, back_inserter(v_),
            [](result_type v) -> result_type { return v & 0xffffffffU; });
}

template<typename RandomAccessIterator>
auto seed_seq::generate(RandomAccessIterator begin,
                        RandomAccessIterator end) -> void {
  using impl::rnd_mod_;
  using delta_type =
      common_type_t<make_unsigned_t<decltype(v_.size())>,
                    make_unsigned_t<decltype(distance(begin, end))>>;

  if (begin == end) return;

  const delta_type s = v_.size();
  const delta_type n = distance(begin, end);

  fill(begin, end, 0x8b8b8b8b);

  const auto t = (n >= 623U ?
                  11U :
                  (n >= 68U ?
                   7U :
                   (n >= 39U ?
                    5U :
                    (n >= 7U ?
                     3U :
                     (n - 1U) / 2U))));
  const auto p = (n - t) / 2U;
  const auto q = p + t;
  const delta_type m = max(static_cast<delta_type>(s + 1),
                           static_cast<delta_type>(n));

  for (delta_type k = 0; k != m; ++k) {
    result_type r1 = 1664525U * T(begin[k % n] ^
                                  begin[(k + p) % n] ^
                                  begin[(k + (k < 1U ? n : 0U) - 1U) % n]);
    result_type r2 = (k == 0 ?
                      s :
                      (k <= s ?
                       rnd_mod_(k, n) + v_[k - 1U] :
                       rnd_mod_(k, n)));
    begin[(k + p) % n] = rnd_mod_(begin[(k + p) % n] + r1, modulus);
    begin[(k + q) % n] = rnd_mod_(begin[(k + q) % n] + r2, modulus);
    begin[k % n] = r2;
  }

  for (delta_type k = m; k != m + n - 1U; ++k) {
    result_type r3 = 1566083941 * T(begin[k % n] +
                                    begin[(k + p) % n] +
                                    begin[(k - 1U) % n]);
    result_type r4 = r3 - k % n;
    begin[(k + p) % n] ^= rnd_mod_(r3, modulus);
    begin[(k + q) % n] ^= rnd_mod_(r4, modulus);
    begin[k % n] = r4;
  }
}

inline auto seed_seq::size() const -> size_t {
  return v_.size();
}

template<typename OutputIterator>
auto seed_seq::param(OutputIterator dest) const -> void {
  copy(v_.begin(), v_.end(), dest);
}


template<typename RealType, size_t Bits, typename URNG>
auto generate_canonical(URNG& g) -> RealType {
  constexpr size_t b =
      min(static_cast<size_t>(numeric_limits<RealType>::digits), Bits);
  constexpr auto R = URNG::max() - URNG::min() + 1U;
  constexpr auto log2_R = abi::ext::log2_down(R);
  auto k = max((b + log2_R - 1U) / (abi::ext::log2_down(R)), 1U);

  RealType R_mul = 1;
  RealType S = 0;
  for (decltype(k) i = 0; i != k; ++i) {
    S += (g() - URNG::min()) * R_mul;
    R_mul *= R;
  }
  return S / R_mul;
}


template<typename IntType>
uniform_int_distribution<IntType>::uniform_int_distribution(result_type a,
                                                            result_type b)
: p_{ a, static_cast<rt_unsigned>(b - a) + 1U }
{
  assert(a <= b);
}

template<typename IntType>
uniform_int_distribution<IntType>::uniform_int_distribution(
    const param_type& parm)
: p_(parm)
{}

template<typename IntType>
auto uniform_int_distribution<IntType>::reset() -> void {
  param(param_type());
}

template<typename IntType>
template<typename URNG>
auto uniform_int_distribution<IntType>::operator()(URNG& g) -> result_type {
  return (*this)(g, param());
}

template<typename IntType>
template<typename URNG>
auto uniform_int_distribution<IntType>::operator()(URNG& g,
                                                   const param_type& parm) ->
    result_type {
  if (parm.range_ == 1) return parm.lo_;
  // Calculate min = (2 ** digits) % range.
  const rt_unsigned min = -parm.range_ % parm.range_;

  /*
   * This could theoretically loop forever, but each retry has p > 0.5
   * worst case of selecting a number inside [0..min).
   * Usually, p will be much better, so it should rarely need to loop.
   */
  rt_unsigned S;
  do {
    S = generate_(g);
  } while (S < min);

  return parm.lo_ + rnd_mod_(S, parm.range_);
}

template<typename IntType>
template<typename URNG>
auto uniform_int_distribution<IntType>::generate_(URNG& g) -> rt_unsigned {
  static_assert(URNG::min() != URNG::max(),
                "Random number generator must generate random numbers.");
  using urng_unsigned = make_unsigned_t<typename URNG::result_type>;

  // Range of values in URNG.
  constexpr urng_unsigned range =
      static_cast<urng_unsigned>(URNG::max() - URNG::min()) + 1U;
  // Number of bits that will be generated by URNG.
  constexpr auto bits = (range == 0 ?
                         numeric_limits<urng_unsigned>::digits :
                         abi::ext::log2_down(range));
  // Values >= reject are to be rejected, reject is a power-of-2.
  constexpr rt_unsigned reject = (bits >= numeric_limits<rt_unsigned>::digits ?
                                  static_cast<rt_unsigned>(0) :
                                  static_cast<rt_unsigned>(1) << bits);
  // Number of cycles to generate enough bits for rt_unsigned.
  constexpr auto k = (numeric_limits<rt_unsigned>::digits + (bits - 1U)) /
                     bits;

  rt_unsigned S = 0;
  for (decltype(k) i = 0; i != k; ++i) {
    // Generate a random number `gv` in the range [0..reject).
    urng_unsigned gv;
    do {
      gv = g() - URNG::min();
    } while (reject != 0U && gv >= reject);

    // Merge bits from `gv` into solution S.
    S = ((S * reject) | gv);
  }
  return S;
}


template<typename RealType>
uniform_real_distribution<RealType>::uniform_real_distribution(result_type a,
                                                               result_type b)
: p_{ a, b - a }
{
  assert(a <= b);
  assert(b - a <= numeric_limits<RealType>::max());
}

template<typename RealType>
uniform_real_distribution<RealType>::uniform_real_distribution(
    const param_type& parm)
: p_(parm)
{}

template<typename RealType>
auto uniform_real_distribution<RealType>::reset() -> void {
  param(param_type());
}

template<typename RealType>
template<typename URNG>
auto uniform_real_distribution<RealType>::operator()(URNG& g) -> result_type {
  return (*this)(g, param());
}

template<typename RealType>
template<typename URNG>
auto uniform_real_distribution<RealType>::operator()(URNG& g,
                                                     const param_type& parm) ->
    result_type {
  constexpr size_t bits = sizeof(RealType) * CHAR_BIT;
  return parm.lo_ + parm.range_ * generate_canonical<RealType, bits>(g);
}


inline bernoulli_distribution::bernoulli_distribution(double p) {
  p_.p_ = p;
  assert(0 <= p && p <= 1);
}

inline bernoulli_distribution::bernoulli_distribution(
    const param_type& parm)
: p_(parm)
{}

inline auto bernoulli_distribution::reset() -> void {
  param(param_type());
}

template<typename URNG>
auto bernoulli_distribution::operator()(URNG& g) -> result_type {
  return (*this)(g, param());
}

template<typename URNG>
auto bernoulli_distribution::operator()(URNG& g, const param_type& parm) ->
    result_type {
  constexpr size_t bits = sizeof(double) * CHAR_BIT;
  return (generate_canonical<double, bits>(g) < parm.p_);
}


template<typename IntType>
binomial_distribution<IntType>::binomial_distribution(result_type t, double p)
: p_{ p, t }
{
  assert(0 <= p && p <= 1);
}

template<typename IntType>
binomial_distribution<IntType>::binomial_distribution(const param_type& parm)
: p_(parm)
{}

template<typename IntType>
auto binomial_distribution<IntType>::reset() -> void {
  p_ = param_type();
}

template<typename IntType>
template<typename URNG>
auto binomial_distribution<IntType>::operator()(URNG& g) -> result_type {
  return (*this)(g, param());
}

template<typename IntType>
template<typename URNG>
auto binomial_distribution<IntType>::operator()(URNG& g,
                                                const param_type& parm) ->
    result_type {
  auto d = bernoulli_distribution(parm.p_);
  result_type count = 0;
  for (result_type i = 0; i != parm.t_; ++i)
    if (d(g)) ++count;
  return count;
}


template<typename IntType>
geometric_distribution<IntType>::geometric_distribution(double p)
: p_{ p }
{
  assert(0 <= p && p <= 1);
}

template<typename IntType>
geometric_distribution<IntType>::geometric_distribution(const param_type& parm)
: p_(parm)
{}

template<typename IntType>
auto geometric_distribution<IntType>::reset() -> void {
  p_ = param_type();
}

template<typename IntType>
template<typename URNG>
auto geometric_distribution<IntType>::operator()(URNG& g) -> result_type {
  return (*this)(g, param());
}

template<typename IntType>
template<typename URNG>
auto geometric_distribution<IntType>::operator()(URNG& g,
                                                 const param_type& parm) ->
    result_type {
  auto d = bernoulli_distribution(parm.p_);
  result_type count = 0;
  while (!d(g)) ++count;
  return count;
}


template<typename IntType>
negative_binomial_distribution<IntType>::negative_binomial_distribution(
    result_type k, double p)
: p_{ p, k }
{
  assert(0 < p && p <= 1);
  assert(0 < k);
}

template<typename IntType>
negative_binomial_distribution<IntType>::negative_binomial_distribution(
    const param_type& parm)
: p_(parm)
{}

template<typename IntType>
auto negative_binomial_distribution<IntType>::reset() -> void {
  p_ = param_type();
}

template<typename IntType>
template<typename URNG>
auto negative_binomial_distribution<IntType>::operator()(URNG& g) ->
    result_type {
  return (*this)(g, param());
}

template<typename IntType>
template<typename URNG>
auto negative_binomial_distribution<IntType>::operator()(
    URNG& g, const param_type& parm) -> result_type {
  auto d = bernoulli_distribution(parm.p_);
  result_type count = 0, keep_going = parm.k_;
  while (keep_going) {
    if (d(g))
      --keep_going;
    else
      ++count;
  }
  return count;
}


template<typename RealType>
normal_distribution<RealType>::normal_distribution(result_type mean,
                                                   result_type stddev)
: p_{ mean, stddev }
{
  assert(0 < stddev);
}

template<typename RealType>
normal_distribution<RealType>::normal_distribution(const param_type& parm)
: p_(parm)
{}

template<typename RealType>
template<typename URNG>
auto normal_distribution<RealType>::operator()(URNG& g) -> result_type {
  return (*this)(g, p_);
}

template<typename RealType>
template<typename URNG>
auto normal_distribution<RealType>::operator()(URNG& g,
                                               const param_type& parm) ->
    result_type {
  const int sign = uniform_int_distribution<int>(0, 1)(g);

  auto x = impl::standard_normal_distribution_ziggurat(g);
  if (sign == 0) x = -x;

  return x * parm.stddev + parm.mean;
}


_namespace_end(std)

#endif /* _RANDOM_INL_H_ */
