#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Polar.h"

TEST(PolarDecoderTest, CorruptedPacket) {
    constexpr std::size_t N = 2048u;
    constexpr std::size_t M = 2048u;
    constexpr std::size_t K = 1024u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>::data_index_sequence;
    using TestDecoder = Thiemar::Polar::SuccessiveCancellationListDecoder<N, M, K, TestDataIndices>;

    /* Set up test buffers. */
    std::array<uint8_t, M / 8u> test_code = {
           48,   77,   88,  229,  144,   16,  207,  156,
          241,  237,  136,   38,  204,   99,  180,    7,
          251,  231,  234,  207,  204,   54,  168,   52,
           29,  145,   84,  174,   16,  243,  253,  216,
          239,  125,  114,  126,   97,  123,  182,   92,
           14,    0,  191,  219,  139,  183,  125,  232,
           39,  131,   26,  197,  144,  135,  253,   98,
          247,  231,  168,   67,  170,  203,  248,   53,
          122,   12,  226,  146,   21,  241,  102,  117,
          191,  133,   62,  121,  221,  252,  123,  226,
           37,   45,  144,   16,   91,   23,   49,   30,
           41,  129,  198,   47,  101,  195,   53,   61,
          113,   37,  155,  138,  111,   98,  120,  129,
           21,   80,  122,  168,  143,   92,   13,   81,
          139,   80,  176,   95,  182,   65,  227,   66,
          136,   93,  185,   66,  104,   60,  228,  143,
            2,  178,  115,  229,   20,   62,  160,  118,
          123,   37,  240,   74,  183,  195,   26,  227,
          100,  102,  216,  177,   11,   60,   45,  224,
           51,  234,  233,   49,  162,  182,  231,   46,
           90,  149,  136,  109,    9,  109,  108,  168,
          163,  128,   33,   82,   63,   73,  232,  121,
           74,  191,  190,  243,   49,  218,   51,  200,
          186,   17,  151,  203,  244,  185,  158,  246,
           77,  149,   26,  222,  194,  100,   54,  225,
           46,  111,   15,  142,  255,  151,   67,   56,
          225,  151,   70,  211,  160,   87,   13,   85,
          247,   44,   63,  127,  147,  152,  148,  160,
          250,  231,  255,  177,  189,   47,  229,  113,
           46,  201,  212,   97,   14,   52,   77,  214,
          209,   82,   25,  238,  204,   63,  117,   27,
          120,  129,  171,   78,    8,  160,   53,  205
    };

    std::array<uint8_t, K / 8u> test_data = {
          126,   39,  149,  254,   48,  138,   55,  210,
          161,  181,  214,   72,   61,  234,  184,   41,
           79,   77,  247,  139,  157,  161,   58,  239,
          174,  224,  225,   56,  176,  222,  221,  124,
          233,   10,  215,  155,  240,  190,   58,  139,
           19,  203,  146,   94,   24,  128,  242,  246,
           37,   74,    0,   61,  136,  123,   84,   64,
           93,    2,    6,  170,  160,  227,  188,   28,
          139,    7,   50,    4,  164,  245,  140,  188,
          195,  176,  128,  225,    5,  199,  145,  115,
          146,  174,  255,  188,   94,  200,   82,   40,
           86,  110,   91,  141,  195,  241,  152,   38,
          223,  206,  155,   89,  143,   19,  143,  111,
           15,   11,  228,  175,  215,   94,   66,  210,
          250,  228,  192,   80,  158,  135,   22,  214,
           45,  118,   45,   25,  255,  235,  152,  251
    };

    auto test_decoded = TestDecoder::decode(test_code);

    for (std::size_t i = 0u; i < test_data.size(); i++) {
        EXPECT_EQ((int)test_data[i], (int)test_decoded[i]) << "Buffers differ at index " << i;
    }
}

TEST(PolarDecoderTest, DecodeBlockSize1024) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 1024u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;
    using TestDecoder = Thiemar::Polar::SuccessiveCancellationListDecoder<N, M, K, TestDataIndices>;

    /* Set up test buffers. */
    std::array<uint8_t, K / 8u> test_in = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    auto test_out = TestEncoder::encode(test_in);
    auto test_decoded = TestDecoder::decode(test_out);

    for (std::size_t i = 0u; i < test_in.size(); i++) {
        EXPECT_EQ((int)test_in[i], (int)test_decoded[i]) << "Buffers differ at index " << i;
    }
}

TEST(PolarDecoderTest, DecodeBlockSize768) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 768u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;
    using TestDecoder = Thiemar::Polar::SuccessiveCancellationListDecoder<N, M, K, TestDataIndices>;

    /* Set up test buffers. */
    std::array<uint8_t, K / 8u> test_in = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    auto test_out = TestEncoder::encode(test_in);
    auto test_decoded = TestDecoder::decode(test_out);

    for (std::size_t i = 0u; i < test_in.size(); i++) {
        EXPECT_EQ((int)test_in[i], (int)test_decoded[i]) << "Buffers differ at index " << i;
    }
}

TEST(PolarDecoderTest, DecodeBlockSize520) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 520u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;
    using TestDecoder = Thiemar::Polar::SuccessiveCancellationListDecoder<N, M, K, TestDataIndices>;

    /* Set up test buffers. */
    std::array<uint8_t, K / 8u> test_in = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    auto test_out = TestEncoder::encode(test_in);
    auto test_decoded = TestDecoder::decode(test_out);

    for (std::size_t i = 0u; i < test_in.size(); i++) {
        EXPECT_EQ((int)test_in[i], (int)test_decoded[i]) << "Buffers differ at index " << i;
    }
}

TEST(PolarDecoderTest, DecodeBlockSize512) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 512u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;
    using TestDecoder = Thiemar::Polar::SuccessiveCancellationListDecoder<N, M, K, TestDataIndices>;

    /* Set up test buffers. */
    std::array<uint8_t, K / 8u> test_in = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    auto test_out = TestEncoder::encode(test_in);
    auto test_decoded = TestDecoder::decode(test_out);

    for (std::size_t i = 0u; i < test_in.size(); i++) {
        EXPECT_EQ((int)test_in[i], (int)test_decoded[i]) << "Buffers differ at index " << i;
    }
}

