#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Polar.h"

TEST(PolarEncoderTest, EncodeBlockSize1024) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 1024u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;

    std::array<uint8_t, K / 8u> test_in = {
        228,  245,  140,   35,   38,   65,  215,   65,
        208,   62,  237,   89,   50,   64,  157,  121,
         90,  212,  149,  140,  234,   73,  193,  192,
         97,  145,   19,   13,  135,  199,  239,   33,
        145,  120,    3,   86,   41,  203,   79,  135,
         42,  154,   67,  167,  176,  191,  115,   21,
         58,  233,   39,  211,  137,  255,   20,  113,
         27,  246,    1,  198,  209,  222,   21,  102
    };

    /* Calculated using support/MATLAB/polar/encode_test.m */
    std::array<uint8_t, M / 8u> ref_out = {
        109,  110,   59,  106,   75,   98,  162,   19,
        165,   22,  136,  153,  206,  212,  235,  123,
        170,  247,   29,  250,    7,  228,   55,  247,
         20,  157,  172,  132,  122,  185,   62,  216,
        204,   80,  211,    3,   69,   98,  171,  177,
        218,  159,  159,   15,   24,  169,   65,   73,
         86,  203,  138,   76,  208,  157,   58,   14,
         80,  143,   93,  171,   19,   36,    9,  215,
          9,   52,  120,  235,  154,  120,   98,  114,
        102,   23,   58,  234,  132,   86,   25,  212,
        223,   42,   79,    3,   56,  134,   25,   17,
        198,   27,   15,  143,  222,   67,   34,  240,
        100,  160,   58,  177,  167,   45,   62,   28,
        213,   52,  135,   79,   97,  126,  230,   42,
        186,  233,   39,  211,  137,  255,   20,  113,
         27,  246,    1,  198,  209,  222,   21,  102
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
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;

    std::array<uint8_t, K / 8u> test_in = {
        228,  245,  140,   35,   38,   65,  215,   65,
        208,   62,  237,   89,   50,   64,  157,  121,
         90,  212,  149,  140,  234,   73,  193,  192,
         97,  145,   19,   13,  135,  199,  239,   33,
        145,  120,    3,   86,   41,  203,   79,  135,
         42,  154,   67,  167,  176,  191,  115,   21,
         58,  233,   39,  211,  137,  255,   20,  113,
         27,  246,    1,  198,  209,  222,   21,  102
    };

    /* Calculated using support/MATLAB/polar/encode_test.m */
    std::array<uint8_t, M / 8u> ref_out = {
         63,   56,   68,  251,  225,   59,  149,   66,
         28,   78,  153,  126,  123,    6,  107,    8,
        249,  181,  101,    2,  250,  148,  144,   58,
        103,   29,   40,  116,  135,  221,  171,   38,
         14,   38,   73,  130,   62,   72,  217,  215,
         31,   42,   51,   90,  201,   88,  206,  164,
         17,   96,  112,   24,  100,   68,  195,   97,
        241,  251,  200,  100,   94,    0,  213,  138,
        198,  222,  204,  218,   82,   95,    7,   42,
        177,  210,   29,   61,  133,  251,  152,  169,
        122,  233,   39,  211,  137,  255,   20,  113,
         27,  246,    1,  198,  209,  222,   21,  102
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
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;

    std::array<uint8_t, K / 8u> test_in = {
        228,  245,  140,   35,   38,   65,  215,   65,
        208,   62,  237,   89,   50,   64,  157,  121,
         90,  212,  149,  140,  234,   73,  193,  192,
         97,  145,   19,   13,  135,  199,  239,   33,
        145,  120,    3,   86,   41,  203,   79,  135,
         42,  154,   67,  167,  176,  191,  115,   21,
         58,  233,   39,  211,  137,  255,   20,  113,
         27,  246,    1,  198,  209,  222,   21,  102
    };

    /* Calculated using support/MATLAB/polar/encode_test.m */
    std::array<uint8_t, M / 8u> ref_out = {
         54,  167,  214,   48,  140,  153,    7,   93,
          7,   64,  251,  181,  100,  201,    2,  117,
        114,  181,  169,   43,   25,  212,  147,  131,
        128,  195,   34,   38,   27,   15,  143,  222,
         33,  145,  120,    3,   86,   41,  203,   79,
        135,   42,  154,   67,  167,  176,  191,  115,
         21,   58,  233,   39,  211,  137,  255,   20,
        113,   27,  246,    1,  198,  209,  222,   21,
        102
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
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;

    std::array<uint8_t, K / 8u> test_in = {
        228,  245,  140,   35,   38,   65,  215,   65,
        208,   62,  237,   89,   50,   64,  157,  121,
         90,  212,  149,  140,  234,   73,  193,  192,
         97,  145,   19,   13,  135,  199,  239,   33,
        145,  120,    3,   86,   41,  203,   79,  135,
         42,  154,   67,  167,  176,  191,  115,   21,
         58,  233,   39,  211,  137,  255,   20,  113,
         27,  246,    1,  198,  209,  222,   21,  102
    };

    /* Calculated using support/MATLAB/polar/encode_test.m */
    std::array<uint8_t, M / 8u> ref_out = {
        228,  245,  140,   35,   38,   65,  215,   65,
        208,   62,  237,   89,   50,   64,  157,  121,
         90,  212,  149,  140,  234,   73,  193,  192,
         97,  145,   19,   13,  135,  199,  239,   33,
        145,  120,    3,   86,   41,  203,   79,  135,
         42,  154,   67,  167,  176,  191,  115,   21,
         58,  233,   39,  211,  137,  255,   20,  113,
         27,  246,    1,  198,  209,  222,   21,  102
    };

    std::array<uint8_t, M / 8u> test_out = TestEncoder::encode(test_in);

    for (std::size_t i = 0u; i < test_out.size(); i++) {
        EXPECT_EQ((int)ref_out[i], (int)test_out[i]) << "Buffers differ at index " << i;
    }
}

