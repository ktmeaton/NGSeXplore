#pragma once

#include <string>
#include <map>

namespace Utilities
{

    template <typename T> std::string to_string( const T& n );

    template<typename KeyType, typename ValueType>
    std::map<KeyType, std::pair<ValueType, ValueType> > IntersectMaps(
                    const std::map<KeyType, ValueType>& m1,
                    const std::map<KeyType, ValueType>& m2 );
}

#include "Utilities.cpp"
