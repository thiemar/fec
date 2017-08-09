#include <gtest/gtest.h>
#include "conv.h"

using TestConvEnc1 = Thiemar::Convolutional::PuncturedConvolutionalEncoder<
    7u,
    Thiemar::Convolutional::PuncturingMatrices::n_2_rate_1_2,
    Thiemar::Convolutional::Polynomials::n_2_k_7_g11,
    Thiemar::Convolutional::Polynomials::n_2_k_7_g12>;

TEST(ConvolutionalEncoderTest, Instantiation) {
    TestConvEnc1 test_encoder;
}

TEST(ConvolutionalEncoderTest, Encode) {
    TestConvEnc1 test_encoder;
    test_encoder.test();
}
