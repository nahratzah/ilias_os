#include <functional>

_namespace_begin(std)


bad_function_call::bad_function_call() noexcept {}
bad_function_call::~bad_function_call() noexcept {}

const char* bad_function_call::what() const noexcept {
  return "std::bad_function_call";
}


namespace {

/* Murmur hash. */
template<typename T, typename N = size_t>
auto hash_impl(T n_) noexcept ->
    enable_if_t<sizeof(N) == 4, N> {
  unsigned int shift = (sizeof(n_) + sizeof(N) - 1) / sizeof(N);

  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;
  uint32_t r1 = 15;
  uint32_t r2 = 13;
  uint32_t m = 5;
  uint32_t n = 0xe6546b64;

  uint32_t hash = 17 * sizeof(T);  // Fixed seed.

  uint32_t k = n_ & 0xffffffff;
  k *= c1;
  k = (k << r1) | (k >> (32 - r1));
  k *= c2;
  hash ^= k;
  hash = (hash << r2) | (hash >> (32 - r2));
  hash = hash * m + n;

  for (unsigned int i = 1; i < shift; ++i) {
    n_ >>= (sizeof(T) <= 4 ? 0 : 32);  // Shift, but silence compiler warning.
    uint32_t rb = n_;
    rb *= c1;
    rb = (rb << r1) | (rb >> (32 - r1));
    rb *= c2;
    hash ^= rb;
  }

  hash ^= sizeof(T);  // hash ^= len.

  hash ^= (hash >> 16);
  hash *= 0x85ebca6b;
  hash ^= (hash >> 13);
  hash *= 0xc2b2ae35;
  hash ^= (hash >> 16);

  return hash;
}

template<typename T, typename N = size_t>
auto hash_impl(T n_) noexcept ->
    enable_if_t<sizeof(N) == 8, N> {
  unsigned int shift = (sizeof(n_) + sizeof(N) - 1) / sizeof(N);

  uint64_t c1 = 0xc6a4a7935bd1e995ULL;
  uint64_t c2 = 0x1b873593;
  uint64_t r1 = 47;
  uint64_t r2 = 13;
  uint64_t m = 5;
  uint64_t n = 0xe6546b64;

  uint64_t hash = 17 * sizeof(T);  // Fixed seed.

  uint64_t k = n_ & 0xffffffff;
  k *= c1;
  k = (k << r1) | (k >> (64 - r1));
  k *= c2;
  hash ^= k;
  hash = (hash << r2) | (hash >> (64 - r2));
  hash = hash * m + n;

  for (unsigned int i = 1; i < shift; ++i) {
    n_ >>= (sizeof(T) <= 8 ? 0 : 64);  // Shift, but silence compiler warning.
    uint32_t rb = n_;
    rb *= c1;
    rb = (rb << r1) | (rb >> (64 - r1));
    rb *= c2;
    hash ^= rb;
  }

  hash ^= sizeof(T);  // hash ^= len.

  hash ^= (hash >> 33);
  hash *= 0xff51afd7ed558ccdULL;
  hash ^= (hash >> 33);
  hash *= 0xc4ceb9fe1a85ec53ULL;
  hash ^= (hash >> 33);

  return hash;
}

} /* namespace std::<unnamed> */


size_t hash<unsigned int>::operator()(unsigned int n) const {
  return hash_impl(n);
}

size_t hash<unsigned long>::operator()(unsigned long n) const {
  return hash_impl(n);
}

size_t hash<unsigned long long>::operator()(unsigned long long n) const {
  return hash_impl(n);
}

size_t hash<float>::operator()(float) const {
  assert_msg(false, "hash<float> needs to be implemented");  // XXX
  return 0;
}

size_t hash<double>::operator()(double) const {
  assert_msg(false, "hash<double> needs to be implemented");  // XXX
  return 0;
}

size_t hash<long double>::operator()(long double) const {
  assert_msg(false, "hash<long double> needs to be implemented");  // XXX
  return 0;
}


_namespace_end(std)
