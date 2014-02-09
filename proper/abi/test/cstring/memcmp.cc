#include <cstring>
#include <cstdio>

constexpr int N = 16;
char a[N+1] = "abcdefghijklmnop";
char b[N+1] = "abcdefghijklmnop";

int main() {
  for (int x = 0; x < N; ++x) {
    for (int y = 0; y < N; ++y) {
      int len = N - x;
      if (N - y < len) len = N - y;
      const int rv = ::test_std::memcmp(&a[x], &b[y], len);

      if (rv != x - y) {
        fprintf(stderr,
                "Fail:\n  (%10p) %.*s\n  (%10p) %.*s\n"
                "memcmp(...) = %d, expected %d\n",
                &a[x], len, &a[x], &b[y], len, &b[y], rv, int(x - y));
        return 1;
      }
    }
  }

  int rv = ::test_std::memcmp("abcd", "abce", 4);
  fprintf(stderr, "memcmp(\"abcd\", \"abce\", 4) = %d, expect %d\n",
          rv, int(-1));
  if (rv != -1) return 1;
}
