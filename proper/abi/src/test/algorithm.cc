#if _ENABLE_TEST_

#include "module.h"
#include <array>
#include <algorithm>
#include <ostream>
#include <ilias/test/json_log.h>
#include <ilias/test/case.h>

using ilias::json::json_log;
using ilias::test::testcase_result;
using ilias::test::testcase_functor;

auto all_of = testcase_functor("std::all_of", [](json_log<std::wostream> l) {
                using std::all_of;
                using std::array;

                const array<int, 8> int_data{{ 0, 1, 2, 3, 4, 5, 6, 7 }};
                l.log("using int_data", int_data);

                bool lt_eight = all_of(int_data.begin(), int_data.end(),
                                       [](int x) { return x < 8; });
                l.log("all_of(int_data.begin(), int_data.end(), "
                      "[](int x) { return x < 8; })", lt_eight);
                if (!lt_eight) return testcase_result::FAILED;

                bool lt_four = all_of(int_data.begin(), int_data.end(),
                                      [](int x) { return x < 4; });
                l.log("all_of(int_data.begin(), int_data.end(), "
                      "[](int x) { return x < 4; })", lt_four);
                if (lt_four) return testcase_result::FAILED;

                return testcase_result::SUCCESS;
              });

#endif /* _ENABLE_TEST_ */
