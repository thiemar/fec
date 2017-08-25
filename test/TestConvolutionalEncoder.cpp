#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Convolutional.h"

/* Test against fecmagic implementation. */
#include "convolutional-encoder.h"

using TestEncoder = Thiemar::Convolutional::PuncturedConvolutionalEncoder<
    7u,
    Thiemar::BinarySequence<1, 1>,
    Thiemar::BinarySequence<1, 1, 1, 0, 0, 1, 1>,
    Thiemar::BinarySequence<1, 0, 1, 1, 1, 0, 1>
>;

TEST(ConvolutionalEncoderTest, Encode) {
    fecmagic::ConvolutionalEncoder<7, uint8_t, 0b1100111, 0b1011101> fecmagic_enc;

    /* Set up test buffers. */
    uint8_t test_in[1024u] = {0};
    uint8_t test_out[TestEncoder::calculate_output_length(sizeof(test_in))] = {0xaa};
    uint8_t test_out_fecmagic[TestEncoder::calculate_output_length(sizeof(test_in))] = {0x55};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < sizeof(test_in); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    fecmagic_enc.reset(test_out_fecmagic);
    fecmagic_enc.encode(test_in, sizeof(test_in));
    fecmagic_enc.flush();

    TestEncoder::encode(test_in, sizeof(test_in), test_out);

    for (std::size_t i = 0u; i < sizeof(test_out); i++) {
        EXPECT_EQ((int)test_out_fecmagic[i], (int)test_out[i]) << "Buffers differ at index " << i;
    }
}

using TestEncoderPunctured = Thiemar::Convolutional::PuncturedConvolutionalEncoder<
    7u,
    Thiemar::BinarySequence<1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0>,
    Thiemar::BinarySequence<1, 1, 1, 0, 0, 1, 1>,
    Thiemar::BinarySequence<1, 0, 1, 1, 1, 0, 1>
>;

TEST(PuncturedConvolutionalEncoderTest, Encode) {
    fecmagic::PuncturedConvolutionalEncoder<
        fecmagic::Sequence<uint8_t, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0>,
        7, uint8_t, 0b1100111, 0b1011101> fecmagic_enc;

    /* Set up test buffers. */
    uint8_t test_in[1024u] = {0};
    uint8_t test_out[TestEncoderPunctured::calculate_output_length(sizeof(test_in))] = {0xaa};
    uint8_t test_out_fecmagic[TestEncoderPunctured::calculate_output_length(sizeof(test_in))] = {0x55};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < sizeof(test_in); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    fecmagic_enc.reset(test_out_fecmagic);
    fecmagic_enc.encode(test_in, sizeof(test_in));
    fecmagic_enc.flush();

    TestEncoderPunctured::encode(test_in, sizeof(test_in), test_out);

    for (std::size_t i = 0u; i < sizeof(test_out); i++) {
        EXPECT_EQ((int)test_out_fecmagic[i], (int)test_out[i]) << "Buffers differ at index " << i;
    }
}
