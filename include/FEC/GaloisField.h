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
#include <utility>

#include "FEC/Types.h"
#include "FEC/BinarySequence.h"

namespace Thiemar {

namespace Detail {
    template <typename T, std::size_t... I>
    constexpr T integer_from_index_sequence(std::index_sequence<I...>) {
        T mask = 0u;
        for (std::size_t i : { I... }) {
            mask |= (i < sizeof(T) * 8u) ? (T)1u << i : 0u;
        }

        return mask;
    }

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

    /* Need an array class with constexpr access operators. */
    template <typename T, int N>
    struct ConstantArray {
        T data[N];
        constexpr T& operator[](int i) { return data[i]; }
        constexpr const T& operator[](int i) const { return data[i]; }
    };
}

/* Class for doing arithmetic operations in the specified Galois field. */
template <typename T, std::size_t M, typename Generator>
class GaloisFieldImpl {
    static_assert(M >= 1u && M <= 16u, "Galois field order must be between 2^1 and 2^16");

    static constexpr std::size_t gen_poly = Detail::integer_from_index_sequence<std::size_t>(
        (typename Generator::ones_index_sequence_reversed){});

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
        Detail::ConstantArray<T, sizeof...(I)> temp = {0u};

        for (std::size_t i = 0u; i < sizeof...(I) - 1u; i++) {
            temp[temp_antilog[i]] = i;
        }

        return std::array<T, sizeof...(I)>{ temp[I]... };
    }

    static constexpr std::array<T, 1u << M> log_table = get_field_elements_inverse(std::make_index_sequence<1u << M>{});

public:
    using gf_t = T;

    static T add(T x, T y) { return x^y; }

    static T subtract(T x, T y) { return x^y; }

    static T log(T x) { return log_table[x]; }

    static T antilog(T x) { return antilog_table[x]; }

    static T multiply(T x, T y) {
        return (x && y) ? antilog((log(x) + log(y)) % ((1u << M) - 1u)) : 0u;
    }

    static T divide(T x, T y) {
        return (x && y) ? antilog((((1u << M) - 1u) + log(x) - log(y)) % ((1u << M) - 1u)) : 0u;
    }
};

template <typename T, std::size_t M, typename Generator>
constexpr std::array<T, 1u << M> GaloisFieldImpl<T, M, Generator>::antilog_table;

template <typename T, std::size_t M, typename Generator>
constexpr std::array<T, 1u << M> GaloisFieldImpl<T, M, Generator>::log_table;

/*
Selects the appropriate type for the Galois field primitive based on the
order of the field.
*/
template <std::size_t M, typename Generator> class GaloisField;
template <typename Generator> class GaloisField<1u, Generator> : public GaloisFieldImpl<uint8_t, 1u, Generator> {};
template <typename Generator> class GaloisField<2u, Generator> : public GaloisFieldImpl<uint8_t, 2u, Generator> {};
template <typename Generator> class GaloisField<3u, Generator> : public GaloisFieldImpl<uint8_t, 3u, Generator> {};
template <typename Generator> class GaloisField<4u, Generator> : public GaloisFieldImpl<uint8_t, 4u, Generator> {};
template <typename Generator> class GaloisField<5u, Generator> : public GaloisFieldImpl<uint8_t, 5u, Generator> {};
template <typename Generator> class GaloisField<6u, Generator> : public GaloisFieldImpl<uint8_t, 6u, Generator> {};
template <typename Generator> class GaloisField<7u, Generator> : public GaloisFieldImpl<uint8_t, 7u, Generator> {};
template <typename Generator> class GaloisField<8u, Generator> : public GaloisFieldImpl<uint8_t, 8u, Generator> {};

}
