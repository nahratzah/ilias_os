#include <forward_list>

_namespace_begin(std)


template class forward_list<char>;
template class forward_list<signed char>;
template class forward_list<unsigned char>;
template class forward_list<short>;
template class forward_list<unsigned short>;
template class forward_list<int>;
template class forward_list<unsigned int>;
template class forward_list<long>;
template class forward_list<unsigned long>;
template class forward_list<long long>;
template class forward_list<unsigned long long>;

template bool operator==(const forward_list<char>&,
                         const forward_list<char>&);
template bool operator==(const forward_list<signed char>&,
                         const forward_list<signed char>&);
template bool operator==(const forward_list<unsigned char>&,
                         const forward_list<unsigned char>&);
template bool operator==(const forward_list<short>&,
                         const forward_list<short>&);
template bool operator==(const forward_list<unsigned short>&,
                         const forward_list<unsigned short>&);
template bool operator==(const forward_list<int>&,
                         const forward_list<int>&);
template bool operator==(const forward_list<unsigned int>&,
                         const forward_list<unsigned int>&);
template bool operator==(const forward_list<long>&,
                         const forward_list<long>&);
template bool operator==(const forward_list<unsigned long>&,
                         const forward_list<unsigned long>&);
template bool operator==(const forward_list<long long>&,
                         const forward_list<long long>&);
template bool operator==(const forward_list<unsigned long long>&,
                         const forward_list<unsigned long long>&);

template bool operator<(const forward_list<char>&,
                        const forward_list<char>&);
template bool operator<(const forward_list<signed char>&,
                        const forward_list<signed char>&);
template bool operator<(const forward_list<unsigned char>&,
                        const forward_list<unsigned char>&);
template bool operator<(const forward_list<short>&,
                        const forward_list<short>&);
template bool operator<(const forward_list<unsigned short>&,
                        const forward_list<unsigned short>&);
template bool operator<(const forward_list<int>&,
                        const forward_list<int>&);
template bool operator<(const forward_list<unsigned int>&,
                        const forward_list<unsigned int>&);
template bool operator<(const forward_list<long>&,
                        const forward_list<long>&);
template bool operator<(const forward_list<unsigned long>&,
                        const forward_list<unsigned long>&);
template bool operator<(const forward_list<long long>&,
                        const forward_list<long long>&);
template bool operator<(const forward_list<unsigned long long>&,
                        const forward_list<unsigned long long>&);


_namespace_end(std)
