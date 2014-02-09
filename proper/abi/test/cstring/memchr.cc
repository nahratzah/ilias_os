#include <cstring>
#include <cstdio>

constexpr int N = 26;
constexpr char BASE = 'a';

int main() {
  char alphabet[N + 1];
  for (int i = 0; i < N; ++i) alphabet[i] = BASE + i;
  alphabet[N] = '\0';

  for (int c = 0; c < N; ++c) {
    for (int off = 0; off < N; ++off) {
      void* rv = ::test_std::memchr(&alphabet[off], BASE + c, N - off);

      void* expect;
      if (off > c)
        expect = nullptr;
      else
        expect = &alphabet[c];

      fprintf(stderr, "%c found at %p, expected %p\n",
              int(BASE + c), rv, expect);
      if (rv != expect) return 1;
    }
  }
}
