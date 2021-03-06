#include <benchmark/benchmark.h>
#include <cstdlib>
#include "FEC/Polar.h"

void PolarEncoderBlockSize1024_Encode(benchmark::State& state) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 1024u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;

    /* Set up test buffers. */
    std::array<uint8_t, K / 8u> test_in = {};
    std::array<uint8_t, M / 8u> test_out = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    while(state.KeepRunning()) {
        test_out = TestEncoder::encode(test_in);
    }
}

BENCHMARK(PolarEncoderBlockSize1024_Encode);

void PolarEncoderBlockSize768_Encode(benchmark::State& state) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 768u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;

    /* Set up test buffers. */
    std::array<uint8_t, K / 8u> test_in = {};
    std::array<uint8_t, M / 8u> test_out = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < test_in.size(); i++) {
        test_in[i] = std::rand() & 0xffu;
    }

    while(state.KeepRunning()) {
        test_out = TestEncoder::encode(test_in);
    }
}

BENCHMARK(PolarEncoderBlockSize768_Encode);
