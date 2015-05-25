#ifndef _ILIAS_ELF_ELF_ERROR_H_
#define _ILIAS_ELF_ELF_ERROR_H_

#include <stdexcept>
#include <iosfwd>

namespace ilias {
namespace elf {


using namespace std;

class elf_error
: public runtime_error
{
 public:
  elf_error();
  elf_error(const string&);
  elf_error(string_ref);
  elf_error(const char*);
  ~elf_error() noexcept override;

  void __throw() __attribute__((__noreturn__));
  void __throw(const string&) __attribute__((__noreturn__));
  void __throw(string_ref) __attribute__((__noreturn__));
  void __throw(const char*) __attribute__((__noreturn__));
};


}} /* namespace ilias::elf */

#endif /* _ILIAS_ELF_ELF_ERROR_H_ */
