#include <abi/dynamic_cast.h>
#include <cstdio>

class base {
 public:
  virtual ~base() noexcept;
};

class derived
: public base {
 public:
  virtual ~derived() noexcept;
};

base::~base() noexcept {}
derived::~derived() noexcept {}


int main() {
  derived d;
  base& b = d;
  const auto& typeid_base =
      static_cast<const abi::__class_type_info&>(typeid(base));
  const auto& typeid_derived =
      static_cast<const abi::__class_type_info&>(typeid(derived));
  derived* d_ = static_cast<derived*>(abi::__dynamic_cast(&b, &typeid_base,
                                                          &typeid_derived, 0));
  fprintf(stderr, "dynamic_cast (hint = 0) returned %p, expected %p\n",
          d_, &d);

  if (&d != d_) return 1;
  d_ = static_cast<derived*>(abi::__dynamic_cast(&b, &typeid_base,
                                                 &typeid_derived, -1));
  fprintf(stderr, "dynamic_cast (hint = -1) returned %p, expected %p\n",
          d_, &d);
  if (&d != d_) return 1;
}
