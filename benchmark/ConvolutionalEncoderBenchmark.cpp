#include <benchmark/benchmark.h>
#include <cstdlib>
#include "conv.h"

/* Benchmark against fecmagic implementation. */
#include "convolutional-encoder.h"

using TestEncoder = Thiemar::Convolutional::PuncturedConvolutionalEncoder<
    7u,
    Thiemar::Convolutional::BinarySequence<1, 1>,
    Thiemar::Convolutional::BinarySequence<1, 1, 1, 0, 0, 1, 1>,
    Thiemar::Convolutional::BinarySequence<1, 0, 1, 1, 1, 0, 1>
>;

void ConvolutionalEncoder_Encode(benchmark::State& state) {
    /* Set up test buffers. */
    uint8_t test_in[1024u] = {};
    uint8_t test_out[TestEncoder::calculate_output_length(sizeof(test_in))] = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < sizeof(test_in); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    while(state.KeepRunning()) {
        TestEncoder::encode(test_in, sizeof(test_in), test_out);
    }
}

BENCHMARK(ConvolutionalEncoder_Encode);

void FECMagicConvolutionalEncoder_Encode(benchmark::State& state) {
    fecmagic::ConvolutionalEncoder<7, uint8_t, 0b1100111, 0b1011101> fecmagic_enc;

    /* Set up test buffers. */
    uint8_t test_in[1024u] = {};
    uint8_t test_out[TestEncoder::calculate_output_length(sizeof(test_in))] = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < sizeof(test_in); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    while(state.KeepRunning()) {
        fecmagic_enc.reset(test_out);
        fecmagic_enc.encode(test_in, sizeof(test_in));
        fecmagic_enc.flush();
    }
}

BENCHMARK(FECMagicConvolutionalEncoder_Encode);
