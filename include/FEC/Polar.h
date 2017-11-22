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

namespace Thiemar {

namespace Detail {
    /*
    Helper classes used to return the N indices in sorted order corresponding
    to the N smallest elements in an integer sequence.
    */

    /*
    This function finds the smallest value of an integer sequence below which
    there are no fewer than K elements.
    */
    template <int32_t... Bs>
    constexpr int32_t get_pivot_value(std::size_t k, int32_t pivot, int32_t max, int32_t min,
            std::integer_sequence<int32_t, Bs...>) {
        std::size_t sum = 0u;
        for (int32_t i : { Bs... }) {
            if (i <= pivot) {
                sum++;
            }
        }

        int32_t next_pivot = pivot;
        int32_t next_max = max;
        int32_t next_min = min;
        if (sum > k) {
            next_pivot = (pivot + min) / 2;
            next_max = pivot;
        } else if(sum < k) {
            next_pivot = (max + pivot) / 2;
            next_min = pivot;
        }

        if (next_pivot == pivot) {
            return pivot;
        } else {
            return get_pivot_value(k, next_pivot, next_max, next_min,
                std::integer_sequence<int32_t, Bs...>{});
        }
    }

    template <int32_t... Bs>
    constexpr int32_t get_pivot_value(std::size_t k, std::integer_sequence<int32_t, Bs...>) {
        auto b_array = std::array<int32_t, sizeof...(Bs)>{ Bs... };
        return get_pivot_value(k, (std::get<0u>(b_array) + std::get<sizeof...(Bs) - 1u>(b_array)) / 2,
            std::get<0u>(b_array), std::get<sizeof...(Bs) - 1u>(b_array), std::integer_sequence<int32_t, Bs...>{});
    }

    /*
    This function return the number of values in Bs which are below the pivot
    value. Used to ensure that only the excess indices with marginal
    B-parameters are frozen, rather than the ones at the end of the sequence.
    */
    template <int32_t... Bs>
    constexpr std::size_t get_num_below_pivot(int32_t pivot, std::integer_sequence<int32_t, Bs...>) {
        std::size_t count = 0u;
        for (int32_t i : { Bs... }) {
            if (i < pivot) {
                count++;
            }
        }

        return count;
    }

    /*
    Return the nth value in Bs which is smaller than or equal to the pivot
    value. Only the first r bits which are equal to the pivot value are
    counted; the rest are ignored.
    */
    template <int32_t... Bs>
    constexpr std::size_t get_nth_index_below_pivot(std::size_t n, std::size_t r, int32_t pivot,
            std::integer_sequence<int32_t, Bs...>) {
        std::size_t idx = 0u;
        std::size_t count = 0u;
        std::size_t residual = r;
        for (int32_t i : { Bs... }) {
            if ((i < pivot || (residual && i == pivot)) && count++ == n) {
                return idx;
            }

            if (residual && i == pivot) {
                residual--;
            }

            idx++;
        }

        return idx;
    }

    template <std::size_t K, int32_t Pivot, typename DataIdxSeq, typename ValSeq>
    struct DataBitsIndexSequenceHelper;

    template <std::size_t K, int32_t Pivot, std::size_t... Fs, typename ValSeq>
    struct DataBitsIndexSequenceHelper<K, Pivot, std::index_sequence<Fs...>, ValSeq> {
        using index_sequence = std::index_sequence<get_nth_index_below_pivot(Fs,
            K - get_num_below_pivot(Pivot, ValSeq{}), Pivot, ValSeq{})...>;
    };

    /*
    Calculate upper B-parameter bound in log-domain using a piecewise integer
    approximation.
    */
    constexpr int32_t update_upper_approx(int32_t in) {
        if (in > 1) {
            return 2*in;
        } else {
            return in + 1;
        }
    }

    /*
    Calculate lower B-parameter bound in log-domain using a piecewise integer
    approximation.
    */
    constexpr int32_t update_lower_approx(int32_t in) {
        if (in > -1) {
            return in - 1;
        } else {
            return 2*in;
        }
    }

    /* Helper class used for calculating non-frozen bit indices. */
    template <std::size_t N, int32_t U, int32_t L>
    struct BhattacharyyaBoundHelper {
        using b_param_sequence = typename Detail::concat_seq<
            typename BhattacharyyaBoundHelper<N - 1u, update_upper_approx(U), update_lower_approx(U)>::b_param_sequence,
            typename BhattacharyyaBoundHelper<N - 1u, update_upper_approx(L), update_lower_approx(L)>::b_param_sequence
        >::integer_sequence;
    };

    template <int32_t U, int32_t L>
    struct BhattacharyyaBoundHelper<1u, U, L> {
        using b_param_sequence = std::integer_sequence<int32_t,
            update_upper_approx(U), update_lower_approx(U), update_upper_approx(L), update_lower_approx(L)>;
    };

    template <std::size_t N, int SNR>
    struct BhattacharyyaBoundSequence {
        using b_param_sequence = typename BhattacharyyaBoundHelper<N - 1u,
            update_upper_approx(SNR), update_lower_approx(SNR)>::b_param_sequence;
    };
}

namespace Polar {

/*
This class facilitates construction of parameterised polar codes. The set of
'good' indices is derived for the chosen parameters using an algorithm
derived from the PCC-0 algorithm described in
https://arxiv.org/pdf/1501.02473.pdf.

In particular, the algorithm has been converted to log-domain and quantised
in order to use integers. This results in a loss of precision and will cause
the constructed code to be less optimal than one constructed with greater
precision. Improving the code construction algorithm would increase the power
of the code with no additional runtime cost; this could be achieved by
introducing a scaling parameter and/or using a polynomial approximation of
the underlying logarithmic functions.

The design-SNR parameter is the log-domain integer representation of the
initial Bhattacharyya parameter given by 0.5*ln(B / (1 - B)), where B is the
initial Bhattacharyya parameter which itself is equal to exp(-10^(S/10)),
where S is the design-SNR in dB. The default value for the 'SNR' parameter is
-2, which corresponds to a design-SNR of 6 dB and seems to give good
results over a range of block sizes and code rates.

Note that the design-SNR described above (6 dB) does not match the optimal
design-SNR for the floating-point version of the algorithm as described for
the PCC-0 algorithm, but generally performs within 0.5 dB or so of PCC-0 with
a design-SNR of 0 dB across a range of block sizes and code rates. If you're
concerned about the code performance I'd recommend you simulate it with a few
different values of the design-SNR parameter and pick the one which performs
best.

Note: Since the code is constructed at compile time using const expressions
and variadic templates, it is important that the compiler is able to handle
the number of template parameters, recursion depth and constexpr depth
needed for the chosen block size. This may require adjusting compiler
options (such as the -ftemplate-depth and -fconstexpr-depth options in GCC
and Clang).
*/
template <std::size_t N, std::size_t K, int SNR = -2>
class PolarCodeConstructor {
    static_assert(N >= 8u && Detail::calculate_hamming_weight(N) == 1u, "Block size must be a power of two and a multiple of 8");
    static_assert(K <= N && K >= 1u, "Number of information bits must be between 1 and block size");
    static_assert(K % 8u == 0u, "Number of information bits must be a multiple of 8");

    using b_param_sequence = typename Detail::BhattacharyyaBoundSequence<Detail::log2(N), SNR>::b_param_sequence;

public:
    /*
    A compile-time index sequence containing the indices of the non-frozen
    bits in sorted order.
    */
    using data_index_sequence = typename Detail::DataBitsIndexSequenceHelper<
        K, Detail::get_pivot_value(K, b_param_sequence{}), std::make_index_sequence<K>, b_param_sequence>::index_sequence;
};

/*
Polar encoder with a block size of N (must be a power of two) and K
information bits (must be smaller than or equal to N). The DataIndices type
is an index sequence with the indices of the K non-frozen bits.

This encoder also supports shortened polar codes, where the output block is
truncated in order to support non-power-of-two encoded lengths.

The implementation here is largely derived from the following papers:
[1] https://arxiv.org/pdf/1507.03614.pdf
[2] https://arxiv.org/pdf/1504.00353.pdf
[3] https://arxiv.org/pdf/1604.08104.pdf
*/
template <std::size_t N, std::size_t K, typename DataIndices>
class PolarEncoder {
    static_assert(N >= 8u && Detail::calculate_hamming_weight(N) == 1u, "Block size must be a power of two and a multiple of 8");
    static_assert(K <= N && K >= 1u, "Number of information bits must be between 1 and block size");
    static_assert(K % 8u == 0u, "Number of information bits must be a multiple of 8");
    static_assert(DataIndices::size() == K, "Number of data bits must be equal to K");

    /*
    Expand the data in the input buffer into the indices of the output buffer
    indicated by the DataIndices index sequence.
    */
    template <std::size_t ShiftIndex, std::size_t... InputIndices, std::size_t... DiffIndices>
    static void expand_buffer(uint8_t *buf, std::index_sequence<InputIndices...>, std::index_sequence<DiffIndices...>) {
        /* Calculate new index sequences. */
        using new_input_sequence = std::index_sequence<DiffIndices & ShiftIndex ? InputIndices + ShiftIndex : InputIndices...>;
        using new_diff_sequence = std::index_sequence<DiffIndices & ShiftIndex ? DiffIndices - ShiftIndex : DiffIndices...>;

        /* Calculate masks. */
        constexpr std::array<uint8_t, N / 8u> mask_shift = Detail::mask_buffer_from_index_sequence<N>(
            std::index_sequence<DiffIndices & ShiftIndex ? InputIndices : N ...>{});
        constexpr std::array<uint8_t, N / 8u> mask_static = Detail::mask_buffer_from_index_sequence<N>(
            std::index_sequence<DiffIndices < ShiftIndex ? InputIndices : N ...>{});

        /* Shift and mask buffer. */
        if (ShiftIndex >= 8u) {
            for (std::size_t i = N / 8u - 1u; i >= ShiftIndex / 8u; i--) {
                buf[i] = (buf[i] & mask_static[i]) | (buf[i - ShiftIndex / 8u] & mask_shift[i - ShiftIndex / 8u]);
            }

            /* Remaining indices are all below the shift index. */
            for (std::size_t i = 0u; i < ShiftIndex / 8u; i++) {
                buf[i] &= mask_static[i];
            }
        } else {
            for (std::size_t i = N / 8u - 1u; i > 0u; i--) {
                buf[i] = (buf[i] & mask_static[i]) |
                    ((buf[i] & mask_shift[i]) >> ShiftIndex) |
                    ((buf[i - 1u] & mask_shift[i - 1u]) << (8u - ShiftIndex));
            }

            buf[0u] = (buf[0u] & mask_static[0u]) | (buf[0u] & mask_shift[0u]) >> ShiftIndex;
        }

        if (ShiftIndex > 1u) {
            expand_buffer<ShiftIndex / 2u>(buf, new_input_sequence{}, new_diff_sequence{});
        }
    }

    template <std::size_t... StageIndices>
    static void encode_stages(const uint8_t *in, std::size_t len, uint8_t *out, std::index_sequence<StageIndices...>) {
        int _[] = { (encode_stage<StageIndices>(), 0)... };
        (void)_;
    }

    template <std::size_t StageIndex>
    static void encode_stage() {

    }

public:
    /*
    Encode a full block. Input buffer must be of size K/8, and output
    buffer must be of size N/8.
    */
    static std::size_t encode(const uint8_t *in, std::size_t len, uint8_t *out) {
        uint8_t buf_expanded[N / 8u] = {};

        /* Expand input buffer to length N/8 bytes. */
        std::memcpy(buf_expanded, in, K / 8u);
        expand_buffer<N / 2u>(buf_expanded, std::make_index_sequence<K>{},
            typename Detail::DiffIndexSequence<DataIndices, std::make_index_sequence<K>>::type{});

        /* Run encoding stages. */
        // encode_stages(in, len, out, std::make_index_sequence<>{});
        std::memcpy(out, buf_expanded, N / 8u);

        return len;
    }
};

/*
Successive cancellation list decoder with a block size of N (must be a power
of two) and K information bits (must be smaller than or equal to N). The
DataIndices type is an index sequence with the indices of the K
non-frozen bits.

The size of the list used is specified by the 'L' parameter. If set to the
default value of one, a non-list version of the algorithm is used.

This decoder also supports shortened polar codes, where the output block is
truncated in order to support non-power-of-two encoded lengths.

The algorithm used is the f-SSCL algorithm (fast simplified successive
cancellation list) described in the following papers:
[1] https://arxiv.org/pdf/1701.08126.pdf
*/
template <std::size_t N, std::size_t K, typename DataIndices, std::size_t L = 1u>
class SuccessiveCancellationListDecoder {
    static_assert(N >= 8u && Detail::calculate_hamming_weight(N) == 1u, "Block size must be a power of two and a multiple of 8");
    static_assert(K <= N && K >= 1u, "Number of information bits must be between 1 and block size");
    static_assert(K % 8u == 0u, "Number of information bits must be a multiple of 8");
    static_assert(DataIndices::size() == K, "Number of data bits must be equal to K");
    static_assert(L >= 1u, "List length must be at least one");

public:
    /*
    Decode using the f-SSC algorithm described in [1]. Input buffer must be
    of size N/8, and output buffer must be of size K/8.
    */
    static std::size_t decode(const uint8_t *in, std::size_t len, uint8_t *out) {

    }
};

}

}
