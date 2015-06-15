#ifndef _ILIAS_TEST_REGISTRY_INL_H_
#define _ILIAS_TEST_REGISTRY_INL_H_

namespace ilias {
namespace test {


template<typename S>
auto operator<<(json_ostream_object<S>& out, const registry& r) -> void {
  for (auto m : r.modules_)
    out << json_key(m.first) << m.second;
}

template<typename S>
auto operator<<(json_ostream_object<S>& out, const module& m) -> void {
  out << json_key("addr") << reinterpret_cast<uintptr_t>(&m);
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
