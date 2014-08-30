#include <queue>

_namespace_begin(std)


template class queue<bool>;
template class queue<char>;
template class queue<signed char>;
template class queue<unsigned char>;
template class queue<short>;
template class queue<unsigned short>;
template class queue<int>;
template class queue<unsigned int>;
template class queue<long>;
template class queue<unsigned long>;
template class queue<long long>;
template class queue<unsigned long long>;

template bool operator==(const queue<bool>&,
                         const queue<bool>&);
template bool operator==(const queue<char>&,
                         const queue<char>&);
template bool operator==(const queue<signed char>&,
                         const queue<signed char>&);
template bool operator==(const queue<unsigned char>&,
                         const queue<unsigned char>&);
template bool operator==(const queue<short>&,
                         const queue<short>&);
template bool operator==(const queue<unsigned short>&,
                         const queue<unsigned short>&);
template bool operator==(const queue<int>&,
                         const queue<int>&);
template bool operator==(const queue<unsigned int>&,
                         const queue<unsigned int>&);
template bool operator==(const queue<long>&,
                         const queue<long>&);
template bool operator==(const queue<unsigned long>&,
                         const queue<unsigned long>&);
template bool operator==(const queue<long long>&,
                         const queue<long long>&);
template bool operator==(const queue<unsigned long long>&,
                         const queue<unsigned long long>&);

template bool operator<(const queue<bool>&,
                        const queue<bool>&);
template bool operator<(const queue<char>&,
                        const queue<char>&);
template bool operator<(const queue<signed char>&,
                        const queue<signed char>&);
template bool operator<(const queue<unsigned char>&,
                        const queue<unsigned char>&);
template bool operator<(const queue<short>&,
                        const queue<short>&);
template bool operator<(const queue<unsigned short>&,
                        const queue<unsigned short>&);
template bool operator<(const queue<int>&,
                        const queue<int>&);
template bool operator<(const queue<unsigned int>&,
                        const queue<unsigned int>&);
template bool operator<(const queue<long>&,
                        const queue<long>&);
template bool operator<(const queue<unsigned long>&,
                        const queue<unsigned long>&);
template bool operator<(const queue<long long>&,
                        const queue<long long>&);
template bool operator<(const queue<unsigned long long>&,
                        const queue<unsigned long long>&);


template class priority_queue<char>;
template class priority_queue<signed char>;
template class priority_queue<unsigned char>;
template class priority_queue<short>;
template class priority_queue<unsigned short>;
template class priority_queue<int>;
template class priority_queue<unsigned int>;
template class priority_queue<long>;
template class priority_queue<unsigned long>;
template class priority_queue<long long>;
template class priority_queue<unsigned long long>;


_namespace_end(std)
