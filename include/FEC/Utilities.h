/*
Copyright (C) 2017 Thiemar Pty Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

#include "FEC/Types.h"

namespace Thiemar {

namespace Detail {

/* Used for doing validity checks on template parameter packs. */
template <bool...> struct bool_pack;
template <bool... v>
using all_true = std::is_same<bool_pack<true, v...>, bool_pack<v..., true>>;

/* Functions to convert C array to std::array. */
template <typename T, std::size_t N, std::size_t... I>
constexpr std::array<T, N> to_array_impl(const T *a, std::index_sequence<I...>) {
    return { {a[I]...} };
}
 
template <typename T, std::size_t N>
constexpr std::array<T, N> to_array(const T *a) {
    return to_array_impl<T, N>(a, std::make_index_sequence<N>{});
}

template <typename T, std::size_t N, std::size_t... I>
constexpr std::array<T, N> to_array_impl(T *a, std::index_sequence<I...>) {
    return { {a[I]...} };
}
 
template <typename T, std::size_t N>
constexpr std::array<T, N> to_array(T *a) {
    return to_array_impl<T, N>(a, std::make_index_sequence<N>{});
}

/* Calculate Hamming weight of a word. */
template <typename T>
constexpr T calculate_hamming_weight(T in) {
    T weight = 0u;

    for (std::size_t i = 0u; i < sizeof(T) * 8u; i++) {
        weight += (in & ((T)1u << i)) ? 1u : 0u;
    }

    return weight;
}

/* Get an value from an integer sequence given an index. */
template <std::size_t N, typename T, T... I>
constexpr T get_index(std::integer_sequence<T, I...>) {
    return std::get<N>(std::array<T, sizeof...(I)>{ I... });
}

/* Return a new integer sequence with the values at the specified indices. */
template <typename T, T... I, std::size_t... N>
constexpr auto get_range(std::integer_sequence<T, I...>, std::index_sequence<N...>) {
    return std::integer_sequence<T, std::get<N>(std::array<T, sizeof...(I)>{ I... })...>{};
}

/*
Helper class for calculating the difference between the elements of two index
sequences.
*/
template <typename Seq1, typename Seq2> struct DiffIndexSequence;

template <std::size_t... Is1, std::size_t... Is2>
struct DiffIndexSequence<std::index_sequence<Is1...>, std::index_sequence<Is2...>> {
    using type = std::index_sequence<Is1 - Is2...>;
};

/* Helper class for creating an offset index sequence. */
template <std::size_t Offset, typename Seq> struct OffsetIndexSequence;

template <std::size_t Offset, std::size_t... Is>
struct OffsetIndexSequence<Offset, std::index_sequence<Is...>> {
    using type = std::index_sequence<(Is + Offset)...>;
};

constexpr bool_vec_t mask_bits(std::size_t N) {
    return ~(((bool_vec_t)1u << (sizeof(bool_vec_t) * 8u - N)) - 1u);
}

template <std::size_t... MaskIndices>
constexpr bool_vec_t mask_from_index_sequence(std::index_sequence<MaskIndices...>) {
    bool_vec_t mask = 0u;
    for (std::size_t i : { MaskIndices... }) {
        mask |= (i < sizeof(bool_vec_t) * 8u) ? (bool_vec_t)1u << ((sizeof(bool_vec_t) * 8u)-1u - i) : 0u;
    }

    return mask;
}

template <std::size_t N, std::size_t... MaskIndices>
constexpr auto mask_buffer_from_index_sequence(std::index_sequence<MaskIndices...>) {
    std::array<uint8_t, N / 8u + (N % 8u)> mask = {};
    for (std::size_t i : { MaskIndices... }) {
        if (i == N) {
            continue;
        }
        mask[i / 8u] |= (uint8_t)1u << (7u - (i % 8u));
    }

    return mask.to_std_array();
}

template <std::size_t... I>
constexpr bool index_sequence_below_threshold(std::size_t threshold, std::index_sequence<I...>) {
    for (std::size_t i : { I... }) {
        if (i >= threshold) {
            return false;
        }
    }

    return true;
}

template <typename T, std::size_t... I>
constexpr T integer_from_index_sequence(std::index_sequence<I...>) {
    T mask = 0u;
    for (std::size_t i : { I... }) {
        mask |= (i < sizeof(T) * 8u) ? (T)1u << i : 0u;
    }

    return mask;
}

/* Function for computing integer log2 at compile time. */
constexpr std::size_t log2(std::size_t n) {
    if (n == 0u) {
        return 0u;
    } else {
        return n == 1u ? 0u : 1u + log2(n / 2u);
    }
}

/* Concatenate integer sequences. */
template <typename... T> struct concat_seq;

template <typename T, T... Is>
struct concat_seq<std::integer_sequence<T, Is...>> {
    using integer_sequence = std::integer_sequence<T, Is...>;
};

template <typename T, T... Is1, T... Is2, typename... Tail>
struct concat_seq<std::integer_sequence<T, Is1...>, std::integer_sequence<T, Is2...>, Tail...> {
    using integer_sequence = typename concat_seq<std::integer_sequence<T, Is1..., Is2...>, Tail...>::integer_sequence;
};

/*
This function gets the first and last indices in an ordered integer sequence
corresponding to values in the range [L, U).
*/
template <typename T, T... Is>
constexpr std::pair<std::size_t, std::size_t> get_range_extents(T L, T U, std::integer_sequence<T, Is...>) {
    std::size_t start = sizeof...(Is);
    std::size_t end = sizeof...(Is);

    bool got_start = false;
    std::size_t idx = 0u;
    for (T i : { Is... }) {
        if (!got_start && i >= L) {
            start = idx;
            got_start = true;
        }

        if (got_start && i >= U) {
            end = idx;
            break;
        }

        idx++;
    }

    return std::make_pair(start, end);
}

/*
Return the first index in an integer sequence whose corresponding element is
equal to the input.
*/
template <typename T, T... Is>
constexpr std::size_t sequence_index(T val, std::integer_sequence<T, Is...>) {
    std::size_t idx = 0;

    for (T i : { Is... }) {
        if (i == val) {
            return idx;
        }

        idx++;
    }

    return idx;
}

}

}
