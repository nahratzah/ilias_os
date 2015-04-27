#include <cmath>
#include "private.h"

using namespace _namespace(std);

long double lgammal(long double x) noexcept {
  return lgammal_r(x, &signgam);
}
