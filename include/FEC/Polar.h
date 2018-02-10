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
#include <cmath>
#include <cstddef>
#include <cstring>
#include <limits>
#include <numeric>
#include <type_traits>
#include <tuple>
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
    This function return the number of values in Bs which are smaller than or
    equal to the pivot value. Used to ensure that only the excess indices
    with marginal B-parameters are frozen, rather than the ones at the end of
    the sequence.

    Only the first m elements of the sequence are taken into account, to
    support shortened codes.
    */
    template <int32_t... Bs>
    constexpr std::size_t get_num_below_pivot(std::size_t m, int32_t pivot, std::integer_sequence<int32_t, Bs...>) {
        std::size_t count = 0u;
        std::size_t idx = 0u;
        for (int32_t i : { Bs... }) {
            if (idx++ == m) {
                break;
            }

            if (i <= pivot) {
                count++;
            }
        }

        return count;
    }

    /*
    This function finds the smallest value of an integer sequence below which
    there are no fewer than K elements. Only the first m elements of the
    sequence are taken into account, to support shortened codes.
    */
    template <int32_t... Bs>
    constexpr int32_t get_pivot_value(std::size_t m, std::size_t k, int32_t pivot, int32_t max, int32_t min,
            std::integer_sequence<int32_t, Bs...>) {
        std::size_t count = get_num_below_pivot(m, pivot, std::integer_sequence<int32_t, Bs...>{});

        int32_t next_pivot = pivot;
        int32_t next_max = max;
        int32_t next_min = min;
        if (count > k) {
            next_pivot = (pivot + min) / 2;
            next_max = pivot + 1;
        } else if (count < k) {
            next_pivot = (max + pivot) / 2;
            next_min = pivot + 1;
        }

        /*
        This ensures that the largest pivot which satisfies the criteria is
        found, by 'walking' the pivot upwards for the last few steps.
        */
        if (next_max - next_min <= 2) {
            next_pivot = next_min;
        }

        if (next_pivot == pivot) {
            return pivot;
        } else {
            return get_pivot_value(m, k, next_pivot, next_max, next_min,
                std::integer_sequence<int32_t, Bs...>{});
        }
    }

    template <int32_t... Bs>
    constexpr int32_t get_pivot_value(std::size_t m, std::size_t k, std::integer_sequence<int32_t, Bs...>) {
        constexpr auto b_array = std::array<int32_t, sizeof...(Bs)>{ Bs... };
        return get_pivot_value(m, k, (b_array[0u] + b_array[sizeof...(Bs) - 1u]) / 2,
            b_array[0u] + 1, b_array[sizeof...(Bs) - 1u], std::integer_sequence<int32_t, Bs...>{});
    }

    /*
    Return the nth value in Bs which is smaller than or equal to the pivot
    value. Only the first r bits which are equal to the pivot value are
    counted; the rest are ignored.
    */
    template <std::size_t N, int32_t... Bs>
    constexpr std::array<std::size_t, N> get_n_indices_below_pivot_impl(std::size_t r, int32_t pivot,
            std::integer_sequence<int32_t, Bs...>) {
        std::array<std::size_t, N> out = {};
        std::size_t idx = 0u;
        std::size_t count = 0u;
        std::size_t residual = r;
        for (int32_t i : { Bs... }) {
            if (i < pivot || (residual && i == pivot)) {
                out[count++] = idx;
            }
            
            if (count == N) {
                break;
            }

            if (residual && i == pivot) {
                residual--;
            }

            idx++;
        }

        return out;
    }
    
    template <std::size_t R, int32_t Pivot, std::size_t... Is, int32_t... Bs>
    constexpr auto get_n_indices_below_pivot(std::index_sequence<Is...>, std::integer_sequence<int32_t, Bs...>) {
        constexpr std::array<std::size_t, sizeof...(Is)> indices = get_n_indices_below_pivot_impl<sizeof...(Is)>(R, Pivot,
            std::integer_sequence<int32_t, Bs...>{});
        return std::index_sequence<indices[Is]...>{};
    }

    template <std::size_t M, std::size_t K, int32_t Pivot, typename IdxSeq, typename ValSeq>
    struct DataBitsIndexSequenceHelper {
        using index_sequence = decltype(
            get_n_indices_below_pivot<K - get_num_below_pivot(M, Pivot-1u, ValSeq{}), Pivot>(IdxSeq{}, ValSeq{}));
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

The algorithm also supports shortened codes, in which case the M parameter
represents the number of bits after shortening.
*/
template <std::size_t N, std::size_t M, std::size_t K, int SNR = -2>
class PolarCodeConstructor {
    static_assert(N >= 8u && Detail::calculate_hamming_weight(N) == 1u, "Block size must be a power of two and a multiple of 8");
    static_assert(K <= N && K >= 1u, "Number of information bits must be between 1 and block size");
    static_assert(K % 8u == 0u, "Number of information bits must be a multiple of 8");
    static_assert(M % 8u == 0u, "Number of shortened bits must be a multiple of 8");
    static_assert(M <= N && M >= K, "Number of information bits must be between number of information bits and block size");

    using b_param_sequence = typename Detail::BhattacharyyaBoundSequence<Detail::log2(N), SNR>::b_param_sequence;

public:
    /*
    A compile-time index sequence containing the indices of the non-frozen
    bits in sorted order.
    */
    using data_index_sequence = typename Detail::DataBitsIndexSequenceHelper<
        M, K, Detail::get_pivot_value(M, K, b_param_sequence{}), std::make_index_sequence<K>, b_param_sequence>::index_sequence;
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
template <std::size_t N, std::size_t M, std::size_t K, typename DataIndices>
class PolarEncoder;

template <std::size_t N, std::size_t M, std::size_t K, std::size_t... Ds>
class PolarEncoder<N, M, K, std::index_sequence<Ds...>> {
    static_assert(N >= sizeof(bool_vec_t) * 8u && Detail::calculate_hamming_weight(N) == 1u,
        "Block size must be a power of two and a multiple of the machine word size");
    static_assert(K <= N && K >= 1u, "Number of information bits must be between 1 and block size");
    static_assert(K % 8u == 0u, "Number of information bits must be a multiple of 8");
    static_assert(M % 8u == 0u, "Number of shortened bits must be a multiple of 8");
    static_assert(M <= N && M >= K, "Number of information bits must be between number of information bits and block size");
    static_assert(sizeof...(Ds) == K, "Number of data bits must be equal to K");

    /* Encode the whole buffer in blocks of bool_vec_t. */
    template <std::size_t... Is>
    static std::array<bool_vec_t, N / (sizeof(bool_vec_t) * 8u)> encode_stages(const std::array<uint8_t, K / 8u> &in,
            std::index_sequence<Is...>) {
        /*
        The first stage uses block operations to expand the buffer into an
        array of bool_vec_t, ready for efficient word-sized operations.
        */
        std::array<bool_vec_t, N / (sizeof(bool_vec_t) * 8u)> non_systematic = { encode_block<Is>(in)... };

        /*
        Carry out the rest of the encoding on the expanded buffer using full
        bool_vec_t operations.
        */
        constexpr std::size_t num_stages = Detail::log2(N / (sizeof(bool_vec_t) * 8u));

        for (std::size_t i = 0u; i < num_stages; i++) {
            for (std::size_t j = 0u; j < N / (sizeof(bool_vec_t) * 8u); j += (std::size_t)1u << (i + 1u)) {
                for (std::size_t k = 0u; k < (std::size_t)1u << i; k++) {
                    non_systematic[j + k] ^= non_systematic[j + k + ((std::size_t)1u << i)];
                }
            }
        }

        /*
        Do another round of encoding, setting all frozen bits to zero. This
        is done to make the output codeword systematic.
        */
        std::array<bool_vec_t, N / (sizeof(bool_vec_t) * 8u)> systematic = { encode_systematic_block<Is>(non_systematic)... };

        for (std::size_t i = 0u; i < num_stages; i++) {
            for (std::size_t j = 0u; j < N / (sizeof(bool_vec_t) * 8u); j += (std::size_t)1u << (i + 1u)) {
                for (std::size_t k = 0u; k < (std::size_t)1u << i; k++) {
                    systematic[j + k] ^= systematic[j + k + ((std::size_t)1u << i)];
                }
            }
        }

        return systematic;
    }

    /*
    Encode the Ith bool_vec_t block using data from the input buffer.
    */
    template <std::size_t I>
    static bool_vec_t encode_block(const std::array<uint8_t, K / 8u> &in) {
        constexpr std::pair<std::size_t, std::size_t> block_extents = Detail::get_range_extents(
            I * sizeof(bool_vec_t) * 8u, (I+1u) * sizeof(bool_vec_t) * 8u, std::index_sequence<Ds...>{});
        using range_indices = typename Detail::OffsetIndexSequence<
            (std::ptrdiff_t)block_extents.first, std::make_index_sequence<block_extents.second - block_extents.first>>::type;
        using block_data_indices = decltype(Detail::get_range(std::index_sequence<Ds...>{}, range_indices{}));

        return encode_block_rows(in, block_data_indices{}, range_indices{});
    }

    template <std::size_t... Bs, std::size_t... Is>
    static bool_vec_t encode_block_rows(const std::array<uint8_t, K / 8u> &in,
            std::index_sequence<Bs...>, std::index_sequence<Is...>) {
        return (0u ^ ... ^ ((in[Is / 8u] & (uint8_t)1u << (7u - (Is % 8u))) ?
            calculate_row(Bs % (sizeof(bool_vec_t) * 8u)) : 0u));
    }

    template <std::size_t I>
    static bool_vec_t encode_systematic_block(const std::array<bool_vec_t, N / (sizeof(bool_vec_t) * 8u)> &in) {
        constexpr std::pair<std::size_t, std::size_t> block_extents = Detail::get_range_extents(
            I * sizeof(bool_vec_t) * 8u, (I+1u) * sizeof(bool_vec_t) * 8u, std::index_sequence<Ds...>{});
        using range_indices = typename Detail::OffsetIndexSequence<
            (std::ptrdiff_t)block_extents.first, std::make_index_sequence<block_extents.second - block_extents.first>>::type;
        using block_data_indices = decltype(Detail::get_range(std::index_sequence<Ds...>{}, range_indices{}));

        return encode_systematic_block_rows(in, block_data_indices{});
    }

    template <std::size_t... Is>
    static bool_vec_t encode_systematic_block_rows(const std::array<bool_vec_t, N / (sizeof(bool_vec_t) * 8u)> &in,
            std::index_sequence<Is...>) {
        return (0u ^ ... ^ ((in[Is / (sizeof(bool_vec_t) * 8u)] &
            (bool_vec_t)1u << ((sizeof(bool_vec_t) * 8u - 1u) - (Is % (sizeof(bool_vec_t) * 8u)))) ?
            calculate_row(Is % (sizeof(bool_vec_t) * 8u)) : 0u));
    }

    /*
    For the given data index, return the corresponding generator matrix row.
    */
    template <std::size_t I = sizeof(bool_vec_t) * 8u>
    constexpr static bool_vec_t calculate_row(std::size_t r) {
        static_assert(I <= sizeof(bool_vec_t) * 8u, "Can only calculate blocks up to the size of bool_vec_t");

        if constexpr (I == 1u) {
            return (bool_vec_t)1u << (sizeof(bool_vec_t) * 8u - 1u);
        } else {
            if (r >= I / 2u) {
                return calculate_row<I / 2u>(r % (I / 2u)) | calculate_row<I / 2u>(r % (I / 2u)) >> (I / 2u);
            } else {
                return calculate_row<I / 2u>(r % (I / 2u));
            }
        }
    }

public:
    /*
    Encode a full block. Input buffer must be of size K/8 bytes, and output
    buffer must be of size M/8 bytes.
    */
    static std::array<uint8_t, M / 8u> encode(const std::array<uint8_t, K / 8u> &in) {
        /* Run encoding stages. */
        auto buf_encoded = encode_stages(in, std::make_index_sequence<N / (sizeof(bool_vec_t) * 8u)>{});

        std::array<uint8_t, M / 8u> out;
        for (std::size_t i = 0u; i < M / 8u; i++) {
            out[i] = buf_encoded[i / sizeof(bool_vec_t)] >> ((sizeof(bool_vec_t)-1u - (i % sizeof(bool_vec_t))) * 8u);
        }

        return out;
    }
};

namespace Decoder::Operations {

/* Do the f-operation (min-sum). */
template <typename llr_t, std::size_t Nv>
static std::array<llr_t, Nv / 2u> f_op(const std::array<llr_t, Nv> &alpha) {
    std::array<llr_t, Nv / 2u> out;
    for (std::size_t i = 0u; i < Nv / 2u; i++) {
        llr_t min_abs = std::min(std::abs(alpha[i]), std::abs(alpha[i + Nv / 2u]));
        out[i] = std::signbit(alpha[i] ^ alpha[i + Nv / 2u]) ? -min_abs : min_abs;
    }

    return out;
}

/* Simplification of f-operation when only the sign is needed. */
template <typename llr_t, std::size_t Nv>
static std::array<llr_t, Nv / 2u> f_op_r1(const std::array<llr_t, Nv> &alpha) {
    std::array<llr_t, Nv / 2u> out;
    for (std::size_t i = 0u; i < Nv / 2u; i++) {
        out[i] = std::signbit(alpha[i] ^ alpha[i + Nv / 2u]);
    }

    return out;
}

/* Do the g-operation. */
template <typename llr_t, std::size_t Nv>
static std::array<llr_t, Nv / 2u> g_op(const std::array<llr_t, Nv> &alpha, const bool *beta) {
    std::array<llr_t, Nv / 2u> out;
    for (std::size_t i = 0u; i < Nv / 2u; i++) {
        out[i] = alpha[i + Nv / 2u] + ((1 - 2 * (llr_t)beta[i]) * alpha[i]);
    }

    return out;
}

/* Overload of the g-operation for the case where beta is all zeros. */
template <typename llr_t, std::size_t Nv>
static std::array<llr_t, Nv / 2u> g_op_pos(const std::array<llr_t, Nv> &alpha) {
    std::array<llr_t, Nv / 2u> out;
    for (std::size_t i = 0u; i < Nv / 2u; i++) {
        out[i] = alpha[i + Nv / 2u] + alpha[i];
    }

    return out;
}

/* Overload of the g-operation for the case where beta is all ones. */
template <typename llr_t, std::size_t Nv>
static std::array<llr_t, Nv / 2u> g_op_neg(const std::array<llr_t, Nv> &alpha) {
    std::array<llr_t, Nv / 2u> out;
    for (std::size_t i = 0u; i < Nv / 2u; i++) {
        out[i] = alpha[i + Nv / 2u] - alpha[i];
    }

    return out;
}

/* Do the h-operation. */
template <std::size_t Nv>
static void h_op(bool *beta) {
    for (std::size_t i = 0u; i < Nv / 2u; i++) {
        beta[i] ^= beta[i + Nv / 2u];
    }
}

/* Simplified operation for rate-1 nodes. */
template <typename llr_t, std::size_t Nv>
static void rate_1(const std::array<llr_t, Nv> &alpha, bool *beta) {
    for (std::size_t i = 0u; i < Nv; i++) {
        beta[i] = std::signbit(alpha[i]);
    }
}

/* Simplified operation for repetition nodes. */
template <typename llr_t, std::size_t Nv>
static void rep(const std::array<llr_t, Nv> &alpha, bool *beta) {
    if (std::signbit(std::accumulate(alpha.begin(), alpha.begin() + Nv, 0))) {
        std::fill_n(beta, Nv, true);
    }
}

/* Simplified operation for SPC nodes. */
template <typename llr_t, std::size_t Nv>
static void spc(const std::array<llr_t, Nv> &alpha, bool *beta) {
    bool parity = beta[0u] = std::signbit(alpha[0u]);
    llr_t abs_min = std::abs(alpha[0u]);
    std::size_t abs_min_idx = 0u;
    for (std::size_t i = 1u; i < Nv; i++) {
        /* Make the bit decision by thresholding the LLR. */
        beta[i] = std::signbit(alpha[i]);

        /* Keep track of the parity. */
        parity ^= beta[i];

        /* Keep track of the worst bit. */
        llr_t alpha_abs = std::abs(alpha[i]);
        if (alpha_abs < abs_min) {
            abs_min = alpha_abs;
            abs_min_idx = i;
        }
    }

    /* Apply the parity to the worst bit. */
    beta[abs_min_idx] ^= parity;
}

}

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
template <std::size_t N, std::size_t M, std::size_t K, typename DataIndices, typename llr_t = int32_t, std::size_t L = 1u>
class SuccessiveCancellationListDecoder;

template <std::size_t N, std::size_t M, std::size_t K, std::size_t... Ds, typename llr_t, std::size_t L>
class SuccessiveCancellationListDecoder<N, M, K, std::index_sequence<Ds...>, llr_t, L> {
    static_assert(N >= 8u && Detail::calculate_hamming_weight(N) == 1u, "Block size must be a power of two and a multiple of 8");
    static_assert(K <= N && K >= 1u, "Number of information bits must be between 1 and block size");
    static_assert(K % 8u == 0u, "Number of information bits must be a multiple of 8");
    static_assert(M % 8u == 0u, "Number of shortened bits must be a multiple of 8");
    static_assert(M <= N && M >= K, "Number of information bits must be between number of information bits and block size");
    static_assert(sizeof...(Ds) == K, "Number of data bits must be equal to K");
    static_assert(L >= 1u, "List length must be at least one");

    /*
    Calculate the initial LLR value for shortened bits. Chosen to avoid
    overflowing llr_t if possible, but otherwise the f-, g- and h-operations
    need to be specialised to use saturating arithmetic.
    */
    static constexpr llr_t init_short =
        std::numeric_limits<llr_t>::max() >> std::min(Detail::log2(N) + 1u, sizeof(llr_t) * 8u - 4u);

    /* Test to see if a node is rate zero (all frozen bits). */
    template <std::size_t... Is>
    static constexpr bool is_rate_0_node(std::index_sequence<Is...>) {
        return false;
    }

    static constexpr bool is_rate_0_node(std::index_sequence<>) {
        return true;
    }

    /* Test to see if a node is rate one (all data bits). */
    template <std::size_t... Is>
    static constexpr bool is_rate_1_node(std::size_t Nv, std::index_sequence<Is...>) {
        return sizeof...(Is) == Nv;
    }

    /*
    Test to see if a node is a repetition node (only last bit not frozen).
    */
    template <std::size_t... Is>
    static constexpr bool is_rep_node(std::size_t Nv, std::index_sequence<Is...>) {
        return sizeof...(Is) == 1u && Detail::get_index<0u>(std::index_sequence<Is...>{}) == Nv - 1u;
    }

    /*
    Test to see if a node is a single parity check (SPC) node (only first bit
    frozen).
    */
    template <std::size_t... Is>
    static constexpr bool is_spc_node(std::size_t Nv, std::index_sequence<Is...>) {
        return sizeof...(Is) == Nv - 1u && Detail::get_index<0u>(std::index_sequence<Is...>{}) == 1u;
    }

    template <std::size_t Nv, std::size_t... Is>
    static void decode_stages(const std::array<llr_t, Nv> &alpha, bool *beta,
            std::index_sequence<Is...>) {
        /* Do special case checks for f-SSCL to reduce computational load. */
        if constexpr (is_rate_1_node(Nv, std::index_sequence<Is...>{})) {
            /*
            If none of the bits are frozen, this is a rate-1 node and we can
            simply threshold all the LLRs directly.
            */
            Decoder::Operations::rate_1(alpha, beta);
        } else if constexpr (is_rep_node(Nv, std::index_sequence<Is...>{})) {
            /*
            If only the last bit is not frozen, this is a repetition node.
            */
            Decoder::Operations::rep(alpha, beta);
        } else if constexpr (is_spc_node(Nv, std::index_sequence<Is...>{})) {
            /*
            If only the first bit is frozen, this is a single parity check
            (SPC) node.
            */
            Decoder::Operations::spc(alpha, beta);
        } else {
            /* Get data indices for the left and right sub-trees. */
            constexpr std::pair<std::size_t, std::size_t> block_extents_left = Detail::get_range_extents(
                (std::size_t)0u, (std::size_t)Nv / 2u, std::index_sequence<Is...>{});
            using range_indices_left = typename Detail::OffsetIndexSequence<
                (ptrdiff_t)block_extents_left.first,
                std::make_index_sequence<block_extents_left.second - block_extents_left.first>>::type;
            using data_indices_left = decltype(Detail::get_range(std::index_sequence<Is...>{}, range_indices_left{}));

            constexpr std::pair<std::size_t, std::size_t> block_extents_right = Detail::get_range_extents(
                (std::size_t)Nv / (std::size_t)2u, Nv, std::index_sequence<Is...>{});
            using range_indices_right = typename Detail::OffsetIndexSequence<
                (ptrdiff_t)block_extents_right.first,
                std::make_index_sequence<block_extents_right.second - block_extents_right.first>>::type;
            using data_indices_right = typename Detail::OffsetIndexSequence<
                -(ptrdiff_t)Nv / 2, decltype(Detail::get_range(std::index_sequence<Is...>{}, range_indices_right{}))>::type;

            /*
            Execute left and right subtrees, with optimisations where possible.
            */
            if constexpr (is_rate_0_node(data_indices_left{})) {
                /* If left is rate-0, then no f-operation is required. */
            } else if constexpr (is_rate_1_node(Nv, data_indices_left{})) {
                /* If left is rate-1, then f-operation is simplified. */
                decode_stages(Decoder::Operations::f_op_r1(alpha), beta, data_indices_left{});
            } else {
                /* Nominal case. */
                decode_stages(Decoder::Operations::f_op(alpha), beta, data_indices_left{});
            }

            if constexpr (is_rate_0_node(data_indices_right{})) {
                /*
                If right is rate-0, then no g- or h-operation is required.
                */
            } else if constexpr (is_rate_0_node(data_indices_left{})) {
                /*
                If left was rate-0, a specialised g-operation can be used.
                */
                decode_stages(Decoder::Operations::g_op_pos(alpha), &beta[Nv / 2u], data_indices_right{});
            } else if constexpr (is_rep_node(Nv, data_indices_left{})) {
                /*
                If left was a repetition node, a specialised g-operation can
                be used.
                */
                decode_stages(beta[0u] ? Decoder::Operations::g_op_neg(alpha) : Decoder::Operations::g_op_pos(alpha),
                    &beta[Nv / 2u], data_indices_right{});
            } else {
                /* Nominal case. */
                decode_stages(Decoder::Operations::g_op(alpha, beta), &beta[Nv / 2u], data_indices_right{});
            }

            /* Make bit-decisions. */
            if constexpr (is_rate_0_node(data_indices_right{})) {
                /* No h-operation required if right node is rate-0. */
            } else if constexpr (is_rate_0_node(data_indices_left{})) {
                std::copy_n(beta + Nv / 2u, Nv / 2u, beta);
            } else {
                Decoder::Operations::h_op<Nv>(beta);
            }
        }
    }

    static std::array<uint8_t, K / 8u> pack_output(std::array<bool, N> in) {
        std::array<uint8_t, K / 8u> out = {};
        std::size_t out_idx = 0u;

        ((out[out_idx++ / 8u] |= in[Ds] ? (uint8_t)1u << (7u - (out_idx % 8u)) : 0u), ...);

        return out;
    }

public:
    /*
    Decode using the f-SSC algorithm described in [1]. Input buffer must be
    of size M/8 bytes, and output buffer must be of size K/8.
    */
    static std::array<uint8_t, K / 8u> decode(const std::array<uint8_t, M / 8u> &in) {
        /* Initialise LLRs based on input data. */
        std::array<llr_t, N> alpha;
        for (std::size_t i = 0u; i < M; i++) {
            alpha[i] = in[i / 8u] & ((uint8_t)1u << (7u - (i % 8u))) ? -1 : 1;
        }

        /*
        The remaining (shortened) bytes are implicitly set to the maximum
        positive value for the LLR datatype, to indicate complete certainty
        as to their value (zero).
        */
        std::fill_n(alpha.begin() + M, N - M, init_short);

        /* Run decoding stages and return packed data. */
        std::array<bool, N> beta = {};
        decode_stages(alpha, beta.data(), std::index_sequence<Ds...>{});
        return pack_output(beta);
    }
};

}

}
