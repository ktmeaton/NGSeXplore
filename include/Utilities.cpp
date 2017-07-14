/*
 *  * Utility functions for NGSeXplore
 *   * Date: 2017-0712
 *    * Author: Katherine Eaton ktmeaton [at sign here] gmail.com
 *     */

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include "Utilities.h"
#include "FastQ.h"

namespace Utilities
{

/*
 *  * Patch for std::to_string
 *     */
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }

    template < typename KeyType, typename ValueType> std::map<KeyType, ValueType> IntersectMaps(const std::map<KeyType, ValueType>& m1, const std::map<KeyType, ValueType>& m2)
    {
      // Do something
    }
    // explicit instantiation
    template std::string to_string<int>(const int&);
    template std::map<std::string, FastQ::FastQPaired> IntersectMaps(const std::map<std::string, FastQ::FastQ>&, std::map<std::string, FastQ::FastQ>&);
}
