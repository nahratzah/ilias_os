#include <deque>

_namespace_begin(std)


template class deque<bool>;
template class deque<char>;
template class deque<signed char>;
template class deque<unsigned char>;
template class deque<short>;
template class deque<unsigned short>;
template class deque<int>;
template class deque<unsigned int>;
template class deque<long>;
template class deque<unsigned long>;
template class deque<long long>;
template class deque<unsigned long long>;

template bool operator==(const deque<bool>&,
                         const deque<bool>&);
template bool operator==(const deque<char>&,
                         const deque<char>&);
template bool operator==(const deque<signed char>&,
                         const deque<signed char>&);
template bool operator==(const deque<unsigned char>&,
                         const deque<unsigned char>&);
template bool operator==(const deque<short>&,
                         const deque<short>&);
template bool operator==(const deque<unsigned short>&,
                         const deque<unsigned short>&);
template bool operator==(const deque<int>&,
                         const deque<int>&);
template bool operator==(const deque<unsigned int>&,
                         const deque<unsigned int>&);
template bool operator==(const deque<long>&,
                         const deque<long>&);
template bool operator==(const deque<unsigned long>&,
                         const deque<unsigned long>&);
template bool operator==(const deque<long long>&,
                         const deque<long long>&);
template bool operator==(const deque<unsigned long long>&,
                         const deque<unsigned long long>&);

template bool operator<(const deque<bool>&,
                        const deque<bool>&);
template bool operator<(const deque<char>&,
                        const deque<char>&);
template bool operator<(const deque<signed char>&,
                        const deque<signed char>&);
template bool operator<(const deque<unsigned char>&,
                        const deque<unsigned char>&);
template bool operator<(const deque<short>&,
                        const deque<short>&);
template bool operator<(const deque<unsigned short>&,
                        const deque<unsigned short>&);
template bool operator<(const deque<int>&,
                        const deque<int>&);
template bool operator<(const deque<unsigned int>&,
                        const deque<unsigned int>&);
template bool operator<(const deque<long>&,
                        const deque<long>&);
template bool operator<(const deque<unsigned long>&,
                        const deque<unsigned long>&);
template bool operator<(const deque<long long>&,
                        const deque<long long>&);
template bool operator<(const deque<unsigned long long>&,
                        const deque<unsigned long long>&);

template class deque<array<short, 768>>;
template bool operator==(const deque<array<short, 768>>&,
                         const deque<array<short, 768>>&);
template bool operator<(const deque<array<short, 768>>&,
                        const deque<array<short, 768>>&);


_namespace_end(std)
