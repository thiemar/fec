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

#include <array>
#include <bitset>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Thiemar {

namespace Detail {
    /* Switch a bit index sequence to reverse. */
    template <std::size_t N, typename Seq> struct reverse_seq;

    template<std::size_t N, std::size_t... Is>
    struct reverse_seq<N, std::index_sequence<Is...>> {
        using index_sequence = std::index_sequence<N - Is...>;
    };

    /* Concatenate two integer sequences. */
    template <typename Seq1, typename Seq2> struct concat_seq;

    template <typename T, std::size_t... Is1, std::size_t... Is2>
    struct concat_seq<std::integer_sequence<T, Is1...>, std::integer_sequence<T, Is2...>> {
        using integer_sequence = std::integer_sequence<T, Is1..., Is2...>;
    };

    /*
    Used to find the indices of set bits in a binary sequence at compile
    time.
    */
    template <std::size_t Offset, bool... Bits>
    struct OneIndices {
        using index_sequence = std::index_sequence<>;
    };

    template <std::size_t Offset, bool... Bits>
    struct OneIndices<Offset, true, Bits...> {
        using index_sequence = typename concat_seq<std::index_sequence<Offset>,
            typename OneIndices<Offset+1u, Bits...>::index_sequence>::integer_sequence;
    };

    template <std::size_t Offset, bool... Bits>
    struct OneIndices<Offset, false, Bits...> {
        using index_sequence = typename OneIndices<Offset+1u, Bits...>::index_sequence;
    };

    /*
    Used to find the indices of cleared bits in a binary sequence at compile
    time.
    */
    template <std::size_t Offset, bool... Bits>
    struct ZeroIndices {
        using index_sequence = std::index_sequence<>;
    };

    template <std::size_t Offset, bool... Bits>
    struct ZeroIndices<Offset, false, Bits...> {
        using index_sequence = typename concat_seq<std::index_sequence<Offset>,
            typename ZeroIndices<Offset+1u, Bits...>::index_sequence>::integer_sequence;
    };

    template <std::size_t Offset, bool... Bits>
    struct ZeroIndices<Offset, true, Bits...> {
        using index_sequence = typename ZeroIndices<Offset+1u, Bits...>::index_sequence;
    };

    /* Test if a particular bit in a binary sequence is set. */
    template <std::size_t Index, bool... Bits>
    struct BitTest {
        static constexpr bool set = false;
    };

    template <std::size_t Index, bool B, bool... Bits>
    struct BitTest<Index, B, Bits...> {
        static constexpr bool set = BitTest<Index-1u, Bits...>{}.set;
    };

    template <bool... Bits>
    struct BitTest<0u, true, Bits...> {
        static constexpr bool set = true;
    };

    template <bool... Bits>
    struct BitTest<0u, false, Bits...> {
        static constexpr bool set = false;
    };

    template <std::size_t I, std::size_t... Ones>
    constexpr std::size_t sum_helper(std::index_sequence<Ones...>) {
        std::size_t sum = 0u;
        for (std::size_t i : { Ones... }) {
            if (i == I) {
                break;
            }

            sum++;
        }

        return sum;
    }

}

/* Class for a compile-time binary sequence. */
template<bool... Bits>
class BinarySequence {
public:
    using index_sequence = std::make_index_sequence<sizeof...(Bits)>;
    using ones_index_sequence = typename Detail::OneIndices<0u, Bits...>::index_sequence;
    using ones_index_sequence_reversed = typename Detail::reverse_seq<sizeof...(Bits)-1u, ones_index_sequence>::index_sequence;
    using zeroes_index_sequence = typename Detail::ZeroIndices<0u, Bits...>::index_sequence;
    using zeroes_index_sequence_reversed = typename Detail::reverse_seq<sizeof...(Bits)-1u, zeroes_index_sequence>::index_sequence; 

    static constexpr std::size_t size() { return sizeof...(Bits); }
    static constexpr std::size_t ones() { return ones_index_sequence::size(); }
    static constexpr std::size_t zeroes() { return zeroes_index_sequence::size(); }
    static constexpr bool all() { return ones() == size(); }
    static constexpr bool none() { return zeroes() == size(); }
    static constexpr bool any() { return ones() > 0u; }

    template <std::size_t I>
    static constexpr bool test() { return Detail::BitTest<I, Bits...>{}.set; }

    static constexpr bool test(std::size_t idx) {
        std::size_t i = 0u;
        for (bool b : { Bits... }) {
            if (i++ == idx) {
                return b;
            }
        }

        return false;
    }

    /* Sum the sequence up to the given index. */
    template <std::size_t I>
    static constexpr std::size_t sum() {
        static_assert(I < sizeof...(Bits), "Index is greater than number of bits");

        return Detail::sum_helper<I>(ones_index_sequence{});
    }

    /*
    Return an integer representation of the sequence, with the first bit in
    the sequence corresponding to the LSB and the last to the MSB.
    */
    static constexpr std::size_t to_integer() {
        std::size_t out = 0u;
        std::size_t i = 0u;

        for (bool b : { Bits... }) {
            out += (std::size_t)b << i++;
        }

        return out;
    }
};

}
