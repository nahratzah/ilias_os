#ifndef _LOADER_LDEXPORT_H_
#define _LOADER_LDEXPORT_H_

#include <loader/memorymap.h>
#include <abi/ext/relptr.h>
#include <string>

namespace loader {


/* All data that the loader exports to the kernel. */
struct ldexport {
 public:
  memorymap physmem;
  std::basic_string<char, std::char_traits<char>,
                    abi::ext::relptr_allocator<char>> cmd_line;

  std::string to_string() const;
};


} /* namespace loader */

#endif /* _LOADER_LDEXPORT_H_ */
