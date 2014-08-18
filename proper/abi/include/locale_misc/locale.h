#ifndef _LOCALE_MISC_LOCALE_H_
#define _LOCALE_MISC_LOCALE_H_

/*
 * Implemented in src/locale.cc, this header exists solely to break
 * the dependancy cycle between <locale>, <ios> and <streambuf>.
 */

#include <cdecl.h>
#include <locale-fwd.h>
#include <locale_misc/locale_t.h>
#include <locale_misc/lc_mask.h>
#include <iosfwd>
#include <atomic>

#ifdef _COMPILING_LOCALE
# include <clocale>
#endif

_namespace_begin(std)


class locale;

template<typename Facet> const Facet& use_facet(const locale&);
template<typename Facet> bool has_facet(const locale&) noexcept;


class locale {
  friend void swap(locale&, locale&) noexcept;  // extension

  template<typename Facet> friend const Facet& use_facet(const locale&);
  template<typename Facet> friend bool has_facet(const locale&) noexcept;
#ifdef _COMPILING_LOCALE
  friend locale_t newlocale(int, const char*, locale_t) noexcept;
  friend locale_t uselocale(locale_t) noexcept;
  friend char* setlocale(int, const char*) noexcept;
#endif

 public:
  class facet;
  class id;
  using category = int;
#ifdef _COMPILING_LOCALE
  class facet_ptr;
#endif

  static constexpr category none     = 0,
                            collate  = LC_COLLATE_MASK,
                            ctype    = LC_CTYPE_MASK,
                            monetary = LC_MONETARY_MASK,
                            numeric  = LC_NUMERIC_MASK,
                            time     = LC_TIME_MASK,
                            messages = LC_MESSAGES_MASK,
                            all      = LC_ALL_MASK;

  locale() noexcept;
  locale(const locale&) noexcept;
  locale(locale&&) noexcept;
  explicit locale(const char*);
  explicit locale(const string&);
  explicit locale(string_ref);
  locale(const locale&, const char*, category);
  locale(const locale&, const string&, category);
  locale(const locale&, string_ref, category);
  template<typename Facet> locale(const locale&, Facet*);
  locale(const locale&, const locale&, category);
  ~locale() noexcept;

  const locale& operator=(const locale&) noexcept;
  const locale& operator=(locale&&) noexcept;
  template<typename Facet> locale combine(const locale&) const;

  string name() const;

  bool operator==(const locale&) const;
  bool operator!=(const locale&) const;

  template<typename Char, typename Traits, typename Allocator>
  bool operator()(const basic_string<Char, Traits, Allocator>&,
                  const basic_string<Char, Traits, Allocator>&) const;
  template<typename Char, typename Traits>
  bool operator()(basic_string_ref<Char, Traits>,
                  basic_string_ref<Char, Traits>) const;

  static locale global(const locale&);
  static const locale& classic();

 private:
  explicit locale(locale_t) noexcept;
  locale(const locale&, const id*, const facet*);
  const facet* has_facet_(const id*) const noexcept;
  const facet& use_facet_(const id*) const;

  locale_t data_ = nullptr;
};

#if 0 // XXX fix type_traits
static_assert(!has_virtual_destructor<locale>::value,
              "Standard explicitly mandates locale has a non-virtual "
              "destructor.");
#endif

namespace impl {

template<typename Facet>
class facet_ref {
 protected:
  explicit facet_ref(locale);
  ~facet_ref() noexcept = default;

  locale loc;
  const Facet& impl;
};

} /* namespace std::impl */


class locale::facet {
#ifdef _COMPILING_LOCALE
  friend class locale::facet_ptr;
#else
  friend class locale;  // Silence compiler warnings.
#endif

 protected:
  explicit facet(size_t = 0);
  virtual ~facet() noexcept;
  facet(const facet&) = delete;
  facet& operator=(const facet&) = delete;

 private:
  mutable atomic<uintptr_t> refs_;
};

class locale::id {
 public:
  constexpr id() = default;
  constexpr id(category cat) noexcept : cat(cat) {}
  id(const id&) = delete;
  void operator=(const id&) = delete;

  const category cat = none;
};


extern template bool locale::operator()(basic_string_ref<char>,
                                        basic_string_ref<char>) const;
extern template bool locale::operator()(basic_string_ref<char16_t>,
                                        basic_string_ref<char16_t>) const;
extern template bool locale::operator()(basic_string_ref<char32_t>,
                                        basic_string_ref<char32_t>) const;
extern template bool locale::operator()(basic_string_ref<wchar_t>,
                                        basic_string_ref<wchar_t>) const;


_namespace_end(std)

#include <locale_misc/locale-inl.h>

#endif /* _LOCALE_MISC_LOCALE_H_ */
