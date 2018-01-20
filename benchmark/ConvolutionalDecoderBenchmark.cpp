#include <benchmark/benchmark.h>
#include <cstdlib>
#include "FEC/Convolutional.h"

/* Benchmark against fecmagic implementation. */
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

void FECMagicConvolutionalDecoder_Decode(benchmark::State& state) {
    fecmagic::PuncturedConvolutionalDecoder<
        fecmagic::Sequence<uint8_t, 1, 1>, 35, 7,
        uint8_t, 0b1011011, 0b1111001> fecmagic_dec;

    /* Set up test buffers. */
    std::array<uint8_t, 1024u> test_in = {};
    std::array<uint8_t, TestDecoder::calculate_output_length(
        TestEncoder::calculate_output_length(test_in.size()))> test_decoded_fecmagic = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    auto test_out = TestEncoder::encode(test_in);

    while(state.KeepRunning()) {
        fecmagic_dec.reset(test_decoded_fecmagic.data());
        fecmagic_dec.decode(test_out.data(), TestEncoder::calculate_output_length(test_in.size()));
        fecmagic_dec.flush();
    }
}

BENCHMARK(FECMagicConvolutionalDecoder_Decode);

void ConvolutionalDecoder_Decode(benchmark::State& state) {
    /* Set up test buffers. */
    std::array<uint8_t, 1024u> test_in = {};
    std::array<uint8_t, TestDecoder::calculate_output_length(
        TestEncoder::calculate_output_length(test_in.size()))> test_decoded = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    auto test_out = TestEncoder::encode(test_in);

    while(state.KeepRunning()) {
        test_decoded = TestDecoder::decode(test_out);
    }
}

BENCHMARK(ConvolutionalDecoder_Decode);

using TestEncoderPunctured = Thiemar::Convolutional::PuncturedConvolutionalEncoder<
    7u,
    Thiemar::BinarySequence<1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0>,
    Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 1>,
    Thiemar::BinarySequence<1, 0, 0, 1, 1, 1, 1>
>;

using TestDecoderPunctured = Thiemar::Convolutional::PuncturedHardDecisionViterbiDecoder<
    7u,
    56u,
    Thiemar::BinarySequence<1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0>,
    Thiemar::BinarySequence<1, 1, 0, 1, 1, 0, 1>,
    Thiemar::BinarySequence<1, 0, 0, 1, 1, 1, 1>
>;

void FECMagicPuncturedConvolutionalDecoder_Decode(benchmark::State& state) {
    fecmagic::PuncturedConvolutionalDecoder<
        fecmagic::Sequence<uint8_t, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0>, 35, 7,
        uint8_t, 0b1011011, 0b1111001> fecmagic_dec;

    /* Set up test buffers. */
    std::array<uint8_t, 1024u> test_in = {};
    std::array<uint8_t, TestDecoderPunctured::calculate_output_length(
        TestEncoderPunctured::calculate_output_length(test_in.size()))> test_decoded_fecmagic = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i <test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    auto test_out = TestEncoderPunctured::encode(test_in);

    while(state.KeepRunning()) {
        fecmagic_dec.reset(test_decoded_fecmagic.data());
        fecmagic_dec.decode(test_out.data(), TestEncoderPunctured::calculate_output_length(test_in.size()));
        fecmagic_dec.flush();
    }
}

BENCHMARK(FECMagicPuncturedConvolutionalDecoder_Decode);

void PuncturedConvolutionalDecoder_Decode(benchmark::State& state) {
    /* Set up test buffers. */
    std::array<uint8_t, 1024u> test_in = {};
    std::array<uint8_t, TestDecoderPunctured::calculate_output_length(
        TestEncoderPunctured::calculate_output_length(test_in.size()))> test_decoded = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    auto test_out = TestEncoderPunctured::encode(test_in);

    while(state.KeepRunning()) {
        test_decoded = TestDecoderPunctured::decode(test_out);
    }
}

BENCHMARK(PuncturedConvolutionalDecoder_Decode);
