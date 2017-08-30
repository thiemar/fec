#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/ReedSolomon.h"

TEST(ReedSolomonEncoderTest, Generator) {
    using TestEncoder = Thiemar::ReedSolomon::ReedSolomonEncoder<8u, Thiemar::ReedSolomon::Polynomials::m_8_285, 8u>;

    uint8_t generator[] = { 1u, 227u, 44u, 178u, 71u, 172u, 8u, 224u, 37u };

    for (std::size_t i = 0u; i < sizeof(generator); i++) {
        EXPECT_EQ((int)generator[i], (int)TestEncoder::generator[i]) << "Buffers differ at index " << i;
    }
}
