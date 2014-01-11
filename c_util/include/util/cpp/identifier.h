#ifndef UTIL_CPP_IDENTIFIER_H
#define UTIL_CPP_IDENTIFIER_H

#include <util/c_string_ptr.h>
#include <memory>

namespace util {
namespace cpp {


class cpp_identifier {
 protected:
  cpp_identifier() = default;

 public:
  virtual ~cpp_identifier() noexcept;
  std::unique_ptr<cpp_identifier> clone() const;

 private:
  virtual c_chord get_printable() const = 0;
  virtual cpp_identifier* clone_() const = 0;

  friend c_string_ptr printable(const cpp_identifier&);
  friend c_chord printable_chord(const cpp_identifier&);
};


}} /* namespace util::cpp */

#endif /* UTIL_CPP_IDENTIFIER_H */
