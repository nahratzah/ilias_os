#include <unordered_map>

_namespace_begin(std)


template class unordered_map<char, char>;
template class unordered_map<signed char, signed char>;
template class unordered_map<unsigned char, unsigned char>;
template class unordered_map<short, short>;
template class unordered_map<unsigned short, unsigned short>;
template class unordered_map<int, int>;
template class unordered_map<unsigned int, unsigned int>;
template class unordered_map<long, long>;
template class unordered_map<unsigned long, unsigned long>;
template class unordered_map<long long, long long>;
template class unordered_map<unsigned long long, unsigned long long>;

template bool operator==(const unordered_map<char,
                                             char>&,
                         const unordered_map<char,
                                             char>&);
template bool operator==(const unordered_map<signed char,
                                             signed char>&,
                         const unordered_map<signed char,
                                             signed char>&);
template bool operator==(const unordered_map<unsigned char,
                                             unsigned char>&,
                         const unordered_map<unsigned char,
                                             unsigned char>&);
template bool operator==(const unordered_map<short,
                                             short>&,
                         const unordered_map<short,
                                             short>&);
template bool operator==(const unordered_map<unsigned short,
                                             unsigned short>&,
                         const unordered_map<unsigned short,
                                             unsigned short>&);
template bool operator==(const unordered_map<int,
                                             int>&,
                         const unordered_map<int,
                                             int>&);
template bool operator==(const unordered_map<unsigned int,
                                             unsigned int>&,
                         const unordered_map<unsigned int,
                                             unsigned int>&);
template bool operator==(const unordered_map<long,
                                             long>&,
                         const unordered_map<long,
                                             long>&);
template bool operator==(const unordered_map<unsigned long,
                                             unsigned long>&,
                         const unordered_map<unsigned long,
                                             unsigned long>&);
template bool operator==(const unordered_map<long long,
                                             long long>&,
                         const unordered_map<long long,
                                             long long>&);
template bool operator==(const unordered_map<unsigned long long,
                                             unsigned long long>&,
                         const unordered_map<unsigned long long,
                                             unsigned long long>&);


_namespace_end(std)
