#include <tuple>
#include <iterator>
#include <type_traits>

#include <boost/range.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/erase.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/less.hpp>

namespace ZIPPED_RANGE
{
namespace DETAIL
{
    /**
     * Extract iterator value_type from the passed in iterator
     * type. This extra function is necessary because the 
     * boost::mpl lambda functions except the metafunction to
     * return the type via a "type" member
     */
    template
    <typename T>
    struct EXTRACT_ITERATOR_VALUE_TYPE
    {
        using type = typename std::iterator_traits<T>::value_type;
    };

    /**
     * Extract iterator reference type from the passed in iterator
     * type. This extra function is necessary because the 
     * boost::mpl lambda functions except the metafunction to
     * return the type via a "type" member
     */
    template
    <typename T>
    struct EXTRACT_ITERATOR_REF_TYPE
    {
        using type = typename std::iterator_traits<T>::reference;
    };

    /**
     * Helper utility to convert a boost::mpl::vector of N types
     * into a std::tuple of N types in the same order
     */
    template
    <typename MPL_VECTOR_T>
    class MPL_VECTOR_TO_TUPLE
    {
    private:
        /**
         * Undefined type when the sequence being created
         * is not a tuple
         */
        template <typename Seq, typename T>
        struct ADD_TO_TYPES;

        /**
         * Specialize the tuple case and add the new
         * element to the tuple and return a new
         * tuple type with the concatenated list of
         * elements
         */
        template <typename T, typename... Ts>
        struct ADD_TO_TYPES<std::tuple<Ts...>, T>
        {
            using type = std::tuple<Ts..., T>;
        };

    public:
        using type = typename boost::mpl::fold <MPL_VECTOR_T, std::tuple<>, ADD_TO_TYPES<boost::mpl::_1, boost::mpl::_2>>::type;
    };

    /**
     * Helper class to create a tuple of the dereferenced values of
     * the passed in tuple of iterators. This class is responsible for
     * creating a tuple from the first to the Nth dereferenced value
     */
    template
    <typename T, typename REF_MPL_ARGS_T, std::size_t N>
    class DEREF_ITER_TUPLE_MAKER
    {
        /**
         * Helper utility to erase the Nth element to the last element
         * in the ERASE_T boost::mpl::vector
         * N is the template argument passed in to DEREF_ITER_TUPLE_MAKER
         */
        template <typename ERASE_T>
        struct ERASE_AFTER_N
        {
            using BEGIN_T = typename boost::mpl::begin<ERASE_T>::type;
            using END_T = typename boost::mpl::end<ERASE_T>::type;
            using BEGIN_ERASE_T = typename boost::mpl::advance<BEGIN_T, boost::mpl::int_<N>>::type;
            using type = typename boost::mpl::erase<ERASE_T, BEGIN_ERASE_T, END_T>::type;
        };

        /**
         * We need to deduce the return type for the "get" function in this class.
         * becomes unnecessary with c++14 but we need it for MSVC 2013
         *
         * So REF_MPL_ARGS_T is a boost::mpl::vector of a tuple of the iterator reference
         * types. The size of this is the same as the number of sequences that were
         * zipped together. However, this class is responsible for returning a 
         * tuple of the 1st to the Nth dereferenced type where N might be smaller
         * than the number of sequences that were zipped together. Thus, we
         * use this piece of metaprogramming to erase the types after the
         * Nth type to create PARTIAL_MPL_ARGS
         */
        using PARTIAL_MPL_ARGS = typename boost::mpl::eval_if<boost::mpl::less<boost::mpl::int_<N>, boost::mpl::size<REF_MPL_ARGS_T>>,
            ERASE_AFTER_N<REF_MPL_ARGS_T>, REF_MPL_ARGS_T> ::type;

    public:
        static auto get(const T& t) -> typename MPL_VECTOR_TO_TUPLE<PARTIAL_MPL_ARGS>::type
        {
            // forward_as_tuple used to preserve the reference types in
            // the tuple
            return std::tuple_cat(
                DEREF_ITER_TUPLE_MAKER<T, REF_MPL_ARGS_T, N - 1>::get(t),
                std::forward_as_tuple(
                *std::get<N - 1>(t)
                )
                );
        }
    };

    /**
     * Base case for the dereferenced iterator tuple maker.
     * This just dereferences the first iterator and wraps
     * it in a tuple. The tuple wrapping is done so that
     * the main template can std::tuple_cat the result
     */
    template
    <typename T, typename REF_MPL_ARGS_T>
    class DEREF_ITER_TUPLE_MAKER<T, REF_MPL_ARGS_T, 1>
    {
    public:
        static auto get(const T& t) -> std::tuple<typename std::iterator_traits<typename std::tuple_element<0, T>::type>::reference>
        {
            // forward_as_tuple used to preserve the reference types in
            // the tuple
            return std::forward_as_tuple(*std::get<0>(t));
        }
    };

    /**
     * Iterator tuple comparison helper. Returns true
     * if any of the iterators in the same position in
     * the two tuples are equal. This template compares
     * the Nth iterators and recursively calls this 
     * template to check the 1st to N-1th iterators
     */
    template
    <typename T, std::size_t N>
    struct ITER_TUPLE_CMP
    {
        static bool get(const T& lhs, const T& rhs)
        {
            return
                ITER_TUPLE_CMP<T, N - 1>::get(lhs, rhs) ||
                (std::get<N - 1>(lhs) == std::get<N - 1>(rhs));
        }
    };

    /**
     * Base case for the tuple of iterator equality checker.
     * Returns if the first iterators in the tuples are equal
     */
    template
    <typename T>
    struct ITER_TUPLE_CMP<T, 1>
    {
        static bool get(const T& lhs, const T& rhs)
        {
            return std::get<0>(lhs) == std::get<0>(rhs);
        }
    };

    /**
     * Tuple of iterator increment helper. This is the general
     * case that increments the Nth iterator and recursively
     * calls this template to increment the 1 to N - 1th
     * iterator
     */
    template
    <typename T, std::size_t N>
    struct ITER_TUPLE_INCR
    {
        static void incr(T& t)
        {
            std::get<N - 1>(t)++;
            ITER_TUPLE_INCR<T, N - 1>::incr(t);
        }
    };

    /**
     * Base case for the tuple of iterator increment helper.
     * Simply increments the first iterator in the tuple
     * and returns
     */
    template
    <typename T>
    struct ITER_TUPLE_INCR<T, 1>
    {
        static void incr(T& t)
        {
            std::get<0>(t)++;
        }
    };

    /**
     * Implementation of a forward style zipped iterator that wraps around a tuple of
     * the individual iterators from each sequence that was zipped together
     * Template args:-
     * INNER_ITERS_T  - Tuple of iterators from the sequences that were zipped
     * REF_MPL_ARGS_T - boost::mpl::vector of the reference type of the individual
     *                  sequence iterators. We need this to derive the return types
     *                  of the internal dereference tuple maker of this zipped iterator
     *                  implementation.
     * REF_T          - Tuple of reference types of the individual sequence iterators.
     *                  This is the return type of the dereference operator on this
     *                  zipped iterator.
     * VAL_T          - Tuple of value types of the individual sequence iterators. This
     *                  is only needed as a template argument to boost::iterator_facade
     */
    template
    <typename INNER_ITERS_T, typename REF_MPL_ARGS_T, typename REF_T, typename VAL_T>
    class ZIPPED_ITER_IMPL : public boost::iterator_facade<
        ZIPPED_ITER_IMPL<INNER_ITERS_T, REF_MPL_ARGS_T, REF_T, VAL_T>,
        VAL_T,
        boost::forward_traversal_tag,
        REF_T>
    {
        public:
            ZIPPED_ITER_IMPL(INNER_ITERS_T iter_tuple) : m_iter_tuple(iter_tuple) { }
        
        private:
        friend class boost::iterator_core_access;
        template<typename... ARGS> friend class ZIPPED_RANGE_IMPL;

        /**
         * Dereferences this iterator and returns a temporary tuple of the reference types of
         * all the iterator of the zipped sequences
         */
        REF_T dereference() const
        {
            return DEREF_ITER_TUPLE_MAKER<INNER_ITERS_T, REF_MPL_ARGS_T, std::tuple_size<INNER_ITERS_T>::value>::get(m_iter_tuple);
        }

        /**
         * Check for equality with the other iterator. This returns true when any of the iterators in
         * the tuple of zipped sequences is equal to their equivalent in the other iterator.
         * This is to terminate this zipped range as soon as any of the zipped sequences have
         * been fully iterated over
         */
        using MY_TYPE = ZIPPED_ITER_IMPL<INNER_ITERS_T, REF_MPL_ARGS_T, REF_T, VAL_T>;
        bool equal(const MY_TYPE& other) const
        {
            return ITER_TUPLE_CMP<INNER_ITERS_T, std::tuple_size<INNER_ITERS_T>::value>::get(m_iter_tuple, other.m_iter_tuple);
        }

        /**
         * Increment all the iterators of the zipped sequences. This must NOT be called after this iterator
         * reports it is equal to the "end" range of the zipped sequence
         */
        void increment()
        {
            ITER_TUPLE_INCR<INNER_ITERS_T, std::tuple_size<INNER_ITERS_T>::value>::incr(m_iter_tuple);
        }

        INNER_ITERS_T m_iter_tuple;
    };

    /**
     * Helper class to extract type traits from the given variadic list of iterable sequences we
     * are zipping up
     */
    template
    <typename... ARGS>
    class ZIPPED_RANGE_TRAITS
    {
    private:
        // Convert the variadic list of iterable sequences we need to zip up into a boost::mpl::vector
        // of types because they allow us to do funky transformations
        using BOOST_MPL_ARGS = boost::mpl::vector<ARGS...>;
        // Remove references from the sequence types
        using NO_REF_MPL_ARGS = typename boost::mpl::transform<BOOST_MPL_ARGS, std::remove_reference<boost::mpl::_1>>::type;
        // Convert the sequence types into their iterator types. Using boost::range_iterator which makes sure we get
        // the pointer types for std::array and C style fixed arrays
        using ITERATOR_MPL_ARGS = typename boost::mpl::transform<NO_REF_MPL_ARGS, boost::range_iterator<boost::mpl::_1>>::type;
        // Extract the value types of the iterator using iterator traits. We need the helper class because that's how
        // boost mpl lambda requies us to do it
        using VALUE_TYPE_MPL_ARGS = typename boost::mpl::transform<ITERATOR_MPL_ARGS, EXTRACT_ITERATOR_VALUE_TYPE<boost::mpl::_1>>::type;
        // Extract the iterator reference type. This is the type we will return a tuple of when the zipped iterator is
        // dereferenced
        using REF_TYPE_MPL_ARGS = typename boost::mpl::transform<ITERATOR_MPL_ARGS, EXTRACT_ITERATOR_REF_TYPE<boost::mpl::_1>>::type;
    public:
        // Convert some of the above boost::mpl::vector(s) into std::tuple of the args in the mpl::vector
        // The Zipped iterator dereference will return this
        using REF_TYPE_TUPLE = typename DETAIL::MPL_VECTOR_TO_TUPLE<REF_TYPE_MPL_ARGS>::type;
        // boost::iterator_facade requires us to provide the non reference value type as well
        // so we make this type too
        using VALUE_TYPE_TUPLE = typename DETAIL::MPL_VECTOR_TO_TUPLE<VALUE_TYPE_MPL_ARGS>::type;
        // Tuple of iterators from the individual container that our zipped iterator class
        // wraps around
        using ITER_TYPE_TUPLE = typename DETAIL::MPL_VECTOR_TO_TUPLE<ITERATOR_MPL_ARGS>::type;
        // The actual zipped iterator class
        using ZIPPED_ITER = ZIPPED_ITER_IMPL<ITER_TYPE_TUPLE, REF_TYPE_MPL_ARGS, REF_TYPE_TUPLE, VALUE_TYPE_TUPLE>;
        // Boost iterator range of our zipped iterator which is what client API will presumably
        // use to iterate
        using ZIPPED_RANGE = boost::iterator_range<ZIPPED_ITER>;
    };

    /**
     * Make the "begin" iterator of the zipped range for the given variadic list of iterable
     * sequences
     */
    template
    <typename... ARGS>
    auto make_begin(ARGS&&... args) -> typename DETAIL::ZIPPED_RANGE_TRAITS<ARGS...>::ZIPPED_ITER
    {
        return std::make_tuple(std::begin(args)...);
    }

    /**
     * Make the "end" iterator of the zipped range for the given variadic list of iterable
     * sequences
     */
    template
    <typename... ARGS>
    auto make_end(ARGS&&... args) -> typename DETAIL::ZIPPED_RANGE_TRAITS<ARGS...>::ZIPPED_ITER
    {
        return std::make_tuple(std::end(args)...);
    }

} /* namespace DETAIL */
} /* namespace ZIPPED_RANGE */