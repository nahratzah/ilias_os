//===------------------------- string.cpp ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "string"
#include "cstdlib"
#include "cwchar"
#include "cerrno"
#if _WIN32
#include "support/win32/support.h"
#endif // _WIN32
#include <libc/strto.h>

_LIBCPP_BEGIN_NAMESPACE_STD

template class __basic_string_common<true>;

template class basic_string<char>;
template class basic_string<wchar_t>;

template
    string
    operator+<char, char_traits<char>, allocator<char> >(char const*, string const&);

int
stoi(const string& str, size_t* idx, int base)
{
    auto ptr = str.begin();
    auto r = libc::from_str<int>(ptr, str.end(), base, false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

int
stoi(const wstring& str, size_t* idx, int base)
{
    auto ptr = str.begin();
    auto r = libc::from_str<int>(ptr, str.end(), base, false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

long
stol(const string& str, size_t* idx, int base)
{
    auto ptr = str.begin();
    auto r = libc::from_str<long>(ptr, str.end(), base, false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

long
stol(const wstring& str, size_t* idx, int base)
{
    auto ptr = str.begin();
    auto r = libc::from_str<long>(ptr, str.end(), base, false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

unsigned long
stoul(const string& str, size_t* idx, int base)
{
    auto ptr = str.begin();
    auto r = libc::from_str<unsigned long>(ptr, str.end(), base, false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

unsigned long
stoul(const wstring& str, size_t* idx, int base)
{
    auto ptr = str.begin();
    auto r = libc::from_str<unsigned long>(ptr, str.end(), base, false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

long long
stoll(const string& str, size_t* idx, int base)
{
    auto ptr = str.begin();
    auto r = libc::from_str<long long>(ptr, str.end(), base, false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

long long
stoll(const wstring& str, size_t* idx, int base)
{
    auto ptr = str.begin();
    auto r = libc::from_str<long long>(ptr, str.end(), base, false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

unsigned long long
stoull(const string& str, size_t* idx, int base)
{
    auto ptr = str.begin();
    auto r = libc::from_str<unsigned long long>(ptr, str.end(), base, false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

unsigned long long
stoull(const wstring& str, size_t* idx, int base)
{
    auto ptr = str.begin();
    auto r = libc::from_str<unsigned long long>(ptr, str.end(), base, false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

float
stof(const string& str, size_t* idx)
{
    auto ptr = str.begin();
    auto r = libc::f_from_str<float>(ptr, str.end(), false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

float
stof(const wstring& str, size_t* idx)
{
    auto ptr = str.begin();
    auto r = libc::f_from_str<float>(ptr, str.end(), false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

double
stod(const string& str, size_t* idx)
{
    auto ptr = str.begin();
    auto r = libc::f_from_str<double>(ptr, str.end(), false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

double
stod(const wstring& str, size_t* idx)
{
    auto ptr = str.begin();
    auto r = libc::f_from_str<double>(ptr, str.end(), false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

long double
stold(const string& str, size_t* idx)
{
    auto ptr = str.begin();
    auto r = libc::f_from_str<long double>(ptr, str.end(), false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

long double
stold(const wstring& str, size_t* idx)
{
    auto ptr = str.begin();
    auto r = libc::f_from_str<long double>(ptr, str.end(), false);
    if (idx)
        *idx = static_cast<size_t>(ptr - str.begin());
    return r;
}

string to_string(int val)
{
    string s;
    libc::to_str(std::back_inserter(s), val);
    return s;
}

string to_string(unsigned val)
{
    string s;
    libc::to_str(std::back_inserter(s), val);
    return s;
}

string to_string(long val)
{
    string s;
    libc::to_str(std::back_inserter(s), val);
    return s;
}

string to_string(unsigned long val)
{
    string s;
    libc::to_str(std::back_inserter(s), val);
    return s;
}

string to_string(long long val)
{
    string s;
    libc::to_str(std::back_inserter(s), val);
    return s;
}

string to_string(unsigned long long val)
{
    string s;
    libc::to_str(std::back_inserter(s), val);
    return s;
}

string to_string(float val)
{
    string s;
    libc::f_to_str(std::back_inserter(s), val);
    return s;
}

string to_string(double val)
{
    string s;
    libc::f_to_str(std::back_inserter(s), val);
    return s;
}

string to_string(long double val)
{
    string s;
    libc::f_to_str(std::back_inserter(s), val);
    return s;
}

wstring to_wstring(int val)
{
    wstring s;
    libc::to_str(std::back_inserter(s), val);
    return s;
}

wstring to_wstring(unsigned val)
{
    wstring s;
    libc::to_str(std::back_inserter(s), val);
    return s;
}

wstring to_wstring(long val)
{
    wstring s;
    libc::to_str(std::back_inserter(s), val);
    return s;
}

wstring to_wstring(unsigned long val)
{
    wstring s;
    libc::to_str(std::back_inserter(s), val);
    return s;
}

wstring to_wstring(long long val)
{
    wstring s;
    libc::to_str(std::back_inserter(s), val);
    return s;
}

wstring to_wstring(unsigned long long val)
{
    wstring s;
    libc::to_str(std::back_inserter(s), val);
    return s;
}

wstring to_wstring(float val)
{
    wstring s;
    libc::f_to_str(std::back_inserter(s), val);
    return s;
}

wstring to_wstring(double val)
{
    wstring s;
    libc::f_to_str(std::back_inserter(s), val);
    return s;
}

wstring to_wstring(long double val)
{
    wstring s;
    libc::f_to_str(std::back_inserter(s), val);
    return s;
}

_LIBCPP_END_NAMESPACE_STD
