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

#include <x86intrin.h>

template <std::size_t Nv>
static inline __m128i load_vec(const int8_t *data) {
    /* Ensure Nv is a power of two and greater than one. */
    static_assert(Nv > 1u && Detail::calculate_hamming_weight(Nv) == 1u,
        "Block size must be a power of two and greater than one");
    /* Ensure Nv is no greater than eight. */
    static_assert(Nv <= 16u, "Block size must be sixteen or less");

    if constexpr (Nv == 16u) {
        return _mm_setr_epi8(
            data[0u], data[1u], data[2u], data[3u],
            data[4u], data[5u], data[6u], data[7u],
            data[8u], data[9u], data[10u], data[11u],
            data[12u], data[13u], data[14u], data[15u]);
    } else if constexpr (Nv == 8u) {
        return _mm_setr_epi8(
            data[0u], data[1u], data[2u], data[3u],
            data[4u], data[5u], data[6u], data[7u],
            0, 0, 0, 0, 0, 0, 0, 0);
    } else if constexpr (Nv == 4u) {
        return _mm_setr_epi8(
            data[0u], data[1u], data[2u], data[3u],
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    } else {
        return _mm_setr_epi8(data[0u], data[1u],
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
}

template <std::size_t Nv>
static inline __m128i load_vec(const uint8_t *data) {
    /* Ensure Nv is a power of two and greater than one. */
    static_assert(Nv > 1u && Detail::calculate_hamming_weight(Nv) == 1u,
        "Block size must be a power of two and greater than one");
    /* Ensure Nv is no greater than eight. */
    static_assert(Nv <= 16u, "Block size must be sixteen or less");

    if constexpr (Nv == 16u) {
        return _mm_setr_epi8(
            data[0u], data[1u], data[2u], data[3u],
            data[4u], data[5u], data[6u], data[7u],
            data[8u], data[9u], data[10u], data[11u],
            data[12u], data[13u], data[14u], data[15u]);
    } else if constexpr (Nv == 8u) {
        return _mm_setr_epi8(
            data[0u], data[1u], data[2u], data[3u],
            data[4u], data[5u], data[6u], data[7u],
            0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u);
    } else if constexpr (Nv == 4u) {
        return _mm_setr_epi8(
            data[0u], data[1u], data[2u], data[3u],
            0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u);
    } else {
        return _mm_setr_epi8(data[0u], data[1u],
            0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u);
    }
}

template <std::size_t Nv>
struct f_op_container<int8_t, Nv> {
    static std::array<int8_t, Nv / 2u> op(const std::array<int8_t, Nv> &alpha) {
        /* Ensure Nv is a power of two and greater than one. */
        static_assert(Nv > 1u && Detail::calculate_hamming_weight(Nv) == 1u,
            "Block size must be a power of two and greater than one");

        std::array<int8_t, Nv / 2u> out;
        if constexpr (Nv > 16u) {
            for (std::size_t i = 0u; i < Nv / 2u; i += 16u) {
                __m128i alpha_vec_1 = _mm_loadu_si128((__m128i *)&alpha[i]);
                __m128i alpha_vec_2 = _mm_loadu_si128((__m128i *)&alpha[i + Nv / 2u]);
                _mm_storeu_si128((__m128i *)&out[i],
                    _mm_sign_epi8(_mm_min_epi8(_mm_abs_epi8(alpha_vec_1), _mm_abs_epi8(alpha_vec_2)),
                    _mm_sign_epi8(alpha_vec_1, alpha_vec_2)));
            }
        } else {
            __m128i alpha_vec_1 = load_vec<Nv / 2u>(alpha.begin());
            __m128i alpha_vec_2 = load_vec<Nv / 2u>(alpha.begin() + Nv / 2u);
            __m128i c = _mm_sign_epi8(_mm_min_epi8(
                _mm_abs_epi8(alpha_vec_1), _mm_abs_epi8(alpha_vec_2)),
                _mm_sign_epi8(alpha_vec_1, alpha_vec_2));
            std::copy_n((int8_t *)&c, Nv / 2u, out.begin());
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

        std::array<int8_t, Nv / 2u> out;
        if constexpr (Nv > 16u) {
            for (std::size_t i = 0u; i < Nv / 2u; i += 16u) {
                __m128i alpha_vec_1 = _mm_loadu_si128((__m128i *)&alpha[i]);
                __m128i alpha_vec_2 = _mm_loadu_si128((__m128i *)&alpha[i + Nv / 2u]);
                _mm_storeu_si128((__m128i *)&out[i], _mm_sign_epi8(alpha_vec_1, alpha_vec_2));
            }
        } else {
            __m128i c = _mm_sign_epi8(load_vec<Nv / 2u>(alpha.begin()), load_vec<Nv / 2u>(alpha.begin() + Nv / 2u));
            std::copy_n((int8_t *)&c, Nv / 2u, out.begin());
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

        std::array<int8_t, Nv / 2u> out;
        if constexpr (Nv > 16u) {
            for (std::size_t i = 0u; i < Nv / 2u; i += 16u) {
                __m128i alpha_vec_1 = _mm_loadu_si128((__m128i *)&alpha[i]);
                __m128i alpha_vec_2 = _mm_loadu_si128((__m128i *)&alpha[i + Nv / 2u]);
                __m128i beta_vec = _mm_loadu_si128((__m128i *)&beta[i]);
                _mm_storeu_si128((__m128i *)&out[i], _mm_blendv_epi8(
                    _mm_subs_epi8(alpha_vec_2, alpha_vec_1),
                    _mm_adds_epi8(alpha_vec_2, alpha_vec_1),
                    _mm_cmpeq_epi8(beta_vec, _mm_setzero_si128())));
            }
        } else {
            __m128i alpha_vec_1 = load_vec<Nv / 2u>(alpha.begin());
            __m128i alpha_vec_2 = load_vec<Nv / 2u>(alpha.begin() + Nv / 2u);
            __m128i beta_vec = load_vec<Nv / 2u>(beta);
            __m128i c = _mm_blendv_epi8(
                _mm_subs_epi8(alpha_vec_2, alpha_vec_1),
                _mm_adds_epi8(alpha_vec_2, alpha_vec_1),
                _mm_cmpeq_epi8(beta_vec, _mm_setzero_si128()));
            std::copy_n((int8_t *)&c, Nv / 2u, out.begin());
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

        std::array<int8_t, Nv / 2u> out;
        if constexpr (Nv > 16u) {
            for (std::size_t i = 0u; i < Nv / 2u; i += 16u) {
                __m128i alpha_vec_1 = _mm_loadu_si128((__m128i *)&alpha[i]);
                __m128i alpha_vec_2 = _mm_loadu_si128((__m128i *)&alpha[i + Nv / 2u]);
                _mm_storeu_si128((__m128i *)&out[i], _mm_adds_epi8(alpha_vec_2, alpha_vec_1));
            }
        } else {
            __m128i c = _mm_adds_epi8(load_vec<Nv / 2u>(alpha.begin() + Nv / 2u), load_vec<Nv / 2u>(alpha.begin()));
            std::copy_n((int8_t *)&c, Nv / 2u, out.begin());
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

        std::array<int8_t, Nv / 2u> out;
        if constexpr (Nv > 16u) {
            for (std::size_t i = 0u; i < Nv / 2u; i += 16u) {
                __m128i alpha_vec_1 = _mm_loadu_si128((__m128i *)&alpha[i]);
                __m128i alpha_vec_2 = _mm_loadu_si128((__m128i *)&alpha[i + Nv / 2u]);
                _mm_storeu_si128((__m128i *)&out[i], _mm_subs_epi8(alpha_vec_2, alpha_vec_1));
            }
        } else {
            __m128i c = _mm_subs_epi8(load_vec<Nv / 2u>(alpha.begin() + Nv / 2u), load_vec<Nv / 2u>(alpha.begin()));
            std::copy_n((int8_t *)&c, Nv / 2u, out.begin());
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

        if constexpr (Nv > 16u) {
            for (std::size_t i = 0u; i < Nv / 2u; i += 16u) {
                __m128i beta_vec_1 = _mm_loadu_si128((__m128i *)&beta[i]);
                __m128i beta_vec_2 = _mm_loadu_si128((__m128i *)&beta[i + Nv / 2u]);
                _mm_storeu_si128((__m128i *)&beta[i], _mm_xor_si128(beta_vec_1, beta_vec_2));
            }
        } else {
            __m128i c = _mm_xor_si128(load_vec<Nv / 2u>(beta), load_vec<Nv / 2u>(beta + Nv / 2u));
            std::copy_n((int8_t *)&c, Nv / 2u, beta);
        }
    }
};

template <std::size_t Nv>
struct rate_1_container<int8_t, Nv> {
    static void op(const std::array<int8_t, Nv> &alpha, uint8_t *beta) {
        /* Ensure Nv is a power of two and greater than one. */
        static_assert(Detail::calculate_hamming_weight(Nv) == 1u,
            "Block size must be a power of two");

        if constexpr (Nv > 8u) {
            for (std::size_t i = 0u; i < Nv; i += 16u) {
                __m128i alpha_vec = _mm_loadu_si128((__m128i *)&alpha[i]);
                _mm_storeu_si128((__m128i *)&beta[i], _mm_blendv_epi8(_mm_set1_epi8(0), _mm_set1_epi8(1), alpha_vec));
            }
        } else if constexpr (Nv > 1u) {
            __m128i c = _mm_blendv_epi8(_mm_set1_epi8(0), _mm_set1_epi8(1), load_vec<Nv>(alpha.data()));
            std::copy_n((int8_t *)&c, Nv, beta);
        } else {
            beta[0] = std::signbit(alpha[0]);
        }
    }
};

template <std::size_t Nv>
struct rep_container<int8_t, Nv> {
    static void op(const std::array<int8_t, Nv> &alpha, uint8_t *beta) {
        /* Ensure Nv is a power of two and greater than one. */
        static_assert(Detail::calculate_hamming_weight(Nv) == 1u,
            "Block size must be a power of two");

        if constexpr (Nv > 8u) {
            __m128i acc = _mm_cvtepi8_epi16(_mm_loadu_si128((__m128i *)&alpha[0u]));
            for (std::size_t i = 8u; i < Nv; i += 8u) {
                acc = _mm_hadd_epi16(acc, _mm_cvtepi8_epi16(_mm_loadu_si128((__m128i *)&alpha[i])));
            }

            acc = _mm_hadd_epi16(acc, acc);
            acc = _mm_hadd_epi16(acc, acc);
            acc = _mm_hadd_epi16(acc, acc);
            int16_t sum = _mm_extract_epi16(acc, 0u);

            for (std::size_t i = 0u; i < Nv; i += 16u) {
                _mm_storeu_si128((__m128i *)&beta[i], _mm_set1_epi8(std::signbit(sum)));
            }
        } else if constexpr (Nv > 2u) {
            __m128i acc = _mm_cvtepi8_epi16(load_vec<Nv>(alpha.data()));
            acc = _mm_hadd_epi16(acc, acc);
            acc = _mm_hadd_epi16(acc, acc);
            acc = _mm_hadd_epi16(acc, acc);
            int16_t sum = _mm_extract_epi16(acc, 0u);

            if (std::signbit(sum)) {
                std::fill_n(beta, Nv, 1u);
            }
        } else {
            if (std::signbit(alpha[0u] + alpha[1u])) {
                beta[0u] = beta[1u] = 1u;
            }
        }
    }
};

// template <std::size_t Nv>
// struct spc_container<int8_t, Nv> {
//     static void op(const std::array<int8_t, Nv> &alpha, uint8_t *beta) {
//         /* Ensure Nv is a power of two and greater than one. */
//         static_assert(Detail::calculate_hamming_weight(Nv) == 1u,
//             "Block size must be a power of two");

//         if constexpr (Nv > 8u) {
//             __m128i acc = _mm_cvtepi8_epi16(_mm_loadu_si128((__m128i *)&alpha[0u]));
//             for (std::size_t i = 8u; i < Nv; i += 8u) {
//                 acc = _mm_hadd_epi16(acc, _mm_cvtepi8_epi16(_mm_loadu_si128((__m128i *)&alpha[i])));
//             }

//             acc = _mm_hadd_epi16(acc, acc);
//             acc = _mm_hadd_epi16(acc, acc);
//             acc = _mm_hadd_epi16(acc, acc);
//             int16_t sum = _mm_extract_epi16(acc, 0u);

//             for (std::size_t i = 0u; i < Nv; i += 16u) {
//                 _mm_storeu_si128((__m128i *)&beta[i], _mm_set1_epi8(std::signbit(sum)));
//             }
//         } else if constexpr (Nv > 2u) {
//             int8_t alpha_vec[16] __attribute__ ((aligned (16))) = {};
//             std::copy_n(alpha.begin(), Nv, alpha_vec);

//             __m128i acc = _mm_cvtepi8_epi16(*(__m128i *)alpha_vec);
//             acc = _mm_hadd_epi16(acc, acc);
//             acc = _mm_hadd_epi16(acc, acc);
//             acc = _mm_hadd_epi16(acc, acc);
//             int16_t sum = _mm_extract_epi16(acc, 0u);

//             if (std::signbit(sum)) {
//                 std::fill_n(beta, Nv, 1u);
//             }
//         }
//     }
// };
