#ifndef _LOADER_LDEXPORT_H_
#define _LOADER_LDEXPORT_H_

#include <loader/memorymap.h>
#include <iosfwd>

namespace loader {


/* All data that the loader exports to the kernel. */
struct ldexport {
 public:
  memorymap physmem;

  std::string to_string() const;
};


} /* namespace loader */

#endif /* _LOADER_LDEXPORT_H_ */
