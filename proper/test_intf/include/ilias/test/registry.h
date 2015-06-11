#ifndef _ILIAS_TEST_REGISTRY_H_
#define _ILIAS_TEST_REGISTRY_H_

#include <map>
#include <ostream>
#include <string>

namespace ilias {
namespace test {


using namespace std;

class registry;
class module;

ostream& operator<<(ostream&, const registry&);
ostream& operator<<(ostream&, const module&);


class registry final {
  friend module;
  friend ostream& operator<<(ostream&, const registry&);

 private:
  registry() noexcept;
  registry(const registry&) = delete;
  registry& operator=(const registry&) = delete;
  ~registry() = delete;

 public:
  static registry& singleton() noexcept;

 private:
  map<string_ref, module> modules_;
};

class module final {
  friend ostream& operator<<(ostream&, const module&);

 private:
  module() noexcept = default;
  module(const module&) = delete;
  module& operator=(const module&) = delete;
  ~module() = delete;

 public:
  module& instance(string_ref);
};


}} /* namespace ilias::test */

#endif /* _ILIAS_TEST_REGISTRY_H_ */
