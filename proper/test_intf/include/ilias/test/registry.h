#ifndef _ILIAS_TEST_REGISTRY_H_
#define _ILIAS_TEST_REGISTRY_H_

#include <map>
#include <ostream>
#include <string>
#include <ilias/test/json.h>

namespace ilias {
namespace test {


using namespace std;

class registry;
class module;


template<typename S>
void operator<<(json_ostream_object<S>&, const registry&);
template<typename S>
void operator<<(json_ostream_object<S>&, const module&);


class module final {
  template<typename S>
  friend void operator<<(json_ostream_object<S>&, const module&);

 public:
  module() noexcept = default;
  module(const module&) = default;
  module& operator=(const module&) = default;

 public:
  ~module() noexcept = default;

 public:
  module& instance(string_ref);
};

class registry final {
  friend module;

  template<typename S>
  friend void operator<<(json_ostream_object<S>&, const registry&);

 private:
  registry() noexcept;
  registry(const registry&) = delete;
  registry& operator=(const registry&) = delete;
  ~registry() = delete;

 public:
  static registry& singleton() noexcept;

 private:
  std::map<string_ref, module> modules_;
};


template<typename C, typename T>
basic_ostream<C, T>& operator<<(basic_ostream<C, T>&, const registry&);
template<typename C, typename T>
basic_ostream<C, T>& operator<<(basic_ostream<C, T>&, const module&);


}} /* namespace ilias::test */

#include "registry-inl.h"

#endif /* _ILIAS_TEST_REGISTRY_H_ */
