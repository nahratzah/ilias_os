#include <abi/dynamic_cast.h>
#include <cstdio>
#include <cstddef>

using _namespace(std)::ptrdiff_t;

class base {
 public:
  virtual ~base() noexcept;

  base* get_base() noexcept { return this; }

 private:
  int dummy_;
};

class derived
: public base {
 public:
  virtual ~derived() noexcept;

 private:
  int dummy_;
};

class virtual_derived
: public virtual base {
 public:
  ~virtual_derived() noexcept;

 private:
  int dummy_;
};

class protected_derived
: protected base {
 public:
  ~protected_derived() noexcept;

  using base::get_base;  // Expose.

 private:
  int dummy_;
};

class private_derived
: private base {
 public:
  ~private_derived() noexcept;

  using base::get_base;  // Expose.

 private:
  int dummy_;
};

class not_derived {
 public:
  virtual ~not_derived() noexcept;

 private:
  int dummy_;
};

base::~base() noexcept {}
derived::~derived() noexcept {}
virtual_derived::~virtual_derived() noexcept {}
protected_derived::~protected_derived() noexcept {}
private_derived::~private_derived() noexcept {}
not_derived::~not_derived() noexcept {}

template<typename Derived> class most_derived_type
: public Derived
{
 public:
  virtual ~most_derived_type() noexcept override {}

 private:
  int dummy_;
};


template<typename T, int I> class ambiguity
: public T
{
 public:
  virtual ~ambiguity() noexcept override {}

 private:
  int dummy_;
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

 private:
  int dummy_;
};

template<int NBase, int NND> class cross_cast
: public cross_cast<NBase - 1, NND - 1>,
  public ambiguity<base, NBase>,
  public ambiguity<not_derived, NND>
{
 public:
  ~cross_cast() noexcept override {}

  inline base* get_base(int i) noexcept {
    if (i == NBase - 1) return static_cast<ambiguity<base, NBase>*>(this);
    return cross_cast<NBase - 1, NND - 1>::get_base(i);
  }

  inline not_derived* get_nd(int i) noexcept {
    if (i == NND - 1) return static_cast<ambiguity<not_derived, NND>*>(this);
    return cross_cast<NBase - 1, NND - 1>::get_nd(i);
  }

 private:
  int dummy_;
};

template<> class cross_cast<0, 0>
{
 public:
  virtual ~cross_cast() noexcept {};

  inline base* get_base(int) noexcept {
    return nullptr;
  }

  inline not_derived* get_nd(int) noexcept {
    return nullptr;
  }

 private:
  int dummy_;
};

template<int NBase> class cross_cast<NBase, 0>
: public cross_cast<NBase - 1, 0>,
  public ambiguity<base, NBase>
{
 public:
  ~cross_cast() noexcept override {}

  inline base* get_base(int i) noexcept {
    if (i == NBase - 1) return static_cast<ambiguity<base, NBase>*>(this);
    return cross_cast<NBase - 1, 0>::get_base(i);
  }

  inline not_derived* get_nd(int i) noexcept {
    return cross_cast<NBase - 1, 0>::get_nd(i);
  }

 private:
  int dummy_;
};

template<int NND> class cross_cast<0, NND>
: public cross_cast<0, NND - 1>,
  public ambiguity<not_derived, NND>
{
 public:
  ~cross_cast() noexcept override {}

  inline base* get_base(int i) noexcept {
    return cross_cast<0, NND - 1>::get_base(i);
  }

  inline not_derived* get_nd(int i) noexcept {
    if (i == NND - 1) return static_cast<ambiguity<not_derived, NND>*>(this);
    return cross_cast<0, NND - 1>::get_nd(i);
  }

 private:
  int dummy_;
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

int test_cross_cast() {
  cross_cast<1, 0> t10;
  for (int bidx = 0; bidx < 1; ++bidx) {
    fprintf(stderr, "%s: %s (base %d)\n", __func__, "not_derived", bidx);
    if (!invoke_dyncast<not_derived>(t10.get_base(bidx), nullptr, -2))
      return 1;
    if (!invoke_dyncast<not_derived>(t10.get_base(bidx), nullptr, -1))
      return 1;
  }

  cross_cast<1, 1> t11;
  for (int bidx = 0; bidx < 1; ++bidx) {
    fprintf(stderr, "%s: %s (base %d)\n", __func__, "not_derived", bidx);
    if (!invoke_dyncast<not_derived>(t11.get_base(bidx), t11.get_nd(0), -2))
      return 1;
    if (!invoke_dyncast<not_derived>(t11.get_base(bidx), t11.get_nd(0), -1))
      return 1;
  }

  cross_cast<2, 0> t20;
  for (int bidx = 0; bidx < 2; ++bidx) {
    fprintf(stderr, "%s: %s (base %d)\n", __func__, "not_derived", bidx);
    if (!invoke_dyncast<not_derived>(t20.get_base(bidx), nullptr, -2))
      return 1;
    if (!invoke_dyncast<not_derived>(t20.get_base(bidx), nullptr, -1))
      return 1;
  }

  cross_cast<2, 1> t21;
  for (int bidx = 0; bidx < 2; ++bidx) {
    fprintf(stderr, "%s: %s (base %d)\n", __func__, "not_derived", bidx);
    if (!invoke_dyncast<not_derived>(t21.get_base(bidx), t21.get_nd(0), -2))
      return 1;
    if (!invoke_dyncast<not_derived>(t21.get_base(bidx), t21.get_nd(0), -1))
      return 1;
  }

  cross_cast<1, 2> t12;
  for (int bidx = 0; bidx < 1; ++bidx) {
    fprintf(stderr, "%s: %s (base %d)\n", __func__, "not_derived", bidx);
    if (!invoke_dyncast<not_derived>(t12.get_base(bidx), nullptr, -2))
      return 1;
    if (!invoke_dyncast<not_derived>(t12.get_base(bidx), nullptr, -1))
      return 1;
  }

  cross_cast<2, 2> t22;
  for (int bidx = 0; bidx < 2; ++bidx) {
    fprintf(stderr, "%s: %s (base %d)\n", __func__, "not_derived", bidx);
    if (!invoke_dyncast<not_derived>(t22.get_base(bidx), nullptr, -2))
      return 1;
    if (!invoke_dyncast<not_derived>(t22.get_base(bidx), nullptr, -1))
      return 1;
  }

  return 0;
}

int main() {
  int err = 0;
  err = (err ? err : test_immediate());
  err = (err ? err : test_intermediate());
  err = (err ? err : test_ambiguous());
  err = (err ? err : test_cross_cast());
  return err;
}
