#ifndef _ILIAS_TEST_JSON_H_
#define _ILIAS_TEST_JSON_H_

#include <ostream>
#include <string>
#include <type_traits>
#include <utility>
#include <ilias/optional.h>

namespace ilias {
namespace json {


using namespace std;


template<typename, typename> class json_term_ostream;
template<typename> class json_ostream;
template<typename> class json_ostream_array;
template<typename> class basic_json_ostream_array;
template<typename> class json_ostream_object_impl;
template<typename> class json_ostream_object;
template<typename Char, typename Traits = char_traits<Char>> class json_key_t;


namespace impl {


template<typename OChar, typename OTraits, typename SChar, typename STraits,
         typename Codecvt, typename Buf>
auto emit_json_string_cvt_helper(basic_ostream<OChar, OTraits>& out,
                                 basic_string_ref<SChar, STraits> s,
                                 const Codecvt& codec, Buf& buf) -> void;

template<typename OChar, typename OTraits, typename SChar, typename STraits>
auto emit_json_string_cvt_helper(basic_ostream<OChar, OTraits>& out,
                                 basic_string_ref<SChar, STraits> s) -> void;

template<typename OChar, typename OTraits, typename SChar, typename STraits>
static basic_ostream<OChar, OTraits>& emit_json_string(
    basic_ostream<OChar, OTraits>&, basic_string_ref<SChar, STraits>);

template<typename OChar, typename OTraits>
static basic_ostream<OChar, OTraits>& emit_json_string(
    basic_ostream<OChar, OTraits>&, const char*);

template<typename OChar, typename OTraits>
static basic_ostream<OChar, OTraits>& emit_json_string(
    basic_ostream<OChar, OTraits>&, const wchar_t*);

template<typename OChar, typename OTraits>
static basic_ostream<OChar, OTraits>& emit_json_string(
    basic_ostream<OChar, OTraits>&, const char16_t*);

template<typename OChar, typename OTraits>
static basic_ostream<OChar, OTraits>& emit_json_string(
    basic_ostream<OChar, OTraits>&, const char32_t*);

template<typename OChar, typename OTraits, typename SChar, typename STraits,
         typename A>
static basic_ostream<OChar, OTraits>& emit_json_string(
    basic_ostream<OChar, OTraits>&,
    const basic_string<SChar, STraits, A>&);


template<typename, typename, typename = int>
struct _is_string_emitable;

template<typename Out, typename String> using is_string_emitable =
    typename _is_string_emitable<Out, String>::type;


template<typename, typename, typename = int>
struct _is_array_emitable;

template<typename Out, typename C> using is_array_emitable =
    typename _is_array_emitable<Out, C>::type;


template<typename, typename, typename = int>
struct _is_object_emitable;

template<typename Out, typename Obj> using is_object_emitable =
    typename _is_object_emitable<Out, Obj>::type;


} /* namespace ilias::json::impl */


struct begin_json_t {
  constexpr begin_json_t() noexcept = default;
  constexpr begin_json_t(const begin_json_t&) noexcept = default;
  begin_json_t& operator=(const begin_json_t&) noexcept = default;
  explicit constexpr begin_json_t(size_t) noexcept;

  constexpr begin_json_t operator()(size_t = 0) const noexcept;

  size_t indent_lvl = 0;
};
constexpr begin_json_t begin_json{};

struct end_json_t {};
constexpr end_json_t end_json{};

struct begin_json_array_t {};
constexpr begin_json_array_t begin_json_array{};

struct end_json_array_t {};
constexpr end_json_array_t end_json_array{};

struct begin_json_object_t {};
constexpr begin_json_object_t begin_json_object{};

struct end_json_object_t {};
constexpr end_json_object_t end_json_object{};


/* Terminal node only. */
template<typename Char, typename Traits>
auto operator<<(json_term_ostream<Char, Traits>&&, end_json_t)
    noexcept -> basic_ostream<Char, Traits>&;

/* Initial node only. */
template<typename Char, typename Traits>
auto operator<<(basic_ostream<Char, Traits>&, begin_json_t) noexcept ->
    json_ostream<json_term_ostream<Char, Traits>>;

/* Nullptr value. */
template<typename S>
auto operator<<(json_ostream<S>&&, nullptr_t) ->
    typename json_ostream<S>::underlying_stream;

template<typename S>
auto operator<<(json_ostream<S>&&, bool) ->
    typename json_ostream<S>::underlying_stream;

template<typename S>
auto operator<<(json_ostream<S>&&, int) ->
    typename json_ostream<S>::underlying_stream;

template<typename S>
auto operator<<(json_ostream<S>&&, unsigned int) ->
    typename json_ostream<S>::underlying_stream;

template<typename S>
auto operator<<(json_ostream<S>&&, long) ->
    typename json_ostream<S>::underlying_stream;

template<typename S>
auto operator<<(json_ostream<S>&&, unsigned long) ->
    typename json_ostream<S>::underlying_stream;

template<typename S>
auto operator<<(json_ostream<S>&&, long long) ->
    typename json_ostream<S>::underlying_stream;

template<typename S>
auto operator<<(json_ostream<S>&&, unsigned long long) ->
    typename json_ostream<S>::underlying_stream;

template<typename S>
auto operator<<(json_ostream<S>&&, float) ->
    typename json_ostream<S>::underlying_stream;

template<typename S>
auto operator<<(json_ostream<S>&&, double) ->
    typename json_ostream<S>::underlying_stream;

template<typename S>
auto operator<<(json_ostream<S>&&, long double) ->
    typename json_ostream<S>::underlying_stream;

template<typename S, typename String>
auto operator<<(json_ostream<S>&&, const String&) ->
    enable_if_t<impl::is_string_emitable<json_ostream<S>, String>::value,
                typename json_ostream<S>::underlying_stream>;


/*
 * Terminal node.
 *
 * Only accepts json_end.
 */
template<typename Char, typename Traits>
class json_term_ostream {
 public:
  using basic_stream = basic_ostream<Char, Traits>;

  template<typename C, typename T>
  friend auto operator<<(json_term_ostream<C, T>&&, end_json_t) noexcept ->
      basic_ostream<C, T>&;

  json_term_ostream() noexcept = default;
  json_term_ostream(const json_term_ostream&) = delete;
  json_term_ostream& operator=(const json_term_ostream&) = delete;
  json_term_ostream(json_term_ostream&&) noexcept;
  json_term_ostream& operator=(json_term_ostream&&) noexcept;
  explicit json_term_ostream(basic_stream&, size_t = 0) noexcept;
  ~json_term_ostream() noexcept = default;

  bool valid() const noexcept { return out_ != nullptr; }
  basic_stream& get_ostream() const noexcept;
  void do_newline(size_t indent);

 private:
  basic_stream* out_ = nullptr;
  size_t indent_;
};


/*
 * Value node.
 *
 * Accepts any value.
 * Accepts json_array_begin, begin_json_object.
 */
template<typename UnderlyingStream>
class json_ostream {
  template<typename> friend class json_ostream_array;
  template<typename> friend class json_ostream_object_impl;

  template<typename S>
  friend auto operator<<(json_ostream<S>&&, nullptr_t) ->
      typename json_ostream<S>::underlying_stream;

  template<typename S>
  friend auto operator<<(json_ostream<S>&&, bool) ->
      typename json_ostream<S>::underlying_stream;

  template<typename S>
  friend auto operator<<(json_ostream<S>&&, int) ->
      typename json_ostream<S>::underlying_stream;

  template<typename S>
  friend auto operator<<(json_ostream<S>&&, unsigned int) ->
      typename json_ostream<S>::underlying_stream;

  template<typename S>
  friend auto operator<<(json_ostream<S>&&, long) ->
      typename json_ostream<S>::underlying_stream;

  template<typename S>
  friend auto operator<<(json_ostream<S>&&, unsigned long) ->
      typename json_ostream<S>::underlying_stream;

  template<typename S>
  friend auto operator<<(json_ostream<S>&&, long long) ->
      typename json_ostream<S>::underlying_stream;

  template<typename S>
  friend auto operator<<(json_ostream<S>&&, unsigned long long) ->
      typename json_ostream<S>::underlying_stream;

  template<typename S>
  friend auto operator<<(json_ostream<S>&&, float) ->
      typename json_ostream<S>::underlying_stream;

  template<typename S>
  friend auto operator<<(json_ostream<S>&&, double) ->
      typename json_ostream<S>::underlying_stream;

  template<typename S>
  friend auto operator<<(json_ostream<S>&&, long double) ->
      typename json_ostream<S>::underlying_stream;

  template<typename S, typename String>
  friend auto operator<<(json_ostream<S>&&, const String&) ->
      enable_if_t<impl::is_string_emitable<json_ostream<S>, String>::value,
                  typename json_ostream<S>::underlying_stream>;

 public:
  template<typename S>
  friend auto operator<<(json_ostream<S>&&, nullptr_t) ->
      typename json_ostream<S>::underlying_stream;
  template<typename S>
  friend auto operator<<(json_ostream<S>&&, int) ->
      typename json_ostream<S>::underlying_stream;
  template<typename S>
  friend auto operator<<(json_ostream<S>&&, unsigned int) ->
      typename json_ostream<S>::underlying_stream;
  template<typename S>
  friend auto operator<<(json_ostream<S>&&, long) ->
      typename json_ostream<S>::underlying_stream;
  template<typename S>
  friend auto operator<<(json_ostream<S>&&, unsigned long) ->
      typename json_ostream<S>::underlying_stream;
  template<typename S>
  friend auto operator<<(json_ostream<S>&&, long long) ->
      typename json_ostream<S>::underlying_stream;
  template<typename S>
  friend auto operator<<(json_ostream<S>&&, unsigned long long) ->
      typename json_ostream<S>::underlying_stream;
  template<typename S, typename String, typename>
  friend auto operator<<(json_ostream<S>&&, const String&) ->
      typename json_ostream<S>::underlying_stream;

  using underlying_stream = UnderlyingStream;
  using basic_stream =
      typename remove_reference_t<underlying_stream>::basic_stream;

  json_ostream() noexcept = default;
  json_ostream(const json_ostream&) noexcept = delete;
  json_ostream(json_ostream&&) noexcept;
  json_ostream& operator=(const json_ostream&) noexcept = delete;
  json_ostream& operator=(json_ostream&&) noexcept;
  explicit json_ostream(add_rvalue_reference_t<underlying_stream>) noexcept;

  bool valid() const noexcept;
  basic_stream& get_ostream() const noexcept;
  void do_newline(size_t indent);

 private:
  optional<underlying_stream> out_;
};


template<typename S, typename T>
auto operator<<(basic_json_ostream_array<S>&, const T&) ->
    decltype(
        declval<json_ostream<basic_json_ostream_array<S>&>>()
        <<
        declval<const T&>());

template<typename S, typename T>
auto operator<<(json_ostream_array<S>&, const T&) ->
    decltype(
        declval<json_ostream<json_ostream_array<S>&>>()
        <<
        declval<const T&>());

template<typename S, typename T>
auto operator<<(json_ostream_array<S>&&, const T&) ->
    decltype(
        declval<json_ostream<json_ostream_array<S>>>()
        <<
        declval<const T&>());

template<typename S>
auto operator<<(json_ostream_array<S>&, begin_json_array_t) ->
    json_ostream_array<json_ostream_array<S>>;

template<typename S>
auto operator<<(basic_json_ostream_array<S>&, begin_json_array_t) ->
    json_ostream_array<basic_json_ostream_array<S>>;


/*
 * Abstract array node.
 *
 * Accepts multiple values.
 * Termination only possible using derivation.
 */
template<typename BasicStream>
class basic_json_ostream_array {
  template<typename S, typename T>
  friend auto operator<<(basic_json_ostream_array<S>&, const T&) ->
      decltype(
          declval<json_ostream<basic_json_ostream_array<S>&>>()
          <<
          declval<const T&>());

  template<typename S, typename T>
  friend auto operator<<(json_ostream_array<S>&, const T&) ->
      decltype(
          declval<json_ostream<json_ostream_array<S>&>>()
          <<
          declval<const T&>());

  template<typename S, typename T>
  friend auto operator<<(json_ostream_array<S>&&, const T&) ->
      decltype(
          declval<json_ostream<json_ostream_array<S>>>()
          <<
          declval<const T&>());

 public:
  using basic_stream = BasicStream;

 protected:
  basic_json_ostream_array() noexcept = default;
  basic_json_ostream_array(const basic_json_ostream_array&) = default;
  basic_json_ostream_array& operator=(const basic_json_ostream_array&) =
      default;

 public:
  virtual ~basic_json_ostream_array() noexcept;

  virtual bool valid() const noexcept = 0;
  virtual basic_stream& get_ostream() const noexcept = 0;
  virtual void do_newline(size_t) = 0;

  template<typename Iter>
  basic_json_ostream_array& put_collection(Iter, Iter);
  template<typename C>
  basic_json_ostream_array& put_collection(const C&);

  bool empty() const noexcept { return first_; }

 private:
  bool first_ = true;
};

template<typename S>
auto operator<<(json_ostream<S>&&, begin_json_array_t) ->
    json_ostream_array<S>;

template<typename S>
auto operator<<(json_ostream_array<S>&&, end_json_array_t) ->
    typename json_ostream_array<S>::underlying_stream;

/*
 * Array node.
 *
 * Accept multiple values.
 * Terminates with json_array_end.
 */
template<typename UnderlyingStream>
class json_ostream_array
: public basic_json_ostream_array<
      typename remove_reference_t<UnderlyingStream>::basic_stream>
{
  template<typename S>
  friend auto operator<<(json_ostream_array<S>&&, end_json_array_t) ->
      typename json_ostream_array<S>::underlying_stream;

 public:
  using underlying_stream = UnderlyingStream;
  using basic_stream =
      typename remove_reference_t<underlying_stream>::basic_stream;

  json_ostream_array() noexcept = default;
  json_ostream_array(const json_ostream_array&) = delete;
  json_ostream_array(json_ostream_array&&) noexcept;
  json_ostream_array& operator=(const json_ostream_array&) = delete;
  json_ostream_array& operator=(json_ostream_array&&) noexcept;
  explicit json_ostream_array(json_ostream<underlying_stream>&&) noexcept;
  ~json_ostream_array() noexcept override;

  bool valid() const noexcept override;
  basic_stream& get_ostream() const noexcept override;
  void do_newline(size_t) override;

  template<typename Iter>
  json_ostream_array& put_collection(Iter, Iter);
  template<typename C>
  json_ostream_array& put_collection(const C&);

 private:
  optional<underlying_stream> out_;
  bool terminated_ = false;
};


template<typename S, typename C, typename T>
auto operator<<(json_ostream_object<S>&, const json_key_t<C, T>&) ->
    json_ostream<json_ostream_object<S>&>;

template<typename S, typename C, typename T>
auto operator<<(json_ostream_object_impl<S>&, const json_key_t<C, T>&) ->
    json_ostream<json_ostream_object_impl<S>&>;

template<typename S, typename C, typename T>
auto operator<<(json_ostream_object_impl<S>&&, const json_key_t<C, T>&) ->
    json_ostream<json_ostream_object_impl<S>>;

/*
 * Abstract object node.
 *
 * Accepts keys.
 * Termination only possible using derivation.
 */
template<typename BasicStream>
class json_ostream_object {
  template<typename S, typename C, typename T>
  friend auto operator<<(json_ostream_object<S>&, const json_key_t<C, T>&) ->
      json_ostream<json_ostream_object<S>&>;

  template<typename S, typename C, typename T>
  friend auto operator<<(json_ostream_object_impl<S>&,
                         const json_key_t<C, T>&) ->
      json_ostream<json_ostream_object_impl<S>&>;

  template<typename S, typename C, typename T>
  friend auto operator<<(json_ostream_object_impl<S>&&,
                         const json_key_t<C, T>&) ->
      json_ostream<json_ostream_object_impl<S>>;

 public:
  using basic_stream = BasicStream;

 protected:
  json_ostream_object() noexcept = default;
  json_ostream_object(const json_ostream_object&) = delete;
  json_ostream_object(json_ostream_object&&) noexcept;
  json_ostream_object& operator=(const json_ostream_object&) = delete;
  json_ostream_object& operator=(json_ostream_object&&) noexcept;
  virtual ~json_ostream_object() noexcept;

 public:
  virtual bool valid() const noexcept = 0;
  virtual basic_stream& get_ostream() const noexcept = 0;
  virtual void do_newline(size_t) = 0;

  bool empty() const noexcept { return first_; }

 private:
  bool first_ = true;
};

/* Begin json object serialization. */
template<typename S>
auto operator<<(json_ostream<S>&& out, begin_json_object_t) ->
    json_ostream_object_impl<S>;

/* End json object serialization. */
template<typename S>
auto operator<<(json_ostream_object_impl<S>&& out, end_json_object_t) ->
    typename json_ostream_object_impl<S>::underlying_stream;

/*
 * Object node.
 *
 * Accepts keys.
 * Terminates with json_object_end.
 */
template<typename UnderlyingStream>
class json_ostream_object_impl
: public json_ostream_object<
      typename remove_reference_t<UnderlyingStream>::basic_stream>
{
  template<typename S>
  friend auto operator<<(json_ostream_object_impl<S>&& out,
                         end_json_object_t) ->
      typename json_ostream_object_impl<S>::underlying_stream;

 public:
  using underlying_stream = UnderlyingStream;
  using basic_stream =
      typename remove_reference_t<underlying_stream>::basic_stream;

  json_ostream_object_impl() noexcept = default;
  json_ostream_object_impl(const json_ostream_object_impl&) = delete;
  json_ostream_object_impl(json_ostream_object_impl&&) noexcept;
  json_ostream_object_impl& operator=(const json_ostream_object_impl&) =
      delete;
  json_ostream_object_impl& operator=(json_ostream_object_impl&&) noexcept;
  explicit json_ostream_object_impl(json_ostream<underlying_stream>&&)
      noexcept;
  ~json_ostream_object_impl() noexcept override;

  bool valid() const noexcept override;
  basic_stream& get_ostream() const noexcept override;
  void do_newline(size_t) override;

 private:
  optional<underlying_stream> out_;
  bool terminated_ = false;
};

template<typename Char, typename Traits>
class json_key_t {
 public:
  explicit json_key_t(basic_string_ref<Char, Traits>);
  template<typename A> explicit json_key_t(basic_string<Char, Traits, A>&&);

  basic_string_ref<Char, Traits> key() const noexcept { return key_; }

 private:
  basic_string<Char, Traits> key_;
};

template<typename Char, typename Traits>
json_key_t<Char, Traits> json_key(basic_string_ref<Char, Traits>);
template<typename Char, typename Traits, typename A>
json_key_t<Char, Traits> json_key(const basic_string<Char, Traits, A>&);
template<typename Char, typename Traits, typename A>
json_key_t<Char, Traits> json_key(basic_string<Char, Traits, A>&&);
template<typename Char>
json_key_t<Char> json_key(const Char*);


/* Use object serialization if it is defined. */
template<typename S, typename T>
auto operator<<(json_ostream<S>&&, const T&) ->
    enable_if_t<impl::is_object_emitable<json_ostream<S>&, T>::value,
                S>;


/* Use array serialization if it is defined. */
template<typename S, typename T>
auto operator<<(json_ostream<S>&&, const T&) ->
    enable_if_t<impl::is_array_emitable<json_ostream<S>&, T>::value,
                S>;


}} /* namespace ilias::json */

#include "json-inl.h"

#endif /* _ILIAS_TEST_JSON_H_ */
