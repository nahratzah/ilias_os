#include <typeinfo>

namespace std {


/*
 * type_info has one pointer to its vtable,
 * plus the one pointer type_info::__type_name.
 */
static_assert(sizeof(type_info) == 2 * sizeof(void*),
              "std::type_info size mismatch");

type_info::~type_info() noexcept {}


bad_cast::~bad_cast() noexcept {}

const char* bad_cast::what() const noexcept {
  return "std::bad_cast";
}


bad_typeid::~bad_typeid() noexcept {}

const char* bad_typeid::what() const noexcept {
  return "std::bad_typeid";
}


bool type_info::__is_pointer_p() const noexcept {
  return false;
}

bool type_info::__is_function_p() const noexcept {
  return false;
}


} /* namespace std */