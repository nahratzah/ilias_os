#include <util/cpp/function.h>

namespace util {
namespace cpp {


cpp_function::~cpp_function() noexcept {
  return;
}

std::unique_ptr<cpp_function> cpp_function::clone() const {
  return std::unique_ptr<cpp_function>(clone_());
}

cpp_function* cpp_function::clone_() const {
  return new cpp_function(*this);
}

c_chord cpp_function::get_printable() const {
  c_chord args_str;
  bool empty = true;
  for (const auto& i : args_) {
    if (empty)
      empty = false;
    else
      args_str += ", ";
    args_str += printable_chord(*i);
  }
  return chord_cat(printable_chord(*name_), "(", args_str, ")", " -> ", printable_chord(*result_));
}


}} /* namespace util::cpp */
