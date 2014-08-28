#include <stdimpl/circular_buffer.h>

_namespace_begin(std)
namespace impl {


template class circular_buffer<void*>;

template bool operator==(const circular_buffer<void*>&,
                         const circular_buffer<void*>&);
template bool operator<(const circular_buffer<void*>&,
                        const circular_buffer<void*>&);


} /* namespace std::impl */
_namespace_end(ilias)
