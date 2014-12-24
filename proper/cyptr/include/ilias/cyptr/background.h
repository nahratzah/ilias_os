#ifndef _ILIAS_CYPTR_BACKGROUND_H_
#define _ILIAS_CYPTR_BACKGROUND_H_

#include <cstddef>

namespace ilias {
namespace cyptr {
namespace background_processing {


void enable();
void disable() noexcept;
bool is_enabled() noexcept;
std::size_t process(bool = true, std::size_t = 0) noexcept;


}}} /* namespace ilias::cyptr::background_processing */

#endif /* _ILIAS_CYPTR_BACKGROUND_H_ */
