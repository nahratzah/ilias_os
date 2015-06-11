#ifndef _ILIAS_TEST_JSON_INL_H_
#define _ILIAS_TEST_JSON_INL_H_

#include "json.h"

namespace ilias {
namespace test {


constexpr begin_json_t::begin_json_t(size_t indent_lvl) noexcept
: indent_lvl(indent_lvl)
{}

constexpr auto begin_json_t::operator()(size_t indent_lvl) const noexcept ->
    begin_json_t {
  return begin_json_t(indent_lvl);
}


template<typename OChar, typename OTraits, typename SChar, typename STraits>
auto emit_json_string(basic_ostream<OChar, OTraits>& out,
                      basic_string_ref<SChar, STraits> s) ->
    basic_ostream<OChar, OTraits>& {
  SChar newline = '\n';
  SChar quote = '"';
  const array<SChar, 2> special_chars{{
    newline,
    quote
  }};
  const basic_string_ref<SChar, STraits> special =
      basic_string_ref<SChar, STraits>(special_chars.data(),
                                       special_chars.size());

  const array<OChar, 2> newline_escape = {{
    out.widen('\\'),
    out.widen('n')
  }};
  const array<OChar, 2> quote_escape = {{
    out.widen('\\'),
    out.widen('"')
  }};
  const OChar out_quote = out.widen('"');

  out.put(out_quote);

  for (auto i = s.find_first_of(special_chars);
       i != basic_string_ref<SChar, STraits>::npos;
       i = (s = s.substr(i + 1)).find_first_of(special_chars)) {
    out << s.substr(0, i);
    if (s[i] == newline) {
      out << basic_string_ref<OChar, OTraits>(newline_escape.data(),
                                              newline_escape.size());
    } else if (s[i] == quote) {
      out << basic_string_ref<OChar, OTraits>(quote_escape.data(),
                                              quote_escape.size());
    }
  }

  return (out << s).put(out_quote);
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


template<typename Char, typename Traits>
auto operator<<(json_ostream<basic_ostream<Char, Traits>>&& out, end_json_t)
    noexcept -> basic_ostream<Char, Traits>& {
  assert(out.valid());
  return *exchange(out.out_, nullptr);
}

template<typename Char, typename Traits>
auto operator<<(basic_ostream<Char, Traits>& out, begin_json_t b) noexcept ->
    json_ostream<json_ostream<basic_ostream<Char, Traits>>> {
  using terminal = json_ostream<basic_ostream<Char, Traits>>;
  using initial = json_ostream<terminal>;

  return initial(terminal(out, b.indent_lvl));
}

template<typename S>
auto operator<<(json_ostream<S>&& out, nullptr_t) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << "null";
  return move(out.out_);
}

template<typename S>
auto operator<<(json_ostream<S>&& out, int v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;
  return move(out.out_);
}

template<typename S>
auto operator<<(json_ostream<S>&& out, unsigned int v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;
  return move(out.out_);
}

template<typename S>
auto operator<<(json_ostream<S>&& out, long v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;
  return move(out.out_);
}

template<typename S>
auto operator<<(json_ostream<S>&& out, unsigned long v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;
  return move(out.out_);
}

template<typename S>
auto operator<<(json_ostream<S>&& out, long long v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;
  return move(out.out_);
}

template<typename S>
auto operator<<(json_ostream<S>&& out, unsigned long long v) ->
    typename json_ostream<S>::underlying_stream {
  out.get_ostream() << v;
  return move(out.out_);
}


template<typename Char, typename Traits>
json_ostream<basic_ostream<Char, Traits>>::json_ostream(
    basic_ostream<Char, Traits>& out, size_t indent) noexcept
: out_(&out),
  indent_(indent)
{}

template<typename Char, typename Traits>
json_ostream<basic_ostream<Char, Traits>>::json_ostream(json_ostream&& other)
    noexcept
: out_(exchange(other.out_, nullptr)),
  indent_(exchange(other.indent_, 0))
{}

template<typename Char, typename Traits>
auto json_ostream<basic_ostream<Char, Traits>>::operator=(json_ostream&& other)
    noexcept -> json_ostream& {
  out_ = exchange(other.out_, nullptr);
  indent_ = exchange(other.indent_, 0);
  return *this;
}

template<typename Char, typename Traits>
auto json_ostream<basic_ostream<Char, Traits>>::get_ostream() const noexcept ->
    basic_stream& {
  assert(out_ != nullptr);
  return *out_;
}

template<typename Char, typename Traits>
auto json_ostream<basic_ostream<Char, Traits>>::do_newline(size_t indent) ->
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
: out_(move(s))
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
    enable_if_t<!is_void<
                    decltype(
                        declval<json_ostream<basic_json_ostream_array<S>&>>()
                        <<
                        declval<const T&>())>::value,
                basic_json_ostream_array<S>&> {
  using basic_stream = typename basic_json_ostream_array<S>::basic_stream;

  assert(out.valid());

  // Emit a comma separator.
  if (!exchange(out.first_, false)) {
    basic_stream& o = out.get_ostream();
    o.put(o.widen(','));
  }
  out.do_newline(0);

  // Emit value.
  json_ostream<basic_json_ostream_array<S>&>(out) << v;
  return out;
}

template<typename S, typename T>
auto operator<<(json_ostream_array<S>& out, const T& v) ->
    enable_if_t<!is_void<
                    decltype(
                        declval<basic_json_ostream_array<
                            typename json_ostream_array<S>::basic_stream>&>()
                        <<
                        declval<const T&>())>::value,
                json_ostream_array<S>&> {
  using basic_stream = typename json_ostream_array<S>::basic_stream;

  basic_json_ostream_array<basic_stream>& basic_this = out;
  basic_this << v;
  return out;
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
auto operator<<(json_ostream_array<S>& out, end_json_array_t) ->
    typename json_ostream_array<S>::underlying_stream {
  assert(out.valid());

  if (!out.empty()) out.out_.do_newline(0);
  out.terminated_ = true;
  auto& o = out.get_ostream();
  o.put(o.widen(']'));
  return move(out.out_);
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
  return out_.valid();
}

template<typename UnderlyingStream>
auto json_ostream_array<UnderlyingStream>::get_ostream() const noexcept ->
    basic_stream& {
  return out_.get_ostream();
}

template<typename UnderlyingStream>
auto json_ostream_array<UnderlyingStream>::do_newline(size_t indent) -> void {
  out_.do_newline(indent + 2);
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
auto operator<<(json_ostream_object_impl<S>& out, end_json_object_t) ->
    typename json_ostream_object<S>::underlying_stream {
  assert(out.valid());

  if (!out.empty()) out.out_.do_newline(0);
  out.terminated_ = true;
  auto& o = out.get_ostream();
  o.put(o.widen('}'));
  return move(out.out_);
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
  return out_.valid();
}

template<typename UnderlyingStream>
auto json_ostream_object_impl<UnderlyingStream>::get_ostream()
    const noexcept -> basic_stream& {
  return out_.get_ostream();
}

template<typename UnderlyingStream>
auto json_ostream_object_impl<UnderlyingStream>::do_newline(size_t indent) ->
    void {
  return out_.do_newline(indent + 2);
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
    enable_if_t<
        !is_void<
            decltype(
                declval<json_ostream_object<
                    typename json_ostream<S>::basic_stream>&>() <<
                declval<const T&>())>::value,
        typename json_ostream<S>::underlying_type> {
  auto objstream = out << begin_json_object;
  out << v;
  return move(out) << end_json_object;
}


template<typename S, typename T>
auto operator<<(json_ostream<S>&& out, const T& v) ->
    enable_if_t<
        !is_void<
            decltype(
                declval<basic_json_ostream_array<
                    typename json_ostream<S>::basic_stream>&>() <<
                declval<const T&>())>::value,
        typename json_ostream<S>::underlying_type> {
  auto objstream = out << begin_json_array;
  out << v;
  return move(out) << end_json_object;
}


}} /* namespace ilias::test */

#endif /* _ILIAS_TEST_JSON_INL_H_ */
