#include <loader/page.h>

namespace loader {

#if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
template class page_allocator<ilias::arch::i386>;
template class page_allocator<ilias::arch::amd64>;
#endif

} /* namespace loader */
