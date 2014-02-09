#include <cstring>
#include <cstdio>

constexpr int N = 26;
constexpr char BASE = 'a';
constexpr char ALT_BASE = 'A';

int main() {
  char d[2 * N];
  char s[2 * N];

  for (int i = 0; i < N; ++i) s[i] = s[i + N] = BASE + i;

  for (int src_off = 0; src_off < N; ++src_off) {
    for (int dst_off = 0; dst_off < N; ++dst_off) {
      /* Reset. */
      for (int i = 0; i < N; ++i) d[i] = d[i + N] = BASE + i;
      fprintf(stderr, "Testing src_off = %2d, dst_off = %2d...",
              src_off, dst_off);

      void* rv = ::test_std::memmove(&d[dst_off], &d[src_off], N);

      void* expect_rv = &d[dst_off];
      if (rv != expect_rv) {
        fprintf(stderr, "\nmemmove returned %p, expected %p\n",
                rv, expect_rv);
        return 1;
      }

      /* Bytes before dst dst_off may not be written. */
      for (int i = 0; i < dst_off; ++i) {
        if (d[i] != BASE + i) {
          fprintf(stderr, "\nWrite to preceding byte noticed.\n");
          return 2;
        }
      }

      /* Bytes after dst dst_off+N may not be written. */
      for (int i = dst_off; i < N; ++i) {
        if (d[N + i] != BASE + i) {
          fprintf(stderr, "\nWrite to succeding byte noticed.\n");
          return 3;
        }
      }

      if (memcmp(&d[dst_off], &s[src_off], N) != 0) {
        fprintf(stderr, "\nWrote    %.*s\n"
                          "Expected %.*s\n",
                int(N), &d[dst_off],
                int(N), &s[src_off]);
        return 4;
      }

      fprintf(stderr, "  %s\n", "\\o/");
    }
  }
}
