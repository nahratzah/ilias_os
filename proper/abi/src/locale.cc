#define _LOCALE_INLINE  /* Not inline: generate here. */
#define _COMPILING_LOCALE  /* Expose hidden types. */

#include <abi/memory.h>
#include <locale>
#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <vector>
#include <cerrno>
#include <streambuf>

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
  using vtype = facet_vector_map_value_type;

  facet_tmp_t facets;

  /* Add ctype facets. */
  facets.push_back(classic_facet<ctype<char>>(ctype<char>::classic_table()));
  facets.push_back(classic_facet<ctype<char16_t>>());
  facets.push_back(classic_facet<ctype<char32_t>>());
  facets.push_back(classic_facet<ctype<wchar_t>>());

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

auto locale::posix() -> const locale& {
  static const locale classic_singleton =
      locale(classic_facet_vector_map("POSIX").release());
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

constexpr ctype_base::mask ctype_base::space,
                           ctype_base::print,
                           ctype_base::cntrl,
                           ctype_base::upper,
                           ctype_base::lower,
                           ctype_base::alpha,
                           ctype_base::digit,
                           ctype_base::punct,
                           ctype_base::xdigit,
                           ctype_base::blank,
                           ctype_base::graph,
                           ctype_base::alnum;


namespace {

constexpr ctype_base::mask ascii_classify(char c) noexcept {
  const bool ascii_  = (c >= 0x00 && c <= 0x7f);
  const bool upper_  = (c >= 'A' && c <= 'Z');
  const bool lower_  = (c >= 'a' && c <= 'z');
  const bool alpha_  = upper_ || lower_;
  const bool digit_  = (c >= '0' && c <= '9');
  const bool alnum_  = alpha_ || digit_;
  const bool xdigit_ = digit_ ||
                       (c >= 'A' && c <= 'F') ||
                       (c >= 'a' && c <= 'f');
  const bool blank_  = (c == ' ' || c == '\t');
  const bool space_  = blank_ ||
                       c == '\f' || c == '\n' ||
                       c == '\r' || c == '\v';
  const bool cntrl_  = (c >= 0x00 && c <= 0x1f) ||
                       c == 0x7f;
  const bool print_  = ascii_ && !cntrl_;
  const bool graph_  = print_ && c != ' ';
  const bool punct_  = graph_ && !alnum_;

  return (upper_  ? ctype_base::upper  : 0) |
         (lower_  ? ctype_base::lower  : 0) |
         (alpha_  ? ctype_base::alpha  : 0) |
         (digit_  ? ctype_base::digit  : 0) |
         (alnum_  ? ctype_base::alnum  : 0) |
         (xdigit_ ? ctype_base::xdigit : 0) |
         (blank_  ? ctype_base::blank  : 0) |
         (space_  ? ctype_base::space  : 0) |
         (cntrl_  ? ctype_base::cntrl  : 0) |
         (print_  ? ctype_base::print  : 0) |
         (graph_  ? ctype_base::graph  : 0) |
         (punct_  ? ctype_base::punct  : 0);
}

constexpr ctype_base::mask ascii_masks[] = {
  ascii_classify(  0),
  ascii_classify(  1),
  ascii_classify(  2),
  ascii_classify(  3),
  ascii_classify(  4),
  ascii_classify(  5),
  ascii_classify(  6),
  ascii_classify(  7),
  ascii_classify(  8),
  ascii_classify(  9),
  ascii_classify( 10),
  ascii_classify( 11),
  ascii_classify( 12),
  ascii_classify( 13),
  ascii_classify( 14),
  ascii_classify( 15),
  ascii_classify( 16),
  ascii_classify( 17),
  ascii_classify( 18),
  ascii_classify( 19),
  ascii_classify( 20),
  ascii_classify( 21),
  ascii_classify( 22),
  ascii_classify( 23),
  ascii_classify( 24),
  ascii_classify( 25),
  ascii_classify( 26),
  ascii_classify( 27),
  ascii_classify( 28),
  ascii_classify( 29),
  ascii_classify( 30),
  ascii_classify( 31),
  ascii_classify( 32),
  ascii_classify( 33),
  ascii_classify( 34),
  ascii_classify( 35),
  ascii_classify( 36),
  ascii_classify( 37),
  ascii_classify( 38),
  ascii_classify( 39),
  ascii_classify( 40),
  ascii_classify( 41),
  ascii_classify( 42),
  ascii_classify( 43),
  ascii_classify( 44),
  ascii_classify( 45),
  ascii_classify( 46),
  ascii_classify( 47),
  ascii_classify( 48),
  ascii_classify( 49),
  ascii_classify( 50),
  ascii_classify( 51),
  ascii_classify( 52),
  ascii_classify( 53),
  ascii_classify( 54),
  ascii_classify( 55),
  ascii_classify( 56),
  ascii_classify( 57),
  ascii_classify( 58),
  ascii_classify( 59),
  ascii_classify( 60),
  ascii_classify( 61),
  ascii_classify( 62),
  ascii_classify( 63),
  ascii_classify( 64),
  ascii_classify( 65),
  ascii_classify( 66),
  ascii_classify( 67),
  ascii_classify( 68),
  ascii_classify( 69),
  ascii_classify( 70),
  ascii_classify( 71),
  ascii_classify( 72),
  ascii_classify( 73),
  ascii_classify( 74),
  ascii_classify( 75),
  ascii_classify( 76),
  ascii_classify( 77),
  ascii_classify( 78),
  ascii_classify( 79),
  ascii_classify( 80),
  ascii_classify( 81),
  ascii_classify( 82),
  ascii_classify( 83),
  ascii_classify( 84),
  ascii_classify( 85),
  ascii_classify( 86),
  ascii_classify( 87),
  ascii_classify( 88),
  ascii_classify( 89),
  ascii_classify( 90),
  ascii_classify( 91),
  ascii_classify( 92),
  ascii_classify( 93),
  ascii_classify( 94),
  ascii_classify( 95),
  ascii_classify( 96),
  ascii_classify( 97),
  ascii_classify( 98),
  ascii_classify( 99),
  ascii_classify(100),
  ascii_classify(101),
  ascii_classify(102),
  ascii_classify(103),
  ascii_classify(104),
  ascii_classify(105),
  ascii_classify(106),
  ascii_classify(107),
  ascii_classify(108),
  ascii_classify(109),
  ascii_classify(110),
  ascii_classify(111),
  ascii_classify(112),
  ascii_classify(113),
  ascii_classify(114),
  ascii_classify(115),
  ascii_classify(116),
  ascii_classify(117),
  ascii_classify(118),
  ascii_classify(119),
  ascii_classify(120),
  ascii_classify(121),
  ascii_classify(122),
  ascii_classify(123),
  ascii_classify(124),
  ascii_classify(125),
  ascii_classify(126),
  ascii_classify(127),
  ascii_classify(128),
  ascii_classify(129),
  ascii_classify(130),
  ascii_classify(131),
  ascii_classify(132),
  ascii_classify(133),
  ascii_classify(134),
  ascii_classify(135),
  ascii_classify(136),
  ascii_classify(137),
  ascii_classify(138),
  ascii_classify(139),
  ascii_classify(140),
  ascii_classify(141),
  ascii_classify(142),
  ascii_classify(143),
  ascii_classify(144),
  ascii_classify(145),
  ascii_classify(146),
  ascii_classify(147),
  ascii_classify(148),
  ascii_classify(149),
  ascii_classify(150),
  ascii_classify(151),
  ascii_classify(152),
  ascii_classify(153),
  ascii_classify(154),
  ascii_classify(155),
  ascii_classify(156),
  ascii_classify(157),
  ascii_classify(158),
  ascii_classify(159),
  ascii_classify(160),
  ascii_classify(161),
  ascii_classify(162),
  ascii_classify(163),
  ascii_classify(164),
  ascii_classify(165),
  ascii_classify(166),
  ascii_classify(167),
  ascii_classify(168),
  ascii_classify(169),
  ascii_classify(170),
  ascii_classify(171),
  ascii_classify(172),
  ascii_classify(173),
  ascii_classify(174),
  ascii_classify(175),
  ascii_classify(176),
  ascii_classify(177),
  ascii_classify(178),
  ascii_classify(179),
  ascii_classify(180),
  ascii_classify(181),
  ascii_classify(182),
  ascii_classify(183),
  ascii_classify(184),
  ascii_classify(185),
  ascii_classify(186),
  ascii_classify(187),
  ascii_classify(188),
  ascii_classify(189),
  ascii_classify(190),
  ascii_classify(191),
  ascii_classify(192),
  ascii_classify(193),
  ascii_classify(194),
  ascii_classify(195),
  ascii_classify(196),
  ascii_classify(197),
  ascii_classify(198),
  ascii_classify(199),
  ascii_classify(200),
  ascii_classify(201),
  ascii_classify(202),
  ascii_classify(203),
  ascii_classify(204),
  ascii_classify(205),
  ascii_classify(206),
  ascii_classify(207),
  ascii_classify(208),
  ascii_classify(209),
  ascii_classify(210),
  ascii_classify(211),
  ascii_classify(212),
  ascii_classify(213),
  ascii_classify(214),
  ascii_classify(215),
  ascii_classify(216),
  ascii_classify(217),
  ascii_classify(218),
  ascii_classify(219),
  ascii_classify(220),
  ascii_classify(221),
  ascii_classify(222),
  ascii_classify(223),
  ascii_classify(224),
  ascii_classify(225),
  ascii_classify(226),
  ascii_classify(227),
  ascii_classify(228),
  ascii_classify(229),
  ascii_classify(230),
  ascii_classify(231),
  ascii_classify(232),
  ascii_classify(233),
  ascii_classify(234),
  ascii_classify(235),
  ascii_classify(236),
  ascii_classify(237),
  ascii_classify(238),
  ascii_classify(239),
  ascii_classify(240),
  ascii_classify(241),
  ascii_classify(242),
  ascii_classify(243),
  ascii_classify(244),
  ascii_classify(245),
  ascii_classify(246),
  ascii_classify(247),
  ascii_classify(248),
  ascii_classify(249),
  ascii_classify(250),
  ascii_classify(251),
  ascii_classify(252),
  ascii_classify(253),
  ascii_classify(254),
  ascii_classify(255),
};

} /* namespace std::<unnamed> */


const size_t ctype<char>::table_size =
    sizeof(ascii_masks) / sizeof(ascii_masks[0]);

auto ctype<char>::classic_table() noexcept -> const mask* {
  return ascii_masks;
}


ctype<char16_t>::ctype(size_t refs)
: locale::facet(refs)
{}

auto ctype<char16_t>::is(mask m, char_type c) const -> bool {
  return do_is(m, c);
}

auto ctype<char16_t>::is(const char_type* b, const char_type* e,
                         mask* vec) const -> const char_type* {
  return do_is(b, e, vec);
}

auto ctype<char16_t>::scan_is(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_is(m, b, e);
}

auto ctype<char16_t>::scan_not(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_not(m, b, e);
}

auto ctype<char16_t>::toupper(char_type c) const -> char_type {
  return do_toupper(c);
}

auto ctype<char16_t>::toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_toupper(b, e);
}

auto ctype<char16_t>::tolower(char_type c) const -> char_type {
  return do_tolower(c);
}

auto ctype<char16_t>::tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_tolower(b, e);
}

auto ctype<char16_t>::widen(char c) const -> char_type {
  return do_widen(c);
}

auto ctype<char16_t>::widen(const char* b, const char* e,
                            char_type* out) const -> const char* {
  return do_widen(b, e, out);
}

auto ctype<char16_t>::narrow(char_type c, char dfl) const -> char {
  return do_narrow(c, dfl);
}

auto ctype<char16_t>::narrow(const char_type* b, const char_type* e,
                             char dfl, char* out) const -> const char_type* {
  return do_narrow(b, e, dfl, out);
}

ctype<char16_t>::~ctype() noexcept {}

auto ctype<char16_t>::do_is(mask m, char_type c) const -> bool {
  return (c >= 0 && c <= 0x7f && bool(ascii_masks[c] & m));  // XXX: all the code points!
}

auto ctype<char16_t>::do_is(const char_type* b, const char_type* e, mask* vec)
    const -> const char_type* {
  transform(b, e, vec,
            [this](char_type c) -> mask {
              if (c >= 0 && c <= 0x7f) return ascii_masks[c];  // XXX: all the code points!
              return 0;
            });
  return e;
}

auto ctype<char16_t>::do_scan_is(mask m,
                                 const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if(b, e,
                 [m, this](char_type c) -> bool {
                   return this->is(m, c);
                 });
}

auto ctype<char16_t>::do_scan_not(mask m,
                                  const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if_not(b, e,
                     [m, this](char_type c) -> bool {
                       return this->is(m, c);
                     });
}

auto ctype<char16_t>::do_toupper(char_type c) const -> char_type {
  // XXX: all the code points!
  return (c >= 'a' && c <= 'z' ?
          c - 'a' + 'A' :
          c);
}

auto ctype<char16_t>::do_toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->toupper(c);
                   });
}

auto ctype<char16_t>::do_tolower(char_type c) const -> char_type {
  // XXX: all the code points!
  return (c >= 'A' && c <= 'A' ?
          c - 'A' + 'a' :
          c);
}

auto ctype<char16_t>::do_tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->tolower(c);
                   });
}

auto ctype<char16_t>::do_widen(char c) const -> char_type {
  return c;
}

auto ctype<char16_t>::do_widen(const char* b, const char* e,
                              char_type* out) const -> const char* {
  transform(b, e, out,
            [this](char c) -> char_type {
              return this->widen(c);
            });
  return e;
}

auto ctype<char16_t>::do_narrow(char_type c, char dfl) const -> char {
  return (c <= 0 || c > 0x7f ? dfl : c);
}

auto ctype<char16_t>::do_narrow(const char_type* b, const char_type* e,
                               char dfl, char* out) const -> const char_type* {
  transform(b, e, out,
            [this, dfl](char c) -> char_type {
              return this->narrow(c, dfl);
            });
  return e;
}


ctype<char32_t>::ctype(size_t refs)
: locale::facet(refs)
{}

auto ctype<char32_t>::is(mask m, char_type c) const -> bool {
  return do_is(m, c);
}

auto ctype<char32_t>::is(const char_type* b, const char_type* e,
                         mask* vec) const -> const char_type* {
  return do_is(b, e, vec);
}

auto ctype<char32_t>::scan_is(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_is(m, b, e);
}

auto ctype<char32_t>::scan_not(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_not(m, b, e);
}

auto ctype<char32_t>::toupper(char_type c) const -> char_type {
  return do_toupper(c);
}

auto ctype<char32_t>::toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_toupper(b, e);
}

auto ctype<char32_t>::tolower(char_type c) const -> char_type {
  return do_tolower(c);
}

auto ctype<char32_t>::tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_tolower(b, e);
}

auto ctype<char32_t>::widen(char c) const -> char_type {
  return do_widen(c);
}

auto ctype<char32_t>::widen(const char* b, const char* e,
                            char_type* out) const -> const char* {
  return do_widen(b, e, out);
}

auto ctype<char32_t>::narrow(char_type c, char dfl) const -> char {
  return do_narrow(c, dfl);
}

auto ctype<char32_t>::narrow(const char_type* b, const char_type* e,
                             char dfl, char* out) const -> const char_type* {
  return do_narrow(b, e, dfl, out);
}

ctype<char32_t>::~ctype() noexcept {}

auto ctype<char32_t>::do_is(mask m, char_type c) const -> bool {
  return (c >= 0 && c <= 0x7f && bool(ascii_masks[c] & m));  // XXX: all the code points!
}

auto ctype<char32_t>::do_is(const char_type* b, const char_type* e, mask* vec)
    const -> const char_type* {
  transform(b, e, vec,
            [this](char_type c) -> mask {
              if (c >= 0 && c <= 0x7f) return ascii_masks[c];  // XXX: all the code points!
              return 0;
            });
  return e;
}

auto ctype<char32_t>::do_scan_is(mask m,
                                 const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if(b, e,
                 [m, this](char_type c) -> bool {
                   return this->is(m, c);
                 });
}

auto ctype<char32_t>::do_scan_not(mask m,
                                  const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if_not(b, e,
                     [m, this](char_type c) -> bool {
                       return this->is(m, c);
                     });
}

auto ctype<char32_t>::do_toupper(char_type c) const -> char_type {
  // XXX: all the code points!
  return (c >= 'a' && c <= 'z' ?
          c - 'a' + 'A' :
          c);
}

auto ctype<char32_t>::do_toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->toupper(c);
                   });
}

auto ctype<char32_t>::do_tolower(char_type c) const -> char_type {
  // XXX: all the code points!
  return (c >= 'A' && c <= 'A' ?
          c - 'A' + 'a' :
          c);
}

auto ctype<char32_t>::do_tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->tolower(c);
                   });
}

auto ctype<char32_t>::do_widen(char c) const -> char_type {
  return c;
}

auto ctype<char32_t>::do_widen(const char* b, const char* e,
                              char_type* out) const -> const char* {
  transform(b, e, out,
            [this](char c) -> char_type {
              return this->widen(c);
            });
  return e;
}

auto ctype<char32_t>::do_narrow(char_type c, char dfl) const -> char {
  return (c <= 0 || c > 0x7f ? dfl : c);
}

auto ctype<char32_t>::do_narrow(const char_type* b, const char_type* e,
                               char dfl, char* out) const -> const char_type* {
  transform(b, e, out,
            [this, dfl](char c) -> char_type {
              return this->narrow(c, dfl);
            });
  return e;
}


ctype<wchar_t>::ctype(size_t refs)
: locale::facet(refs)
{}

auto ctype<wchar_t>::is(mask m, char_type c) const -> bool {
  return do_is(m, c);
}

auto ctype<wchar_t>::is(const char_type* b, const char_type* e,
                        mask* vec) const -> const char_type* {
  return do_is(b, e, vec);
}

auto ctype<wchar_t>::scan_is(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_is(m, b, e);
}

auto ctype<wchar_t>::scan_not(mask m, const char_type* b, const char_type* e)
    const -> const char_type* {
  return do_scan_not(m, b, e);
}

auto ctype<wchar_t>::toupper(char_type c) const -> char_type {
  return do_toupper(c);
}

auto ctype<wchar_t>::toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_toupper(b, e);
}

auto ctype<wchar_t>::tolower(char_type c) const -> char_type {
  return do_tolower(c);
}

auto ctype<wchar_t>::tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  return do_tolower(b, e);
}

auto ctype<wchar_t>::widen(char c) const -> char_type {
  return do_widen(c);
}

auto ctype<wchar_t>::widen(const char* b, const char* e,
                           char_type* out) const -> const char* {
  return do_widen(b, e, out);
}

auto ctype<wchar_t>::narrow(char_type c, char dfl) const -> char {
  return do_narrow(c, dfl);
}

auto ctype<wchar_t>::narrow(const char_type* b, const char_type* e,
                            char dfl, char* out) const -> const char_type* {
  return do_narrow(b, e, dfl, out);
}

ctype<wchar_t>::~ctype() noexcept {}

auto ctype<wchar_t>::do_is(mask m, char_type c) const -> bool {
  return (c >= 0 && c <= 0x7f && bool(ascii_masks[c] & m));  // XXX: all the code points!
}

auto ctype<wchar_t>::do_is(const char_type* b, const char_type* e, mask* vec)
    const -> const char_type* {
  transform(b, e, vec,
            [this](char_type c) -> mask {
              if (c >= 0 && c <= 0x7f) return ascii_masks[c];  // XXX: all the code points!
              return 0;
            });
  return e;
}

auto ctype<wchar_t>::do_scan_is(mask m,
                                const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if(b, e,
                 [m, this](char_type c) -> bool {
                   return this->is(m, c);
                 });
}

auto ctype<wchar_t>::do_scan_not(mask m,
                                 const char_type* b, const char_type* e)
    const -> const char_type* {
  return find_if_not(b, e,
                     [m, this](char_type c) -> bool {
                       return this->is(m, c);
                     });
}

auto ctype<wchar_t>::do_toupper(char_type c) const -> char_type {
  // XXX: all the code points!
  return (c >= 'a' && c <= 'z' ?
          c - 'a' + 'A' :
          c);
}

auto ctype<wchar_t>::do_toupper(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->toupper(c);
                   });
}

auto ctype<wchar_t>::do_tolower(char_type c) const -> char_type {
  // XXX: all the code points!
  return (c >= 'A' && c <= 'A' ?
          c - 'A' + 'a' :
          c);
}

auto ctype<wchar_t>::do_tolower(char_type* b, const char_type* e) const ->
    const char_type* {
  const char_type* bb = b;
  return transform(bb, e, b,
                   [this](char_type c) -> char_type {
                     return this->tolower(c);
                   });
}

auto ctype<wchar_t>::do_widen(char c) const -> char_type {
  return c;
}

auto ctype<wchar_t>::do_widen(const char* b, const char* e,
                              char_type* out) const -> const char* {
  transform(b, e, out,
            [this](char c) -> char_type {
              return this->widen(c);
            });
  return e;
}

auto ctype<wchar_t>::do_narrow(char_type c, char dfl) const -> char {
  return (c <= 0 || c > 0x7f ? dfl : c);
}

auto ctype<wchar_t>::do_narrow(const char_type* b, const char_type* e,
                               char dfl, char* out) const -> const char_type* {
  transform(b, e, out,
            [this, dfl](char c) -> char_type {
              return this->narrow(c, dfl);
            });
  return e;
}


auto numpunct<char>::decimal_point() const -> char_type {
  return do_decimal_point();
}
auto numpunct<char16_t>::decimal_point() const -> char_type {
  return do_decimal_point();
}
auto numpunct<char32_t>::decimal_point() const -> char_type {
  return do_decimal_point();
}
auto numpunct<wchar_t>::decimal_point() const -> char_type {
  return do_decimal_point();
}

auto numpunct<char>::thousands_sep() const -> char_type {
  return do_thousands_sep();
}
auto numpunct<char16_t>::thousands_sep() const -> char_type {
  return do_thousands_sep();
}
auto numpunct<char32_t>::thousands_sep() const -> char_type {
  return do_thousands_sep();
}
auto numpunct<wchar_t>::thousands_sep() const -> char_type {
  return do_thousands_sep();
}

auto numpunct<char>::grouping() const -> string {
  return do_grouping();
}
auto numpunct<char16_t>::grouping() const -> string {
  return do_grouping();
}
auto numpunct<char32_t>::grouping() const -> string {
  return do_grouping();
}
auto numpunct<wchar_t>::grouping() const -> string {
  return do_grouping();
}

auto numpunct<char>::truename() const -> string_type {
  return do_truename();
}
auto numpunct<char16_t>::truename() const -> string_type {
  return do_truename();
}
auto numpunct<char32_t>::truename() const -> string_type {
  return do_truename();
}
auto numpunct<wchar_t>::truename() const -> string_type {
  return do_truename();
}

auto numpunct<char>::falsename() const -> string_type {
  return do_falsename();
}
auto numpunct<char16_t>::falsename() const -> string_type {
  return do_falsename();
}
auto numpunct<char32_t>::falsename() const -> string_type {
  return do_falsename();
}
auto numpunct<wchar_t>::falsename() const -> string_type {
  return do_falsename();
}

auto numpunct<char>::do_decimal_point() const -> char_type {
  return '.';
}
auto numpunct<char16_t>::do_decimal_point() const -> char_type {
  return '.';
}
auto numpunct<char32_t>::do_decimal_point() const -> char_type {
  return '.';
}
auto numpunct<wchar_t>::do_decimal_point() const -> char_type {
  return L'.';
}

auto numpunct<char>::do_thousands_sep() const -> char_type {
  return ',';
}
auto numpunct<char16_t>::do_thousands_sep() const -> char_type {
  return u',';
}
auto numpunct<char32_t>::do_thousands_sep() const -> char_type {
  return U',';
}
auto numpunct<wchar_t>::do_thousands_sep() const -> char_type {
  return L',';
}

auto numpunct<char>::do_grouping() const -> string {
  return string();
}
auto numpunct<char16_t>::do_grouping() const -> string {
  return string();
}
auto numpunct<char32_t>::do_grouping() const -> string {
  return string();
}
auto numpunct<wchar_t>::do_grouping() const -> string {
  return string();
}

auto numpunct<char>::do_truename() const -> string_type {
  return "true";
}
auto numpunct<char16_t>::do_truename() const -> string_type {
  return u"true";
}
auto numpunct<char32_t>::do_truename() const -> string_type {
  return U"true";
}
auto numpunct<wchar_t>::do_truename() const -> string_type {
  return L"true";
}

auto numpunct<char>::do_falsename() const -> string_type {
  return "false";
}
auto numpunct<char16_t>::do_falsename() const -> string_type {
  return u"false";
}
auto numpunct<char32_t>::do_falsename() const -> string_type {
  return U"false";
}
auto numpunct<wchar_t>::do_falsename() const -> string_type {
  return L"false";
}


template bool locale::operator()(basic_string_ref<char>,
                                 basic_string_ref<char>) const;
template bool locale::operator()(basic_string_ref<char16_t>,
                                 basic_string_ref<char16_t>) const;
template bool locale::operator()(basic_string_ref<char32_t>,
                                 basic_string_ref<char32_t>) const;
template bool locale::operator()(basic_string_ref<wchar_t>,
                                 basic_string_ref<wchar_t>) const;

constexpr locale::id ctype<char>::id;
constexpr locale::id ctype<char16_t>::id;
constexpr locale::id ctype<char32_t>::id;
constexpr locale::id ctype<wchar_t>::id;

template class ctype_byname<char>;
template class ctype_byname<char16_t>;
template class ctype_byname<char32_t>;
template class ctype_byname<wchar_t>;

template class codecvt<char,     char,     mbstate_t>;
template class codecvt<char16_t, char,     mbstate_t>;
template class codecvt<char32_t, char,     mbstate_t>;
template class codecvt<wchar_t,  char,     mbstate_t>;
template class codecvt<char,     char16_t, mbstate_t>;
template class codecvt<char16_t, char16_t, mbstate_t>;
template class codecvt<char32_t, char16_t, mbstate_t>;
template class codecvt<wchar_t,  char16_t, mbstate_t>;
template class codecvt<char,     char32_t, mbstate_t>;
template class codecvt<char16_t, char32_t, mbstate_t>;
template class codecvt<char32_t, char32_t, mbstate_t>;
template class codecvt<wchar_t,  char32_t, mbstate_t>;
template class codecvt<char,     wchar_t,  mbstate_t>;
template class codecvt<char16_t, wchar_t,  mbstate_t>;
template class codecvt<char32_t, wchar_t,  mbstate_t>;
template class codecvt<wchar_t,  wchar_t,  mbstate_t>;

template class codecvt_byname<char,     char,     mbstate_t>;
template class codecvt_byname<char16_t, char,     mbstate_t>;
template class codecvt_byname<char32_t, char,     mbstate_t>;
template class codecvt_byname<wchar_t,  char,     mbstate_t>;
template class codecvt_byname<char,     char16_t, mbstate_t>;
template class codecvt_byname<char16_t, char16_t, mbstate_t>;
template class codecvt_byname<char32_t, char16_t, mbstate_t>;
template class codecvt_byname<wchar_t,  char16_t, mbstate_t>;
template class codecvt_byname<char,     char32_t, mbstate_t>;
template class codecvt_byname<char16_t, char32_t, mbstate_t>;
template class codecvt_byname<char32_t, char32_t, mbstate_t>;
template class codecvt_byname<wchar_t,  char32_t, mbstate_t>;
template class codecvt_byname<char,     wchar_t,  mbstate_t>;
template class codecvt_byname<char16_t, wchar_t,  mbstate_t>;
template class codecvt_byname<char32_t, wchar_t,  mbstate_t>;
template class codecvt_byname<wchar_t,  wchar_t,  mbstate_t>;

template class num_put<char>;
template class num_put<char16_t>;
template class num_put<char32_t>;
template class num_put<wchar_t>;

constexpr locale::id numpunct<char>::id;
constexpr locale::id numpunct<char16_t>::id;
constexpr locale::id numpunct<char32_t>::id;
constexpr locale::id numpunct<wchar_t>::id;

template class numpunct_byname<char>;
template class numpunct_byname<char16_t>;
template class numpunct_byname<char32_t>;
template class numpunct_byname<wchar_t>;

template class collate<char>;
template class collate<char16_t>;
template class collate<char32_t>;
template class collate<wchar_t>;

template class collate_byname<char>;
template class collate_byname<char16_t>;
template class collate_byname<char32_t>;
template class collate_byname<wchar_t>;

template class messages<char>;
template class messages<char16_t>;
template class messages<char32_t>;
template class messages<wchar_t>;

template class messages_byname<char>;
template class messages_byname<char16_t>;
template class messages_byname<char32_t>;
template class messages_byname<wchar_t>;


template bool isspace(char, const locale&);
template bool isprint(char, const locale&);
template bool iscntrl(char, const locale&);
template bool isupper(char, const locale&);
template bool islower(char, const locale&);
template bool isalpha(char, const locale&);
template bool isdigit(char, const locale&);
template bool ispunct(char, const locale&);
template bool isxdigit(char, const locale&);
template bool isalnum(char, const locale&);
template bool isgraph(char, const locale&);
template bool isblank(char, const locale&);
template char toupper(char, const locale&);
template char tolower(char, const locale&);

template bool isspace(char16_t, const locale&);
template bool isprint(char16_t, const locale&);
template bool iscntrl(char16_t, const locale&);
template bool isupper(char16_t, const locale&);
template bool islower(char16_t, const locale&);
template bool isalpha(char16_t, const locale&);
template bool isdigit(char16_t, const locale&);
template bool ispunct(char16_t, const locale&);
template bool isxdigit(char16_t, const locale&);
template bool isalnum(char16_t, const locale&);
template bool isgraph(char16_t, const locale&);
template bool isblank(char16_t, const locale&);
template char16_t toupper(char16_t, const locale&);
template char16_t tolower(char16_t, const locale&);

template bool isspace(char32_t, const locale&);
template bool isprint(char32_t, const locale&);
template bool iscntrl(char32_t, const locale&);
template bool isupper(char32_t, const locale&);
template bool islower(char32_t, const locale&);
template bool isalpha(char32_t, const locale&);
template bool isdigit(char32_t, const locale&);
template bool ispunct(char32_t, const locale&);
template bool isxdigit(char32_t, const locale&);
template bool isalnum(char32_t, const locale&);
template bool isgraph(char32_t, const locale&);
template bool isblank(char32_t, const locale&);
template char32_t toupper(char32_t, const locale&);
template char32_t tolower(char32_t, const locale&);

template bool isspace(wchar_t, const locale&);
template bool isprint(wchar_t, const locale&);
template bool iscntrl(wchar_t, const locale&);
template bool isupper(wchar_t, const locale&);
template bool islower(wchar_t, const locale&);
template bool isalpha(wchar_t, const locale&);
template bool isdigit(wchar_t, const locale&);
template bool ispunct(wchar_t, const locale&);
template bool isxdigit(wchar_t, const locale&);
template bool isalnum(wchar_t, const locale&);
template bool isgraph(wchar_t, const locale&);
template bool isblank(wchar_t, const locale&);
template wchar_t toupper(wchar_t, const locale&);
template wchar_t tolower(wchar_t, const locale&);


_namespace_end(std)
