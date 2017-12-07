#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Polar.h"

TEST(PolarEncoderTest, EncodeBlockSize128) {
    constexpr std::size_t N = 128u;
    constexpr std::size_t K = 64u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, K, TestDataIndices>;

    std::array<uint8_t, K / 8u> test_in = { 61u, 219u, 159u, 102u, 183u, 50u, 205u, 93u };

    /* Calculated using support/polar/encode_test.m */
    std::array<uint8_t, N / 8u> ref_out = { 27u, 11u, 141u, 171u, 14u, 255u, 104u, 80u, 86u, 133u, 149u, 112u, 67u, 113u, 112u, 139u };

    std::array<uint8_t, N / 8u> test_out = TestEncoder::encode(test_in.data());

    for (std::size_t i = 0u; i < test_out.size(); i++) {
        EXPECT_EQ((int)ref_out[i], (int)test_out[i]) << "Buffers differ at index " << i;
    }
}
