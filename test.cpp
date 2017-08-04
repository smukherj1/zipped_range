#include <iostream>
#include <tuple>
#include <vector>
#include <list>
#include <unordered_map>
#include <array>
#include <string>

#include "zipped_range.hpp"

int main()
{
    std::cout << "Hello World\n";
    std::list<int> il = { 0, 1, 2, 3 };
    std::vector<int> iv = { 3, 2, 1, 0, 5 };
    std::array<int, 6> ia = { 7, 8, 9, 10, 11, 12 };
    int ica[] = { 100, 200, 300, 400, 500, 600 };
    std::unordered_map<int, std::string> umap = { { 0, "abcd" }, { 1, "cbca" }, { 2, "bcbc" }, { 3, "foo" } };

    auto zipped_range = ZIPPED_RANGE::make_range(il, iv, ia, ica, umap);
    for (const auto& t : zipped_range)
    {
        auto first = std::get<0>(t);
        auto second = std::get<1>(t);
        auto third = std::get<2>(t);
        auto fourth = std::get<3>(t);
        auto fifth = std::get<4>(t);
        std::cout << first << ", " << second << ", " << third << ", " << fourth << ", "
            << "(" << fifth.first << " -> " << fifth.second << ")\n";
    }
    return 0;
}
