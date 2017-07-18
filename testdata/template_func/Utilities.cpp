// File "foo.cpp"
#pragma once
#include<iostream>
#include<typeinfo>
#include<map>

namespace Utilities
{

    template<typename T> void IntersectMaps(const T& t)
    {
        std::cout << "bar function!\n" << std::endl;
        std::cout << typeid(t).name() << std::endl;
    }

    template void IntersectMaps(const int&); 
    template void IntersectMaps(const long&);
    template void IntersectMaps(const std::map<int,int>&);

    template<typename T, typename BF> void BinFunc(const T& t1, const T& t2, BF bf)
    {
        std::cout << "Entering BinFunc" << std::endl;
        std::cout << "First parameter: " << t1 << std::endl;
        std::cout << "Second parameter: " << t2 << std::endl;
        int i;
        i = bf(t1, t2);
        std::cout << i << std::endl;
    }
}
