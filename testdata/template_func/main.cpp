// File "foo.cpp"
#include "Utilities.h"
#include<map>


int add(int x1, int x2)
{
    return(x1 + x2);  
}

int main()
{
    int i1=1;
    int i5=5;
    long l2=3;
    std::map<int,int> m1;
    m1[1] = 2;

    // int (*foo)(int);
    // foo = &add1;

    // Utilities::IntersectMaps<int>(i1);
    // Utilities::IntersectMaps<long>(l2);
    // Utilities::IntersectMaps<std::map<int,int> >(m1);

    // Dealing with "unresolved overload"
    Utilities::BinFunc(i1,i5,static_cast<int const&(*)(int const&, int const&)>(std::max));
}
