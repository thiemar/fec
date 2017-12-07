#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Polar.h"

TEST(PolarEncoderTest, EncodeBlockSize1024) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, K, TestDataIndices>;

    std::array<uint8_t, K / 8u> test_in = {
        228, 245, 140,  35,  38,  65, 215,  65,
        208,  62, 237,  89,  50,  64, 157, 121,
         90, 212, 149, 140, 234,  73, 193, 192,
         97, 145,  19,  13, 135, 199, 239,  33,
        145, 120,   3,  86,  41, 203,  79, 135,
         42, 154,  67, 167, 176, 191, 115,  21,
         58, 233,  39, 211, 137, 255,  20, 113,
         27, 246,   1, 198, 209, 222,  21, 102
    };

    /* Calculated using support/polar/encode_test.m */
    std::array<uint8_t, N / 8u> ref_out = {
        203, 210, 132, 113, 203, 179,  90, 251,
         68, 235,  77, 189, 213, 228,  77, 155,
        109, 204, 228, 192,  32, 114,  23, 246,
        109,  47, 128, 205, 179,  47, 175, 135,
        254,  82, 152,  35, 180,  50, 237, 208,
        172, 167,  79, 239,  17, 169, 215,  26,
         46,  43, 235,  92,  49, 214,  84, 174,
         86, 161,  52, 244,  67,  29, 155,  47,
         19, 184,  48,  34,  72, 249, 171, 102,
        120,  23, 169, 204, 129,  11,  32, 232,
         87, 187,  77, 113, 190, 218,   4, 118,
         76,  49, 134, 161,   5, 221, 223,  22,
        173,  37, 168,  98, 188, 101, 152,  95,
        228, 185, 208, 115,  49, 164, 193, 132,
        202,  20, 147, 170, 113,  54, 150, 105,
         86,   8,  28,  32, 212,  88, 197,   6
    };

    std::array<uint8_t, N / 8u> test_out = TestEncoder::encode(test_in.data());

    for (std::size_t i = 0u; i < test_out.size(); i++) {
        EXPECT_EQ((int)ref_out[i], (int)test_out[i]) << "Buffers differ at index " << i;
    }
}

TEST(PolarEncoderTest, EncodeBlockSize1024_Shortened) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, K, TestDataIndices>;

    std::array<uint8_t, K / 8u> test_in = {
        228, 245, 140,  35,  38,  65, 215,  65,
        208,  62, 237,  89,  50,  64, 157, 121,
         90, 212, 149, 140, 234,  73, 193, 192,
         97, 145,  19,  13, 135, 199, 239,  33,
        145, 120,   3,  86,  41, 203,  79, 135,
         42, 154,  67, 167, 176, 191, 115,  21,
         58, 233,  39, 211, 137, 255,  20, 113,
         27, 246,   1, 198, 209, 222,  21, 102
    };

    /* Calculated using support/polar/encode_test.m */
    std::array<uint8_t, N / 8u> ref_out = {
        203, 210, 132, 113, 203, 179,  90, 251,
         68, 235,  77, 189, 213, 228,  77, 155,
        109, 204, 228, 192,  32, 114,  23, 246,
        109,  47, 128, 205, 179,  47, 175, 135,
        254,  82, 152,  35, 180,  50, 237, 208,
        172, 167,  79, 239,  17, 169, 215,  26,
         46,  43, 235,  92,  49, 214,  84, 174,
         86, 161,  52, 244,  67,  29, 155,  47,
         19, 184,  48,  34,  72, 249, 171, 102,
        120,  23, 169, 204, 129,  11,  32, 232,
         87, 187,  77, 113, 190, 218,   4, 118,
         76,  49, 134, 161,   5, 221, 223,  22,
        173,  37, 168,  98, 188, 101, 152,  95,
        228, 185, 208, 115,  49, 164, 193, 132,
        202,  20, 147, 170, 113,  54, 150, 105,
         86,   8,  28,  32, 212,  88, 197,   6
    };

    std::array<uint8_t, 107u> test_out = TestEncoder::encode<107u>(test_in.data());

    for (std::size_t i = 0u; i < test_out.size(); i++) {
        EXPECT_EQ((int)ref_out[i], (int)test_out[i]) << "Buffers differ at index " << i;
    }
}

