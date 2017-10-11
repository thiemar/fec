#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Convolutional.h"

/* Test against fecmagic implementation. */
#include "sequence.h"
#include "convolutional-decoder.h"

using TestEncoder = Thiemar::Convolutional::PuncturedConvolutionalEncoder<
    7u,
    Thiemar::Convolutional::PuncturingMatrices::n_2_rate_1_2,
    Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 1>,
    Thiemar::BinarySequence<1, 0, 0, 1, 1, 1, 1>
>;

using TestDecoder = Thiemar::Convolutional::PuncturedHardDecisionViterbiDecoder<
    7u,
    32u,
    Thiemar::Convolutional::PuncturingMatrices::n_2_rate_1_2,
    Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 1>,
    Thiemar::BinarySequence<1, 0, 0, 1, 1, 1, 1>
>;

TEST(ConvolutionalDecoderTest, ReferenceDecode) {
    fecmagic::PuncturedConvolutionalDecoder<
        fecmagic::Sequence<uint8_t, 1, 1>, 35, 7,
        uint8_t, 0b1011011, 0b1111001> fecmagic_dec;

    /* Set up test buffers. */
    uint8_t test_in[1024u] = {};
    uint8_t test_out[TestEncoder::calculate_output_length(sizeof(test_in))] = {0xaa};
    uint8_t test_decoded_fecmagic[1024u] = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < sizeof(test_in); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    TestEncoder::encode(test_in, sizeof(test_in), test_out);

    /* Decode with fecmagic. */
    fecmagic_dec.reset(test_decoded_fecmagic);
    fecmagic_dec.decode(test_out, TestEncoder::calculate_output_length(sizeof(test_in)));
    fecmagic_dec.flush();

    for (std::size_t i = 0u; i < sizeof(test_in); i++) {
        EXPECT_EQ((int)test_in[i], (int)test_decoded_fecmagic[i]) << "Buffers differ at index " << i;
    }
}

TEST(ConvolutionalDecoderTest, Decode) {
    /* Set up test buffers. */
    uint8_t test_in[1024u] = {};
    uint8_t test_out[TestEncoder::calculate_output_length(sizeof(test_in))] = {0xaa};
    uint8_t test_decoded[1024u] = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < sizeof(test_in); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    TestEncoder::encode(test_in, sizeof(test_in), test_out);
    TestDecoder::decode(test_out, sizeof(test_out), test_decoded);

    for (std::size_t i = 0u; i < sizeof(test_in); i++) {
        EXPECT_EQ((int)test_in[i], (int)test_decoded[i]) << "Buffers differ at index " << i;
    }
}

using TestEncoderPunctured = Thiemar::Convolutional::PuncturedConvolutionalEncoder<
    7u,
    Thiemar::Convolutional::PuncturingMatrices::n_2_rate_7_8,
    Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 1>,
    Thiemar::BinarySequence<1, 0, 0, 1, 1, 1, 1>
>;

using TestDecoderPunctured = Thiemar::Convolutional::PuncturedHardDecisionViterbiDecoder<
    7u,
    56u,
    Thiemar::Convolutional::PuncturingMatrices::n_2_rate_7_8,
    Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 1>,
    Thiemar::BinarySequence<1, 0, 0, 1, 1, 1, 1>
>;

TEST(PuncturedConvolutionalDecoderTest, Decode) {
    /* Set up test buffers. */
    uint8_t test_in[1024u] = {};
    uint8_t test_out[TestEncoderPunctured::calculate_output_length(sizeof(test_in))] = {0xaa};
    uint8_t test_decoded[1024u] = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < sizeof(test_in); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    TestEncoderPunctured::encode(test_in, sizeof(test_in), test_out);
    TestDecoderPunctured::decode(test_out, sizeof(test_out), test_decoded);

    for (std::size_t i = 0u; i < sizeof(test_in); i++) {
        EXPECT_EQ((int)test_in[i], (int)test_decoded[i]) << "Buffers differ at index " << i;
    }
}
