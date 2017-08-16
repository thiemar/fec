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

#include "FEC/Types.h"
#include "FEC/BinarySequence.h"

namespace Thiemar {

namespace Detail {
    /* Helper functions for interleaver. */
    template <std::size_t N, std::size_t... I>
    constexpr std::size_t get_index(std::index_sequence<I...>) {
        return std::get<N>(std::array<std::size_t, sizeof...(I)>{ I... });
    }

    template <std::size_t NumPoly, std::size_t PolyIndex, std::size_t Count, typename PuncturingMatrix, bool... RowBits>
    struct PuncturingMatrixRow;

    template <std::size_t NumPoly, std::size_t PolyIndex, std::size_t Count, bool... Bits, bool... RowBits>
    struct PuncturingMatrixRow<NumPoly, PolyIndex, Count, BinarySequence<Bits...>, RowBits...> {
        using type = typename PuncturingMatrixRow<NumPoly, PolyIndex, Count-1u, BinarySequence<Bits...>,
            RowBits..., BinarySequence<Bits...>::template test<
            (BinarySequence<Bits...>::size() / NumPoly - 1u - Count) * NumPoly + PolyIndex>()>::type;
    };

    template <std::size_t NumPoly, std::size_t PolyIndex, bool... Bits, bool... RowBits>
    struct PuncturingMatrixRow<NumPoly, PolyIndex, 0u, BinarySequence<Bits...>, RowBits...> {
        using type = BinarySequence<RowBits...,
            BinarySequence<Bits...>::template test<
            (BinarySequence<Bits...>::size() / NumPoly - 1u) * NumPoly + PolyIndex>()>;
    };

    template <std::size_t NumPoly, std::size_t PolyIndex, std::size_t N, bool... Bits>
    constexpr std::size_t wrapped_input_index(BinarySequence<Bits...>) {
        /* Get this row of the puncturing matrix. */
        using puncturing_matrix_row = typename PuncturingMatrixRow<
            NumPoly, PolyIndex, BinarySequence<Bits...>::size() / NumPoly - 1u, BinarySequence<Bits...>>::type;
        
        return (N / puncturing_matrix_row::ones()) * puncturing_matrix_row::size() +
            get_index<N % puncturing_matrix_row::ones()>((typename puncturing_matrix_row::ones_index_sequence){});
    }

    template <std::size_t NumPoly, std::size_t PolyIndex, std::size_t N, bool... Bits>
    constexpr std::size_t wrapped_output_index(BinarySequence<Bits...>) {
        /* Get this row of the puncturing matrix. */
        using puncturing_matrix_row = typename PuncturingMatrixRow<
            NumPoly, PolyIndex, BinarySequence<Bits...>::size() / NumPoly - 1u, BinarySequence<Bits...>>::type;

        return (N / puncturing_matrix_row::ones()) * BinarySequence<Bits...>::ones() +
            BinarySequence<Bits...>::template sum<get_index<N % puncturing_matrix_row::ones()>(
            (typename puncturing_matrix_row::ones_index_sequence){}) * NumPoly + PolyIndex>() - PolyIndex;
    }

    template <typename PuncturingMatrix, std::size_t NumPoly, std::size_t PolyIndex, typename Seq>
    struct UnwrappedInputIndexSequence;

    template <typename PuncturingMatrix, std::size_t NumPoly, std::size_t PolyIndex, std::size_t... N>
    struct UnwrappedInputIndexSequence<PuncturingMatrix, NumPoly, PolyIndex, std::index_sequence<N...>> {
        using type = std::index_sequence<wrapped_input_index<NumPoly, PolyIndex, N>(PuncturingMatrix{})...>;
    };

    template <typename PuncturingMatrix, std::size_t NumPoly, std::size_t PolyIndex, typename Seq>
    struct UnwrappedOutputIndexSequence;

    template <typename PuncturingMatrix, std::size_t NumPoly, std::size_t PolyIndex, std::size_t... N>
    struct UnwrappedOutputIndexSequence<PuncturingMatrix, NumPoly, PolyIndex, std::index_sequence<N...>> {
        using type = std::index_sequence<wrapped_output_index<NumPoly, PolyIndex, N>(PuncturingMatrix{})...>;
    };

    template <typename Seq1, typename Seq2> struct DiffIndexSequence;

    template <std::size_t... Is1, std::size_t... Is2>
    struct DiffIndexSequence<std::index_sequence<Is1...>, std::index_sequence<Is2...>> {
        using type = std::index_sequence<Is1 - Is2...>;
    };

    template <std::size_t... MaskIndices>
    constexpr bool_vec_t mask_from_index_sequence(std::index_sequence<MaskIndices...>) {
        bool_vec_t mask = 0u;
        for (std::size_t i : { MaskIndices... }) {
            mask |= (bool_vec_t)1u << ((sizeof(bool_vec_t) * 8u)-1u - i);
        }

        return mask;
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

    /*
    Use magic numbers to spread input word as efficiently as possible based
    on the puncturing matrix.
    */

    /* Need these forward declarations. */
    template <std::size_t ShiftIndex, std::size_t... InputIndices, std::size_t... DiffIndices>
    typename std::enable_if_t<index_sequence_below_threshold(ShiftIndex, std::index_sequence<DiffIndices...>{}) &&
    (ShiftIndex > 1u), bool_vec_t> spread_word(
            bool_vec_t in, std::index_sequence<InputIndices...>, std::index_sequence<DiffIndices...>);

    template <std::size_t ShiftIndex, std::size_t... InputIndices, std::size_t... DiffIndices>
    typename std::enable_if_t<!index_sequence_below_threshold(ShiftIndex, std::index_sequence<DiffIndices...>{}) &&
    (ShiftIndex > 1u), bool_vec_t> spread_word(
            bool_vec_t in, std::index_sequence<InputIndices...>, std::index_sequence<DiffIndices...>);

    /* This is the final shift/mask operation. */
    template <std::size_t ShiftIndex, std::size_t... InputIndices, std::size_t... DiffIndices>
    typename std::enable_if_t<ShiftIndex == 1u, bool_vec_t> spread_word(
            bool_vec_t in, std::index_sequence<InputIndices...>, std::index_sequence<DiffIndices...>) {
        /*
        Add ShiftIndex to the InputIndices which correspond to DiffIndices
        greater than or equal to ShiftIndex.
        */
        using new_input_sequence = std::index_sequence<DiffIndices >= 1u ? InputIndices + 1u : InputIndices...>;

        return (in | in >> 1u) & mask_from_index_sequence(new_input_sequence{});
    }

    /*
    Skip if none of the DiffIndices are greater than or equal to ShiftIndex.
    */
    template <std::size_t ShiftIndex, std::size_t... InputIndices, std::size_t... DiffIndices>
    typename std::enable_if_t<index_sequence_below_threshold(ShiftIndex, std::index_sequence<DiffIndices...>{}) &&
    (ShiftIndex > 1u), bool_vec_t> spread_word(
            bool_vec_t in, std::index_sequence<InputIndices...>, std::index_sequence<DiffIndices...>) {
        return spread_word<ShiftIndex / 2u>(in, std::index_sequence<InputIndices...>{}, std::index_sequence<DiffIndices...>{});
    }

    template <std::size_t ShiftIndex, std::size_t... InputIndices, std::size_t... DiffIndices>
    typename std::enable_if_t<!index_sequence_below_threshold(ShiftIndex, std::index_sequence<DiffIndices...>{}) &&
    (ShiftIndex > 1u), bool_vec_t> spread_word(
            bool_vec_t in, std::index_sequence<InputIndices...>, std::index_sequence<DiffIndices...>) {
        /*
        Add ShiftIndex to the InputIndices which correspond to DiffIndices
        greater than or equal to ShiftIndex.
        */
        using new_input_sequence = std::index_sequence<DiffIndices >= ShiftIndex ? InputIndices + ShiftIndex : InputIndices...>;

        /*
        Subtract ShiftIndex from DiffIndices which are greater than or equal
        to ShiftIndex.
        */
        using new_diff_sequence = std::index_sequence<DiffIndices >= ShiftIndex ? DiffIndices - ShiftIndex : DiffIndices...>;

        /*
        Create the mask from InputIndices and apply it after the shift
        operation.
        */
        return spread_word<ShiftIndex / 2u>((in | in >> ShiftIndex) & mask_from_index_sequence(new_input_sequence{}),
            new_input_sequence{}, new_diff_sequence{});
    }
}

namespace Convolutional {

/* Class to do arbitrary bitwise interleaving. */
template <typename PuncturingMatrix, std::size_t NumPoly, std::size_t BlockSize>
class Interleaver {
    static_assert(NumPoly > 1u, "Minimum of two polynomials are required");
    static_assert(PuncturingMatrix::size() % NumPoly == 0u,
        "Puncturing matrix size must be an integer multiple of the code rate");
    static_assert(PuncturingMatrix::size() > 0u, "Puncturing matrix size must be larger than zero");
    static_assert(sizeof(bool_vec_t) * 8u / PuncturingMatrix::ones() > 0u,
        "Word size must be large enough to fit at least one puncturing matrix cycle");

    /* Calculate number of bits per stream consumed per interleaver cycle. */
    static constexpr std::size_t num_in_bits(std::size_t rem_bits) {
        std::size_t max_bits = sizeof(bool_vec_t) * 8u * (PuncturingMatrix::size() / NumPoly) /
            PuncturingMatrix::ones();

        return std::min((max_bits / (PuncturingMatrix::size() / NumPoly)) * (PuncturingMatrix::size() / NumPoly), rem_bits);
    }

    static constexpr std::size_t num_in_bits() {
        std::size_t max_bits = sizeof(bool_vec_t) * 8u * (PuncturingMatrix::size() / NumPoly) /
            PuncturingMatrix::ones();

        return (max_bits / (PuncturingMatrix::size() / NumPoly)) * (PuncturingMatrix::size() / NumPoly);
    }

    /*
    Calculate number of bits generated by a given polynomial per interleaver
    cycle.
    */
    template <std::size_t PolyIndex>
    static constexpr std::size_t num_poly_bits() {
        using puncturing_matrix_row = typename Detail::PuncturingMatrixRow<
            NumPoly, PolyIndex, PuncturingMatrix::size() / NumPoly - 1u, PuncturingMatrix>::type;

        return (num_in_bits() / (PuncturingMatrix::size() / NumPoly)) * puncturing_matrix_row::ones();
    }

    /* Calculate number of output bits produced per interleaver cycle. */
    template <std::size_t N>
    static constexpr std::size_t num_out_bits() {
        static_assert(!(N % (PuncturingMatrix::size() / NumPoly)),
            "Interleaver needs an integer number of puncturing matrix cycles");

        return N * PuncturingMatrix::ones() / (PuncturingMatrix::size() / NumPoly);
    }

    template <std::size_t PolyIndex>
    using input_index_sequence = typename Detail::UnwrappedInputIndexSequence<PuncturingMatrix, NumPoly, PolyIndex,
        std::make_index_sequence<num_poly_bits<PolyIndex>()>>::type;

    template <std::size_t PolyIndex>
    using output_index_sequence = typename Detail::UnwrappedOutputIndexSequence<PuncturingMatrix, NumPoly, PolyIndex,
        std::make_index_sequence<num_poly_bits<PolyIndex>()>>::type;

    // template <std::size_t... I>
    // static void do_interleave_iterations(std::index_sequence<I...>) {
    //     int _[] = { (blah, 0)... };
    //     (void)_;
    // }

public:
    /* Number of interleaver cycles required for the given block size. */
    static constexpr std::size_t num_iterations() {
        return (BlockSize * 8u) % num_in_bits() ? ((BlockSize * 8u) / num_in_bits()) + 1u : (BlockSize * 8u) / num_in_bits();
    }

    /*
    Takes an array of bool_vec_t of size NumPoly, each packed with N bits,
    where N is an integer multiple of the puncturing matrix row length.
    */
    template <std::size_t N, std::size_t... PolyIndices>
    static bool_vec_t interleave(const bool_vec_t *in, std::index_sequence<PolyIndices...>) {
        static_assert(!(N % (PuncturingMatrix::size() / NumPoly)),
            "Interleaver input vectors must have an integer number of puncturing matrix cycles");

        bool_vec_t out = 0u;
        int _[] = { (out |= Detail::spread_word<sizeof(bool_vec_t) * 8u / 2u>(
            in[PolyIndices], input_index_sequence<PolyIndices>{},
            (typename Detail::DiffIndexSequence<output_index_sequence<PolyIndices>,
            input_index_sequence<PolyIndices>>::type){}) <<
            PolyIndices, 0)... };
        (void)_;

        return out;
    }

    // template <std::size_t... I>
    // static void print_helper(std::index_sequence<I...>) {
    //     for (std::size_t i : { I... }) {
    //         printf("%lu, ", i);
    //     }
    //     printf("\n");
    // }

    // static void print_info() {
    //     printf("input, stream 0: ");
    //     print_helper(input_index_sequence<0u>{});
    //     printf("input, stream 1: ");
    //     print_helper(input_index_sequence<1u>{});
    //     printf("output, stream 0: ");
    //     print_helper(output_index_sequence<0u>{});
    //     printf("output, stream 1: ");
    //     print_helper(output_index_sequence<1u>{});
    // }

    // static void interleave_bits(const bool_vec_t *in, uint8_t *out) {
    //     do_interleave_iterations()
    // }
};

}

}
