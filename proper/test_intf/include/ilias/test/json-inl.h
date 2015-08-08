#ifndef _ILIAS_TEST_JSON_INL_H_
#define _ILIAS_TEST_JSON_INL_H_

#include "json.h"
#include <algorithm>
#include <iterator>
#include <locale>

namespace ilias {
namespace json {
namespace impl {


template<typename OChar, typename OTraits, typename SChar, typename STraits,
         typename Codecvt, typename Buf>
auto emit_json_string_cvt_helper(basic_ostream<OChar, OTraits>& out,
                                 basic_string_ref<SChar, STraits> s,
                                 const Codecvt& codec, Buf& buf) -> void {
  using state_type = typename Codecvt::state_type;
  using codec_result_type = typename Codecvt::result;

  state_type st;
  __builtin_bzero(&st, sizeof(st));
  auto buf_tail = begin(buf);
  auto s_begin = s.begin();

  codec_result_type r;
  for (r = codec.out(st, s_begin, s.end(), s_begin,
                     buf_tail, buf.end(), buf_tail);
       r != codec_result_type::error && s_begin != s.end();
       r = codec.out(st, s_begin, s.end(), s_begin,
                     buf_tail, buf.end(), buf_tail)) {
    switch (r) {
    case codec_result_type::noconv:
      {
        /*
         * Use buffer to convert without encoding change.
         * This usually only happens if a type change is required.
         */
        const auto l = min(size_t(buf.end() - buf_tail),
                           size_t(s.end() - s_begin));
        copy_n(s_begin, l, buf_tail);
        buf_tail += l;
        s_begin += l;
      }
      /* FALLTHROUGH */
    case codec_result_type::ok:
    case codec_result_type::partial:
      out << basic_string_ref<OChar, OTraits>(buf.begin(),
                                              buf_tail - buf.begin());
      buf_tail = begin(buf);
      break;
    case codec_result_type::error:
      break;
    }
  }

  switch (r) {
  case codec_result_type::noconv:
  case codec_result_type::ok:
    break;
  case codec_result_type::partial:
  case codec_result_type::error:
    throw range_error("codecvt: conversion failure");
  }

  assert(s_begin == s.end());
}

template<typename OChar, typename OTraits, typename SChar, typename STraits>
auto emit_json_string_cvt_helper(basic_ostream<OChar, OTraits>& out,
                                 basic_string_ref<SChar, STraits> s) -> void {
  using codecvt_type = codecvt<SChar, OChar, mbstate_t>;
  const codecvt_type& codec = use_facet<codecvt_type>(out.getloc());
  array<OChar, 64> buf;

  emit_json_string_cvt_helper(out, s, codec, buf);
}

template<typename OChar, typename OTraits, typename SChar, typename STraits>
auto emit_json_string(basic_ostream<OChar, OTraits>& out,
                      basic_string_ref<SChar, STraits> s) ->
    basic_ostream<OChar, OTraits>& {
  using codecvt_type = codecvt<SChar, OChar, mbstate_t>;
  const codecvt_type& codec = use_facet<codecvt_type>(out.getloc());
  array<OChar, 64> buf;

  SChar newline = '\n';
  SChar quote = '"';
  SChar backslash = '\\';
  const array<SChar, 3> special_chars{{
    newline,
    quote,
    backslash
  }};
  const basic_string_ref<SChar, STraits> special =
      basic_string_ref<SChar, STraits>(special_chars.data(),
                                       special_chars.size());

  const OChar out_backslash = out.widen('\\');
  const OChar out_quote = out.widen('"');
  const array<OChar, 2> newline_escape = {{
    out_backslash,
    out.widen('n')
  }};
  const array<OChar, 2> quote_escape = {{
    out_backslash,
    out_quote
  }};
  const array<OChar, 2> backslash_escape = {{
    out_backslash,
    out_backslash
  }};

  out.put(out_quote);

  for (auto i = s.find_first_of(special);
       i != basic_string_ref<SChar, STraits>::npos;
       i = (s = s.substr(i + 1)).find_first_of(special)) {
    emit_json_string_cvt_helper(out, s.substr(0, i), codec, buf);
    if (s[i] == newline) {
      out << basic_string_ref<OChar, OTraits>(newline_escape.data(),
                                              newline_escape.size());
    } else if (s[i] == quote) {
      out << basic_string_ref<OChar, OTraits>(quote_escape.data(),
                                              quote_escape.size());
    } else if (s[i] == backslash) {
      out << basic_string_ref<OChar, OTraits>(backslash_escape.data(),
                                              backslash_escape.size());
    }
  }

  emit_json_string_cvt_helper(out, s, codec, buf);
  return out.put(out_quote);
}

template<typename OChar, typename OTraits>
auto emit_json_string(basic_ostream<OChar, OTraits>& out, const char* s) ->
    basic_ostream<OChar, OTraits>& {
  return emit_json_string(out, string_ref(s));
}

template<typename OChar, typename OTraits>
auto emit_json_string(basic_ostream<OChar, OTraits>& out, const wchar_t* s) ->
    basic_ostream<OChar, OTraits>& {
  return emit_json_string(out, wstring_ref(s));
}

template<typename OChar, typename OTraits>
auto emit_json_string(basic_ostream<OChar, OTraits>& out, const char16_t* s) ->
    basic_ostream<OChar, OTraits>& {
  return emit_json_string(out, u16string_ref(s));
}

template<typename OChar, typename OTraits>
auto emit_json_string(basic_ostream<OChar, OTraits>& out, const char32_t* s) ->
    basic_ostream<OChar, OTraits>& {
  return emit_json_string(out, u32string_ref(s));
}

template<typename OChar, typename OTraits, typename SChar, typename STraits,
         typename A>
auto emit_json_string(basic_ostream<OChar, OTraits>& out,
                      const basic_string<SChar, STraits, A>& s) ->
    basic_ostream<OChar, OTraits>& {
  return emit_json_string(out, basic_string_ref<SChar, STraits>(s));
}


template<typename, typename, typename>
struct _is_string_emitable {
  using type = false_type;
};
/* Specialize for string emitable types. */
template<typename Out, typename String>
struct _is_string_emitable<
    Out, String,
    decltype(emit_json_string(declval<Out>().get_ostream(),
                              declval<const String&>()), 0)> {
  using type = true_type;
};


template<typename, typename, typename>
struct _is_array_emitable {
  using type = false_type;
};
/* Specialize for array emitable types. */
template<typename Out, typename C>
struct _is_array_emitable<
    Out, C,
    decltype(is_base_of<forward_iterator_tag,
                        typename iterator_traits<
                            decltype(std::begin(declval<const C&>()))
                          >::iterator_category
                       >(),
             0)> {
  using iterator = decltype(std::begin(declval<const C&>()));
  using iter_cat = typename iterator_traits<iterator>::iterator_category;
  static constexpr bool is_forward_iter_ =
      is_base_of<std::forward_iterator_tag, iter_cat>::value;
  static constexpr bool is_string_emitable_ =
      is_string_emitable<Out, C>::value;
  static constexpr bool is_object_emitable_ =
      is_object_emitable<Out, C>::value;

  /*
   * We want to select all collection types,
   * except for strings.
   */
  using type = integral_constant<bool, (is_forward_iter_ &&
                                        !is_string_emitable_ &&
                                        !is_object_emitable_)>;
};


template<typename, typename, typename>
struct _is_object_emitable {
  using type = false_type;
};
/* Specialize for object emitable types. */
template<typename Out, typename Obj>
struct _is_object_emitable<
    Out, Obj,
    decltype(declval<json_ostream_object<
                 typename remove_reference_t<Out>::basic_stream>&>() <<
                      declval<const Obj&>(),
             0)> {
  using type = true_type;
};


} /* namespace ilias::test::impl */


constexpr begin_json_t::begin_json_t(size_t indent_lvl) noexcept
: indent_lvl(indent_lvl)
{}

constexpr auto begin_json_t::operator()(size_t indent_lvl) const noexcept ->
    begin_json_t {
  return begin_json_t(indent_lvl);
}


template<typename Char, typename Traits>
auto operator<<(json_term_ostream<Char, Traits>&& out, end_json_t)
    noexcept -> basic_ostream<Char, Traits>& {
  assert(out.valid());
  return *exchange(out.out_, nullptr);
}

template<typename Char, typename Traits>
auto operator<<(basic_ostream<Char, Traits>& out, begin_json_t b) noexcept ->
    json_ostream<json_term_ostream<Char, Traits>> {
  using terminal = json_term_ostream<Char, Traits>;
  using initial = json_ostream<terminal>;

  return initial(terminal(out, b.indent_lvl));
}

template<typename S>
auto operator<<(json_ostream<S>&& out, nullptr_t) ->
    typename json_ostream<S>::underlying_stream {
  using impl::emit_json_string_cvt_helper;

  emit_json_string_cvt_helper(out.get_ostream(), string_ref("null", 4));
  return out.out_.release();
}

template<typename S>
auto operator<<(json_ostream<S>&& out, bool v) ->
    typename json_ostream<S>::underlying_stream {
  using impl::emit_json_string_cvt_helper;

  emit_json_string_cvt_helper(out.get_ostream(),
                              (v ? string_ref("true", 4) :
                                   string_ref("false", 5)));
  return out.out_.release();
}

template<typename S>
auto operator<<(json_ostream<S>&& out, int v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;
  return out.out_.release();
}

template<typename S>
auto operator<<(json_ostream<S>&& out, unsigned int v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;
  return out.out_.release();
}

template<typename S>
auto operator<<(json_ostream<S>&& out, long v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;
  return out.out_.release();
}

template<typename S>
auto operator<<(json_ostream<S>&& out, unsigned long v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;
  return out.out_.release();
}

template<typename S>
auto operator<<(json_ostream<S>&& out, long long v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;
  return out.out_.release();
}

template<typename S>
auto operator<<(json_ostream<S>&& out, unsigned long long v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;
  return out.out_.release();
}

template<typename S>
auto operator<<(json_ostream<S>&& out, float v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;  // XXX adapt precision?
  return out.out_.release();
}

template<typename S>
auto operator<<(json_ostream<S>&& out, double v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;  // XXX adapt precision?
  return out.out_.release();
}

template<typename S>
auto operator<<(json_ostream<S>&& out, long double v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;  // XXX adapt precision?
  return out.out_.release();
}

template<typename S, typename String>
auto operator<<(json_ostream<S>&& out, const String& v) ->
    enable_if_t<impl::is_string_emitable<json_ostream<S>, String>::value,
                typename json_ostream<S>::underlying_stream> {
  impl::emit_json_string(out.get_ostream(), v);
  return out.out_.release();
}


template<typename Char, typename Traits>
json_term_ostream<Char, Traits>::json_term_ostream(
    basic_ostream<Char, Traits>& out, size_t indent) noexcept
: out_(&out),
  indent_(indent)
{}

template<typename Char, typename Traits>
json_term_ostream<Char, Traits>::json_term_ostream(json_term_ostream&& other)
    noexcept
: out_(exchange(other.out_, nullptr)),
  indent_(exchange(other.indent_, 0))
{}

template<typename Char, typename Traits>
auto json_term_ostream<Char, Traits>::operator=(json_term_ostream&& other)
    noexcept -> json_term_ostream& {
  out_ = exchange(other.out_, nullptr);
  indent_ = exchange(other.indent_, 0);
  return *this;
}

template<typename Char, typename Traits>
auto json_term_ostream<Char, Traits>::get_ostream() const noexcept ->
    basic_stream& {
  assert(out_ != nullptr);
  return *out_;
}

template<typename Char, typename Traits>
auto json_term_ostream<Char, Traits>::do_newline(size_t indent) ->
    void {
  assert(valid());

  out_->put(out_->widen('\n'));
  const auto spc = out_->widen(' ');

  for (size_t i = 0; i < indent + indent_; ++i) out_->put(spc);
}


template<typename UnderlyingStream>
json_ostream<UnderlyingStream>::json_ostream(json_ostream&& other) noexcept
: out_(move(other.out_))
{}

template<typename UnderlyingStream>
auto json_ostream<UnderlyingStream>::operator=(json_ostream&& other)
    noexcept -> json_ostream& {
  out_ = move(other.out_);
  return *this;
}

template<typename UnderlyingStream>
json_ostream<UnderlyingStream>::json_ostream(
    add_rvalue_reference_t<underlying_stream> s) noexcept
: out_(forward<add_rvalue_reference_t<underlying_stream>>(s))
{}

template<typename UnderlyingStream>
auto json_ostream<UnderlyingStream>::valid() const noexcept -> bool {
  return out_.is_present() && out_->valid();
}

template<typename UnderlyingStream>
auto json_ostream<UnderlyingStream>::get_ostream() const noexcept ->
    basic_stream& {
  assert(out_.is_present());
  return out_->get_ostream();
}

template<typename UnderlyingStream>
auto json_ostream<UnderlyingStream>::do_newline(size_t indent) -> void {
  if (out_.is_present()) out_->do_newline(indent);
}


template<typename S, typename T>
auto operator<<(basic_json_ostream_array<S>& out, const T& v) ->
    decltype(
        declval<json_ostream<basic_json_ostream_array<S>&>>()
        <<
        declval<const T&>()) {
  using basic_stream = typename basic_json_ostream_array<S>::basic_stream;

  assert(out.valid());

  // Emit a comma separator.
  if (!exchange(out.first_, false)) {
    basic_stream& o = out.get_ostream();
    o.put(o.widen(','));
  }
  out.do_newline(0);

  // Emit value.
  return json_ostream<basic_json_ostream_array<S>&>(out) << v;
}

template<typename S, typename T>
auto operator<<(json_ostream_array<S>& out, const T& v) ->
    decltype(
        declval<json_ostream<json_ostream_array<S>&>>()
        <<
        declval<const T&>()) {
  using basic_stream = typename json_ostream_array<S>::basic_stream;

  assert(out.valid());

  // Emit a comma separator.
  if (!exchange(out.first_, false)) {
    basic_stream& o = out.get_ostream();
    o.put(o.widen(','));
  }
  out.do_newline(0);

  // Emit value.
  return json_ostream<json_ostream_array<S>&>(out) << v;
}

template<typename S, typename T>
auto operator<<(json_ostream_array<S>&& out, const T& v) ->
    decltype(
        declval<json_ostream<basic_json_ostream_array<S>>>()
        <<
        declval<const T&>()) {
  using basic_stream = typename json_ostream_array<S>::basic_stream;

  assert(out.valid());

  // Emit a comma separator.
  if (!exchange(out.first_, false)) {
    basic_stream& o = out.get_ostream();
    o.put(o.widen(','));
  }
  out.do_newline(0);

  // Emit value.
  return json_ostream<basic_json_ostream_array<S>>(move(out)) << v;
}


template<typename BasicStream>
basic_json_ostream_array<BasicStream>::~basic_json_ostream_array() noexcept {}

template<typename BasicStream>
template<typename Iter>
auto basic_json_ostream_array<BasicStream>::put_collection(Iter b, Iter e) ->
    basic_json_ostream_array& {
  assert(valid());
  for_each(b, e,
           [this](typename iterator_traits<Iter>::reference elem) {
             *this << elem;
           });
  return *this;
}

template<typename BasicStream>
template<typename C>
auto basic_json_ostream_array<BasicStream>::put_collection(const C& c) ->
    basic_json_ostream_array& {
  return put_collection(begin(c), end(c));
}


template<typename S>
auto operator<<(json_ostream<S>&& out, begin_json_array_t) ->
    json_ostream_array<S> {
  assert(out.valid());

  auto rv = json_ostream_array<S>(move(out));
  auto& o = rv.get_ostream();
  o.put(o.widen('['));
  return rv;
}

template<typename S>
auto operator<<(json_ostream_array<S>&& out, end_json_array_t) ->
    typename json_ostream_array<S>::underlying_stream {
  assert(out.valid());

  if (!out.empty()) out.out_->do_newline(0);
  out.terminated_ = true;
  auto& o = out.get_ostream();
  o.put(o.widen(']'));
  return out.out_.release();
}


template<typename UnderlyingStream>
json_ostream_array<UnderlyingStream>::json_ostream_array(
    json_ostream_array&& other) noexcept
: basic_json_ostream_array<basic_stream>(move(*this)),
  out_(move(other.out_)),
  terminated_(exchange(other.terminated_, true))
{}

template<typename UnderlyingStream>
auto json_ostream_array<UnderlyingStream>::operator=(
    json_ostream_array&& other) noexcept -> json_ostream_array& {
  this->basic_json_ostream_array<basic_stream>::operator=(move(other));
  out_ = move(other.out_);
  terminated_ = exchange(other.terminated_, true);
  return *this;
}

template<typename UnderlyingStream>
json_ostream_array<UnderlyingStream>::json_ostream_array(
    json_ostream<underlying_stream>&& s) noexcept
: out_(move(s.out_))
{}

template<typename UnderlyingStream>
json_ostream_array<UnderlyingStream>::~json_ostream_array() noexcept {
  if (!terminated_ && valid()) {
    auto& o = get_ostream();
    o.put(o.widen(']'));
  }
}

template<typename UnderlyingStream>
auto json_ostream_array<UnderlyingStream>::valid() const noexcept ->
    bool {
  return out_.is_present() && out_->valid();
}

template<typename UnderlyingStream>
auto json_ostream_array<UnderlyingStream>::get_ostream() const noexcept ->
    basic_stream& {
  return out_->get_ostream();
}

template<typename UnderlyingStream>
auto json_ostream_array<UnderlyingStream>::do_newline(size_t indent) -> void {
  out_->do_newline(indent + 2);
}

template<typename UnderlyingStream>
template<typename Iter>
auto json_ostream_array<UnderlyingStream>::put_collection(Iter b, Iter e) ->
    json_ostream_array& {
  this->basic_json_ostream_array<basic_stream>::put_collection(b, e);
  return *this;
}

template<typename UnderlyingStream>
template<typename C>
auto json_ostream_array<UnderlyingStream>::put_collection(const C& c) ->
    json_ostream_array& {
  this->basic_json_ostream_array<basic_stream>::put_collection(c);
  return *this;
}


template<typename S, typename C, typename T>
auto operator<<(json_ostream_object<S>& out, const json_key_t<C, T>& key) ->
    json_ostream<json_ostream_object<S>&> {
  using basic_stream = typename json_ostream_object<S>::basic_stream;

  assert(out.valid());
  basic_stream& out_ostream = out.get_ostream();

  // Emit comma separator.
  if (!exchange(out.first_, false))
    out_ostream.put(out_ostream.widen(','));
  out.do_newline(0);

  // Emit key.
  impl::emit_json_string(out_ostream, key.key()) << out_ostream.widen(':') <<
                                                    out_ostream.widen(' ');
  return json_ostream<json_ostream_object<S>&>(out);
}

template<typename S, typename C, typename T>
auto operator<<(json_ostream_object_impl<S>& out,
                const json_key_t<C, T>& key) ->
    json_ostream<json_ostream_object_impl<S>&> {
  using basic_stream = typename json_ostream_object_impl<S>::basic_stream;

  assert(out.valid());
  basic_stream& out_ostream = out.get_ostream();

  // Emit comma separator.
  if (!exchange(out.first_, false))
    out_ostream.put(out_ostream.widen(','));
  out.do_newline(0);

  // Emit key.
  impl::emit_json_string(out_ostream, key.key()) << out_ostream.widen(':') <<
                                                    out_ostream.widen(' ');
  return json_ostream<json_ostream_object_impl<S>&>(out);
}

template<typename S, typename C, typename T>
auto operator<<(json_ostream_object_impl<S>&& out,
                const json_key_t<C, T>& key) ->
    json_ostream<json_ostream_object_impl<S>> {
  using basic_stream = typename json_ostream_object_impl<S>::basic_stream;

  assert(out.valid());
  basic_stream& out_ostream = out.get_ostream();

  // Emit comma separator.
  if (!exchange(out.first_, false))
    out_ostream.put(out_ostream.widen(','));
  out.do_newline(0);

  // Emit key.
  impl::emit_json_string(out_ostream, key.key()) << out_ostream.widen(':') <<
                                                    out_ostream.widen(' ');
  return json_ostream<json_ostream_object_impl<S>>(move(out));
}


template<typename BasicStream>
json_ostream_object<BasicStream>::json_ostream_object(
    json_ostream_object&& other) noexcept
: first_(exchange(other.first_, false))
{}

template<typename BasicStream>
auto json_ostream_object<BasicStream>::operator=(json_ostream_object&& other)
    noexcept -> json_ostream_object& {
  first_ = exchange(other.first_, false);
  return *this;
}

template<typename BasicStream>
json_ostream_object<BasicStream>::~json_ostream_object() noexcept {}


template<typename S>
auto operator<<(json_ostream<S>&& out, begin_json_object_t) ->
    json_ostream_object_impl<S> {
  assert(out.valid());

  auto rv = json_ostream_object_impl<S>(move(out));
  auto& o = rv.get_ostream();
  o.put(o.widen('{'));
  return rv;
}

template<typename S>
auto operator<<(json_ostream_object_impl<S>&& out, end_json_object_t) ->
    typename json_ostream_object_impl<S>::underlying_stream {
  assert(out.valid());

  if (!out.empty()) out.out_->do_newline(0);
  out.terminated_ = true;
  auto& o = out.get_ostream();
  o.put(o.widen('}'));
  return out.out_.release();
}


template<typename UnderlyingStream>
json_ostream_object_impl<UnderlyingStream>::json_ostream_object_impl(
    json_ostream_object_impl&& other) noexcept
: json_ostream_object<basic_stream>(move(other)),
  out_(move(other.out_)),
  terminated_(other.terminated_)
{}

template<typename UnderlyingStream>
auto json_ostream_object_impl<UnderlyingStream>::operator=(
    json_ostream_object_impl&& other) noexcept -> json_ostream_object_impl& {
  this->json_ostream_object<basic_stream>::operator=(move(other));
  out_ = move(other.out_);
  terminated_ = other.terminated_;
  return *this;
}

template<typename UnderlyingStream>
json_ostream_object_impl<UnderlyingStream>::json_ostream_object_impl(
    json_ostream<underlying_stream>&& s) noexcept
: out_(move(s.out_))
{}

template<typename UnderlyingStream>
json_ostream_object_impl<UnderlyingStream>::~json_ostream_object_impl()
    noexcept {
  if (!terminated_ && valid()) {
    auto& o = get_ostream();
    o.put(o.widen('}'));
  }
}

template<typename UnderlyingStream>
auto json_ostream_object_impl<UnderlyingStream>::valid() const noexcept ->
    bool {
  return out_.is_present() && out_->valid();
}

template<typename UnderlyingStream>
auto json_ostream_object_impl<UnderlyingStream>::get_ostream()
    const noexcept -> basic_stream& {
  return out_->get_ostream();
}

template<typename UnderlyingStream>
auto json_ostream_object_impl<UnderlyingStream>::do_newline(size_t indent) ->
    void {
  return out_->do_newline(indent + 2);
}


template<typename Char, typename Traits>
json_key_t<Char, Traits>::json_key_t(basic_string_ref<Char, Traits> k)
: key_(k)
{}

template<typename Char, typename Traits>
template<typename A>
json_key_t<Char, Traits>::json_key_t(basic_string<Char, Traits, A>&& k)
: key_(move(k))
{}

template<typename Char, typename Traits>
auto json_key(basic_string_ref<Char, Traits> k) -> json_key_t<Char, Traits> {
  return json_key_t<Char, Traits>(k);
}

template<typename Char, typename Traits, typename A>
auto json_key(const basic_string<Char, Traits, A>& k) ->
    json_key_t<Char, Traits> {
  return json_key_t<Char, Traits>(k);
}

template<typename Char, typename Traits, typename A>
auto json_key(basic_string<Char, Traits, A>&& k) ->
    json_key_t<Char, Traits> {
  return json_key_t<Char, Traits>(move(k));
}

template<typename Char>
auto json_key(const Char* k) ->
    json_key_t<Char> {
  return json_key_t<Char>(k);
}


template<typename S, typename T>
auto operator<<(json_ostream<S>&& out, const T& v) ->
    enable_if_t<impl::is_object_emitable<json_ostream<S>&, T>::value,
                S> {
  auto objstream = move(out) << begin_json_object;
  objstream << v;
  return move(objstream) << end_json_object;
}


template<typename S, typename T>
auto operator<<(json_ostream<S>&& out, const T& v) ->
    enable_if_t<impl::is_array_emitable<json_ostream<S>&, T>::value,
                S> {
  auto arr = move(out) << begin_json_array;
  arr.put_collection(v);
  return move(arr) << end_json_array;
}


}} /* namespace ilias::test */

#endif /* _ILIAS_TEST_JSON_INL_H_ */
