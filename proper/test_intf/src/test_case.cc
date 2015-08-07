#include <ilias/test/case.h>

namespace ilias {
namespace test {


auto to_string(testcase_result r) noexcept -> string_ref {
  static const string_ref success = "success";
  static const string_ref failed = "failed";
  static const string_ref skipped = "skipped";
  static const string_ref except = "except";

  switch (r) {
  case testcase_result::SUCCESS:
    return success;
  case testcase_result::FAILED:
    return failed;
  case testcase_result::SKIPPED:
    return skipped;
  case testcase_result::EXCEPT:
    return except;
  }
}


testcase::~testcase() noexcept {}


}} /* namespace ilias::test */
