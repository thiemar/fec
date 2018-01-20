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
    std::array<uint8_t, 1024u> test_in = {};
    uint8_t test_decoded_fecmagic[TestDecoder::calculate_output_length(TestEncoder::calculate_output_length(test_in.size()))] = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    auto test_out = TestEncoder::encode(test_in);

    /* Decode with fecmagic. */
    fecmagic_dec.reset(test_decoded_fecmagic);
    fecmagic_dec.decode(test_out.data(), test_out.size());
    fecmagic_dec.flush();

    for (std::size_t i = 0u; i < test_in.size(); i++) {
        EXPECT_EQ((int)test_in[i], (int)test_decoded_fecmagic[i]) << "Buffers differ at index " << i;
    }
}

TEST(ConvolutionalDecoderTest, Decode) {
    /* Set up test buffers. */
    std::array<uint8_t, 1024u> test_in = {};

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

using TestEncoderPunctured = Thiemar::Convolutional::PuncturedConvolutionalEncoder<
    7u,
    Thiemar::Convolutional::PuncturingMatrices::n_2_rate_7_8,
    Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 1>,
    Thiemar::BinarySequence<1, 0, 0, 1, 1, 1, 1>
>;

using TestDecoderPunctured = Thiemar::Convolutional::PuncturedHardDecisionViterbiDecoder<
    7u,
    112u,
    Thiemar::Convolutional::PuncturingMatrices::n_2_rate_7_8,
    Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 1>,
    Thiemar::BinarySequence<1, 0, 0, 1, 1, 1, 1>
>;

TEST(PuncturedConvolutionalDecoderTest, Decode) {
    /* Set up test buffers. */
    std::array<uint8_t, 1024u> test_in = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    auto test_out = TestEncoderPunctured::encode(test_in);
    auto test_decoded = TestDecoderPunctured::decode(test_out);

    for (std::size_t i = 0u; i < test_in.size(); i++) {
        EXPECT_EQ((int)test_in[i], (int)test_decoded[i]) << "Buffers differ at index " << i;
    }
}

using TestEncoderRate3 = Thiemar::Convolutional::PuncturedConvolutionalEncoder<
    7u,
    Thiemar::Convolutional::PuncturingMatrices::n_3_rate_1_3,
    Thiemar::Convolutional::Polynomials::n_3_k_7_g11,
    Thiemar::Convolutional::Polynomials::n_3_k_7_g12,
    Thiemar::Convolutional::Polynomials::n_3_k_7_g13
>;

using TestDecoderRate3 = Thiemar::Convolutional::PuncturedHardDecisionViterbiDecoder<
    7u,
    48u,
    Thiemar::Convolutional::PuncturingMatrices::n_3_rate_1_3,
    Thiemar::Convolutional::Polynomials::n_3_k_7_g11,
    Thiemar::Convolutional::Polynomials::n_3_k_7_g12,
    Thiemar::Convolutional::Polynomials::n_3_k_7_g13
>;

TEST(ConvolutionalDecoderRate3Test, Decode) {
    /* Set up test buffers. */
    std::array<uint8_t, 1024u> test_in = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    auto test_out = TestEncoderRate3::encode(test_in);
    auto test_decoded = TestDecoderRate3::decode(test_out);

    for (std::size_t i = 0u; i < test_in.size(); i++) {
        EXPECT_EQ((int)test_in[i], (int)test_decoded[i]) << "Buffers differ at index " << i;
    }
}
