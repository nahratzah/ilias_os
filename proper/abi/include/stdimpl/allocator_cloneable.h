#ifndef _ALLOCATOR_CLONEABLE_H_
#define _ALLOCATOR_CLONEABLE_H_

#include <cdecl.h>
#include <cstdint>
#include <utility>
#include <memory>
#include <stdimpl/invoke.h>

_namespace_begin(std)
namespace impl {


class allocator_cloneable {
 public:
  class deleter {
   public:
    using offset_type = uint16_t;

    constexpr deleter() noexcept = default;
    constexpr deleter(const deleter&) noexcept = default;
    deleter& operator=(const deleter&) noexcept = default;

    template<typename Alloc>
    deleter(const void*, Alloc*, const allocator_cloneable*, size_t) noexcept;

    void operator()(allocator_cloneable* p) const noexcept;

   private:
    static offset_type calc_off(const void*, const void*) noexcept;
    static void* apply_off(const void*, offset_type) noexcept;

    template<typename Alloc>
    static void cleanup_alloc_(void*, void*, size_t) noexcept;

    void (*cleanup_fn_)(void*, void*) = nullptr;
    uint32_t store_items_ = 0;
    int16_t base_off_ = 0;
    int16_t alloc_off_ = 0;
  };

  class size_align {
   public:
    constexpr size_align(size_t, size_t) noexcept;

    size_t size;
    size_t align;
  };

  template<typename T> using pointer_type =
      enable_if_t<is_base_of<allocator_cloneable, T>::value,
                  unique_ptr<remove_cv_t<T>, deleter>>;

  allocator_cloneable() = delete;
  allocator_cloneable& operator=(const allocator_cloneable&) = delete;
  virtual ~allocator_cloneable() noexcept;

  template<typename T, typename Alloc>
  static auto clone(const T& v, Alloc&& a) -> pointer_type<T>;

  template<typename T, typename Alloc>
  static auto clone(T&& v, Alloc&& a) -> pointer_type<T>;

 private:
  template<typename T, typename Alloc, typename CloneImpl>
  static auto clone_impl(T&&, Alloc&&, CloneImpl) -> pointer_type<T>;

 protected:
  allocator_cloneable(const allocator_cloneable&) = default;
  allocator_cloneable(size_align) noexcept;

 private:
  virtual allocator_cloneable* copy_into(void*) const = 0;
  virtual allocator_cloneable* move_into(void*) = 0;
  size_t alloc_size_(size_t, size_t) const noexcept;

  using alloc_type_ = uintptr_t;

  const size_align constraints_;
};


} /* namespace std::impl */
_namespace_end(std)

#include <stdimpl/allocator_cloneable-inl.h>

#endif /* _ALLOCATOR_CLONEABLE_H_ */
