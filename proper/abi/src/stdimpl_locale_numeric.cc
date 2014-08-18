#include <stdimpl/locale_numeric.h>

_namespace_begin(std)
namespace impl {


constexpr char num_encoder_base::digits_char_lower[16];
constexpr char num_encoder_base::digits_char_upper[16];


bool compare_grouping_measure_spec(string_ref measure, size_t measure_before,
                                   string_ref spec) noexcept {
  for (;;) {
    /* Detect point of no-more-grouping. */
    const bool spec_nomore = spec.empty() ||
                             spec[0] <= 0 ||
                             spec[0] == CHAR_MAX;

    /* Handle end of measurements;
     * digits before the first separator must be less than or equal to
     * what the spec describes (because if there were more, there should be
     * another separator). */
    if (measure.empty()) {
      return spec_nomore ||
             measure_before <= static_cast<unsigned char>(spec[0]);
    }

    /* Compare grouping. */
    if (spec_nomore || measure.front() != spec.front())
      return false;

    /* Pop group from measurement. */
    measure = measure.substr(1);
    /* Pop group, except for last group (last group repeats). */
    if (spec.length() > 1) spec = spec.substr(1);
  }
}


template class basic_num_encoder<char>;
template class basic_num_encoder<char16_t>;
template class basic_num_encoder<char32_t>;
template class basic_num_encoder<wchar_t>;

template class basic_num_decoder<char>;
template class basic_num_decoder<char16_t>;
template class basic_num_decoder<char32_t>;
template class basic_num_decoder<wchar_t>;


} /* namespace std::impl */
_namespace_end(std)
