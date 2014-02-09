#include <util/cpp/template.h>

namespace util {
namespace cpp {


cpp_template::~cpp_template() noexcept {
  return;
}

c_chord cpp_template::get_printable() const {
  c_chord rv = printable_chord(*base_name_);
  rv += "<";
  bool empty = true;
  for (const auto& i : tmpl_args_) {
    if (empty)
      empty = false;
    else
      rv += ", ";
    rv += printable_chord(*i);
  }
  rv += ">";
  return rv;
}

cpp_template* cpp_template::clone_() const {
  return new cpp_template(*this);
}


}} /* namespace util::cpp */
