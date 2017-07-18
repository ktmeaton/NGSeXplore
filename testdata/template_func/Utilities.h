// File "foo.h"
#pragma once

namespace Utilities
{
    template<typename T> void IntersectMaps(const T& t);

    template<typename T, typename BF> void BinFunc(const T& t1, const T& t2, BF bf);

}

#include "Utilities.cpp"

