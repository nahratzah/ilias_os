#ifndef _ILIAS_TEST_JSON_LOG_INL_H_
#define _ILIAS_TEST_JSON_LOG_INL_H_

#include "json_log.h"
#include <utility>

namespace ilias {
namespace json {


template<typename S>
json_log<S>::json_log(basic_json_ostream_array<S>& impl) noexcept
: impl_(impl)
{}

template<typename S>
template<typename C, typename T, typename... Args>
auto json_log<S>::log(basic_string_ref<C, T> msg, Args&&... data) -> void {
  using std::forward;

  auto obj = impl_ << begin_json_object << json_key("msg") << msg;
  if (sizeof...(Args) != 0) {
    auto arr = obj << json_key("data") << begin_json_array;
    lshift_each_(arr, forward<Args>(data)...);
    move(arr) << end_json_array;
  }
  move(obj) << end_json_object;
}

template<typename S>
template<typename Out>
auto json_log<S>::lshift_each_(Out&&) noexcept -> void {}

template<typename S>
template<typename Out, typename Arg0, typename... Args>
auto json_log<S>::lshift_each_(Out&& out, Arg0&& arg0, Args&&... args) ->
    void {
  using std::forward;

  lshift_each_(forward<Out>(out) << forward<Arg0>(arg0),
               forward<Args>(args)...);
}


}} /* namespace ilias::test */

#endif /* _ILIAS_TEST_JSON_LOG_INL_H_ */
