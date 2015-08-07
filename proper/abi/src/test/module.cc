#if _ENABLE_TEST_

#include "module.h"

ilias::test::module& abi_stl_module() {
  return ilias::test::module::instance("abi/stl");
}

#endif /* _ENABLE_TEST_ */
