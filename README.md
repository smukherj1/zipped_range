# Python like zipped range for C++

Header only library to do python style zipped iteration in C++. Look at the
comments in zipped_range.hpp for more details.

## Requirements
1. Needs access to boost headers. Download for free from [here](http://www.boost.org/)
2. A C++ 11 compiler. Tested to work on GCC 5.3, Clang 4.0.1, MSVC 2013 and MSVC 2015

## Usage examples
Look in test.cpp for more scenarios. A simple example below:-

```C++
#include <array>
#include <iostream>
#include <tuple>
#include <vector>
#include <list>

#include "zipped_range.hpp"

int main()
{
    std::vector<int> ivec = {0, 1, 2, 3, 4};
    std::list<int> ilist = {1, 2, 3, 4};
    std::array<int, 4> iarr = {10, 20, 30, 40};
    int icarr[] = {11, 12, 13, 14};

    for(auto t : ZIPPED_RANGE::make_range(ivec, ilist, iarr, icarr))
    {
        auto first = std::get<0>(t);
        auto second = std::get<1>(t);
        auto third = std::get<2>(t);
        auto fourth = std::get<3>(t);

        std::cout << "(" << first << ", " << second << ", " << third << ", " << fourth << ")\n";
    }

    return 0;
}
```
