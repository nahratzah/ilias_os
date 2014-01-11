namespace util {
namespace cpp {


inline cpp_template::cpp_template(const cpp_template& o)
: base_name_(o.base_name_->clone())
{
  for (const auto& i : o.tmpl_args_) tmpl_args_.emplace_back(i->clone());
}

template<typename Iter> cpp_template::cpp_template(const cpp_name& name, Iter tmpl_args_begin, Iter tmpl_args_end)
: base_name_(name.clone()),
  tmpl_args_(tmpl_args_begin, tmpl_args_end)
{}

template<typename Arg> cpp_template::cpp_template(const cpp_name& name, const std::initializer_list<Arg>& tmpl_args)
: base_name_(name.clone()),
  tmpl_args_(tmpl_args.begin(), tmpl_args.end())
{}


}} /* namespace util::cpp */
