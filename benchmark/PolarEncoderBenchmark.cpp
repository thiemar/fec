#include <benchmark/benchmark.h>
#include <cstdlib>
#include "FEC/Polar.h"

void PolarEncoderBlockSize1024_Encode(benchmark::State& state) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, K, -2>::data_index_sequence;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, K, TestDataIndices>;

    /* Set up test buffers. */
    uint8_t test_in[K / 8u] = {};
    uint8_t test_out[N / 8u] = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < sizeof(test_in); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    while(state.KeepRunning()) {
        TestEncoder::encode(test_in, sizeof(test_in), test_out);
    }
}

BENCHMARK(PolarEncoderBlockSize1024_Encode);

