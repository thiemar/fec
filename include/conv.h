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

#include <bitset>
#include <cstddef>
#include <tuple>
#include <utility>

namespace Thiemar {

namespace Detail {
    /* Used for doing validity checks on template parameter packs. */
    template <bool...> struct bool_pack;
    template <bool... v>
    using all_true = std::is_same<bool_pack<true, v...>, bool_pack<v..., true>>;

    /* Concatenate two integer sequences, with an offset for the second. */
    template <typename Seq1, std::size_t Offset, typename Seq2> struct concat_seq;

    template <std::size_t... Is1, std::size_t Offset, std::size_t... Is2>
    struct concat_seq<std::index_sequence<Is1...>, Offset, std::index_sequence<Is2...>> {
        using index_sequence = std::index_sequence<Is1..., (Offset + Is2)...>;
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
        using index_sequence = typename concat_seq<std::index_sequence<Offset>, Offset+1u,
            typename OneIndices<Offset+1u, Bits...>::index_sequence>::index_sequence;
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
        using index_sequence = typename concat_seq<std::index_sequence<Offset>, Offset+1u,
            typename ZeroIndices<Offset+1u, Bits...>::index_sequence>::index_sequence;
    };

    template <std::size_t Offset, bool... Bits>
    struct ZeroIndices<Offset, true, Bits...> {
        using index_sequence = typename ZeroIndices<Offset+1u, Bits...>::index_sequence;
    };

    /* Test if a particular bit in a binary sequence is set. */
    template <std::size_t Index, bool... Bits>
    struct BitTest {
        bool set = false;
    };

    template <std::size_t Index, bool B, bool... Bits>
    struct BitTest<Index, B, Bits...> {
        bool set = BitTest<Index-1u, Bits...>{}.set;
    };

    template <bool... Bits>
    struct BitTest<0u, true, Bits...> {
        bool set = true;
    };

    template <bool... Bits>
    struct BitTest<0u, false, Bits...> {
        bool set = false;
    };
}

namespace Convolutional {

/*
For best performance, bool_vec_t should be a type with the same width as the
machine word size.
*/
using bool_vec_t = std::size_t;

/* Class for a compile-time binary sequence. */
template<bool... Bits>
class BinarySequence {
public:
    using index_sequence = std::make_index_sequence<sizeof...(Bits)>;
    using ones_index_sequence = typename Detail::OneIndices<0u, Bits...>::index_sequence;
    using zeroes_index_sequence = typename Detail::ZeroIndices<0u, Bits...>::index_sequence;

    static constexpr std::size_t size() { return sizeof...(Bits); }
    static constexpr std::size_t ones() { return ones_index_sequence::size(); }
    static constexpr std::size_t zeroes() { return zeroes_index_sequence::size(); }
    static constexpr bool all() { return ones() == size(); }
    static constexpr bool none() { return zeroes() == size(); }
    static constexpr bool any() { return ones() > 0u; }

    template <std::size_t I>
    static constexpr bool test() { return Detail::BitTest<I, Bits...>{}.set; }
};

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
    using n_2_rate_1_2 = BinarySequence<1>;
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
    static_assert(PuncturingMatrix::all() || (PuncturingMatrix::size() % ConstraintLength*sizeof...(Polynomials) == 0u),
        "Puncturing matrix size must be an integer multiple of the code rate");
    static_assert(PuncturingMatrix::size() > 0u, "Puncturing matrix size must be larger than zero");
    static_assert(PuncturingMatrix::size() <= ConstraintLength*sizeof...(Polynomials),
        "Puncturing matrix size must be no greater than the constraint length multiplied by the code rate");

    static constexpr bool use_puncturing = PuncturingMatrix::all() ? false : true;

    /*
    Pull in a number of data bits from 'in' corresponding to the size of the
    bool_vec_t data type. The 'len' argument refers to the number of bytes
    available from 'in'. If the limit is reached, zero-padding is used to
    fill the workung vector.
    */
    template <std::size_t... ConstraintIndices>
    void process_data(const uint8_t *in, std::size_t len, bool_vec_t *out, std::index_sequence<ConstraintIndices...>) {
        /*
        Number of bytes from the input buffer that we need is determined by
        the working vector size and the constraint length index.
        */
        int _[] = { (calculate_convolutions<ConstraintIndices % 8u>(
            in, ConstraintIndices / 8u, std::min(len, sizeof(bool_vec_t) + ((ConstraintIndices + 7u) / 8u)), out,
            std::make_index_sequence<sizeof...(Polynomials)>{}), 0)... };
        (void)_;
    }

    /* For the given polynomial, carry out an XOR if this bit is set. */
    template <std::size_t Shift, std::size_t... PolyIndices>
    void calculate_convolutions(const uint8_t *in, std::size_t start_idx, std::size_t end_idx, bool_vec_t *out,
            std::index_sequence<PolyIndices...>) {
        bool_vec_t cur_vec = 0u;

        for (std::size_t i = 0u; i < sizeof(bool_vec_t); i++) {
            if (i + start_idx >= end_idx) {
                break;
            }

            uint8_t temp;
            if (Shift) {
                temp = in[i + start_idx] >> Shift;
                if (i + start_idx + 1u < end_idx) {
                    temp |= in[i + start_idx + 1u] << (8u - Shift);
                }
            } else {
                temp = in[i + start_idx];
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
        int _[] = { (calculate_taps<Shift, Polynomials, PolyIndices>(cur_vec, out), 0)... };
        (void)_;
    }

    template <std::size_t Shift, typename Poly, std::size_t PolyIndex>
    void calculate_taps(bool_vec_t in, bool_vec_t *out) {
        if (Poly::template test<Shift>() == true) {
            out[PolyIndex] ^= in;
        }
    }

    /*
    Unpack the index sequence corresponding to non-punctured bits, and pack
    the corresponding bits from the input vector into the output array.

    Return the number of bits packed into the output array.
    */
    template <std::size_t... NonPunctured>
    std::size_t pack_output_bits(const bool_vec_t *in, std::size_t len, uint8_t *out, std::index_sequence<NonPunctured...>) {
        return 0u;
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

    Need to interleave the output and handle account puncturing, which adds
    some overhead.
    */
    std::size_t encode(const uint8_t *input, std::size_t len, uint8_t *output) {
        std::size_t num_bits = 0u;

        for (std::size_t i = 0u; i < len; i += sizeof(bool_vec_t)) {
            bool_vec_t conv_vec[sizeof...(Polynomials)];

            /*
            Iterate over constraint length, XORing shifted data into the
            corresponding working vector for each polynomial when the
            index matches a set bit in the polynomial.
            */
            process_data(&input[i], len - (i * sizeof(bool_vec_t)), conv_vec, std::make_index_sequence<ConstraintLength>{});

            /*
            Pack the resulting bits into the output buffer using the
            puncturing matrix.
            */
            num_bits += pack_output_bits(conv_vec, 0u, output, PuncturingMatrix::ones_index_sequence);
        }

        /* Clear any trailing bits in the output buffer. */

        return 0u;
    }

    /* TEMPORARY: for testing only. */
    void test() {
        uint8_t blah[5] = {1u, 2u, 3u, 4u, 5u};
        bool_vec_t conv_vec[sizeof...(Polynomials)] = {0u};
        process_data(blah, sizeof(blah), conv_vec, std::make_index_sequence<ConstraintLength>{});

        printf("%lx %lx\n", conv_vec[0], conv_vec[1]);
    }
};

template <std::size_t ConstraintLength, typename... Polynomials>
using ConvolutionalEncoder = PuncturedConvolutionalEncoder<ConstraintLength, BinarySequence<true>, Polynomials...>;

}

}
