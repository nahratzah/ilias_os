namespace util {
namespace cpp {


inline cpp_name::cpp_name(const cpp_identifier& parent_name, c_string_piece name)
: parent_(parent_name.clone()),
  name_(name)
{}

inline cpp_name::cpp_name(c_string_piece name) : name_(name) {}

inline cpp_name::cpp_name(const cpp_name& o)
: parent_((o.parent_ ? o.parent_->clone() : nullptr)),
  name_(o.name_)
{}


}} /* namespace util::cpp */
