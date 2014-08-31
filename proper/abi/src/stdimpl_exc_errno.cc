#include <stdimpl/exc_errno.h>
#include <system_error>
#include <stdexcept>
#include <new>
#include <cerrno>
#include <exception>
#include <cassert>

_namespace_begin(std)
namespace impl {


pair<bool, int> errno_from_current_exception(bool throw_unrecognized) {
  if (!uncaught_exception()) return { false, 0 };

  try {
    throw;
  } catch (const bad_alloc&) {
    return { true, ENOMEM };
  } catch (const invalid_argument&) {
    return { true, EINVAL };
  } catch (const system_error& err) {
    const error_code& errc = err.code();
    if (errc.category() == system_category()) {
      errno = errc.value();
      return { true, errc.value() };
    }
  } catch (...) {
    if (throw_unrecognized) throw;
    return { false, 0 };
  }

  __builtin_unreachable();
}

bool errno_catch_handler(bool throw_unrecognized) {
  assert_msg(uncaught_exception(),
             "errno_catch_handler: "
             "must be called from within a catch handler.");

  pair<bool, int> ec = errno_from_current_exception(throw_unrecognized);
  if (ec.first) errno = ec.second;
  return ec.first;
}


} /* namespace std::impl */
_namespace_end(std)
