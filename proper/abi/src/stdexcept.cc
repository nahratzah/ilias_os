#include <stdexcept>
#include <string>

namespace std {


logic_error::logic_error(const string& msg)
: msg_(abi::ext::make_c_string_ptr(msg.c_str(), msg.length()))
{}

logic_error::logic_error(const char* msg)
: msg_(abi::ext::make_c_string_ptr(msg))
{}

logic_error::logic_error(const logic_error& other) noexcept
: exception(other),
  msg_(other.msg_)
{}

logic_error& logic_error::operator=(const logic_error& other) noexcept
{
  this->std::exception::operator=(other);
  msg_ = other.msg_;
  return *this;
}

logic_error::~logic_error() noexcept {}

const char* logic_error::what() const noexcept {
  return msg_.c_str();
}


runtime_error::runtime_error(const string& msg)
: msg_(abi::ext::make_c_string_ptr(msg.c_str(), msg.length()))
{}

runtime_error::runtime_error(const char* msg)
: msg_(abi::ext::make_c_string_ptr(msg))
{}

runtime_error::runtime_error(const runtime_error& other) noexcept
: exception(other),
  msg_(other.msg_)
{}

runtime_error& runtime_error::operator=(const runtime_error& other) noexcept {
  this->std::exception::operator=(other);
  msg_ = other.msg_;
  return *this;
}

runtime_error::~runtime_error() noexcept {}

const char* runtime_error::what() const noexcept {
  return msg_.c_str();
}


domain_error::domain_error(const string& msg) : logic_error(msg) {}
domain_error::domain_error(const char* msg) : logic_error(msg) {}
domain_error::~domain_error() noexcept {}

invalid_argument::invalid_argument(const string& msg) : logic_error(msg) {}
invalid_argument::invalid_argument(const char* msg) : logic_error(msg) {}
invalid_argument::~invalid_argument() noexcept {}

length_error::length_error(const string& msg) : logic_error(msg) {}
length_error::length_error(const char* msg) : logic_error(msg) {}
length_error::~length_error() noexcept {}

out_of_range::out_of_range(const string& msg) : logic_error(msg) {}
out_of_range::out_of_range(const char* msg) : logic_error(msg) {}
out_of_range::~out_of_range() noexcept {}

range_error::range_error(const string& msg) : runtime_error(msg) {}
range_error::range_error(const char* msg) : runtime_error(msg) {}
range_error::~range_error() noexcept {}

overflow_error::overflow_error(const string& msg) : runtime_error(msg) {}
overflow_error::overflow_error(const char* msg) : runtime_error(msg) {}
overflow_error::~overflow_error() noexcept {}

underflow_error::underflow_error(const string& msg) : runtime_error(msg) {}
underflow_error::underflow_error(const char* msg) : runtime_error(msg) {}
underflow_error::~underflow_error() noexcept {}


} /* namespace std */
