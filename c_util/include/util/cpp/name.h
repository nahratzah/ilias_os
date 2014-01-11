#ifndef UTIL_CPP_NAME_H
#define UTIL_CPP_NAME_H

#include <memory>
#include <vector>
#include <util/c_string_ptr.h>
#include <util/string_piece.h>
#include <util/cpp/identifier.h>
#include <util/cpp/namespace.h>

namespace util {
namespace cpp {


class cpp_name
: public cpp_identifier
{
 private:
  std::unique_ptr<cpp_identifier> parent_;
  c_string_ptr name_;

 public:
  inline cpp_name(const cpp_identifier& parent_name, string_piece name);
  inline cpp_name(string_piece name);
  std::unique_ptr<cpp_name> clone() const;

 private:
  inline cpp_name(const cpp_name& o);

  c_chord get_printable() const override;
  cpp_name* clone_() const override;
};


}}  /* namespace util::cpp */

#include <util/cpp/name-inl.h>

#endif /* UTIL_CPP_NAME_H */
