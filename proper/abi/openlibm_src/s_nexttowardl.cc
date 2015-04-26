#include <cmath>

using namespace _namespace(std);

long double nexttowardl(long double x, long double y) noexcept {
  return nextafterl(x, y);
}
