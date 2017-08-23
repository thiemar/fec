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

#include <cstddef>
#include <type_traits>
#include <utility>

#include "FEC/Types.h"
#include "FEC/BinarySequence.h"
#include "FEC/Interleaver.h"

namespace Thiemar {

namespace Detail {
    /* Used for doing validity checks on template parameter packs. */
    template <bool...> struct bool_pack;
    template <bool... v>
    using all_true = std::is_same<bool_pack<true, v...>, bool_pack<v..., true>>;
}

namespace Convolutional {

/*
Standard polynomial convenience definitions.
From https://web.stanford.edu/group/cioffi/doc/book/chap10.pdf
*/
namespace Polynomials {
    using n_2_k_7_g11 = BinarySequence<1, 1, 1, 0, 0, 1, 1>;
    using n_2_k_7_g12 = BinarySequence<1, 0, 1, 1, 1, 0, 1>;
}

/* Standard puncturing matrix convenience definitions. */
namespace PuncturingMatrices {
    using n_2_rate_1_2 = BinarySequence<1, 1>;
    using n_2_rate_2_3 = BinarySequence<1, 1, 0, 1>;
    using n_2_rate_3_4 = BinarySequence<1, 1, 0, 1, 1, 0>;
    using n_2_rate_5_6 = BinarySequence<1, 1, 0, 1, 1, 0, 0, 1, 1, 0>;
    using n_2_rate_7_8 = BinarySequence<1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0>;
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
    bool_vec_t data type. The 'len' argument refers to the number of bytes
    available from 'in'. If the limit is reached, zero-padding is used to
    fill the working vector.
    */
    template <std::size_t... ConstraintIndices>
    static void process_data(std::size_t in_idx, const uint8_t *in, std::size_t len, bool_vec_t *out,
            std::index_sequence<ConstraintIndices...>) {
        /*
        Number of bytes from the input buffer that we need is determined by
        the working vector size and the constraint length index.
        */
        int _[] = { (calculate_convolutions<ConstraintIndices % 8u>(
            in_idx + ConstraintIndices / 8u, in, in_idx + len, out,
            std::make_index_sequence<sizeof...(Polynomials)>{}), 0)... };
        (void)_;
    }

    template <std::size_t Shift, std::size_t... PolyIndices>
    static void calculate_convolutions(std::size_t start_idx, const uint8_t *in, std::size_t end_idx, bool_vec_t *out,
            std::index_sequence<PolyIndices...>) {
        bool_vec_t cur_vec = 0u;

        for (std::size_t i = 0u; i < sizeof(bool_vec_t); i++) {
            uint8_t temp;
            if (i + start_idx > end_idx) {
                temp = 0u;
            } else {
                if (Shift) {
                    temp = in[i + start_idx] >> Shift;
                    if (i + start_idx >= 1u) {
                        temp |= in[i + start_idx - 1u] << (8u - Shift);
                    }
                } else {
                    temp = in[i + start_idx];
                }
            }

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
defined(__BIG_ENDIAN__) || \
defined(__ARMEB__) || \
defined(__THUMBEB__)
            /* Get input data from the buffer (big-endian). */
            ((uint8_t *)&cur_vec)[i] = temp;
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
defined(__LITTLE_ENDIAN__) || \
defined(__ARMEL__) || \
defined(__THUMBEL__)
            /* Get input data from the buffer (little-endian). */
            ((uint8_t *)&cur_vec)[sizeof(cur_vec)-1u - i] = temp;
#else
#error "Unable to detect endianness"
#endif
        }

        /* Calculate applicable polynomial taps. */
        int _[] = { (calculate_taps<ConstraintLength-1u - Shift, Polynomials, PolyIndices>(cur_vec, out), 0)... };
        (void)_;
    }

    /* For the given polynomial, carry out an XOR if this bit is set. */
    template <std::size_t Shift, typename Poly, std::size_t PolyIndex>
    static void calculate_taps(bool_vec_t in, bool_vec_t *out) {
        if (Poly::template test<Shift>()) {
            out[PolyIndex] ^= in;
        }
    }

    /*
    Unpack the index sequence corresponding to non-punctured bits, and pack
    the corresponding bits from the input vector into the output buffer.

    Return the number of bits packed into the output buffer.
    */
    template <std::size_t... PolyIndices>
    static std::size_t pack_output_bits(std::size_t in_idx, const bool_vec_t *in, std::size_t out_idx, uint8_t *out,
            std::index_sequence<PolyIndices...>) {
        /* Iterate over all non-punctured bit indices. */
        for (std::size_t i = 0u; i < sizeof(bool_vec_t) * 8u; i++) {
            int _[] = { (out_idx += extract_bits<PolyIndices>(
                in_idx + (i * sizeof...(Polynomials)), (sizeof(bool_vec_t) * 8u)-1u - i, in, out_idx, out), 0)... };
            (void)_;
        }

        return out_idx;
    }

    template <std::size_t PolyIndex>
    static std::size_t extract_bits(std::size_t in_bits, std::size_t in_idx, const bool_vec_t *in,
            std::size_t out_idx, uint8_t *out) {
        if (PuncturingMatrix::bits[(in_bits + PolyIndex) % PuncturingMatrix::size()]) {
            out[out_idx / 8u] = in[PolyIndex] & ((bool_vec_t)1u << in_idx) ?
                out[out_idx / 8u] | (1u << (7u - (out_idx % 8u))) :
                out[out_idx / 8u] & ~(1u << (7u - (out_idx % 8u)));

            return 1u;
        } else {
            return 0u;
        }
    }

    /*
    Efficiently encode a block of bytes. The number of bytes read from 'in'
    will be equal to block_size(), and the number of bytes written to 'out'
    will be equal to the number of input bytes multipled by the reciprocal
    of the code rate.
    */
    static void encode_block(std::size_t in_idx, const uint8_t *in, uint8_t *out) {
        bool_vec_t conv_vec[interleaver::in_buf_len()] = {0};

        std::size_t n_conv = block_size() / sizeof(bool_vec_t) + ((block_size() % sizeof(bool_vec_t)) ? 1u : 0u);

        for (std::size_t i = 0u; i < n_conv; i++) {
            process_data(in_idx + i*sizeof(bool_vec_t), in, block_size() - i*sizeof(bool_vec_t),
                &conv_vec[i*sizeof...(Polynomials)], std::make_index_sequence<ConstraintLength>{});
        }

        interleaver::interleave(conv_vec, out);
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
        std::size_t max_len = ((len * 8u) * PuncturingMatrix::size()) /
            (PuncturingMatrix::ones() * sizeof...(Polynomials));

        return max_len > ConstraintLength ? (max_len - ConstraintLength) / 8u : 0u;
    }

    /*
    Do efficient convolutional encoding by carrying out a number of
    convolutions in parallel equal to the word size.
    */
    static std::size_t encode(const uint8_t *input, std::size_t len, uint8_t *out) {
        /*
        Use temporary arrays to allow input and output lengths of sizes which
        aren't integer multiples of the block size.
        */
        std::size_t out_idx = 0u;
        for (std::size_t i = 0u; i < len; i += block_size()) {
            uint8_t out_block[interleaver::out_buf_len()] = {0};

            encode_block(i, input, out_block);
            std::memcpy(&out[out_idx], out_block, interleaver::out_buf_len());
            out_idx += interleaver::out_buf_len();
        }

        return out_idx;
    }
};

template <std::size_t ConstraintLength, typename... Polynomials>
using ConvolutionalEncoder = PuncturedConvolutionalEncoder<ConstraintLength, BinarySequence<true>, Polynomials...>;

}

}
