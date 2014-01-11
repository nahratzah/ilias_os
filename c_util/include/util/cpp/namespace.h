#ifndef UTIL_CPP_NAMESPACE_H
#define UTIL_CPP_NAMESPACE_H

#include <vector>
#include <util/c_string_ptr.h>
#include <util/string_piece.h>
#include <util/cpp/identifier.h>

namespace util {
namespace cpp {


class cpp_namespace
: public cpp_identifier
{
 private:
  std::vector<c_string_ptr> path_;

 public:
  cpp_namespace() = default;
  template<typename PathElem> cpp_namespace(std::initializer_list<PathElem> path);
  template<typename PathElem> cpp_namespace(std::initializer_list<PathElem>&& path);
  ~cpp_namespace() noexcept override;

  cpp_namespace operator+(c_string_ptr&&) const;
  cpp_namespace operator+(const c_string_ptr&) const;
  cpp_namespace operator+(c_string_piece&) const;

  std::unique_ptr<cpp_namespace> clone() const;

 private:
  c_chord get_printable() const override;
  cpp_namespace* clone_() const override;
};


}} /* namespace util::cpp */

#include <util/cpp/namespace-inl.h>

#endif /* UTIL_CPP_NAMESPACE_H */
