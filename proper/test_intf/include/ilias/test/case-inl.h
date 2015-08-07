#ifndef _ILIAS_TEST_CASE_INL_H_
#define _ILIAS_TEST_CASE_INL_H_

#include "case.h"

namespace ilias {
namespace test {


template<typename S>
auto operator<<(json_ostream<S>&& out, testcase_result r) ->
    decltype(declval<json_ostream<S>>()
             <<
             to_string(declval<testcase_result>())) {
  return out << to_string(r);
}


inline testcase::testcase(string_ref name) noexcept
: name(name)
{}


template<typename T>
template<typename... Args>
testcase_functor<T>::testcase_functor(string_ref name, Args&&... args)
: testcase(name),
  impl_(forward<Args>(args)...)
{}

template<typename T>
testcase_functor<T>::~testcase_functor() noexcept {}

template<typename T>
auto testcase_functor<T>::run(json_log<wostream> log) const ->
    testcase_result {
  return impl_(log);
}


template<typename S>
auto operator<<(json_ostream_object<S>& obj, const testcase& tc) -> void {
  testcase_result result;

  obj << json_key("name") << tc.name;
  try {
    auto arr = obj << json_key("log") << begin_json_array;
    result = tc.run(json_log<wostream>(arr));
    move(arr) << end_json_array;
  } catch (...) {
    result = testcase_result::EXCEPT;
  }
  obj << json_key("result") << result;
}

template<typename C, typename T>
auto operator<<(basic_ostream<C, T>& out, const testcase& tc) ->
    basic_ostream<C, T>& {
  return out << begin_json << tc << end_json;
}


}} /* namespace ilias::test */

#endif /* _ILIAS_TEST_CASE_INL_H_ */
