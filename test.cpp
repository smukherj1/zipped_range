#include <iostream>
#include <tuple>
#include <vector>
#include <list>
#include <unordered_map>
#include <array>
#include <string>

#include "zipped_range.hpp"

#define BOOST_TEST_MODULE example
#include <boost/test/unit_test.hpp>

namespace
{

// Sequences used for unit testing
std::list<int> int_list = { 0, 1, 2, 3 };
std::vector<int> int_vec = { 3, 2, 1, 5 };
std::vector<int> int_vec_1 = { 3, 2, 1, 0, 5 };
std::array<int, 6> int_array = { 7, 8, 9, 10, 11, 12 };
int int_carray[] = { 100, 200, 300, 400, 500, 600 };
std::unordered_map<int, std::string> int_to_str_map = { { 0, "abcd" }, { 1, "cbca" }, { 2, "bcbc" }, { 3, "foo" } };

} /* namespace anonymous */

BOOST_AUTO_TEST_CASE( zip_single_seq )
{
    auto zipped_range = ZIPPED_RANGE::make_range(int_list);
    auto begin = int_list.begin();
    std::size_t iters = 0;
    for(const auto& t : zipped_range)
    {
        BOOST_TEST(std::get<0>(t) == *begin);

        ++iters;
        ++begin;
    }

}

BOOST_AUTO_TEST_CASE( zip_two_seq_equal )
{
    auto zipped_range = ZIPPED_RANGE::make_range(int_list, int_vec);
    int iters = 0;
    auto begin_0 = int_list.begin();
    auto begin_1 = int_vec.begin();
    for(const auto& t : zipped_range)
    {
        BOOST_TEST(std::get<0>(t) == *begin_0);
        BOOST_TEST(std::get<1>(t) == *begin_1);

        ++iters;
        ++begin_0;
        ++begin_1;
    }
    BOOST_TEST(iters ==  4);
}

BOOST_AUTO_TEST_CASE( zip_two_seq_unequal )
{
    auto zipped_range = ZIPPED_RANGE::make_range(int_list, int_vec_1);
    int iters = 0;
    auto begin_0 = int_list.begin();
    auto begin_1 = int_vec_1.begin();
    for(const auto& t : zipped_range)
    {
        BOOST_TEST(std::get<0>(t) == *begin_0);
        BOOST_TEST(std::get<1>(t) == *begin_1);

        ++iters;
        ++begin_0;
        ++begin_1;
    }
    BOOST_TEST(iters ==  4);
}

BOOST_AUTO_TEST_CASE( zip_five_sequences )
{
    auto zipped_range = ZIPPED_RANGE::make_range(int_list, int_vec, int_array, int_carray, int_to_str_map);
    auto ilist_it = int_list.begin();
    auto ivec_it = int_vec.begin();
    auto imap_it = int_to_str_map.begin();
    std::size_t iters = 0;

    for(const auto& t : zipped_range)
    {
        BOOST_TEST(std::get<0>(t) == *ilist_it);
        BOOST_TEST(std::get<1>(t) == *ivec_it);
        BOOST_TEST(std::get<2>(t) == int_array[iters]);
        BOOST_TEST(std::get<3>(t) == int_carray[iters]);
        BOOST_TEST((std::get<4>(t) == *imap_it));

        ++iters;
        ++ilist_it;
        ++ivec_it;
        ++imap_it;
    }

    BOOST_TEST(iters == 4);
}

BOOST_AUTO_TEST_CASE( zip_modify_element )
{
    auto map_copy = int_to_str_map;
    auto ilist_copy = int_list;

    for(auto t : ZIPPED_RANGE::make_range(map_copy, ilist_copy))
    {
        auto& map_item = std::get<0>(t);
        auto& ilist_item = std::get<1>(t);

        ++ilist_item;
        map_item.second = "foobar";
    }

    // Check the elements in the copied list were actually
    // modified as expected
    auto ilist_begin = int_list.begin();
    for(auto item : ilist_copy)
    {
        BOOST_TEST(item == (*ilist_begin + 1));
        ++ilist_begin;
    }

    // Check the elements in the map were modified as expected
    for(auto item : map_copy)
    {
        BOOST_TEST((item.second == "foobar"));
    }
}