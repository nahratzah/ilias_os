namespace util {
namespace cpp {


inline cpp_function::cpp_function(const cpp_function& o)
: name_(o.name_->clone()),
  result_(o.result_->clone())
{
  for (const auto& i : o.args_)
    args_.emplace_back(i->clone());
}

inline cpp_function::cpp_function(const cpp_identifier& name, const cpp_identifier& result)
: name_(name.clone()),
  result_(result.clone())
{}

template<typename Iter> cpp_function::cpp_function(const cpp_identifier& name, const cpp_identifier& result, Iter args_begin, Iter args_end)
: name_(name.clone()),
  result_(result.clone()),
  args_(args_begin, args_end)
{}

template<typename Arg> cpp_function::cpp_function(const cpp_identifier& name, const cpp_identifier& result, const std::initializer_list<Arg>& args)
: cpp_function(name, result, args.begin(), args.end())
{}


}} /* namespace util::cpp */
