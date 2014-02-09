namespace __cxxabiv1 {


inline void _assert_test(bool p, const char* file, const char* func, int line,
                         const char* pred, const char* opt_msg) noexcept {
  if (_predict_false(!p)) _assert_fail(file, func, line, pred, opt_msg);
}


} /* namespace __cxxabiv1 */
