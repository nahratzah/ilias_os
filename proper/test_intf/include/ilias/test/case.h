#ifndef _ILIAS_TEST_CASE_H_
#define _ILIAS_TEST_CASE_H_

#include <string>
#include <ilias/linked_forward_list.h>
#include <ilias/test/json.h>
#include <ilias/test/json_log.h>
#include <iosfwd>

namespace ilias {
namespace test {


using namespace ::ilias::json;


enum class testcase_result {
  SUCCESS,
  FAILED,
  SKIPPED,
  EXCEPT,
};

std::string_ref to_string(testcase_result) noexcept;

template<typename S>
auto operator<<(json_ostream<S>&&, testcase_result) ->
    decltype(declval<json_ostream<S>>()
             <<
             to_string(declval<testcase_result>()));


class testcase
: public linked_forward_list_element<>
{
 protected:
  testcase(string_ref) noexcept;
  testcase() = delete;
  testcase(const testcase&) = delete;
  testcase(testcase&&) noexcept;
  testcase& operator=(const testcase&) = delete;

 public:
  virtual ~testcase() noexcept;

  virtual testcase_result run(json_log<wostream>) const = 0;

  const string_ref name;
};

template<typename T>
class testcase_functor_t
: public testcase
{
 public:
  template<typename... Args> explicit testcase_functor_t(string_ref,
                                                         Args&&...);
  testcase_functor_t(testcase_functor_t&&);
  ~testcase_functor_t() noexcept override;

  testcase_result run(json_log<wostream>) const override;

 private:
  T impl_;
};

template<typename S>
void operator<<(json_ostream_object<S>&, const testcase&);

template<typename C, typename T>
basic_ostream<C, T>& operator<<(basic_ostream<C, T>&, const testcase&);


template<typename F>
auto testcase_functor(string_ref, F&& f) ->
    testcase_functor_t<remove_const_t<remove_reference_t<F>>>;
template<typename F, typename... Args>
auto testcase_functor(string_ref, Args&&... args) ->
    testcase_functor_t<F>;


}} /* namespace ilias::test */

#include "case-inl.h"

#endif /* _ILIAS_TEST_CASE_H_ */
