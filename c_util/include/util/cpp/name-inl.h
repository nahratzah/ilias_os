namespace util {
namespace cpp {


inline cpp_name::cpp_name(const cpp_identifier& parent_name, string_piece name)
: parent_(parent_name.clone()),
  name_(name)
{}

inline cpp_name::cpp_name(string_piece name) : name_(name) {}

inline cpp_name::cpp_name(const cpp_name& o)
: parent_(parent_->clone()),
  name_(name)
{}


}} /* namespace util::cpp */
