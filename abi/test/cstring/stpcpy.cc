#include <cstring>
#include <cstdio>

constexpr int N = 26;
constexpr char BASE = 'a';

int main() {
  char dst[2 * N + 1];
  char src[N + 1];

  for (int i = 0; i < N; ++i) src[i] = BASE + i;
  src[N] = '\0';

  for (int src_off = 0; src_off < N; ++src_off) {
    for (int dst_off = 0; dst_off < N; ++dst_off) {
      char* dst_ptr = &dst[dst_off];
      const char* src_ptr = &src[src_off];
      char* expect_rv = dst_ptr + (N - src_off);

      char* rv = ::test_std::stpcpy(dst_ptr, src_ptr);
      fprintf(stderr, "Copied:   %.*s\n"
                      "Expected: %.*s\n",
              int(N), dst_ptr,
              int(N + 1 - src_off), src_ptr);

      if (rv != expect_rv) {
        fprintf(stderr, "stpcpy returned %p, expected %p\n", rv, expect_rv);
        return 1;
      }

      if (memcmp(dst_ptr, src_ptr, N + 1 - src_off) != 0) {
        fprintf(stderr, "stpcpy did not make a proper copy\n");
        return 2;
      }
    }
  }
}
