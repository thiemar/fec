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
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <utility>

#include "FEC/Types.h"
#include "FEC/Utilities.h"
#include "FEC/BinarySequence.h"
#include "FEC/Interleaver.h"

namespace Thiemar {

namespace Detail {
    /* Create a Hamming distance look-up table for the given type. */
    template <typename T, std::size_t NumPoly, T PunctureMask, T Reference>
    class HammingDistance {
        template <std::size_t... I>
        static constexpr std::array<T, sizeof...(I)> get_hamming_table_elements(std::index_sequence<I...>) {
            return std::array<T, sizeof...(I)>{ calculate_hamming_weight((I ^ Reference) & PunctureMask) ... };
        }

    public:
        static constexpr std::array<T, (std::size_t)1u << NumPoly> table =
            get_hamming_table_elements(std::make_index_sequence<(std::size_t)1u << NumPoly>{});
    };

    template <typename T, std::size_t NumPoly, T PunctureMask, T Reference>
    constexpr std::array<T, (std::size_t)1u << NumPoly> HammingDistance<T, NumPoly, PunctureMask, Reference>::table;
}

namespace Convolutional {

/*
Standard polynomial convenience definitions.
From https://web.stanford.edu/group/cioffi/doc/book/chap10.pdf
*/
namespace Polynomials {
    using n_2_k_7_g11 = BinarySequence<1, 1, 1, 0, 0, 1, 1>;
    using n_2_k_7_g12 = BinarySequence<1, 0, 1, 1, 1, 0, 1>;
    using n_2_k_8_g11 = BinarySequence<1, 1, 0, 1, 0, 0, 1, 1>;
    using n_2_k_8_g12 = BinarySequence<1, 0, 1, 1, 1, 1, 0, 1>;
    using n_2_k_9_g11 = BinarySequence<1, 1, 0, 1, 0, 1, 1, 1, 1>;
    using n_2_k_9_g12 = BinarySequence<1, 0, 0, 0, 1, 1, 1, 0, 1>;
    using n_2_k_10_g11 = BinarySequence<1, 0, 1, 1, 0, 1, 1, 1, 1, 1>;
    using n_2_k_10_g12 = BinarySequence<1, 1, 0, 0, 1, 1, 1, 1, 0, 1>;
    using n_2_k_11_g11 = BinarySequence<1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1>;
    using n_2_k_11_g12 = BinarySequence<1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1>;
    using n_3_k_7_g11 = BinarySequence<1, 1, 1, 1, 0, 0, 1>;
    using n_3_k_7_g12 = BinarySequence<1, 1, 1, 0, 1, 0, 1>;
    using n_3_k_7_g13 = BinarySequence<1, 0, 1, 1, 0, 1, 1>;
}

/* Standard puncturing matrix convenience definitions. */
namespace PuncturingMatrices {
    using n_2_rate_1_2 = BinarySequence<1, 1>;
    using n_2_rate_2_3 = BinarySequence<1, 1, 0, 1>;
    using n_2_rate_3_4 = BinarySequence<1, 1, 0, 1, 1, 0>;
    using n_2_rate_5_6 = BinarySequence<1, 1, 0, 1, 1, 0, 0, 1, 1, 0>;
    using n_2_rate_7_8 = BinarySequence<1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0>;
    using n_3_rate_1_3 = BinarySequence<1, 1, 1>;
}

template <std::size_t ConstraintLength, typename PuncturingMatrix, typename... Polynomials>
class PuncturedConvolutionalEncoder {
    static_assert(ConstraintLength > 1u, "Constraint length must be at least two");
    static_assert(sizeof...(Polynomials) > 1u, "Minimum of two polynomials are required");
    static_assert(Detail::all_true<(Polynomials::size() == ConstraintLength)...>::value,
        "Length of polynomials must be equal to constraint length");
    static_assert(PuncturingMatrix::size() % sizeof...(Polynomials) == 0u,
        "Puncturing matrix size must be an integer multiple of the code rate");
    static_assert(PuncturingMatrix::size() > 0u, "Puncturing matrix size must be larger than zero");
    static_assert(PuncturingMatrix::size() <= ConstraintLength*sizeof...(Polynomials),
        "Puncturing matrix size must be no greater than the constraint length multiplied by the code rate");
    static_assert(sizeof(bool_vec_t) * 8u / PuncturingMatrix::ones() > 0u,
        "Word size must be large enough to fit at least one puncturing matrix cycle");

    /*
    Number of input bytes processed per call of encode_block. This number is
    calculated to generate an integer number of output bytes.
    */
    static constexpr std::size_t block_size() {
        std::size_t puncturing_row_len = PuncturingMatrix::size() / sizeof...(Polynomials);
        return sizeof(bool_vec_t) > puncturing_row_len ? (sizeof(bool_vec_t) / puncturing_row_len) * puncturing_row_len :
            puncturing_row_len;
    }

    using interleaver = Interleaver<PuncturingMatrix, sizeof...(Polynomials), block_size()>;

    /*
    Pull in a number of data bits from 'in' corresponding to the size of the
    bool_vec_t data type.
    */
    template <std::size_t... ConstraintIndices, std::size_t... ByteIndices>
    static void process_data(const uint8_t *in, bool_vec_t *out,
            std::index_sequence<ConstraintIndices...>, std::index_sequence<ByteIndices...>) {
        /* Pack input data into a bool_vec_t. */
        bool_vec_t cur_vec = (((bool_vec_t)in[ByteIndices] << ((sizeof(bool_vec_t)-1u - ByteIndices) * 8u)) | ...);

        /*
        Number of bytes from the input buffer that we need is determined by
        the working vector size and the constraint length index.
        */
        ((cur_vec = calculate_convolutions<ConstraintIndices>(cur_vec,
            in[-(ConstraintIndices / 8u) - 1u] & ((uint8_t)1u << (ConstraintIndices % 8u)), out,
            std::make_index_sequence<sizeof...(Polynomials)>{})), ...);
    }

    template <std::size_t ConstraintIndex, std::size_t... PolyIndices>
    static bool_vec_t calculate_convolutions(bool_vec_t in_vec, bool next_bit, bool_vec_t *out, std::index_sequence<PolyIndices...>) {
        /* Calculate applicable polynomial taps. */
        (calculate_taps<ConstraintLength-1u - ConstraintIndex, Polynomials, PolyIndices>(in_vec, out), ...);

        /* Return the next convolution vector. */
        return (in_vec >> 1u) | (next_bit ? ((bool_vec_t)1u << (sizeof(bool_vec_t) * 8u - 1u)) : 0u);
    }

    /* For the given polynomial, carry out an XOR if this bit is set. */
    template <std::size_t Shift, typename Poly, std::size_t PolyIndex>
    static void calculate_taps(bool_vec_t in, bool_vec_t *out) {
        if constexpr (Poly::template test<Shift>()) {
            out[PolyIndex] ^= in;
        }
    }

    /*
    Efficiently encode a block of bytes. The 'in' buffer should point to a
    buffer of size equal to the block size plus the minimum number of bytes
    occupied by the constraint length. The 'in' pointer should point to the
    first byte of the block, so the leading constraint bytes are below the
    'in' pointer in memory. If 'in' points to the start of the input data,
    these bytes should be zero.
    */
    static std::array<uint8_t, interleaver::out_buf_len()> encode_block(const uint8_t *in) {
        std::array<bool_vec_t, interleaver::in_buf_len()> conv_vec = {};

        constexpr std::size_t n_conv = block_size() / sizeof(bool_vec_t) + ((block_size() % sizeof(bool_vec_t)) ? 1u : 0u);

        for (std::size_t i = 0u; i < n_conv; i++) {
            process_data(&in[i*sizeof(bool_vec_t)], &conv_vec.data()[i*sizeof...(Polynomials)],
                std::make_index_sequence<ConstraintLength>{}, std::make_index_sequence<sizeof(bool_vec_t)>{});
        }

        return interleaver::interleave(conv_vec);
    }

public:
    /* Calculate the number of output bytes for a given input length. */
    static constexpr std::size_t calculate_output_length(std::size_t len) {
        std::size_t out_len = (len * 8u + ConstraintLength) * sizeof...(Polynomials) * PuncturingMatrix::ones();

        if (out_len % PuncturingMatrix::size()) {
            out_len = out_len / PuncturingMatrix::size() + 1u;
        } else {
            out_len /= PuncturingMatrix::size();
        }

        return out_len % 8u ? (out_len / 8u) + 1u : out_len / 8u;
    }

    /*
    Calculate the number of input bytes which can be represented using the
    given number of output bytes.
    */
    static constexpr std::size_t calculate_input_length(std::size_t len) {
        std::size_t max_len = (len * 8u * PuncturingMatrix::size()) /
            (PuncturingMatrix::ones() * sizeof...(Polynomials));

        return max_len > ConstraintLength ? (max_len - ConstraintLength) / 8u : 0u;
    }

    /*
    Do efficient convolutional encoding by carrying out a number of
    convolutions in parallel equal to the word size.
    */
    template <std::size_t Len>
    static std::array<uint8_t, calculate_output_length(Len)> encode(const std::array<uint8_t, Len> &in) {
        std::array<uint8_t, calculate_output_length(Len)> out;

        /*
        Use temporary arrays to allow input and output lengths of sizes which
        aren't integer multiples of the block size.
        */
        constexpr std::size_t flush_bytes = ConstraintLength / 8u + ((ConstraintLength % 8u) ? 1u : 0u);
        std::size_t out_idx = 0u;
        for (std::size_t i = 0u; i < Len + flush_bytes; i += block_size()) {
            std::array<uint8_t, block_size() + flush_bytes> in_block = {};
            std::copy_n(in.begin() + i - std::min(i, flush_bytes),
                std::min(block_size(), Len - i) + std::min(i, flush_bytes),
                in_block.begin() + flush_bytes - std::min(i, flush_bytes));

            std::array<uint8_t, interleaver::out_buf_len()> out_block = encode_block(&in_block[flush_bytes]);
            std::copy_n(out_block.begin(), std::min(interleaver::out_buf_len(), calculate_output_length(Len) - out_idx),
                out.begin() + out_idx);

            out_idx += std::min(interleaver::out_buf_len(), calculate_output_length(Len) - out_idx);
        }

        return out;
    }
};

/* Decoder implementing the Viterbi algorithm using hard-decisions. */
template <std::size_t ConstraintLength, std::size_t TracebackLength, typename PuncturingMatrix, typename... Polynomials>
class PuncturedHardDecisionViterbiDecoder {
    static_assert(ConstraintLength > 1u, "Constraint length must be at least two");
    static_assert(sizeof...(Polynomials) > 1u, "Minimum of two polynomials are required");
    static_assert(sizeof...(Polynomials) <= 8u, "Maximum supported code rate is 1/8");
    static_assert(Detail::all_true<(Polynomials::size() == ConstraintLength)...>::value,
        "Length of polynomials must be equal to constraint length");
    static_assert(PuncturingMatrix::size() % sizeof...(Polynomials) == 0u,
        "Puncturing matrix size must be an integer multiple of the code rate");
    static_assert(PuncturingMatrix::size() > 0u, "Puncturing matrix size must be larger than zero");
    static_assert(PuncturingMatrix::size() <= ConstraintLength*sizeof...(Polynomials),
        "Puncturing matrix size must be no greater than the constraint length multiplied by the code rate");
    static_assert(TracebackLength % (PuncturingMatrix::size() / sizeof...(Polynomials)) == 0u,
        "Traceback length must be an integer multiple of puncturing matrix row length");
    static_assert(TracebackLength % 8u == 0u, "Traceback length must be a multiple of eight");
    static_assert(sizeof(bool_vec_t) * 8u / PuncturingMatrix::ones() > 0u,
        "Word size must be large enough to fit at least one puncturing matrix cycle");

    using metric_t = bool_vec_t;
    using state_vec_t = bool_vec_t;
    using bit_vec_t = uint8_t;

    static constexpr std::size_t num_states() { return (state_vec_t)1u << (ConstraintLength - 1u); }

    /* Number of bool_vec_t required for a decision vector. */
    static constexpr std::size_t decision_size() {
        return std::max((num_states() / 8u) / sizeof(bool_vec_t) +
            ((num_states() / 8u) % sizeof(bool_vec_t) ? 1u : 0u), (std::size_t)1u);
    }

    /*
    Number of output bytes generated per call of decode_block. This number is
    calculated to consume an integer number of input bytes.
    */
    static constexpr std::size_t block_size() {
        std::size_t puncturing_row_len = PuncturingMatrix::size() / sizeof...(Polynomials);
        return sizeof(bool_vec_t) > puncturing_row_len ? (sizeof(bool_vec_t) / puncturing_row_len) * puncturing_row_len :
            puncturing_row_len;
    }

    using interleaver = Interleaver<PuncturingMatrix, sizeof...(Polynomials), block_size()>;

    /* Decode a number of bits equal to the traceback length. */
    static std::size_t decode_traceback(const uint8_t *in, std::size_t len, uint8_t *out, metric_t *path_metrics) {
        std::size_t traceback_bits = std::min(calculate_output_length(len) * 8u, TracebackLength);

        bool_vec_t decisions[TracebackLength * decision_size()] = {};
        metric_t temp_path_metrics[num_states()];

        /* Do as many full block operations as required. */
        std::size_t i = 0u, idx = 0u;
        for (; i < TracebackLength / (block_size() * 8u); i++) {
            decode_block(&in[idx], path_metrics, temp_path_metrics, &decisions[i * block_size() * 8u * decision_size()],
                std::make_index_sequence<block_size() * 8u>{});
            idx += interleaver::out_buf_len();
        }

        /* Do a final partial block operation if needed. */
        if constexpr ((TracebackLength % (block_size() * 8u)) != 0u) {
            decode_block(&in[idx], path_metrics, temp_path_metrics, &decisions[i * block_size() * 8u * decision_size()],
                std::make_index_sequence<TracebackLength % (block_size() * 8u)>{});
        }

        /* Find best path metric at the end. */
        state_vec_t min_path = std::min_element(path_metrics, path_metrics + num_states()) - path_metrics;

        /* Run traceback. */
        state_vec_t state = min_path;
        for (std::size_t i = 0u; i < traceback_bits; i++) {
            std::size_t coarse_offset = state / (sizeof(bool_vec_t) * 8u);
            std::size_t fine_offset = state % (sizeof(bool_vec_t) * 8u);

            out[(traceback_bits - i - 1u) / 8u] |= (state & (num_states() / 2u)) ? (uint8_t)1u << (i % 8u) : 0u;

            state <<= 1u;

            if (decisions[(traceback_bits - i - 1u) * decision_size() + coarse_offset] & ((bool_vec_t)1u << fine_offset)) {
                state |= 1u;
            }

            state &= (num_states() - 1u);
        }

        return traceback_bits / 8u;
    }

    /* Decode a block of bytes. */
    template <std::size_t... BitIndices>
    static void decode_block(const uint8_t *in,
            metric_t *path_metrics_1, metric_t *path_metrics_2,
            bool_vec_t *decisions, std::index_sequence<BitIndices...>) {
        /* Deinterleave the block into boolean vectors. */
        std::array<bool_vec_t, interleaver::in_buf_len()> in_vec = interleaver::deinterleave(
            Detail::to_array<uint8_t, interleaver::out_buf_len()>(in));

        /*
        Each iteration, we need the current and next path metrics. The
        calculate_trellis_step function takes the current path metrics and
        returns the new path metrics.
        */
        (calculate_trellis_step<BitIndices % (PuncturingMatrix::size() / sizeof...(Polynomials))>(
            get_in_bits<BitIndices>(in_vec.data(), std::make_index_sequence<sizeof...(Polynomials)>{}),
            BitIndices % 2u ? path_metrics_2 : path_metrics_1,
            BitIndices % 2u ? path_metrics_1 : path_metrics_2,
            &decisions[BitIndices * decision_size()], std::make_integer_sequence<state_vec_t, num_states()>{}), ...);
    }

    template <std::size_t BitIndex, std::size_t... PolyIndices>
    static bit_vec_t get_in_bits(const bool_vec_t *in_vec, std::index_sequence<PolyIndices...>) {
        constexpr std::size_t coarse_offset = BitIndex / (sizeof(bool_vec_t) * 8u);
        constexpr std::size_t fine_offset = BitIndex % (sizeof(bool_vec_t) * 8u);

        return ((((in_vec[coarse_offset * sizeof...(Polynomials) + PolyIndices] &
            ((bool_vec_t)1u << ((sizeof(bool_vec_t) * 8u)-1u - fine_offset))) ? 1u : 0u) << PolyIndices) | ...);
    }

    template <std::size_t PunctureIndex, state_vec_t... StateIndices>
    static void calculate_trellis_step(bit_vec_t in_bits,
            const metric_t *cur_path_metrics, metric_t *next_path_metrics,
            bool_vec_t *decisions, std::integer_sequence<state_vec_t, StateIndices...>) {
        /* Carry out add-compare-select for each possible state. */
        ((next_path_metrics[StateIndices] = add_compare_select<
            calculate_puncture_mask<PunctureIndex>(std::make_index_sequence<sizeof...(Polynomials)>{}), StateIndices>(
            in_bits, cur_path_metrics, decisions)), ...);
    }

    template <std::size_t I, std::size_t... PolyIndices>
    static constexpr bit_vec_t calculate_puncture_mask(std::index_sequence<PolyIndices...>) {
        bit_vec_t out = {};

        for (std::size_t i : { PolyIndices... }) {
            out |= PuncturingMatrix::test(I * sizeof...(Polynomials) + i) << i;
        }

        return out;
    }

    /* Carry out an add-compare-select operation for a single bit. */
    template <bit_vec_t PunctureMask, state_vec_t State>
    static metric_t add_compare_select(bit_vec_t in_bits, const metric_t *prev_path_metrics, bool_vec_t *decisions) {
        /* Work out ancestor state indices. */
        constexpr state_vec_t ancestor_1 = State << 1u;
        constexpr state_vec_t ancestor_2 = ancestor_1 | 1u;

        /* Calculate expected bits for each branch. */
        constexpr bit_vec_t expected_bits_1 = calculate_expected_bits(ancestor_1,
            std::make_index_sequence<sizeof...(Polynomials)>{});
        constexpr bit_vec_t expected_bits_2 = calculate_expected_bits(ancestor_2,
            std::make_index_sequence<sizeof...(Polynomials)>{});

        constexpr state_vec_t idx_1 = ancestor_1 & (num_states() - 1u);
        constexpr state_vec_t idx_2 = ancestor_2 & (num_states() - 1u);

        /* Calculate updated path metrics for ancestor branches. */
        metric_t path_1 = prev_path_metrics[idx_1] +
            (metric_t)Detail::HammingDistance<bit_vec_t, sizeof...(Polynomials), PunctureMask, expected_bits_1>::table[in_bits];
        metric_t path_2 = prev_path_metrics[idx_2] +
            (metric_t)Detail::HammingDistance<bit_vec_t, sizeof...(Polynomials), PunctureMask, expected_bits_2>::table[in_bits];

        /* Choose and store smallest path metric and store decision. */
        constexpr std::size_t coarse_offset = State / (sizeof(bool_vec_t) * 8u);
        constexpr std::size_t fine_offset = State % (sizeof(bool_vec_t) * 8u);

        if (path_2 < path_1) {
            decisions[coarse_offset] |= (bool_vec_t)1u << fine_offset;
            return path_2;
        }

        return path_1;
    }

    /* Calculate the expected set of encoder outputs for a given state. */
    template <std::size_t... PolyIndices>
    static constexpr bit_vec_t calculate_expected_bits(state_vec_t state, std::index_sequence<PolyIndices...>) {
        bit_vec_t out = 0u;
        constexpr state_vec_t poly_vec[sizeof...(Polynomials)] = { Polynomials::to_integer()... };

        for (std::size_t i : { PolyIndices... }) {
            out |= (Detail::calculate_hamming_weight(state & poly_vec[i]) % 2u) << i;
        }

        return out;
    }

public:
    /* Calculate the number of output bytes for a given input length. */
    static constexpr std::size_t calculate_output_length(std::size_t len) {
        std::size_t out_bits = (len * 8u * PuncturingMatrix::size()) / (PuncturingMatrix::ones() * sizeof...(Polynomials)) +
            (((len * 8u * PuncturingMatrix::size()) % (PuncturingMatrix::ones() * sizeof...(Polynomials))) ? 1u : 0u);

        return out_bits / 8u + ((out_bits % 8u) ? 1u : 0u);
    }

    /*
    Decode a block of convolutionally encoded data using the Viterbi
    algorithm with hard-decisions.
    */
    template <std::size_t Len>
    static std::array<uint8_t, calculate_output_length(Len)> decode(const std::array<uint8_t, Len> &in) {
        std::array<uint8_t, calculate_output_length(Len)> out = {};

        /*
        Initialise path metric corresponding to state 0 to 0, and all other
        paths to a sufficiently large value.
        */
        metric_t path_metrics[num_states()];
        path_metrics[0] = 0u;
        std::fill_n(&path_metrics[1], num_states() - 1u, (metric_t)1u << (sizeof(metric_t) * 8u - 1u));

        constexpr std::size_t in_bytes = (TracebackLength / 8u) * PuncturingMatrix::ones() /
            (PuncturingMatrix::size() / sizeof...(Polynomials));
        std::size_t out_idx = 0u;

        for (std::size_t i = 0u; i < Len; i += in_bytes) {
            out_idx += decode_traceback(&in.data()[i], Len - i, &out[out_idx], path_metrics);
        }

        return out;
    }
};

}

}
