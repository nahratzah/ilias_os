#include <cstring>
#include <cstdio>

constexpr char BASE = 'a';
const int N = 26;

int main() {
  char src[N];
  char dst[N + 1];

  for (int i = 0; i < N; ++i) src[i] = BASE + i;

  for (int base = BASE; base < BASE + N; ++base) {
    fprintf(stderr, "Testing %c\n", base);
    for (int i = 0; i < N + 1; ++i) dst[i] = 'X';

    void* rv = ::test_std::memccpy(dst, src, base, N);
    void* expect_rv = &dst[base - BASE + 1];

    if (rv != expect_rv) {
      fprintf(stderr, "memcccpy returned %p, expected %p\n", rv, expect_rv);
      return 1;
    }

    if (memcmp(dst, src, base - BASE + 1) != 0) {
      fprintf(stderr, "Copied:   %.*s\n"
                      "Expected: %.*s\n",
              int(base - BASE + 1), &dst[0],
              int(base - BASE + 1), &src[0]);
      return 2;
    }

    if (dst[base - BASE + 1] != 'X') {
      fprintf(stderr, "Too many bytes copied, found %c at %d, expected %c\n",
              dst[base - BASE + 1], int(base - BASE + 1), 'X');
      return 3;
    }
  }
}
