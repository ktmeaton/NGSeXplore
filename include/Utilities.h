#pragma once

#include <string>

namespace Utilities
{

	template <typename T> std::string to_string( const T& n);

	template < typename KeyType, typename ValueType> map<KeyType, ValueType> IntersectMaps(const map<KeyType, ValueType>& m1, const map<KeyType, ValueType>& m2);
}
