#ifndef UTIL_CPP_TEMPLATE_H
#define UTIL_CPP_TEMPLATE_H

#include <util/cpp/identifier.h>
#include <util/cpp/name.h>

namespace util {
namespace cpp {


class cpp_template
: public cpp_identifier
{
 private:
  std::unique_ptr<cpp_name> base_name_;
  std::vector<std::unique_ptr<cpp_identifier>> tmpl_args_;

  inline cpp_template(const cpp_template& o);

 public:
  template<typename Iter> cpp_template(const cpp_name& name, Iter tmpl_args_begin, Iter tmpl_args_end);
  template<typename Arg> cpp_template(const cpp_name& name, const std::initializer_list<Arg>& tmpl_args);

  ~cpp_template() noexcept override;
  std::unique_ptr<cpp_template> clone() const;

 private:
  c_chord get_printable() const;
  cpp_template* clone_() const;
};


}} /* namespace util::cpp */

#include <util/cpp/template-inl.h>

#endif /* UTIL_CPP_TEMPLATE_H */
