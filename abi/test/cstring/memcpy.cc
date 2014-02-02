#include <cstring>
#include <cstdio>

using namespace std;

constexpr size_t OFF = 16;
constexpr int N = 26;
constexpr char BASE = 'a';

int main() {
  char src[N + OFF + 1];
  char dst[N + OFF + 1];

  for (size_t src_off = 0; src_off < OFF; ++src_off) {
    for (int i = 0; i < N; ++i) src[src_off + i] = BASE + i;
    src[src_off + N] = '\0';

    for (size_t dst_off = 0; dst_off < OFF; ++dst_off) {
      if (dst_off > 0) dst[dst_off - 1] = 'X';
      dst[dst_off + N] = 'X';
      void* rv = ::test_std::memcpy(&dst[dst_off], &src[src_off], N);
      void* expect_rv = &dst[dst_off];

      fprintf(stderr, "memcpy(dst, src, %zu):\n"
                      "\tdst =    %.*s\n"
                      "\texpected %.*s\n",
              size_t(N),
              int(N), &dst[dst_off],
              int(N), &src[src_off]);
      fprintf(stderr, "  returned %p, expected %p\n", rv, expect_rv);
      if (memcmp(&src[src_off], &dst[dst_off], N) != 0) return 1;
      if (dst_off > 0 && dst[dst_off - 1] != 'X') {
        fprintf(stderr, "memcpy overwrote preceding byte");
        return 2;
      }
      if (dst[dst_off + N] != 'X') {
        fprintf(stderr, "memcpy overwrote past last byte");
        return 3;
      }
      if (rv != expect_rv) return 4;
    }
  }
}
