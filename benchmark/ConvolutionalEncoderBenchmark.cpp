#include <benchmark/benchmark.h>
#include "conv.h"

using TestConvEnc1 = Thiemar::Convolutional::PuncturedConvolutionalEncoder<
    7u,
    Thiemar::Convolutional::PuncturingMatrices::n_2_rate_1_2,
    Thiemar::Convolutional::Polynomials::n_2_k_7_g11,
    Thiemar::Convolutional::Polynomials::n_2_k_7_g12>;

void ConvolutionalEncoder_Encode(benchmark::State& state) {
    TestConvEnc1 test_encoder;

    while(state.KeepRunning()) {
        /* Encode some data. */
    }
}

BENCHMARK(ConvolutionalEncoder_Encode);
