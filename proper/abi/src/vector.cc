#include <vector>

_namespace_begin(std)


template class vector<bool>;
template class vector<char>;
template class vector<signed char>;
template class vector<unsigned char>;
template class vector<short>;
template class vector<unsigned short>;
template class vector<int>;
template class vector<unsigned int>;
template class vector<long>;
template class vector<unsigned long>;
template class vector<long long>;
template class vector<unsigned long long>;

template bool operator==(const vector<bool>&,
                         const vector<bool>&);
template bool operator==(const vector<char>&,
                         const vector<char>&);
template bool operator==(const vector<signed char>&,
                         const vector<signed char>&);
template bool operator==(const vector<unsigned char>&,
                         const vector<unsigned char>&);
template bool operator==(const vector<short>&,
                         const vector<short>&);
template bool operator==(const vector<unsigned short>&,
                         const vector<unsigned short>&);
template bool operator==(const vector<int>&,
                         const vector<int>&);
template bool operator==(const vector<unsigned int>&,
                         const vector<unsigned int>&);
template bool operator==(const vector<long>&,
                         const vector<long>&);
template bool operator==(const vector<unsigned long>&,
                         const vector<unsigned long>&);
template bool operator==(const vector<long long>&,
                         const vector<long long>&);
template bool operator==(const vector<unsigned long long>&,
                         const vector<unsigned long long>&);


_namespace_end(std)
