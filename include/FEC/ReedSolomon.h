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
#include "FEC/Utilities.h"
#include "FEC/GaloisField.h"

namespace Thiemar {

namespace Detail {
    /*
    Code to carry out compile-time multiplication of Galois field polynomials
    represented using integer sequences.
    */
    template <typename GF, std::size_t Len1, std::size_t Len2>
    static constexpr typename GF::gf_t poly_multiply_impl(std::size_t idx,
            Thiemar::Detail::ConstantArray<typename GF::gf_t, Len1> poly1,
            Thiemar::Detail::ConstantArray<typename GF::gf_t, Len2> poly2) {
        typename GF::gf_t temp = 0u;
        for (std::size_t i = 0u; i <= idx; i++) {
            if ((idx - i) < Len1 && i < Len2) {
                temp ^= GF::multiply(poly1[idx - i], poly2[i]);
            }
        }

        return temp;
    }

    template <typename GF, typename Poly1, typename Poly2, typename Indices> struct poly_multiply;

    template <typename GF, typename GF::gf_t... P1, typename GF::gf_t... P2, std::size_t... Is>
    struct poly_multiply<GF, std::integer_sequence<typename GF::gf_t, P1...>, std::integer_sequence<typename GF::gf_t, P2...>,
            std::index_sequence<Is...>> {
        using integer_sequence = std::integer_sequence<typename GF::gf_t, poly_multiply_impl<GF>(Is,
            Thiemar::Detail::ConstantArray<typename GF::gf_t, sizeof...(P1)>{ P1... },
            Thiemar::Detail::ConstantArray<typename GF::gf_t, sizeof...(P2)>{ P2... })...>;
    };

    /*
    Calculate the Reed-Solomon generator polynomial as a function of the
    number of parity words.
    */
    template <typename GF, std::size_t Parity, std::size_t N = Parity>
    struct generator_polynomial {
        using coefficients = typename poly_multiply<GF,
            typename generator_polynomial<GF, Parity, N - 1u>::coefficients,
            std::integer_sequence<typename GF::gf_t, 1u, GF::antilog(N)>,
            std::make_index_sequence<N + 1u>>::integer_sequence;
    };

    template <typename GF, std::size_t Parity>
    struct generator_polynomial<GF, Parity, 0u> {
        using coefficients = std::integer_sequence<typename GF::gf_t, 1u>;
    };
}

namespace ReedSolomon {

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
    using generator = typename Detail::generator_polynomial<gf, Parity>::coefficients;

    /*
    Calculate parity for up to (2^M - Parity - 1) message bytes from 'input',
    placing the parity bytes at the end of the message bytes.
    */
    template <std::size_t N>
    static void encode(gf_t *buf) {
        static_assert(N <= (1u << M) - 1u - Parity,
            "Data length must be smaller than or equal to block size minus parity length");
        std::array<gf_t, N+Parity> message = Detail::to_array<gf_t, N+Parity>(buf);

        std::array<gf_t, Parity> parity = gf::remainder(message, generator{},
            std::make_index_sequence<Parity>{});
        memcpy(&buf[N], parity.data(), Parity);
    }
};

}

}
