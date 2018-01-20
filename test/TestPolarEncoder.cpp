#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Polar.h"

TEST(PolarEncoderTest, EncodeBlockSize1024) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 1024u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;

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

    /* Calculated using support/MATLAB/polar/encode_test.m */
    std::array<uint8_t, M / 8u> ref_out = {
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

    std::array<uint8_t, M / 8u> test_out = TestEncoder::encode(test_in);

    for (std::size_t i = 0u; i < test_out.size(); i++) {
        EXPECT_EQ((int)ref_out[i], (int)test_out[i]) << "Buffers differ at index " << i;
    }
}

TEST(PolarEncoderTest, EncodeBlockSize768) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 768u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;

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

    /* Calculated using support/MATLAB/polar/encode_test.m */
    std::array<uint8_t, M / 8u> ref_out = {
        145, 100, 150, 219,  29, 187, 220, 210, 
         64, 230, 112, 232,  90,  23, 239,  81,
        163,  21,   4,   1, 179, 132, 132, 224,
         38, 153,  62, 180, 130,  55, 223,  98,
        200,  39, 161, 148, 190,  76, 207, 248,
         22,  47, 176, 240, 178, 246,  99,  78,
         20,  72,  39, 233, 213,  49, 171,  61,
        185, 195,  69,  14, 255, 162,  33, 162,
        252, 216, 153,  70,  90, 207,  34, 188,
        178, 227,  19, 156,   5, 194, 106,  81,
        106,  20, 147, 170, 113,  54, 150, 105,
         86,   8,  28,  32, 212,  88, 197,   6
    };

    std::array<uint8_t, M / 8u> test_out = TestEncoder::encode(test_in);

    for (std::size_t i = 0u; i < test_out.size(); i++) {
        EXPECT_EQ((int)ref_out[i], (int)test_out[i]) << "Buffers differ at index " << i;
    }
}

TEST(PolarEncoderTest, EncodeBlockSize520) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 520u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;

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

    /* Calculated using support/MATLAB/polar/encode_test.m */
    std::array<uint8_t, M / 8u> ref_out = {
        167, 249, 175,  70, 192, 170, 204,  72,
         65, 119, 243, 180, 248,  66,  95, 251,
         93, 117,  94,   9,  35, 161, 159, 116,
        149, 194,  23, 164,   7, 195,  82, 157,
        110,   0,  69, 237, 123,  84, 232, 235,
         14,  92, 215, 108, 153, 153, 153,   6,
         15, 222, 135,  57, 219,  71, 160, 255,
         63,  94,  20,  60, 244, 140, 157, 195,
          6
    };

    std::array<uint8_t, M / 8u> test_out = TestEncoder::encode(test_in);

    for (std::size_t i = 0u; i < test_out.size(); i++) {
        EXPECT_EQ((int)ref_out[i], (int)test_out[i]) << "Buffers differ at index " << i;
    }
}

TEST(PolarEncoderTest, EncodeBlockSize512) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 512u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;

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

    /* Calculated using support/MATLAB/polar/encode_test.m */
    std::array<uint8_t, M / 8u> ref_out = {
        225, 194, 173, 149, 249, 230,  51, 116,
        217, 141, 182,  30, 187, 251,  65, 125,
        201, 233,  91, 202, 149,  71, 199, 118,
         54, 186, 172, 196, 186, 222, 107,  89,
         55,  55, 114, 159, 228, 176,  88, 179,
        189, 225,  54,  90, 195,  90, 195, 197,
        202,  20, 147, 170, 113,  54, 150, 105,
         86,   8,  28,  32, 212,  88, 197,   6
    };

    std::array<uint8_t, M / 8u> test_out = TestEncoder::encode(test_in);

    for (std::size_t i = 0u; i < test_out.size(); i++) {
        EXPECT_EQ((int)ref_out[i], (int)test_out[i]) << "Buffers differ at index " << i;
    }
}

