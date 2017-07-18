/*
 *  * Utility functions for NGSeXplore
 *   * Date: 2017-0712
 *    * Author: Katherine Eaton ktmeaton [at sign here] gmail.com
 *     */

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include "FastQ.h"

namespace Utilities
{

    /*
     *  * Patch for std::to_string
     *  
    */
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }

    /*
     *  * Intersection of two maps
     *  BF modesl a binary function 
    */
    template<typename KeyType, typename ValueType, typename BF> std::map<KeyType, ValueType> IntersectMaps(const std::map<KeyType, ValueType>& m1, const std::map<KeyType, ValueType>& m2, BF bf)
    {
        std::map<KeyType, ValueType> mr;
        std::cout << "Entering IntersectMaps" << std::endl;
        std::cout << "Exiting IntersectMaps" << std::endl;
        return mr;
    }


    template std::string to_string<int>(const int&);
}
