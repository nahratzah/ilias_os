#if _ENABLE_TEST_

#include "module.h"
#include <array>
#include <algorithm>
#include <ostream>
#include <vector>
#include <ilias/test/json_log.h>
#include <ilias/test/case.h>

using ilias::json::json_log;
using ilias::test::testcase_result;
using ilias::test::testcase_functor;

auto all_of_all = testcase_functor(
    "std::all_of -- all",
    [](json_log<std::wostream> l) {
      using std::all_of;
      using std::array;

      const array<int, 8> int_data{{ 0, 1, 2, 3, 4, 5, 6, 7 }};
      l.log("using int_data", int_data);

      bool lt_eight = all_of(int_data.begin(), int_data.end(),
                             [](int x) { return x < 8; });
      l.log("all_of(int_data.begin(), int_data.end(), "
            "[](int x) { return x < 8; })", lt_eight);
      if (!lt_eight) return testcase_result::FAILED;

      return testcase_result::SUCCESS;
    });

auto all_of_some = testcase_functor(
    "std::all_of -- some",
    [](json_log<std::wostream> l) {
      using std::all_of;
      using std::array;

      const array<int, 8> int_data{{ 0, 1, 2, 3, 4, 5, 6, 7 }};
      l.log("using int_data", int_data);

      bool lt_four = all_of(int_data.begin(), int_data.end(),
                            [](int x) { return x < 4; });
      l.log("all_of(int_data.begin(), int_data.end(), "
            "[](int x) { return x < 4; })", lt_four);
      if (lt_four) return testcase_result::FAILED;

      return testcase_result::SUCCESS;
    });

auto all_of_none = testcase_functor(
    "std::all_of -- none",
    [](json_log<std::wostream> l) {
      using std::all_of;
      using std::array;

      const array<int, 8> int_data{{ 0, 1, 2, 3, 4, 5, 6, 7 }};
      l.log("using int_data", int_data);

      bool lt_zero = all_of(int_data.begin(), int_data.end(),
                            [](int x) { return x < 0; });
      l.log("all_of(int_data.begin(), int_data.end(), "
            "[](int x) { return x < 0; })", lt_zero);
      if (lt_zero) return testcase_result::FAILED;

      return testcase_result::SUCCESS;
    });


auto any_of_all = testcase_functor(
    "std::any_of -- all",
    [](json_log<std::wostream> l) {
      using std::any_of;
      using std::array;

      const array<int, 8> int_data{{ 0, 1, 2, 3, 4, 5, 6, 7 }};
      l.log("using int_data", int_data);

      bool lt_eight = any_of(int_data.begin(), int_data.end(),
                             [](int x) { return x < 8; });
      l.log("any_of(int_data.begin(), int_data.end(), "
            "[](int x) { return x < 8; })", lt_eight);
      if (!lt_eight) return testcase_result::FAILED;

      return testcase_result::SUCCESS;
    });

auto any_of_some = testcase_functor(
    "std::any_of -- some",
    [](json_log<std::wostream> l) {
      using std::any_of;
      using std::array;

      const array<int, 8> int_data{{ 0, 1, 2, 3, 4, 5, 6, 7 }};
      l.log("using int_data", int_data);

      bool lt_four = any_of(int_data.begin(), int_data.end(),
                            [](int x) { return x < 4; });
      l.log("any_of(int_data.begin(), int_data.end(), "
            "[](int x) { return x < 4; })", lt_four);
      if (!lt_four) return testcase_result::FAILED;

      return testcase_result::SUCCESS;
    });

auto any_of_none = testcase_functor(
    "std::any_of -- none",
    [](json_log<std::wostream> l) {
      using std::any_of;
      using std::array;

      const array<int, 8> int_data{{ 0, 1, 2, 3, 4, 5, 6, 7 }};
      l.log("using int_data", int_data);

      bool lt_zero = any_of(int_data.begin(), int_data.end(),
                            [](int x) { return x < 0; });
      l.log("any_of(int_data.begin(), int_data.end(), "
            "[](int x) { return x < 0; })", lt_zero);
      if (lt_zero) return testcase_result::FAILED;

      return testcase_result::SUCCESS;
    });


auto none_of_all = testcase_functor(
    "std::none_of -- all",
    [](json_log<std::wostream> l) {
      using std::none_of;
      using std::array;

      const array<int, 8> int_data{{ 0, 1, 2, 3, 4, 5, 6, 7 }};
      l.log("using int_data", int_data);

      bool lt_eight = none_of(int_data.begin(), int_data.end(),
                             [](int x) { return x < 8; });
      l.log("none_of(int_data.begin(), int_data.end(), "
            "[](int x) { return x < 8; })", lt_eight);
      if (lt_eight) return testcase_result::FAILED;

      return testcase_result::SUCCESS;
    });

auto none_of_some = testcase_functor(
    "std::none_of -- some",
    [](json_log<std::wostream> l) {
      using std::none_of;
      using std::array;

      const array<int, 8> int_data{{ 0, 1, 2, 3, 4, 5, 6, 7 }};
      l.log("using int_data", int_data);

      bool lt_four = none_of(int_data.begin(), int_data.end(),
                            [](int x) { return x < 4; });
      l.log("none_of(int_data.begin(), int_data.end(), "
            "[](int x) { return x < 4; })", lt_four);
      if (lt_four) return testcase_result::FAILED;

      return testcase_result::SUCCESS;
    });

auto none_of_none = testcase_functor(
    "std::none_of -- none",
    [](json_log<std::wostream> l) {
      using std::none_of;
      using std::array;

      const array<int, 8> int_data{{ 0, 1, 2, 3, 4, 5, 6, 7 }};
      l.log("using int_data", int_data);

      bool lt_zero = none_of(int_data.begin(), int_data.end(),
                            [](int x) { return x < 0; });
      l.log("none_of(int_data.begin(), int_data.end(), "
            "[](int x) { return x < 0; })", lt_zero);
      if (!lt_zero) return testcase_result::FAILED;

      return testcase_result::SUCCESS;
    });


auto for_each_check_functor = testcase_functor(
    "std::for_each -- check functor",
    [](json_log<std::wostream> l) {
      using std::for_each;
      using std::array;
      using std::vector;
      using std::equal;

      struct visitor {
        void operator()(int x) {
          out.push_back(x);
        }

        vector<int> out;
      };

      const array<int, 3> int_data{{ 0, 1, 2 }};
      l.log("using int_data", int_data);
      auto result = for_each(int_data.begin(), int_data.end(), visitor());
      l.log("result", result.out);

      if (!equal(result.out.begin(), result.out.end(),
                 int_data.begin(), int_data.end()))
        return testcase_result::FAILED;
      return testcase_result::SUCCESS;
    });

auto for_each_check_modification = testcase_functor(
    "std::for_each -- check modification",
    [](json_log<std::wostream> l) {
      using std::for_each;
      using std::array;
      using std::vector;
      using std::equal;

      struct visitor {
        void operator()(int& x) {
          x *= x;
        }
      };

      array<int, 3> int_data{{ 0, 1, 2 }};
      const array<int, 3> expect{{ 0 * 0, 1 * 1, 2 * 2 }};
      l.log("using int_data", int_data);
      for_each(int_data.begin(), int_data.end(), visitor());
      l.log("int_data, expect", int_data, expect);

      if (int_data != expect) return testcase_result::FAILED;
      return testcase_result::SUCCESS;
    });

#endif /* _ENABLE_TEST_ */
