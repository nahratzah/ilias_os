#include <cstring>
#include <cstdio>

int main() {
  const char* abba = "abba";
  if (::test_std::memrchr(abba, 'a', strlen(abba)) != &abba[3]) return 1;
  if (::test_std::memrchr(abba, 'b', strlen(abba)) != &abba[2]) return 2;
  if (::test_std::memrchr(abba, 'b', strlen(abba) - 2) != &abba[1]) return 3;
}
