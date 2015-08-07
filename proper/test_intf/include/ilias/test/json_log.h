#ifndef _ILIAS_TEST_JSON_LOG_H_
#define _ILIAS_TEST_JSON_LOG_H_

#include <ilias/test/json.h>
#include <string>

namespace ilias {
namespace json {


using namespace std;

template<typename S>
class json_log {
 public:
  json_log(basic_json_ostream_array<S>&) noexcept;
  json_log(const json_log&) noexcept = default;

  template<typename C, typename T, typename... Args>
  void log(basic_string_ref<C, T>, Args&&...);
  template<typename C, typename T, typename A, typename... Args>
  void log(const basic_string<C, T, A>&, Args&&...);
  template<typename... Args>
  void log(const char*, Args&&...);
  template<typename... Args>
  void log(const wchar_t*, Args&&...);
  template<typename... Args>
  void log(const char16_t*, Args&&...);
  template<typename... Args>
  void log(const char32_t*, Args&&...);

 private:
  template<typename Out>
  static void lshift_each_(Out&&) noexcept;

  template<typename Out, typename Arg0, typename... Args>
  static void lshift_each_(Out&&, Arg0&&, Args&&...);

  basic_json_ostream_array<S>& impl_;
};


}} /* namespace ilias::json */

#include "json_log-inl.h"

#endif /* _ILIAS_TEST_JSON_LOG_H_ */
