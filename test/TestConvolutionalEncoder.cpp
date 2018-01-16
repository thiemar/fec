#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Convolutional.h"

/* Test against fecmagic implementation. */
#include "convolutional-encoder.h"

using TestEncoder_k_7 = Thiemar::Convolutional::PuncturedConvolutionalEncoder<
    7u,
    Thiemar::Convolutional::PuncturingMatrices::n_2_rate_1_2,
    Thiemar::Convolutional::Polynomials::n_2_k_7_g11,
    Thiemar::Convolutional::Polynomials::n_2_k_7_g12
>;

TEST(ConvolutionalEncoderTest_k_7, Encode) {
    fecmagic::ConvolutionalEncoder<7, uint16_t, 0b1100111, 0b1011101> fecmagic_enc;

    /* Set up test buffers. */
    std::array<uint8_t, 1024u> test_in = {};
    uint8_t test_out_fecmagic[TestEncoder_k_7::calculate_output_length(test_in.size())] = {0x55};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    fecmagic_enc.reset(test_out_fecmagic);
    fecmagic_enc.encode(test_in.data(), test_in.size());
    fecmagic_enc.flush();

    auto test_out = TestEncoder_k_7::encode(test_in);

    for (std::size_t i = 0u; i < test_out.size(); i++) {
        EXPECT_EQ((int)test_out_fecmagic[i], (int)test_out[i]) << "Buffers differ at index " << i;
    }
}

using TestEncoder_k_9 = Thiemar::Convolutional::PuncturedConvolutionalEncoder<
    9u,
    Thiemar::Convolutional::PuncturingMatrices::n_2_rate_1_2,
    Thiemar::Convolutional::Polynomials::n_2_k_9_g11,
    Thiemar::Convolutional::Polynomials::n_2_k_9_g12
>;

TEST(ConvolutionalEncoderTest_k_9, Encode) {
    fecmagic::ConvolutionalEncoder<9, uint16_t, 0b111101011, 0b101110001> fecmagic_enc;

    /* Set up test buffers. */
    std::array<uint8_t, 1024u> test_in = {};
    uint8_t test_out_fecmagic[TestEncoder_k_9::calculate_output_length(test_in.size())] = {0x55};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    fecmagic_enc.reset(test_out_fecmagic);
    fecmagic_enc.encode(test_in.data(), test_in.size());
    fecmagic_enc.flush();

    auto test_out = TestEncoder_k_9::encode(test_in);

    for (std::size_t i = 0u; i < test_out.size(); i++) {
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
    std::array<uint8_t, 1024u> test_in = {};
    uint8_t test_out_fecmagic[TestEncoderPunctured::calculate_output_length(test_in.size())] = {0x55};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    fecmagic_enc.reset(test_out_fecmagic);
    fecmagic_enc.encode(test_in.data(), test_in.size());
    fecmagic_enc.flush();

    auto test_out = TestEncoderPunctured::encode(test_in);

    for (std::size_t i = 0u; i < test_out.size(); i++) {
        EXPECT_EQ((int)test_out_fecmagic[i], (int)test_out[i]) << "Buffers differ at index " << i;
    }
}
