#pragma once

#include <tuple>
#include <zipped_range_detail.hpp>

namespace ZIPPED_RANGE
{

template
<typename... ARGS>
auto make_begin(ARGS&&... args) -> typename DETAIL::ZIPPED_RANGE_IMPL<ARGS...>::ITER_TYPE_TUPLE
{
    return std::make_tuple(std::begin(args)...);
}

template
<typename... ARGS>
auto make_end(ARGS&&... args) -> typename DETAIL::ZIPPED_RANGE_IMPL<ARGS...>::ITER_TYPE_TUPLE
{
    return std::make_tuple(std::end(args)...);
}

template
<typename... ARGS>
auto make_range(ARGS&&... args) -> DETAIL::ZIPPED_RANGE_IMPL<ARGS...>
{
    return DETAIL::ZIPPED_RANGE_IMPL<ARGS...>(
        make_begin(args...),
        make_end(args...)
    );
}
} /* namespace ZIPPED_RANGE */