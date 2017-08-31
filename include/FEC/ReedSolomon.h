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
#include "FEC/GaloisField.h"

namespace Thiemar {

namespace ReedSolomon {

namespace Detail {
    /*
    Calculate the Reed-Solomon generator polynomial as a function of the
    number of parity words.
    */
    template <typename GF, std::size_t Parity, std::size_t N = Parity>
    struct generator_polynomial {
        static constexpr Thiemar::Detail::ConstantArray<typename GF::gf_t, N + 1u> poly = GF::multiply(
            generator_polynomial<GF, Parity, N - 1u>::poly,
            Thiemar::Detail::ConstantArray<typename GF::gf_t, 2u>{ 1u, GF::antilog(N) });
    };

    template <typename GF, std::size_t Parity>
    struct generator_polynomial<GF, Parity, 0u> {
        static constexpr Thiemar::Detail::ConstantArray<typename GF::gf_t, 1u> poly = { 1u };
    };

    template <typename GF, std::size_t N>
    constexpr Thiemar::Detail::ConstantArray<typename GF::gf_t, N> poly_to_index_form(
            Thiemar::Detail::ConstantArray<typename GF::gf_t, N> in) {
        Thiemar::Detail::ConstantArray<typename GF::gf_t, N> out = {};
        
        for (std::size_t i = 0; i < N; i++) {
            out[i] = GF::log(in[i]);
        }

        return out;
    }

    template <typename T, std::size_t N, std::size_t... I>
    constexpr std::array<T, N> to_array_impl(const T (&a)[N], std::index_sequence<I...>) {
        return { {a[I]...} };
    }
     
    template <typename T, std::size_t N>
    constexpr std::array<T, N> to_array(const T (&a)[N]) {
        return to_array_impl(a, std::make_index_sequence<N>{});
    }

    template <typename T, std::size_t N, std::size_t... I>
    constexpr std::array<T, N> to_array_impl(T *a, std::index_sequence<I...>) {
        return { {a[I]...} };
    }
     
    template <typename T, std::size_t N>
    constexpr std::array<T, N> to_array(T *a) {
        return to_array_impl<T, N>(a, std::make_index_sequence<N>{});
    }
}

namespace Polynomials {
    using m_8_285 = BinarySequence<1, 0, 0, 0, 1, 1, 1, 0, 1>;
    using m_8_301 = BinarySequence<1, 0, 0, 1, 0, 1, 1, 0, 1>;
}

template <std::size_t M, typename Primitive, std::size_t Parity>
class ReedSolomonEncoder {
    static_assert(Parity < (1u << M) - 1u, "Parity must be smaller than the field size");

    using gf = GaloisField<M, Primitive>;
    using gf_t = typename gf::gf_t;

public:
    static constexpr Thiemar::Detail::ConstantArray<gf_t, Parity + 1u> generator = Detail::poly_to_index_form<gf>(
        Detail::generator_polynomial<gf, Parity>::poly);

    /*
    Calculate parity for up to (2^M - Parity - 1) message bytes from 'input',
    placing the parity bytes at the end of the message bytes.
    */
    template <std::size_t N>
    static void encode(gf_t *buf) {
        static_assert(N <= (1u << M) - 1u - Parity,
            "Data length must be smaller than or equal to block size minus parity length");
        std::array<gf_t, N+Parity> message = Detail::to_array<gf_t, N+Parity>(buf);

        std::array<gf_t, Parity> parity = gf::remainder_logdivisor(message, generator,
            std::make_index_sequence<Parity>{});
        memcpy(&buf[N], parity.data(), Parity);
    }
};

template <std::size_t M, typename Primitive, std::size_t Parity>
constexpr Thiemar::Detail::ConstantArray<typename GaloisField<M, Primitive>::gf_t, Parity + 1u> ReedSolomonEncoder<
    M, Primitive, Parity>::generator;

}

}
