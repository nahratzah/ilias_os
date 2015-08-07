#ifndef _ILIAS_TEST_REGISTRY_INL_H_
#define _ILIAS_TEST_REGISTRY_INL_H_

namespace ilias {
namespace test {


inline module::module(module&& m) noexcept
: tests_(move(m.tests_))
{}


template<typename S>
auto operator<<(json_ostream_object<S>& out, const registry& r) -> void {
  for (const auto& m : r.modules_)
    out << json_key(m.first) << m.second;
}

template<typename S>
auto operator<<(json_ostream_object<S>& out, const module& m) -> void {
  for (const auto& test : m.tests_)
    out << json_key(test.name) << test;
}


template<typename C, typename T>
auto operator<<(basic_ostream<C, T>& out, const registry& r) ->
    basic_ostream<C, T>& {
  return out << begin_json << r << end_json;
}

template<typename C, typename T>
auto operator<<(basic_ostream<C, T>& out, const module& m) ->
    basic_ostream<C, T>& {
  return out << begin_json << m << end_json;
}


}} /* namespsace ilias::test */

#endif /* _ILIAS_TEST_REGISTRY_INL_H_ */
