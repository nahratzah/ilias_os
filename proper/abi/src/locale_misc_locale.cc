#define _LOCALE_INLINE  /* Not inline: generate here. */
#define _COMPILING_LOCALE  /* Expose hidden types. */

#include <locale_misc/locale.h>
#include <abi/memory.h>
#include <algorithm>
#include <cerrno>
#include <iterator>
#include <utility>
#include <locale_misc/ctype.h>
#include <locale_misc/codecvt.h>
#include <locale_misc/num_get.h>
#include <locale_misc/num_put.h>
#include <locale_misc/numpunct.h>

_namespace_begin(std)
namespace {


/* Track locale heap usage. */
::abi::big_heap& locale_heap() {
  using ::abi::big_heap;

  static aligned_storage_t<sizeof(big_heap), alignof(big_heap)> data;
  static once_flag guard;

  void* data_ptr = &data;
  call_once(guard,
            [](void* p) { new (p) big_heap("locale"); },
            data_ptr);
  return *static_cast<big_heap*>(data_ptr);
}


} /* namespace std::<unnamed> */


class locale::facet_ptr {
 public:
  facet_ptr() noexcept = default;
  facet_ptr(nullptr_t) noexcept;
  explicit facet_ptr(const facet*) noexcept;
  facet_ptr(const facet_ptr&) noexcept;
  facet_ptr(facet_ptr&&) noexcept;
  facet_ptr& operator=(nullptr_t) noexcept;
  facet_ptr& operator=(const facet_ptr&) noexcept;
  facet_ptr& operator=(facet_ptr&&) noexcept;
  ~facet_ptr() noexcept;

  void swap(facet_ptr&) noexcept;

  const facet& operator*() const noexcept;
  const facet* operator->() const noexcept;

 private:
  const facet* ptr_ = nullptr;
};

inline locale::facet_ptr::facet_ptr(nullptr_t) noexcept
: facet_ptr()
{}

inline locale::facet_ptr::facet_ptr(const facet* p) noexcept
: ptr_(p)
{
  if (ptr_) ptr_->refs_.fetch_add(1U, memory_order_acquire);
}

inline locale::facet_ptr::facet_ptr(const facet_ptr& p) noexcept
: facet_ptr(p.ptr_)
{}

inline locale::facet_ptr::facet_ptr(facet_ptr&& p) noexcept
{
  p.swap(*this);
}

inline auto locale::facet_ptr::operator=(nullptr_t) noexcept ->
    facet_ptr& {
  facet_ptr(nullptr).swap(*this);
  return *this;
}

inline auto locale::facet_ptr::operator=(const facet_ptr& p) noexcept ->
    facet_ptr& {
  facet_ptr(p).swap(*this);
  return *this;
}

inline auto locale::facet_ptr::operator=(facet_ptr&& p) noexcept ->
    facet_ptr& {
  facet_ptr(move(p)).swap(*this);
  return *this;
}

inline locale::facet_ptr::~facet_ptr() noexcept {
  if (ptr_ && ptr_->refs_.fetch_sub(1U, memory_order_release) == 1U)
    delete ptr_;
}

inline auto locale::facet_ptr::swap(facet_ptr& p) noexcept -> void {
  using _namespace(std)::swap;
  swap(ptr_, p.ptr_);
}

inline auto locale::facet_ptr::operator*() const noexcept -> const facet& {
  assert(ptr_ != nullptr);
  return *ptr_;
}

inline auto locale::facet_ptr::operator->() const noexcept -> const facet* {
  assert(ptr_ != nullptr);
  return ptr_;
}

inline void swap(locale::facet_ptr& x, locale::facet_ptr& y) noexcept {
  x.swap(y);
}


namespace {


/*
 * Vector map layout:
 * [0]    -- uintptr, size of vector
 * [1]    -- uintptr, reference counter
 * [2]    -- const char*, locale name
 * [3...] -- vector: pointers to facets
 * vector is sorted by &facet::id.
 */
using facet_vector_map_ = __locale;  // Opaque.
using facet_vector_map_value_type = tuple<const locale::id*,
                                          locale::facet_ptr>;

constexpr size_t FACET_VECTOR_MAP__SIZE = 0;
constexpr size_t FACET_VECTOR_MAP__REFCOUNTER = 1;
constexpr size_t FACET_VECTOR_MAP__NAME = 2;
constexpr size_t FACET_VECTOR_MAP__NAME_LEN = 3;
constexpr size_t FACET_VECTOR_MAP__VECTOR_BEGIN = 4;

static_assert(alignof(atomic<uintptr_t>) <= alignof(uintptr_t),
              "Atomic won't work properly.");
static_assert(sizeof(atomic<uintptr_t>) <= sizeof(uintptr_t),
              "Atomic won't work properly.");
static_assert(alignof(string) <= alignof(uintptr_t),
              "Facet array won't work properly.");
static_assert(alignof(facet_vector_map_value_type) <= alignof(uintptr_t),
              "Facet array won't work properly.");


inline uintptr_t facet_vector_map_size(const facet_vector_map_* m)
    noexcept {
  if (m == nullptr) return 0;
  return reinterpret_cast<const uintptr_t*>(m)[FACET_VECTOR_MAP__SIZE];
}

inline uintptr_t& facet_vector_map_size(facet_vector_map_* m) noexcept {
  assert(m != nullptr);
  return reinterpret_cast<uintptr_t*>(m)[FACET_VECTOR_MAP__SIZE];
}

inline string_ref facet_vector_map_name(const facet_vector_map_* m) noexcept {
  if (m == nullptr) return string_ref();
  return string_ref(
      reinterpret_cast<const char*>(
          reinterpret_cast<const uintptr_t*>(m)[FACET_VECTOR_MAP__NAME]),
      reinterpret_cast<const uintptr_t*>(m)[FACET_VECTOR_MAP__NAME_LEN]);
}

inline atomic<uintptr_t>* facet_vector_map_refcounter(
    const facet_vector_map_* m) noexcept {
  if (m == nullptr) return nullptr;
  facet_vector_map_* mm = const_cast<facet_vector_map_*>(m);
  return reinterpret_cast<atomic<uintptr_t>*>(
      &reinterpret_cast<uintptr_t*>(mm)[FACET_VECTOR_MAP__REFCOUNTER]);
}

inline const facet_vector_map_value_type* begin(const __locale& m)
    noexcept {
  return reinterpret_cast<const facet_vector_map_value_type*>(
      &reinterpret_cast<const uintptr_t*>(&m)[FACET_VECTOR_MAP__VECTOR_BEGIN]);
}

inline facet_vector_map_value_type* begin(__locale& m) noexcept {
  return reinterpret_cast<facet_vector_map_value_type*>(
      &reinterpret_cast<uintptr_t*>(&m)[FACET_VECTOR_MAP__VECTOR_BEGIN]);
}

inline const facet_vector_map_value_type* end(const __locale& m)
    noexcept {
  return begin(m) + facet_vector_map_size(&m);
}

inline facet_vector_map_value_type* end(__locale& m) noexcept {
  return begin(m) + facet_vector_map_size(&m);
}

inline constexpr size_t facet_vector_map_bytes(size_t nelem, string_ref name)
    noexcept {
  return sizeof(uintptr_t) * FACET_VECTOR_MAP__VECTOR_BEGIN +
         sizeof(facet_vector_map_value_type) * nelem +
         name.length() + (name.empty() ? 0U : 1U);
}


class facet_vector_map_ptr {
 public:
  facet_vector_map_ptr() noexcept = default;
  explicit facet_vector_map_ptr(const facet_vector_map_*) noexcept;
  facet_vector_map_ptr(const facet_vector_map_ptr&) noexcept;
  facet_vector_map_ptr(facet_vector_map_ptr&&) noexcept;
  facet_vector_map_ptr& operator=(const facet_vector_map_ptr&) noexcept;
  facet_vector_map_ptr& operator=(facet_vector_map_ptr&&) noexcept;
  ~facet_vector_map_ptr() noexcept;

  explicit operator bool() const noexcept { return ptr_ != nullptr; }

  static facet_vector_map_ptr steal(const facet_vector_map_*) noexcept;

  void swap(facet_vector_map_ptr&) noexcept;

  const facet_vector_map_* get() const noexcept;
  const facet_vector_map_* release() noexcept;

 private:
  const __locale* ptr_ = nullptr;
};

void __attribute__((unused))
    swap(facet_vector_map_ptr& x, facet_vector_map_ptr& y) noexcept {
  x.swap(y);
}

facet_vector_map_ptr::facet_vector_map_ptr(const facet_vector_map_* p) noexcept
: ptr_(p)
{
  auto refcounter = facet_vector_map_refcounter(ptr_);
  if (refcounter)
    refcounter->fetch_add(1U, memory_order_acquire);
}

facet_vector_map_ptr::facet_vector_map_ptr(const facet_vector_map_ptr& p)
    noexcept
: facet_vector_map_ptr(p.ptr_)
{}

facet_vector_map_ptr::facet_vector_map_ptr(facet_vector_map_ptr&& p) noexcept
{
  p.swap(*this);
}

auto facet_vector_map_ptr::operator=(const facet_vector_map_ptr& p) noexcept ->
    facet_vector_map_ptr& {
  facet_vector_map_ptr(p).swap(*this);
  return *this;
}

auto facet_vector_map_ptr::operator=(facet_vector_map_ptr&& p) noexcept ->
    facet_vector_map_ptr& {
  facet_vector_map_ptr(move(p)).swap(*this);
  return *this;
}

facet_vector_map_ptr::~facet_vector_map_ptr() noexcept {
  auto refcounter = facet_vector_map_refcounter(ptr_);
  if (refcounter && refcounter->fetch_sub(1U, memory_order_release) == 1U) {
    for (auto i = begin(*ptr_); i != end(*ptr_); ++i)
      i->~facet_vector_map_value_type();
    locale_heap().free(ptr_,
                       facet_vector_map_bytes(facet_vector_map_size(ptr_),
                                              facet_vector_map_name(ptr_)));
  }
}

facet_vector_map_ptr facet_vector_map_ptr::steal(const facet_vector_map_* m)
    noexcept {
  facet_vector_map_ptr rv;
  rv.ptr_ = m;
  return rv;
}

auto facet_vector_map_ptr::swap(facet_vector_map_ptr& p) noexcept -> void {
  using _namespace(std)::swap;
  swap(ptr_, p.ptr_);
}

auto facet_vector_map_ptr::get() const noexcept -> const facet_vector_map_* {
  return ptr_;
}

auto facet_vector_map_ptr::release() noexcept -> const facet_vector_map_* {
  return exchange(ptr_, nullptr);
}

struct facet_vector_map_less {
 private:
  static constexpr const locale::id* key(const locale::id* k) noexcept {
    return k;
  }
  static constexpr const locale::id* key(const facet_vector_map_value_type& v)
      noexcept {
    return get<0>(v);
  }

 public:
  template<typename X, typename Y>
  bool operator()(const X& x, const Y& y) const noexcept {
    return key(x) < key(y);
  }
};

facet_vector_map_* new_facet_vector_map(size_t nelem,
                                        string_ref name = string_ref()) {
  const auto bytes = facet_vector_map_bytes(nelem, name);

  facet_vector_map_* m = static_cast<facet_vector_map_*>(
      locale_heap().malloc(bytes, alignof(uintptr_t)));
  if (m == nullptr) __throw_bad_alloc();

  /* Initialize header. */
  facet_vector_map_size(m) = 0;
  atomic_init(facet_vector_map_refcounter(m), uintptr_t(0));

  /* Initialize fields. */
  for (auto i = begin(*m); i != end(*m); ++i) {
    void* i_void = &*i;
    new (i_void) facet_vector_map_value_type(nullptr, nullptr);
  }

  /* Initialize name. */
  char*const name_ptr = (name.empty() ?
                         nullptr :
                         reinterpret_cast<char*>(m) + bytes);
  if (!name.empty())
    *copy(name.begin(), name.end(), name_ptr) = '\0';
  reinterpret_cast<uintptr_t*>(m)[FACET_VECTOR_MAP__NAME] =
      reinterpret_cast<uintptr_t>(name_ptr);
  reinterpret_cast<uintptr_t*>(m)[FACET_VECTOR_MAP__NAME_LEN] = name.length();

  return m;
}

template<typename T, typename... Args>
facet_vector_map_value_type classic_facet(Args&&... args) {
  locale::facet_ptr p = locale::facet_ptr(new T(forward<Args>(args)...));
  return facet_vector_map_value_type(&T::id, move(p));
}

facet_vector_map_ptr classic_facet_vector_map(const string_ref name) {
  using facet_tmp_t =
      vector<facet_vector_map_value_type,
             temporary_buffer_allocator<facet_vector_map_value_type>>;

  facet_tmp_t facets;

  /* Add codecvt facets. */
  facets.push_back(classic_facet<codecvt<char,     char,     mbstate_t>>());
  facets.push_back(classic_facet<codecvt<char,     char16_t, mbstate_t>>());
  facets.push_back(classic_facet<codecvt<char,     char32_t, mbstate_t>>());
  facets.push_back(classic_facet<codecvt<char,     wchar_t,  mbstate_t>>());
  facets.push_back(classic_facet<codecvt<char16_t, char,     mbstate_t>>());
  facets.push_back(classic_facet<codecvt<char16_t, char16_t, mbstate_t>>());
  facets.push_back(classic_facet<codecvt<char16_t, char32_t, mbstate_t>>());
  facets.push_back(classic_facet<codecvt<char16_t, wchar_t,  mbstate_t>>());
  facets.push_back(classic_facet<codecvt<char32_t, char,     mbstate_t>>());
  facets.push_back(classic_facet<codecvt<char32_t, char16_t, mbstate_t>>());
  facets.push_back(classic_facet<codecvt<char32_t, char32_t, mbstate_t>>());
  facets.push_back(classic_facet<codecvt<char32_t, wchar_t,  mbstate_t>>());
  facets.push_back(classic_facet<codecvt<wchar_t,  char,     mbstate_t>>());
  facets.push_back(classic_facet<codecvt<wchar_t,  char16_t, mbstate_t>>());
  facets.push_back(classic_facet<codecvt<wchar_t,  char32_t, mbstate_t>>());
  facets.push_back(classic_facet<codecvt<wchar_t,  wchar_t,  mbstate_t>>());

  /* Add collate facets. */
  facets.push_back(classic_facet<collate<char>>());
  facets.push_back(classic_facet<collate<char16_t>>());
  facets.push_back(classic_facet<collate<char32_t>>());
  facets.push_back(classic_facet<collate<wchar_t>>());

  /* Add ctype facets. */
  facets.push_back(classic_facet<ctype<char>>(ctype<char>::classic_table()));
  facets.push_back(classic_facet<ctype<char16_t>>());
  facets.push_back(classic_facet<ctype<char32_t>>());
  facets.push_back(classic_facet<ctype<wchar_t>>());

#if 0  // XXX: messages contain virtual functions at the moment
  /* Add messages facets. */
  facets.push_back(classic_facet<messages<char>>());
  facets.push_back(classic_facet<messages<char16_t>>());
  facets.push_back(classic_facet<messages<char32_t>>());
  facets.push_back(classic_facet<messages<wchar_t>>());
#endif

  /* Add num_get facets. */
  facets.push_back(classic_facet<num_get<char>>());
  facets.push_back(classic_facet<num_get<char16_t>>());
  facets.push_back(classic_facet<num_get<char32_t>>());
  facets.push_back(classic_facet<num_get<wchar_t>>());

  /* Add num_put facets. */
  facets.push_back(classic_facet<num_put<char>>());
  facets.push_back(classic_facet<num_put<char16_t>>());
  facets.push_back(classic_facet<num_put<char32_t>>());
  facets.push_back(classic_facet<num_put<wchar_t>>());

  /* Add numpunct facets. */
  facets.push_back(classic_facet<numpunct<char>>());
  facets.push_back(classic_facet<numpunct<char16_t>>());
  facets.push_back(classic_facet<numpunct<char32_t>>());
  facets.push_back(classic_facet<numpunct<wchar_t>>());

  /* XXX other facets. */

  /* Sort facets. */
  std::sort(begin(facets), end(facets), facet_vector_map_less());

  /* Build facet map. */
  facet_vector_map_* map = new_facet_vector_map(facets.size(), name);
  facet_vector_map_ptr map_ptr = facet_vector_map_ptr(map);
  auto copy_rv = move(begin(facets), end(facets), begin(*map));
  assert(copy_rv == end(*map));
  return map_ptr;
}

thread_local facet_vector_map_ptr global_loc;

const facet_vector_map_ptr& get_global_loc() noexcept {
  return global_loc;
}

facet_vector_map_ptr copy_global_loc() noexcept {
  return get_global_loc();
}


/*
 * Tiny helper function for locale construction, validates C string names.
 */
string_ref locale_name_c_str_cast_(const char* name) {
  if (_predict_false(name == nullptr))
    throw invalid_argument("null locale name");
  return string_ref(name);
}


} /* namespace std::<unnamed> */


locale::locale() noexcept
: data_(copy_global_loc().release())
{}

locale::locale(const locale& other) noexcept
: data_(facet_vector_map_ptr(other.data_).release())
{}

locale::locale(const char* name)
: locale(locale_name_c_str_cast_(name))
{}

locale::locale(string_ref) {
  assert_msg(false, "XXX implement");  // XXX
}

locale::locale(const locale& loc, const char* name, category c)
: locale(loc, locale_name_c_str_cast_(name), c)
{}

locale::locale(const locale& loc, string_ref name, category c)
: locale(loc, locale(name), c)
{}

locale::locale(const locale& loc, const id* idp, const facet* f) {
  using vtype = facet_vector_map_value_type;

  /* Argument validation. */
  if (idp == nullptr || f == nullptr)
    throw std::invalid_argument("locale: null argument");

  /* Figure out which elements are to stay. */
  const vtype* ld_begin = (loc.data_ ? begin(*loc.data_) : nullptr);
  const vtype* ld_end = (loc.data_ ? end(*loc.data_) : nullptr);
  const vtype* excl_begin;
  const vtype* excl_end;
  tie(excl_begin, excl_end) = equal_range(ld_begin, ld_end,
                                          idp, facet_vector_map_less());

  /* Allocate new map. */
  const size_t nelem = (excl_begin - ld_begin) + (ld_end - excl_end) + 1;
  facet_vector_map_*const new_map = new_facet_vector_map(nelem);
  facet_vector_map_ptr new_map_ptr = facet_vector_map_ptr(new_map);

  /* Fill map with elements. */
  auto out = begin(*new_map);
  out = copy(ld_begin, excl_begin, out);
  *out++ = vtype(idp, locale::facet_ptr(f));
  out = copy(excl_end, ld_end, out);
  assert(out == end(*new_map));

  /* Finish intialization. */
  data_ = new_map_ptr.release();
}

locale::locale(const locale& loc_a, const locale& loc_b, category c) {
  using facet_tmp_t =
      vector<facet_vector_map_value_type,
             temporary_buffer_allocator<facet_vector_map_value_type>>;
  using vtype = facet_vector_map_value_type;

  assert(loc_a.data_ != nullptr && loc_b.data_ != nullptr);

  /* Predicates, filters category c. */
  const auto pred = [c](const vtype& v) {
                      return get<0>(v)->cat == c;
                    };
  const auto not_pred = [c](const vtype& v) {
                          return get<0>(v)->cat == c;
                        };

  /* Select facets to use, using a merge strategy. */
  facet_tmp_t facets;
  auto a_i = begin(*loc_a.data_);
  auto b_i = begin(*loc_b.data_);
  while (a_i != end(*loc_a.data_) && b_i != end(*loc_b.data_)) {
    if (!not_pred(*a_i)) {
      ++a_i;
    } else if (!pred(*b_i)) {
      ++b_i;
    } else {
      auto& i = (facet_vector_map_less()(*a_i, *b_i) ? a_i : b_i);
      facets.push_back(*i++);
    }
  }
  /* Copy remaining facets (note that at least one of the ranges is empty). */
  copy_if(a_i, end(*loc_a.data_), back_inserter(facets), not_pred);
  copy_if(b_i, end(*loc_b.data_), back_inserter(facets), pred);

  /* Build facet map. */
  facet_vector_map_* result = new_facet_vector_map(facets.size());
  facet_vector_map_ptr result_ptr = facet_vector_map_ptr(result);
  const auto move_rv = move(begin(facets), end(facets), begin(*result));
  assert(move_rv == end(*result));

  /* Finish initialization. */
  data_ = result_ptr.release();
}

locale::~locale() noexcept {
  facet_vector_map_ptr::steal(data_);
}

const locale& locale::operator=(const locale& other) noexcept {
  auto tmp = facet_vector_map_ptr(other.data_);
  facet_vector_map_ptr::steal(static_cast<const facet_vector_map_*>(
      exchange(data_, tmp.release())));
  return *this;
}

auto locale::name() const -> string {
  const auto fvm_name = facet_vector_map_name(data_);
  return (fvm_name.empty() ? string_ref("*", 1) : fvm_name);
}

auto locale::global(const locale& loc) -> locale {
  auto ptr = facet_vector_map_ptr(loc.data_);
  assert(ptr);  // XXX throw EINVAL

  global_loc.swap(ptr);
  return locale(ptr.release());
}

auto locale::classic() -> const locale& {
  static const locale classic_singleton =
      locale(classic_facet_vector_map("C").release());
  return classic_singleton;
}

auto locale::has_facet_(const id* idp) const noexcept -> const facet* {
  if (_predict_false(data_ == nullptr)) return nullptr;

  auto range = equal_range(begin(*data_), end(*data_), idp,
                           facet_vector_map_less());
  if (range.first == range.second) return nullptr;
  return &*get<1>(*range.first);
}

auto locale::use_facet_(const id* idp) const -> const facet& {
  if (_predict_true(data_ != nullptr)) {
    auto f = lower_bound(begin(*data_), end(*data_),
                         idp, facet_vector_map_less());
    if (_predict_true(f != end(*data_) && get<0>(*f) == idp))
      return *get<1>(*f);
  }

  throw bad_cast();
}


locale_t duplocale(locale_t loc) noexcept {
  return facet_vector_map_ptr(loc).release();
}

void freelocale(locale_t loc) noexcept {
  facet_vector_map_ptr::steal(loc);
}

locale_t newlocale(int mask, const char* name, locale_t base) noexcept {
  try {
    if (_predict_false(name == nullptr)) throw invalid_argument("null name");

    facet_vector_map_ptr ref;
    if (base == nullptr)
      ref = facet_vector_map_ptr(new_facet_vector_map(0, string_ref()));
    else
      ref = facet_vector_map_ptr(base);
    locale base_loc = locale(ref.release());

    locale rv_loc = locale(base_loc, name, mask);
    auto rv = facet_vector_map_ptr::steal(rv_loc.data_);
    rv_loc.data_ = nullptr;
    return rv.release();
  } catch (const bad_alloc&) {
    errno = _ABI_ENOMEM;
    return nullptr;
  } catch (const invalid_argument&) {
    errno = _ABI_EINVAL;
    return nullptr;
  } catch (...) {
    assert_msg(false, "XXX implement");  // XXX
    return nullptr;
  }
}

locale_t uselocale(locale_t loc) noexcept {
  try {
    locale rv_loc;
    if (loc == nullptr)
      rv_loc = locale();
    else
      rv_loc = locale::global(locale(facet_vector_map_ptr(loc).release()));

    auto rv = facet_vector_map_ptr::steal(rv_loc.data_);
    rv_loc.data_ = nullptr;
    return rv.release();
  } catch (const bad_alloc&) {
    errno = _ABI_ENOMEM;
    return nullptr;
  } catch (const invalid_argument&) {
    errno = _ABI_EINVAL;
    return nullptr;
  } catch (...) {
    assert_msg(false, "XXX implement");  // XXX
    return nullptr;
  }
}

char* setlocale(int mask, const char* name) noexcept {
  try {
    locale assign = locale(locale(), name, mask);
    locale::global(assign);
    return const_cast<char*>(facet_vector_map_name(assign.data_).data());
  } catch (const bad_alloc&) {
    errno = _ABI_ENOMEM;
    return nullptr;
  } catch (const invalid_argument&) {
    errno = _ABI_EINVAL;
    return nullptr;
  } catch (...) {
    assert_msg(false, "XXX implement");  // XXX
    return nullptr;
  }
}


constexpr locale::category locale::none,
                           locale::collate,
                           locale::ctype,
                           locale::monetary,
                           locale::numeric,
                           locale::time,
                           locale::messages,
                           locale::all;


template bool locale::operator()(basic_string_ref<char>,
                                 basic_string_ref<char>) const;
template bool locale::operator()(basic_string_ref<char16_t>,
                                 basic_string_ref<char16_t>) const;
template bool locale::operator()(basic_string_ref<char32_t>,
                                 basic_string_ref<char32_t>) const;
template bool locale::operator()(basic_string_ref<wchar_t>,
                                 basic_string_ref<wchar_t>) const;


_namespace_end(std)
