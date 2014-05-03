#ifndef _CHRONO_INL_H_
#define _CHRONO_INL_H_

#include <cdecl.h>
#include <chrono>
#include <type_traits>
#include <ctime>

_namespace_begin(std)
namespace chrono {


template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator+(const duration<Rep1, Period1>& lhs,
                         const duration<Rep2, Period2>& rhs) ->
    common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>> {
  using CD = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;

  return CD(CD(lhs).count() + CD(rhs).count());
}

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator-(const duration<Rep1, Period1>& lhs,
                         const duration<Rep2, Period2>& rhs) ->
    common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>> {
  using CD = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;

  return CD(CD(lhs).count() - CD(rhs).count());
}

template<typename Rep1, typename Period, typename Rep2>
constexpr auto operator*(const duration<Rep1, Period>& lhs, const Rep2& rhs) ->
    enable_if_t<impl::implicitly_convertible<common_type_t<Rep1, Rep2>,
                                             Rep2>::value,
                duration<common_type_t<Rep1, Rep2>, Period>> {
  using CD = duration<common_type_t<Rep1, Rep2>, Period>;

  return CD(CD(lhs).count() * rhs);
}

template<typename Rep1, typename Rep2, typename Period>
constexpr auto operator*(const Rep1& lhs, const duration<Rep2, Period>& rhs) ->
    enable_if_t<impl::implicitly_convertible<common_type_t<Rep1, Rep2>,
                                             Rep2>::value,
                duration<common_type_t<Rep1, Rep2>, Period>> {
  return rhs * lhs;
}

template<typename Rep1, typename Period, typename Rep2>
constexpr auto operator/(const duration<Rep1, Period>& lhs, const Rep2& rhs) ->
    enable_if_t<impl::implicitly_convertible<common_type_t<Rep1, Rep2>,
                                             Rep2>::value &&
                !impl::is_duration<Rep2>::value,
                duration<common_type_t<Rep1, Rep2>, Period>> {
  using CD = duration<common_type_t<Rep1, Rep2>, Period>;

  return CD(CD(lhs).count() / rhs);
}

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator/(const duration<Rep1, Period1>& lhs,
                         const duration<Rep2, Period2>& rhs) ->
    common_type_t<Rep1, Rep2> {
  using CD = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;

  return CD(lhs).count() / CD(rhs).count();
}

template<typename Rep1, typename Period, typename Rep2>
constexpr auto operator%(const duration<Rep1, Period>& lhs, const Rep2& rhs) ->
    enable_if_t<impl::implicitly_convertible<common_type_t<Rep1, Rep2>,
                                             Rep2>::value &&
                !impl::is_duration<Rep2>::value,
                duration<common_type_t<Rep1, Rep2>, Period>> {
  using CD = duration<common_type_t<Rep1, Rep2>, Period>;

  return CD(CD(lhs).count() % rhs);
}

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator%(const duration<Rep1, Period1>& lhs,
                         const duration<Rep2, Period2>& rhs) ->
    common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>> {
  using CD = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;

  return CD(CD(lhs).count() % CD(rhs).count());
}


template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr bool operator==(const duration<Rep1, Period1>& lhs,
                           const duration<Rep2, Period2>& rhs) {
  using CT = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;

  return CT(lhs).count() == CT(rhs).count();
}

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr bool operator!=(const duration<Rep1, Period1>& lhs,
                           const duration<Rep2, Period2>& rhs) {
  return !(lhs == rhs);
}

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr bool operator<(const duration<Rep1, Period1>& lhs,
                          const duration<Rep2, Period2>& rhs) {
  using CT = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;

  return CT(lhs).count() < CT(rhs).count();
}

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr bool operator<=(const duration<Rep1, Period1>& lhs,
                           const duration<Rep2, Period2>& rhs) {
  return !(rhs < lhs);
}

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr bool operator>(const duration<Rep1, Period1>& lhs,
                          const duration<Rep2, Period2>& rhs) {
  return rhs < lhs;
}

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr bool operator>=(const duration<Rep1, Period1>& lhs,
                           const duration<Rep2, Period2>& rhs) {
  return !(lhs < rhs);
}


template<typename ToDuration, typename Rep, typename Period>
constexpr auto duration_cast(const duration<Rep, Period>& d) ->
    enable_if_t<impl::is_duration<ToDuration>::value, ToDuration> {
  using CF = ratio_divide<Period, typename ToDuration::period>;
  using CR = common_type_t<typename ToDuration::rep, Rep, intmax_t>;

  if (CF::num == 1 && CF::den == 1)
    return ToDuration(static_cast<typename ToDuration::rep>(d.count()));
  if (CF::num != 1 && CF::den == 1) {
    return ToDuration(static_cast<typename ToDuration::rep>(
        static_cast<CR>(d.count()) * static_cast<CR>(CF::num)));
  }
  if (CF::num == 1 && CF::den != 1) {
    return ToDuration(static_cast<typename ToDuration::rep>(
        static_cast<CR>(d.count()) * static_cast<CR>(CF::num)));
  }
  return ToDuration(static_cast<typename ToDuration::rep>(
      static_cast<CR>(d.count()) * static_cast<CR>(CF::num) /
      static_cast<CR>(CF::den)));
}


template<typename Clock, typename Duration1, typename Rep2, typename Period2>
constexpr auto operator+(const time_point<Clock, Duration1>& t,
                         const duration<Rep2, Period2>& d) ->
    time_point<Clock, common_type_t<Duration1, duration<Rep2, Period2>>> {
  using CT = time_point<Clock,
                        common_type_t<Duration1, duration<Rep2, Period2>>>;
  return CT(t.time_since_epoch() + d);
}

template<typename Rep1, typename Period1, typename Clock, typename Duration2>
constexpr auto operator+(const duration<Rep1, Period1>& d,
                         const time_point<Clock, Duration2>& t) ->
    time_point<Clock, common_type_t<duration<Rep1, Period1>, Duration2>> {
  return t + d;
}

template<typename Clock, typename Duration1, typename Rep2, typename Period2>
constexpr auto operator-(const time_point<Clock, Duration1>& t,
                         const duration<Rep2, Period2>& d) ->
    time_point<Clock, common_type_t<Duration1, duration<Rep2, Period2>>> {
  using CT = time_point<Clock,
                        common_type_t<Duration1, duration<Rep2, Period2>>>;
  return CT(t.time_since_epoch() + (-d));
}

template<typename Clock, typename Duration1, typename Duration2>
constexpr auto operator-(const time_point<Clock, Duration1>& lhs,
                         const time_point<Clock, Duration2>& rhs) ->
    common_type_t<Duration1, Duration2> {
  return lhs.time_since_epoch() - rhs.time_since_epoch();
}


template<typename Clock, typename Duration1, typename Duration2>
constexpr bool operator==(const time_point<Clock, Duration1>& lhs,
                          const time_point<Clock, Duration2>& rhs) {
  return lhs.time_since_epoch() == rhs.time_since_epoch();
}

template<typename Clock, typename Duration1, typename Duration2>
constexpr bool operator!=(const time_point<Clock, Duration1>& lhs,
                          const time_point<Clock, Duration2>& rhs) {
  return !(lhs == rhs);
}

template<typename Clock, typename Duration1, typename Duration2>
constexpr bool operator<(const time_point<Clock, Duration1>& lhs,
                         const time_point<Clock, Duration2>& rhs) {
  return lhs.time_since_epoch() < rhs.time_since_epoch();
}

template<typename Clock, typename Duration1, typename Duration2>
constexpr bool operator<=(const time_point<Clock, Duration1>& lhs,
                          const time_point<Clock, Duration2>& rhs) {
  return !(rhs < lhs);
}

template<typename Clock, typename Duration1, typename Duration2>
constexpr bool operator>(const time_point<Clock, Duration1>& lhs,
                         const time_point<Clock, Duration2>& rhs) {
  return rhs < lhs;
}

template<typename Clock, typename Duration1, typename Duration2>
constexpr bool operator>=(const time_point<Clock, Duration1>& lhs,
                          const time_point<Clock, Duration2>& rhs) {
  return !(lhs < rhs);
}


template<typename ToDuration, typename Clock, typename Duration>
constexpr auto time_point_cast(const time_point<Clock, Duration>& t) ->
    enable_if_t<impl::is_duration<ToDuration>::value,
                time_point<Clock, ToDuration>> {
  return time_point<Clock, ToDuration>(
      duration_cast<ToDuration>(t.time_since_epoch()));
}


inline auto system_clock::to_time_t(const time_point& t) noexcept -> time_t {
  const auto t_ = time_point_cast<chrono::duration<time_t, ratio<1>>>(t);
  return t_.time_since_epoch().count();
}

inline auto system_clock::from_time_t(time_t tm) noexcept -> time_point {
  return chrono::time_point<system_clock, chrono::duration<time_t, ratio<1>>>(
      chrono::duration<time_t, ratio<1>>(tm));
}


template<typename Rep, typename Period>
constexpr auto duration<Rep, Period>::count() const -> rep {
  return rep_;
}

template<typename Rep, typename Period>
constexpr auto duration<Rep, Period>::operator+() const -> duration {
  return *this;
}

template<typename Rep, typename Period>
constexpr auto duration<Rep, Period>::operator-() const -> duration {
  return duration(-rep_);
}

template<typename Rep, typename Period>
auto duration<Rep, Period>::operator++() -> duration& {
  ++rep_;
  return *this;
}

template<typename Rep, typename Period>
auto duration<Rep, Period>::operator++(int) -> duration {
  return duration(rep_++);
}

template<typename Rep, typename Period>
auto duration<Rep, Period>::operator--() -> duration& {
  --rep_;
  return *this;
}

template<typename Rep, typename Period>
auto duration<Rep, Period>::operator--(int) -> duration {
  return duration(rep_--);
}

template<typename Rep, typename Period>
auto duration<Rep, Period>::operator+=(const duration& d) -> duration& {
  rep_ += d.count();
  return *this;
}

template<typename Rep, typename Period>
auto duration<Rep, Period>::operator-=(const duration& d) -> duration& {
  rep_ -= d.count();
  return *this;
}

template<typename Rep, typename Period>
auto duration<Rep, Period>::operator*=(const rep& r) -> duration& {
  rep_ *= r;
  return *this;
}

template<typename Rep, typename Period>
auto duration<Rep, Period>::operator/=(const rep& r) -> duration& {
  rep_ /= r;
  return *this;
}

template<typename Rep, typename Period>
auto duration<Rep, Period>::operator%=(const rep& r) -> duration& {
  rep_ %= r;
  return *this;
}

template<typename Rep, typename Period>
auto duration<Rep, Period>::operator%=(const duration& d) -> duration& {
  rep_ %= d.count();
  return *this;
}

template<typename Rep, typename Period>
constexpr auto duration<Rep, Period>::zero() -> duration {
  return duration(duration_values<rep>::zero());
}

template<typename Rep, typename Period>
constexpr auto duration<Rep, Period>::min() -> duration {
  return duration(duration_values<rep>::min());
}

template<typename Rep, typename Period>
constexpr auto duration<Rep, Period>::max() -> duration {
  return duration(duration_values<rep>::max());
}


template<typename Clock, typename Duration>
constexpr time_point<Clock, Duration>::time_point()
: d_(duration::zero())
{}

template<typename Clock, typename Duration>
constexpr time_point<Clock, Duration>::time_point(const duration& d)
: d_(d)
{}

template<typename Clock, typename Duration>
template<typename Duration2, typename>
constexpr time_point<Clock, Duration>::time_point(
    const time_point<clock, Duration2>& t)
: d_(t.time_since_epoch())
{}

template<typename Clock, typename Duration>
constexpr auto time_point<Clock, Duration>::time_since_epoch() const ->
    duration {
  return d_;
}

template<typename Clock, typename Duration>
auto time_point<Clock, Duration>::operator+=(const duration& d) ->
    time_point& {
  d_ += d;
  return *this;
}

template<typename Clock, typename Duration>
auto time_point<Clock, Duration>::operator-=(const duration& d) ->
    time_point& {
  d_ -= d;
  return *this;
}

template<typename Clock, typename Duration>
constexpr auto time_point<Clock, Duration>::min() -> time_point {
  return time_point(duration::min());
}

template<typename Clock, typename Duration>
constexpr auto time_point<Clock, Duration>::max() -> time_point {
  return time_point(duration::max());
}


} /* namespace std::chrono */


inline namespace literals {
inline namespace chrono_literals {


constexpr auto operator"" h(unsigned long long v) -> chrono::hours {
  return chrono::hours(v);
}
constexpr auto operator"" h(long double v) ->
    chrono::duration<long double, ratio<3600, 1>> {
  return chrono::duration<long double, ratio<3600, 1>>(v);
}
constexpr auto operator"" min(unsigned long long v) -> chrono::minutes {
  return chrono::minutes(v);
}
constexpr auto operator"" min(long double v) ->
    chrono::duration<long double, ratio<60, 1>> {
  return chrono::duration<long double, ratio<60, 1>>(v);
}
constexpr auto operator"" s(unsigned long long v) -> chrono::seconds {
  return chrono::seconds(v);
}
constexpr auto operator"" s(long double v) ->
    chrono::duration<long double> {
  return chrono::duration<long double>(v);
}
constexpr auto operator"" ms(unsigned long long v) -> chrono::milliseconds {
  return chrono::milliseconds(v);
}
constexpr auto operator"" ms(long double v) ->
    chrono::duration<long double, milli> {
  return chrono::duration<long double, milli>(v);
}
constexpr auto operator"" us(unsigned long long v) -> chrono::microseconds {
  return chrono::microseconds(v);
}
constexpr auto operator"" us(long double v) ->
    chrono::duration<long double, micro> {
  return chrono::duration<long double, micro>(v);
}
constexpr auto operator"" ns(unsigned long long v) -> chrono::nanoseconds {
  return chrono::nanoseconds(v);
}
constexpr auto operator"" ns(long double v) ->
    chrono::duration<long double, nano> {
  return chrono::duration<long double, nano>(v);
}


}} /* namespace std::literals::chrono_literals */
_namespace_end(std)


#endif /* _CHRONO_INL_H_ */
