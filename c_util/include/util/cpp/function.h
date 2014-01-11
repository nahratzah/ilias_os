#ifndef UTIL_CPP_FUNCTION_H
#define UTIL_CPP_FUNCTION_H

#include <util/cpp/identifier.h>
#include <util/string_piece.h>

namespace util {
namespace cpp {


class cpp_function
: public cpp_identifier
{
 private:
  std::unique_ptr<cpp_identifier> name_;
  std::unique_ptr<cpp_identifier> result_;
  std::vector<std::unique_ptr<cpp_identifier>> args_;

  inline cpp_function(const cpp_function& o);

 public:
  cpp_function(const cpp_identifier& name, const cpp_identifier& result);
  template<typename Iter> cpp_function(const cpp_identifier& name, const cpp_identifier& result, Iter args_begin, Iter args_end);
  template<typename Arg> cpp_function(const cpp_identifier& name, const cpp_identifier& result, const std::initializer_list<Arg>& args);
  ~cpp_function() noexcept override;

  std::unique_ptr<cpp_function> clone() const;

 private:
  c_chord get_printable() const override;
  cpp_function* clone_() const override;
};


}} /* namespace util::cpp */

#include <util/cpp/function-inl.h>

#endif /* UTIL_CPP_FUNCTION_H */
