#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Polar.h"

TEST(PolarEncoderTest, EncodeBlockSize128) {
    constexpr std::size_t N = 128u;
    constexpr std::size_t K = 64u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, K, TestDataIndices>;

    uint8_t test_in[K / 8u] = {};
    uint8_t test_out[N / 8u] = {};

    TestEncoder::encode(test_in, sizeof(test_in), test_out);

    for (std::size_t i = 0u; i < sizeof(test_out); i++) {
        // EXPECT_EQ((int)test_in[i], (int)test_decoded_fecmagic[i]) << "Buffers differ at index " << i;
    }
}
