#include <unordered_set>

_namespace_begin(std)


template class unordered_set<char>;
template class unordered_set<signed char>;
template class unordered_set<unsigned char>;
template class unordered_set<short>;
template class unordered_set<unsigned short>;
template class unordered_set<int>;
template class unordered_set<unsigned int>;
template class unordered_set<long>;
template class unordered_set<unsigned long>;
template class unordered_set<long long>;
template class unordered_set<unsigned long long>;

template bool operator==(const unordered_set<char>&,
                         const unordered_set<char>&);
template bool operator==(const unordered_set<signed char>&,
                         const unordered_set<signed char>&);
template bool operator==(const unordered_set<unsigned char>&,
                         const unordered_set<unsigned char>&);
template bool operator==(const unordered_set<short>&,
                         const unordered_set<short>&);
template bool operator==(const unordered_set<unsigned short>&,
                         const unordered_set<unsigned short>&);
template bool operator==(const unordered_set<int>&,
                         const unordered_set<int>&);
template bool operator==(const unordered_set<unsigned int>&,
                         const unordered_set<unsigned int>&);
template bool operator==(const unordered_set<long>&,
                         const unordered_set<long>&);
template bool operator==(const unordered_set<unsigned long>&,
                         const unordered_set<unsigned long>&);
template bool operator==(const unordered_set<long long>&,
                         const unordered_set<long long>&);
template bool operator==(const unordered_set<unsigned long long>&,
                         const unordered_set<unsigned long long>&);


template class unordered_multiset<char>;
template class unordered_multiset<signed char>;
template class unordered_multiset<unsigned char>;
template class unordered_multiset<short>;
template class unordered_multiset<unsigned short>;
template class unordered_multiset<int>;
template class unordered_multiset<unsigned int>;
template class unordered_multiset<long>;
template class unordered_multiset<unsigned long>;
template class unordered_multiset<long long>;
template class unordered_multiset<unsigned long long>;

template bool operator==(const unordered_multiset<char>&,
                         const unordered_multiset<char>&);
template bool operator==(const unordered_multiset<signed char>&,
                         const unordered_multiset<signed char>&);
template bool operator==(const unordered_multiset<unsigned char>&,
                         const unordered_multiset<unsigned char>&);
template bool operator==(const unordered_multiset<short>&,
                         const unordered_multiset<short>&);
template bool operator==(const unordered_multiset<unsigned short>&,
                         const unordered_multiset<unsigned short>&);
template bool operator==(const unordered_multiset<int>&,
                         const unordered_multiset<int>&);
template bool operator==(const unordered_multiset<unsigned int>&,
                         const unordered_multiset<unsigned int>&);
template bool operator==(const unordered_multiset<long>&,
                         const unordered_multiset<long>&);
template bool operator==(const unordered_multiset<unsigned long>&,
                         const unordered_multiset<unsigned long>&);
template bool operator==(const unordered_multiset<long long>&,
                         const unordered_multiset<long long>&);
template bool operator==(const unordered_multiset<unsigned long long>&,
                         const unordered_multiset<unsigned long long>&);


_namespace_end(std)
