#pragma once

#include <tuple>
#include <zipped_range_detail.hpp>

/**
 * Header only library that implements a C++ equivalent of python's
 * in-built "zip" utility. See https://docs.python.org/2/library/functions.html#zip
 *
 * Basically given 2 to N iterable sequences, this library returns a forward
 * iterable range over a tuple of the elements in each of those sequences
 *
 * So calling ZIPPED_RANGE::make_range on the following:-
 * std::vector<int>
 * std::list<char>
 * std::unordered_map<std::string, int>
 *
 * will return a range where each element is:-
 * std::tuple<
 *     int&,
 *     char&,
 *     std::unordered_map<std::string, int>::value_type&
 * >
 * Note that the individual elements are reference types. Hence, the objects
 * may be mutated. If the zipped range is passed in const sequences, the
 * objects will be const references as well.
 *
 * WARNING: Even though element mutation is allowed, care must be taken to
 * not do any operation on the containers that might invalidate their
 * iterators. The behavior of the zipped range is undefined in this case.
 */

namespace ZIPPED_RANGE
{

template
<typename... ARGS>
struct RANGE
{
    using type = typename DETAIL::ZIPPED_RANGE_TRAITS<ARGS...>::ZIPPED_RANGE;
};

template
<typename... ARGS>
using RANGE_T = typename RANGE<ARGS...>::type;

template
<typename... ARGS>
auto make_range(ARGS&&... args) -> RANGE_T<ARGS...>
{
    static_assert(sizeof...(args) > 0, "You must provide at least one sequence to iterate over");
    return boost::make_iterator_range(
        DETAIL::make_begin(args...),
        DETAIL::make_end(args...)
    );
}
} /* namespace ZIPPED_RANGE */