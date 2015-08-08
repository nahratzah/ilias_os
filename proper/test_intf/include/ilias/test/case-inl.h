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
  return move(out) << to_string(r);
}


inline testcase::testcase(string_ref name) noexcept
: name(name)
{}

inline testcase::testcase(testcase&& other) noexcept
: name(std::move(other.name))
{}


template<typename T>
template<typename... Args>
testcase_functor_t<T>::testcase_functor_t(string_ref name, Args&&... args)
: testcase(name),
  impl_(forward<Args>(args)...)
{}

template<typename T>
testcase_functor_t<T>::testcase_functor_t(testcase_functor_t&& other)
: testcase(move(other)),
  impl_(move(other.impl_))
{}

template<typename T>
testcase_functor_t<T>::~testcase_functor_t() noexcept {}

template<typename T>
auto testcase_functor_t<T>::run(json_log<wostream> log) const ->
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


template<typename F>
auto testcase_functor(string_ref name, F&& f) ->
    testcase_functor_t<remove_const_t<remove_reference_t<F>>> {
  using result_type =
      testcase_functor_t<remove_const_t<remove_reference_t<F>>>;

  return result_type(name, std::forward<F>(f));
}

template<typename F, typename... Args>
auto testcase_functor(string_ref name, Args&&... args) ->
    testcase_functor_t<F> {
  using result_type =
      testcase_functor_t<F>;

  return result_type(name, std::forward<Args>(args)...);
}


}} /* namespace ilias::test */

#endif /* _ILIAS_TEST_CASE_INL_H_ */
