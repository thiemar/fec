#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Polar.h"

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

