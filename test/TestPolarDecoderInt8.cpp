#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Polar.h"

TEST(PolarDecoderInt8Test, CorruptedPacket) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 1024u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>::data_index_sequence;
    using TestDecoder = Thiemar::Polar::SuccessiveCancellationListDecoder<N, M, K, TestDataIndices, int8_t>;

    /* Set up test buffers. */
    std::array<uint8_t, M / 8u> test_code = {
        130,  203,  189,   91,  191,   32,   12,  119,
        244,  197,  173,   72,  151,  140,   64,  119,
        192,   16,  108,  135,  221,  241,  253,  243,
        174,  212,  255,  251,   50,   95,   39,  180,
        198,  196,  125,  219,  105,  229,   81,  124,
        136,   27,  190,   68,   32,  244,  111,   67,
          2,   51,  207,  252,  111,  223,   65,   21,
         39,   26,   69,   48,  247,  143,  193,    4,
        221,   28,  121,  227,  231,  169,  145,  220,
         86,  130,   26,  168,   80,  121,   66,  160,
        212,  240,    6,  100,   65,    7,  121,   46,
        172,   67,  129,   54,  237,  107,  247,  200,
         22,  207,   37,  110,  190,  234,  108,  221,
        234,  101,   43,   93,  159,  185,  135,   97,
        243,  250,  206,  226,  168,   16,   37,  157,
        138,   81,  119,  247,  167,  119,  173,  248
    };

    std::array<uint8_t, K / 8u> test_data = {
        247,  187,   97,  143,   15,  205,  248,   34,
        174,  106,   41,  135,  120,  220,   24,  199,
        138,   81,   30,   36,   80,   33,  178,    0,
        127,  178,  230,   41,  136,  155,  118,  181,
        251,  228,   52,  173,   15,  186,  155,  119,
        117,   50,  181,  238,  207,  220,  194,  176,
        226,  234,  206,  226,  168,   16,   36,   29,
        138,   91,   55,  247,  167,  127,  141,  248
    };

    auto test_decoded = TestDecoder::decode(test_code);

    for (std::size_t i = 0u; i < test_data.size(); i++) {
        EXPECT_EQ((int)test_data[i], (int)test_decoded[i]) << "Buffers differ at index " << i;
    }
}

TEST(PolarDecoderInt8Test, DecodeBlockSize1024) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 1024u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;
    using TestDecoder = Thiemar::Polar::SuccessiveCancellationListDecoder<N, M, K, TestDataIndices, int8_t>;

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

TEST(PolarDecoderInt8Test, DecodeBlockSize768) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 768u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;
    using TestDecoder = Thiemar::Polar::SuccessiveCancellationListDecoder<N, M, K, TestDataIndices, int8_t>;

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

TEST(PolarDecoderInt8Test, DecodeBlockSize520) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 520u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;
    using TestDecoder = Thiemar::Polar::SuccessiveCancellationListDecoder<N, M, K, TestDataIndices, int8_t>;

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

TEST(PolarDecoderInt8Test, DecodeBlockSize512) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 512u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;
    using TestDecoder = Thiemar::Polar::SuccessiveCancellationListDecoder<N, M, K, TestDataIndices, int8_t>;

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

