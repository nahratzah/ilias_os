#ifndef _ABI_TYPEINFO_H_
#define _ABI_TYPEINFO_H_

#include <abi/abi.h>
#include <typeinfo>

namespace __cxxabiv1 {


class __class_type_info; // Forward declaration


struct __base_class_type_info {
  const __class_type_info* __base_type;
  long __offset_flags;

  enum __offset_flags_masks {
    __virtual_mask = 0x1,  // Base class is virtual.
    __public_mask = 0x2,  // Base class is public.
    __offset_shift = 8
  };

  static inline long get_offset(const __base_class_type_info&) noexcept;
  static inline long get_flags(const __base_class_type_info&) noexcept;
};


/* Builtin types. */
class __fundamental_type_info
: public std::type_info
{
 public:
  ~__fundamental_type_info() noexcept override;
};

/* Arrays. */
class __array_type_info
: public std::type_info
{
 public:
  ~__array_type_info() noexcept override;
};

/* Functions. */
class __function_type_info
: public std::type_info
{
 public:
  ~__function_type_info() noexcept override;
};

/* Enums. */
class __enum_type_info
: public std::type_info
{
 public:
  ~__enum_type_info() noexcept override;
};


/* Classes without a base class. */
class __class_type_info
: public std::type_info
{
 public:
  ~__class_type_info() noexcept override;
  virtual const void* __dynamic_cast(const void*, const std::type_info&)
                                                         const noexcept;
};

/* Classes with 1 base class at offset 0. */
class __si_class_type_info
: public __class_type_info
{
 public:
  ~__si_class_type_info() noexcept override;
  const void* __dynamic_cast(const void*, const std::type_info&)
                                        const noexcept override;
 private:
  const __class_type_info* __base_type;
};

/* Any class not a __si_class_type_info. */
class __vmi_class_type_info
: public __class_type_info
{
 public:
  enum __flags_masks {
    __non_diamond_repeat_mask = 0x1,
    __diamond_shaped_mask = 0x2
  };

  ~__vmi_class_type_info() noexcept override;
  const void* __dynamic_cast(const void*, const std::type_info&)
                                        const noexcept override;

 private:
  unsigned int __flags;  // XXX lookup per abi: type is arch dependant
  unsigned int __base_count;  // XXX lookup per abi: type is arch dependant
  __base_class_type_info __base_info[1];
};

/* Pointer (basic implementation). */
class __pbase_type_info
: public std::type_info
{
 public:
  inline constexpr __pbase_type_info(const char*, unsigned int,
                                     const std::type_info&);

  ~__pbase_type_info() noexcept override;

 private:
  unsigned int __flags;
  const std::type_info* __pointee;

  enum __masks {
    __const_mask = 0x1,
    __volatile_mask = 0x2,
    __restrict_mask = 0x4,
    __incomplete_mask = 0x8,
    __incomplete_class_mask = 0x10
  };
};

/* Pointer to type. */
class __pointer_type_info
: public __pbase_type_info
{
 public:
  using __pbase_type_info::__pbase_type_info;
  ~__pointer_type_info() noexcept override;
};

/* Pointer to member type. */
class __pointer_to_member_type_info
: public __pbase_type_info
{
 public:
  inline constexpr __pointer_to_member_type_info(const char*, unsigned int,
                                                 const std::type_info&,
                                                 const __class_type_info&);

  ~__pointer_to_member_type_info() noexcept override;

 private:
  const __class_type_info* __context;  // The 'A' in 'int A::*'.
};


} /* namespace __cxxabiv1 */

#include <abi/typeinfo.h>

#endif /* _ABI_TYPEINFO_H_ */
