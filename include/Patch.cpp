/*
 * Patch for std::to_string
 * Date: 2016-0215
 * Author: Katherine Eaton ktmeaton [at sign here] gmail.com
 */

#include <iostream>
#include <string>
#include <sstream>

namespace Patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}
