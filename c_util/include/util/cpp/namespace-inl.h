#include <utility>

namespace util {
namespace cpp {


template<typename PathElem> cpp_namespace::cpp_namespace(std::initializer_list<PathElem> path_elem)
: path_(path_elem)
{
  return;
}

template<typename PathElem> cpp_namespace::cpp_namespace(std::initializer_list<PathElem>&& path_elem)
: path_(std::move(path_elem))
{
  return;
}


}} /* namespace util::cpp */
