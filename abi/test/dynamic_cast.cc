#include <abi/dynamic_cast.h>
#include <cstdio>

class base {
 public:
  virtual ~base() noexcept;

  base* get_base() noexcept { return this; }
};

class derived
: public base {
 public:
  virtual ~derived() noexcept;
};

class virtual_derived
: public virtual base {
 public:
  ~virtual_derived() noexcept;
};

class protected_derived
: protected base {
 public:
  ~protected_derived() noexcept;

  using base::get_base;  // Expose.
};

class private_derived
: private base {
 public:
  ~private_derived() noexcept;

  using base::get_base;  // Expose.
};

base::~base() noexcept {}
derived::~derived() noexcept {}
virtual_derived::~virtual_derived() noexcept {}
protected_derived::~protected_derived() noexcept {}
private_derived::~private_derived() noexcept {}

template<typename Derived> class most_derived_type
: public Derived
{
 public:
  virtual ~most_derived_type() noexcept override {}
};


template<typename T, int I> class ambiguity
: public T
{
 public:
  virtual ~ambiguity() noexcept override {}
};

template<typename T> class ambiguous
: public ambiguity<T, 0>,
  public ambiguity<T, 1>
{
 public:
  virtual ~ambiguous() noexcept override {}

  base* get_base(int i) noexcept {
    if (i == 0) return ambiguity<T, 0>::get_base();
    if (i == 1) return ambiguity<T, 1>::get_base();
    return nullptr;
  }

  T* get_t(int i) {
    if (i == 0) return static_cast<ambiguity<T, 0>*>(this);
    if (i == 1) return static_cast<ambiguity<T, 1>*>(this);
    return nullptr;
  }
};


template<typename Derived> bool invoke_dyncast(base* b, Derived* expect,
                                               ptrdiff_t hint) noexcept {
  const auto& typeid_base =
      static_cast<const abi::__class_type_info&>(typeid(base));
  const auto& typeid_derived =
      static_cast<const abi::__class_type_info&>(typeid(Derived));
  Derived* d_ = static_cast<Derived*>(abi::__dynamic_cast(b, &typeid_base,
                                                          &typeid_derived,
                                                          hint));
  fprintf(stderr, "dynamic_cast (hint = %td) returned %p, expected %p%s\n",
          hint, d_, expect, (d_ == expect ? "\t\\o/" : ""));
  return (d_ == expect);
}


int test_immediate() {
  fprintf(stderr, "%s: %s\n", __func__, "derived");
  derived d;
  if (!invoke_dyncast<derived>(&d, &d, 0)) return 1;
  if (!invoke_dyncast<derived>(&d, &d, -1)) return 1;

  fprintf(stderr, "%s: %s\n", __func__, "virtual_derived");
  virtual_derived vd;
  if (!invoke_dyncast<virtual_derived>(&vd, &vd, -1)) return 1;

  fprintf(stderr, "%s: %s\n", __func__, "protected_derived");
  protected_derived pd;
  if (!invoke_dyncast<protected_derived>(pd.get_base(), &pd, -2)) return 1;
  if (!invoke_dyncast<protected_derived>(pd.get_base(), &pd, -1)) return 1;

  fprintf(stderr, "%s: %s\n", __func__, "private_derived");
  private_derived ppd;
  if (!invoke_dyncast<private_derived>(ppd.get_base(), &ppd, -2)) return 1;
  if (!invoke_dyncast<private_derived>(ppd.get_base(), &ppd, -1)) return 1;

  return 0;
}

int test_intermediate() {
  fprintf(stderr, "%s: %s\n", __func__, "derived");
  most_derived_type<derived> d;
  if (!invoke_dyncast<derived>(&d, &d, 0)) return 1;
  if (!invoke_dyncast<derived>(&d, &d, -1)) return 1;

  fprintf(stderr, "%s: %s\n", __func__, "virtual_derived");
  most_derived_type<virtual_derived> vd;
  if (!invoke_dyncast<virtual_derived>(&vd, &vd, -1)) return 1;

  fprintf(stderr, "%s: %s\n", __func__, "protected_derived");
  most_derived_type<protected_derived> pd;
  if (!invoke_dyncast<protected_derived>(pd.get_base(), &pd, -2)) return 1;
  if (!invoke_dyncast<protected_derived>(pd.get_base(), &pd, -1)) return 1;

  fprintf(stderr, "%s: %s\n", __func__, "private_derived");
  most_derived_type<private_derived> ppd;
  if (!invoke_dyncast<private_derived>(ppd.get_base(), &ppd, -2)) return 1;
  if (!invoke_dyncast<private_derived>(ppd.get_base(), &ppd, -1)) return 1;

  return 0;
}

int test_ambiguous() {
  for (int i = 0; i < 2; ++i) {
    fprintf(stderr, "%s: %s\n", __func__, "derived");
    ambiguous<derived> d;
    if (!invoke_dyncast<derived>(d.get_base(i), d.get_t(i), 0)) return 1;
    if (!invoke_dyncast<derived>(d.get_base(i), d.get_t(i), -1)) return 1;

    fprintf(stderr, "%s: %s\n", __func__, "virtual_derived");
    ambiguous<virtual_derived> vd;
    if (!invoke_dyncast<virtual_derived>(vd.get_base(i), nullptr, -1))
      return 1;

    fprintf(stderr, "%s: %s\n", __func__, "protected_derived");
    ambiguous<protected_derived> pd;
    if (!invoke_dyncast<protected_derived>(pd.get_base(i), nullptr, -2))
      return 1;
    if (!invoke_dyncast<protected_derived>(pd.get_base(i), nullptr, -1))
      return 1;

    fprintf(stderr, "%s: %s\n", __func__, "private_derived");
    ambiguous<private_derived> ppd;
    if (!invoke_dyncast<private_derived>(ppd.get_base(i), nullptr, -2))
      return 1;
    if (!invoke_dyncast<private_derived>(ppd.get_base(i), nullptr, -1))
      return 1;
  }

  return 0;
}

int main() {
  int err = 0;
  err = (err ? err : test_immediate());
  err = (err ? err : test_intermediate());
  err = (err ? err : test_ambiguous());
  return err;
}
