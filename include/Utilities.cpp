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

    */
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }

    /*
     *  * Intersection of two maps
    */
    template<typename KeyType, typename ValueType>
    std::map<KeyType, std::pair < ValueType, ValueType > > IntersectMaps( const std::map<KeyType, ValueType>&
                    m1, const std::map<KeyType, ValueType>& m2)
    {
        std::map<KeyType, std::pair<ValueType, ValueType> > map_result;
        typename std::map<KeyType, ValueType>::const_iterator im1 = m1.begin();
        typename std::map<KeyType, ValueType>::const_iterator im2 = m2.begin();

        while ( im1 != m1.end() && im2 != m2.end() )
        {
            if ( im1->first < im2->first )
                ++im1;
            else if ( im2->first < im1->first )
                ++im2;
            else
            {
                map_result.insert( std::make_pair( im1->first, std::make_pair( im1->second,
                        im2->second ) ) );
                ++im1;
                ++im2;
            }
        }

        return map_result;
    }

    // explicit instantiation
    template std::string to_string<int>( const int& );
}
