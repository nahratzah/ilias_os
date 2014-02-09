#include <cstring>

using namespace std;

constexpr int N = 256;

int main() {
  char buf[N];
  char expect[N];

  for (int i = 0; i < N; ++i) {
    ::test_std::memset(&buf[i], i, N - i);
    expect[i] = i;
  }

  if (memcmp(buf, expect, N) != 0) return 1;
}
