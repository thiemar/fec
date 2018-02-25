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

#include "arm_math.h"

template <std::size_t N>
static inline uint32_t simd_q7_load(const int8_t *in) {
    static_assert(N >= 1u && N <= 4u, "Block size out of range for SIMD vector");

    uint32_t out = 0u;
    std::copy_n(in, N, (int8_t *)&out);

    return out;
}

template <std::size_t N>
static inline void simd_q7_store(const uint32_t *in, int8_t *out) {
    static_assert(N >= 1u && N <= 4u, "Block size out of range for SIMD vector");

    std::copy_n((int8_t *)in, N, out);
}

static inline uint32_t simd_q7_abs(uint32_t in) {
    uint32_t in_neg = __SSUB8(0u, in);

    __SADD8(in, 0u);
    return __SEL(in, in_neg);
}

template <std::size_t Nv>
struct f_op_container<int8_t, Nv> {
    static std::array<int8_t, Nv / 2u> op(const std::array<int8_t, Nv> &alpha) {
        /* Ensure Nv is a power of two and greater than one. */
        static_assert(Nv > 1u && Detail::calculate_hamming_weight(Nv) == 1u,
            "Block size must be a power of two and greater than one");

        constexpr std::size_t block_size = std::min(Nv / 2u, 4u);

        std::array<int8_t, Nv / 2u> out;
        for (std::size_t i = 0u; i < Nv / 2u; i += 4u) {
            uint32_t alpha_1 = simd_q7_load<block_size>(alpha.data() + i);
            uint32_t alpha_2 = simd_q7_load<block_size>(alpha.data() + i + Nv / 2u);

            /* Get absolute values. */
            uint32_t abs1 = simd_q7_abs(alpha_1);
            uint32_t abs2 = simd_q7_abs(alpha_2);

            /* Select the smallest. */
            __SSUB8(abs1, abs2);
            uint32_t min_abs = __SEL(abs2, abs1);

            /* Select positive or negative values based on the sign. */
            uint32_t min_abs_neg = __SSUB8(0u, min_abs);

            /* Get the signs of the output vector. */
            __SSUB8(alpha_1 ^ alpha_2, 0u);
            uint32_t out_vec = __SEL(min_abs, min_abs_neg);
            simd_q7_store<block_size>(&out_vec, out.data() + i);
        }

        return out;
    }
};

template <std::size_t Nv>
struct f_op_r1_container<int8_t, Nv> {
    static std::array<int8_t, Nv / 2u> op(const std::array<int8_t, Nv> &alpha) {
        /* Ensure Nv is a power of two and greater than one. */
        static_assert(Nv > 1u && Detail::calculate_hamming_weight(Nv) == 1u,
            "Block size must be a power of two and greater than one");

        constexpr std::size_t block_size = std::min(Nv / 2u, 4u);

        std::array<int8_t, Nv / 2u> out;
        for (std::size_t i = 0u; i < Nv / 2u; i += 4u) {
            uint32_t alpha_1 = simd_q7_load<block_size>(alpha.data() + i);
            uint32_t alpha_2 = simd_q7_load<block_size>(alpha.data() + i + Nv / 2u);

            uint32_t out_vec = alpha_1 ^ alpha_2;
            simd_q7_store<block_size>(&out_vec, out.data() + i);
        }

        return out;
    }
};

template <std::size_t Nv>
struct g_op_container<int8_t, Nv> {
    static std::array<int8_t, Nv / 2u> op(const std::array<int8_t, Nv> &alpha, const uint8_t *beta) {
        /* Ensure Nv is a power of two and greater than one. */
        static_assert(Nv > 1u && Detail::calculate_hamming_weight(Nv) == 1u,
            "Block size must be a power of two and greater than one");

        constexpr std::size_t block_size = std::min(Nv / 2u, 4u);

        std::array<int8_t, Nv / 2u> out;
        for (std::size_t i = 0u; i < Nv / 2u; i += 4u) {
            uint32_t alpha_1 = simd_q7_load<block_size>(alpha.data() + i);
            uint32_t alpha_2 = simd_q7_load<block_size>(alpha.data() + i + Nv / 2u);
            uint32_t beta_vec = simd_q7_load<block_size>((int8_t *)beta + i);

            uint32_t alpha_1_neg = __SSUB8(0u, alpha_1);

            __SSUB8(0u, beta_vec);
            uint32_t out_vec = __SADD8(alpha_2, __SEL(alpha_1, alpha_1_neg));
            simd_q7_store<block_size>(&out_vec, out.data() + i);
        }

        return out;
    }
};

template <std::size_t Nv>
struct g_op_0_container<int8_t, Nv> {
    static std::array<int8_t, Nv / 2u> op(const std::array<int8_t, Nv> &alpha) {
        /* Ensure Nv is a power of two and greater than one. */
        static_assert(Nv > 1u && Detail::calculate_hamming_weight(Nv) == 1u,
            "Block size must be a power of two and greater than one");

        constexpr std::size_t block_size = std::min(Nv / 2u, 4u);

        std::array<int8_t, Nv / 2u> out;
        for (std::size_t i = 0u; i < Nv / 2u; i += 4u) {
            uint32_t alpha_1 = simd_q7_load<block_size>(alpha.data() + i);
            uint32_t alpha_2 = simd_q7_load<block_size>(alpha.data() + i + Nv / 2u);

            uint32_t out_vec = __SADD8(alpha_2, alpha_1);
            simd_q7_store<block_size>(&out_vec, out.data() + i);
        }

        return out;
    }
};

template <std::size_t Nv>
struct g_op_1_container<int8_t, Nv> {
    static std::array<int8_t, Nv / 2u> op(const std::array<int8_t, Nv> &alpha) {
        /* Ensure Nv is a power of two and greater than one. */
        static_assert(Nv > 1u && Detail::calculate_hamming_weight(Nv) == 1u,
            "Block size must be a power of two and greater than one");

        constexpr std::size_t block_size = std::min(Nv / 2u, 4u);

        std::array<int8_t, Nv / 2u> out;
        for (std::size_t i = 0u; i < Nv / 2u; i += 4u) {
            uint32_t alpha_1 = simd_q7_load<block_size>(alpha.data() + i);
            uint32_t alpha_2 = simd_q7_load<block_size>(alpha.data() + i + Nv / 2u);

            uint32_t out_vec = __SSUB8(alpha_2, alpha_1);
            simd_q7_store<block_size>(&out_vec, out.data() + i);
        }

        return out;
    }
};

template <std::size_t Nv>
struct h_op_container<int8_t, Nv> {
    static void op(uint8_t *beta) {
        /* Ensure Nv is a power of two and greater than one. */
        static_assert(Nv > 1u && Detail::calculate_hamming_weight(Nv) == 1u,
            "Block size must be a power of two and greater than one");

        constexpr std::size_t block_size = std::min(Nv / 2u, 4u);

        for (std::size_t i = 0u; i < Nv / 2u; i += 4u) {
            uint32_t beta_vec_1 = simd_q7_load<block_size>((int8_t *)beta + i);
            uint32_t beta_vec_2 = simd_q7_load<block_size>((int8_t *)beta + i + Nv / 2u);

            uint32_t out_vec = beta_vec_1 ^ beta_vec_2;
            simd_q7_store<block_size>(&out_vec, (int8_t *)beta + i);
        }
    }
};

template <std::size_t Nv>
struct rate_1_container<int8_t, Nv> {
    static void op(const std::array<int8_t, Nv> &alpha, uint8_t *beta) {
        /* Ensure Nv is a power of two and greater than one. */
        static_assert(Nv > 1u && Detail::calculate_hamming_weight(Nv) == 1u,
            "Block size must be a power of two and greater than one");

        constexpr std::size_t block_size = std::min(Nv, 4u);

        for (std::size_t i = 0u; i < Nv; i += 4u) {
            uint32_t alpha_vec = simd_q7_load<block_size>(alpha.data() + i);

            __SADD8(alpha_vec, 0u);
            uint32_t out_vec = __SEL(0x00000000u, 0x01010101u);
            simd_q7_store<block_size>(&out_vec, (int8_t *)beta + i);
        }
    }
};

template <std::size_t Nv>
struct rep_container<int8_t, Nv> {
    static void op(const std::array<int8_t, Nv> &alpha, uint8_t *beta) {
        /* Ensure Nv is a power of two and greater than one. */
        static_assert(Nv > 1u && Detail::calculate_hamming_weight(Nv) == 1u,
            "Block size must be a power of two and greater than one");

        constexpr std::size_t block_size = std::min(Nv, 4u);

        uint32_t accumulator = 0u;
        for (std::size_t i = 0u; i < Nv; i += 4u) {
            uint32_t alpha_vec = simd_q7_load<block_size>(alpha.data() + i);

            accumulator = __SXTAB16(accumulator, alpha_vec);
            accumulator = __SXTAB16(accumulator, alpha_vec >> 8u);
        }

        /* Add accumulator halves. */
        std::fill_n(beta, Nv, ((int16_t)accumulator + (int16_t)(accumulator >> 16u)) < 0);
    }
};

template <std::size_t Nv>
struct spc_container<int8_t, Nv> {
    static void op(const std::array<int8_t, Nv> &alpha, uint8_t *beta) {
        /* Ensure Nv is a power of two and greater than one. */
        static_assert(Nv > 1u && Detail::calculate_hamming_weight(Nv) == 1u,
            "Block size must be a power of two and greater than one");

        constexpr std::size_t block_size = std::min(Nv, 4u);

        std::size_t abs_min_idx = 0u;
        uint32_t parity = 0u;
        int8_t abs_min = std::numeric_limits<int8_t>::max();
        for (std::size_t i = 0u; i < Nv; i += 4u) {
            uint32_t alpha_vec = simd_q7_load<block_size>(alpha.data() + i);

            __SADD8(alpha_vec, 0u);
            uint32_t out_vec = __SEL(0x00000000u, 0x01010101u);
            simd_q7_store<block_size>(&out_vec, (int8_t *)beta + i);

            /* Calculate running parity. */
            parity = __USADA8(out_vec, 0u, parity);

            /* Keep track of absolute minimum. */
            uint32_t alpha_abs = simd_q7_abs(alpha_vec);
            for (std::size_t j = 0u; j < block_size; j++) {
                if ((int8_t)(alpha_abs >> (j * 8u)) < abs_min) {
                    abs_min = (int8_t)(alpha_abs >> (j * 8u));
                    abs_min_idx = i * 4u + j;
                }
            }
        }

        /* Apply the parity to the worst bit. */
        beta[abs_min_idx] ^= (parity % 2u);
    }
};
