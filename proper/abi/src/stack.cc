#include <stack>

_namespace_begin(std)


template class stack<bool>;
template class stack<char>;
template class stack<signed char>;
template class stack<unsigned char>;
template class stack<short>;
template class stack<unsigned short>;
template class stack<int>;
template class stack<unsigned int>;
template class stack<long>;
template class stack<unsigned long>;
template class stack<long long>;
template class stack<unsigned long long>;

template bool operator==(const stack<bool>&,
                         const stack<bool>&);
template bool operator==(const stack<char>&,
                         const stack<char>&);
template bool operator==(const stack<signed char>&,
                         const stack<signed char>&);
template bool operator==(const stack<unsigned char>&,
                         const stack<unsigned char>&);
template bool operator==(const stack<short>&,
                         const stack<short>&);
template bool operator==(const stack<unsigned short>&,
                         const stack<unsigned short>&);
template bool operator==(const stack<int>&,
                         const stack<int>&);
template bool operator==(const stack<unsigned int>&,
                         const stack<unsigned int>&);
template bool operator==(const stack<long>&,
                         const stack<long>&);
template bool operator==(const stack<unsigned long>&,
                         const stack<unsigned long>&);
template bool operator==(const stack<long long>&,
                         const stack<long long>&);
template bool operator==(const stack<unsigned long long>&,
                         const stack<unsigned long long>&);

template bool operator<(const stack<bool>&,
                        const stack<bool>&);
template bool operator<(const stack<char>&,
                        const stack<char>&);
template bool operator<(const stack<signed char>&,
                        const stack<signed char>&);
template bool operator<(const stack<unsigned char>&,
                        const stack<unsigned char>&);
template bool operator<(const stack<short>&,
                        const stack<short>&);
template bool operator<(const stack<unsigned short>&,
                        const stack<unsigned short>&);
template bool operator<(const stack<int>&,
                        const stack<int>&);
template bool operator<(const stack<unsigned int>&,
                        const stack<unsigned int>&);
template bool operator<(const stack<long>&,
                        const stack<long>&);
template bool operator<(const stack<unsigned long>&,
                        const stack<unsigned long>&);
template bool operator<(const stack<long long>&,
                        const stack<long long>&);
template bool operator<(const stack<unsigned long long>&,
                        const stack<unsigned long long>&);


_namespace_end(std)
