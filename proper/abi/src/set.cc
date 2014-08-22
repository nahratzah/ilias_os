#include <set>

_namespace_begin(std)


template class set<char>;
template class set<signed char>;
template class set<unsigned char>;
template class set<short>;
template class set<unsigned short>;
template class set<int>;
template class set<unsigned int>;
template class set<long>;
template class set<unsigned long>;
template class set<long long>;
template class set<unsigned long long>;

template bool operator==(const set<char>&,
                         const set<char>&);
template bool operator==(const set<signed char>&,
                         const set<signed char>&);
template bool operator==(const set<unsigned char>&,
                         const set<unsigned char>&);
template bool operator==(const set<short>&,
                         const set<short>&);
template bool operator==(const set<unsigned short>&,
                         const set<unsigned short>&);
template bool operator==(const set<int>&,
                         const set<int>&);
template bool operator==(const set<unsigned int>&,
                         const set<unsigned int>&);
template bool operator==(const set<long>&,
                         const set<long>&);
template bool operator==(const set<unsigned long>&,
                         const set<unsigned long>&);
template bool operator==(const set<long long>&,
                         const set<long long>&);
template bool operator==(const set<unsigned long long>&,
                         const set<unsigned long long>&);

template bool operator<(const set<char>&,
                        const set<char>&);
template bool operator<(const set<signed char>&,
                        const set<signed char>&);
template bool operator<(const set<unsigned char>&,
                        const set<unsigned char>&);
template bool operator<(const set<short>&,
                        const set<short>&);
template bool operator<(const set<unsigned short>&,
                        const set<unsigned short>&);
template bool operator<(const set<int>&,
                        const set<int>&);
template bool operator<(const set<unsigned int>&,
                        const set<unsigned int>&);
template bool operator<(const set<long>&,
                        const set<long>&);
template bool operator<(const set<unsigned long>&,
                        const set<unsigned long>&);
template bool operator<(const set<long long>&,
                        const set<long long>&);
template bool operator<(const set<unsigned long long>&,
                        const set<unsigned long long>&);


template class multiset<char>;
template class multiset<signed char>;
template class multiset<unsigned char>;
template class multiset<short>;
template class multiset<unsigned short>;
template class multiset<int>;
template class multiset<unsigned int>;
template class multiset<long>;
template class multiset<unsigned long>;
template class multiset<long long>;
template class multiset<unsigned long long>;

template bool operator==(const multiset<char>&,
                         const multiset<char>&);
template bool operator==(const multiset<signed char>&,
                         const multiset<signed char>&);
template bool operator==(const multiset<unsigned char>&,
                         const multiset<unsigned char>&);
template bool operator==(const multiset<short>&,
                         const multiset<short>&);
template bool operator==(const multiset<unsigned short>&,
                         const multiset<unsigned short>&);
template bool operator==(const multiset<int>&,
                         const multiset<int>&);
template bool operator==(const multiset<unsigned int>&,
                         const multiset<unsigned int>&);
template bool operator==(const multiset<long>&,
                         const multiset<long>&);
template bool operator==(const multiset<unsigned long>&,
                         const multiset<unsigned long>&);
template bool operator==(const multiset<long long>&,
                         const multiset<long long>&);
template bool operator==(const multiset<unsigned long long>&,
                         const multiset<unsigned long long>&);

template bool operator<(const multiset<char>&,
                        const multiset<char>&);
template bool operator<(const multiset<signed char>&,
                        const multiset<signed char>&);
template bool operator<(const multiset<unsigned char>&,
                        const multiset<unsigned char>&);
template bool operator<(const multiset<short>&,
                        const multiset<short>&);
template bool operator<(const multiset<unsigned short>&,
                        const multiset<unsigned short>&);
template bool operator<(const multiset<int>&,
                        const multiset<int>&);
template bool operator<(const multiset<unsigned int>&,
                        const multiset<unsigned int>&);
template bool operator<(const multiset<long>&,
                        const multiset<long>&);
template bool operator<(const multiset<unsigned long>&,
                        const multiset<unsigned long>&);
template bool operator<(const multiset<long long>&,
                        const multiset<long long>&);
template bool operator<(const multiset<unsigned long long>&,
                        const multiset<unsigned long long>&);


_namespace_end(std)
