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
#include <cstddef>
#include <utility>

#include "FEC/Types.h"
#include "FEC/Utilities.h"
#include "FEC/BinarySequence.h"

namespace Thiemar {

namespace Detail {
    template <typename T, std::size_t M, std::size_t G>
    constexpr T get_field_element(std::size_t idx) {
        T e = 1u;
        for (std::size_t i = 0u; i < idx; i++) {
            if (e & (1u << (M - 1u))) {
                e = (e << 1u) ^ G;
            } else {
                e = e << 1u;
            }
        }

        return e;
    }
}

/* Class for doing arithmetic operations in the specified Galois field. */
template <typename T, std::size_t M, typename Primitive>
class GaloisFieldImpl {
    static_assert(M >= 1u && M <= 16u, "Galois field order must be between 2^1 and 2^16");

    static constexpr std::size_t gen_poly = Detail::integer_from_index_sequence<std::size_t>(
        (typename Primitive::ones_index_sequence_reversed){});

    /*
    Get an array containing the elements of the Galois field in
    exponent-order.
    */
    template <std::size_t... I>
    static constexpr std::array<T, sizeof...(I)> get_field_elements(std::index_sequence<I...>) {
        return std::array<T, sizeof...(I)>{ Detail::get_field_element<T, M, gen_poly>(I) ... };
    }

    static constexpr std::array<T, 1u << M> antilog_table = get_field_elements(std::make_index_sequence<1u << M>{});

    /*
    Get an array containing the exponents corresponding to each element of
    the Galois field.
    */
    template <std::size_t... I>
    static constexpr std::array<T, sizeof...(I)> get_field_elements_inverse(std::index_sequence<I...>) {
        constexpr std::array<T, sizeof...(I)> temp_antilog = get_field_elements(std::make_index_sequence<1u << M>{});
        std::array<T, sizeof...(I)> temp = {};

        for (std::size_t i = 0u; i < sizeof...(I) - 1u; i++) {
            temp[temp_antilog[i]] = i;
        }

        return temp;
    }

    static constexpr std::array<T, 1u << M> log_table = get_field_elements_inverse(std::make_index_sequence<1u << M>{});

public:
    using gf_t = T;

    static constexpr T add(T x, T y) { return x^y; }

    static constexpr T subtract(T x, T y) { return x^y; }

    static constexpr T log(T x) { return log_table[x]; }

    static constexpr T antilog(T x) { return antilog_table[x]; }

    static constexpr T multiply(T x, T y) {
        return (x && y) ? antilog((log(x) + log(y)) % ((1u << M) - 1u)) : 0u;
    }

    static constexpr T divide(T x, T y) {
        return (x && y) ? antilog((((1u << M) - 1u) + log(x) - log(y)) % ((1u << M) - 1u)) : 0u;
    }

    template <std::size_t Len1, std::size_t Len2>
    static std::array<gf_t, std::max(Len1, Len2)> add(
            const std::array<gf_t, Len1>& x, const std::array<gf_t, Len2>& y) {
        std::array<gf_t, std::max(Len1, Len2)> r = {};

        for (std::size_t i = 0u; i < Len1; i++) {
            r[i + r.size() - Len1] = x[i];
        }

        for (std::size_t i = 0u; i < Len2; i++) {
            r[i + r.size() - Len2] ^= y[i];
        }

        return r;
    }

    template <std::size_t Len1, std::size_t Len2>
    static std::array<gf_t, Len1 + Len2 - 1u> multiply(
            const std::array<gf_t, Len1>& x, const std::array<gf_t, Len2>& y) {
        std::array<gf_t, Len1 + Len2 - 1u> r = {};

        for (std::size_t i = 0u; i < Len1; i++) {
            for (std::size_t j = 0u; j < Len2; j++) {
                r[i + j] ^= multiply(x[i], y[j]);
            }
        }

        return r;
    }

    template <std::size_t Len1, std::size_t Len2>
    static std::array<gf_t, Len2 - 1u> remainder(
            const std::array<gf_t, Len1>& x, const std::array<gf_t, Len2>& y) {
        std::array<gf_t, Len1> d = x;

        for (std::size_t i = 0u; i < Len1 - Len2 + 1u; i++) {
            if (d[i] != 0u) {
                for (std::size_t j = 1u; j < Len2; j++) {
                    if (y[j] != 0u) {
                        /*
                        Already have zero-checks here, so avoid repeating
                        them in the GF multiplication function.
                        */
                        d[i + j] ^= antilog((log(y[j]) + log(d[i])) % ((1u << M) - 1u));
                    }
                }
            }
        }

        std::array<gf_t, Len2 - 1u> r;

        for (std::size_t i = 0u; i < Len2 - 1u; i++) {
            r[i] = d[Len1 - Len2 + 1u + i];
        }

        return r;
    }

    /*
    This function is used to speed up encoding, the generator polynomial can
    be given as a compile-time integer sequence.
    */
    template <std::size_t Len1, gf_t G, gf_t... Gs, std::size_t... Is>
    static std::array<gf_t, sizeof...(Gs)> remainder(const std::array<gf_t, Len1>& x,
            std::integer_sequence<gf_t, G, Gs...>, std::index_sequence<Is...>) {
        std::array<gf_t, Len1> d = x;

        for (std::size_t i = 0u; i < Len1 - sizeof...(Gs); i++) {
            if (d[i] != 0u) {
                ((d[i + Is + 1u] ^= antilog((log(Gs) + log(d[i])) % ((1u << M) - 1u))), ...);
            }
        }

        return std::array<gf_t, sizeof...(Gs)>{ d[Len1 - sizeof...(Gs) + Is]... };
    }
};

template <typename T, std::size_t M, typename Primitive>
constexpr std::array<T, 1u << M> GaloisFieldImpl<T, M, Primitive>::antilog_table;

template <typename T, std::size_t M, typename Primitive>
constexpr std::array<T, 1u << M> GaloisFieldImpl<T, M, Primitive>::log_table;

/*
Selects the appropriate type for the Galois field primitive based on the
order of the field.
*/
template <std::size_t M, typename Primitive> class GaloisField;
template <typename Primitive> class GaloisField<1u, Primitive> : public GaloisFieldImpl<uint8_t, 1u, Primitive> {};
template <typename Primitive> class GaloisField<2u, Primitive> : public GaloisFieldImpl<uint8_t, 2u, Primitive> {};
template <typename Primitive> class GaloisField<3u, Primitive> : public GaloisFieldImpl<uint8_t, 3u, Primitive> {};
template <typename Primitive> class GaloisField<4u, Primitive> : public GaloisFieldImpl<uint8_t, 4u, Primitive> {};
template <typename Primitive> class GaloisField<5u, Primitive> : public GaloisFieldImpl<uint8_t, 5u, Primitive> {};
template <typename Primitive> class GaloisField<6u, Primitive> : public GaloisFieldImpl<uint8_t, 6u, Primitive> {};
template <typename Primitive> class GaloisField<7u, Primitive> : public GaloisFieldImpl<uint8_t, 7u, Primitive> {};
template <typename Primitive> class GaloisField<8u, Primitive> : public GaloisFieldImpl<uint8_t, 8u, Primitive> {};
template <typename Primitive> class GaloisField<9u, Primitive> : public GaloisFieldImpl<uint16_t, 9u, Primitive> {};
template <typename Primitive> class GaloisField<10u, Primitive> : public GaloisFieldImpl<uint16_t, 10u, Primitive> {};
template <typename Primitive> class GaloisField<11u, Primitive> : public GaloisFieldImpl<uint16_t, 11u, Primitive> {};
template <typename Primitive> class GaloisField<12u, Primitive> : public GaloisFieldImpl<uint16_t, 12u, Primitive> {};
template <typename Primitive> class GaloisField<13u, Primitive> : public GaloisFieldImpl<uint16_t, 13u, Primitive> {};
template <typename Primitive> class GaloisField<14u, Primitive> : public GaloisFieldImpl<uint16_t, 14u, Primitive> {};
template <typename Primitive> class GaloisField<15u, Primitive> : public GaloisFieldImpl<uint16_t, 15u, Primitive> {};
template <typename Primitive> class GaloisField<16u, Primitive> : public GaloisFieldImpl<uint16_t, 16u, Primitive> {};

}
