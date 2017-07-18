#pragma once

#include <string>
#include <map>

namespace Utilities
{

    template <typename T> std::string to_string( const T& n);


<<<<<<< HEAD
    template<typename KeyType, typename ValueType> std::map<KeyType,std::pair<ValueType, ValueType> > IntersectMaps(const std::map<KeyType, ValueType>& m1, const std::map<KeyType, ValueType>& m2);
=======
    template<typename KeyType, typename ValueType, typename BF> std::map<KeyType,ValueType> IntersectMaps(const std::map<KeyType, ValueType>& m1, const std::map<KeyType, ValueType>& m2, BF bf);
>>>>>>> 65f2833463a68eec6ace181f266c1bc22ae88c5d

}

#include "Utilities.cpp"
