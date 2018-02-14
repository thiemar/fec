#include <benchmark/benchmark.h>
#include <cstdlib>
#include "FEC/Polar.h"

void PolarDecoderInt8BlockSize1024_Decode(benchmark::State& state) {
    constexpr std::size_t N = 1024u;
    constexpr std::size_t M = 1024u;
    constexpr std::size_t K = 512u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;
    using TestDecoder = Thiemar::Polar::SuccessiveCancellationListDecoder<N, M, K, TestDataIndices, int8_t>;

    /* Set up test buffers. */
    std::array<uint8_t, K / 8u> test_in = {};
    std::array<uint8_t, K / 8u> test_decoded = {};

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

BENCHMARK(PolarDecoderInt8BlockSize1024_Decode);

void PolarDecoderInt8BlockSize1536_Decode(benchmark::State& state) {
    constexpr std::size_t N = 2048u;
    constexpr std::size_t M = 1536u;
    constexpr std::size_t K = 768u;
    using TestDataIndices = Thiemar::Polar::PolarCodeConstructor<N, M, K, -2>;
    using TestEncoder = Thiemar::Polar::PolarEncoder<N, M, K, TestDataIndices>;
    using TestDecoder = Thiemar::Polar::SuccessiveCancellationListDecoder<N, M, K, TestDataIndices, int8_t>;

    /* Set up test buffers. */
    std::array<uint8_t, K / 8u> test_in = {};
    std::array<uint8_t, K / 8u> test_decoded = {};

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

BENCHMARK(PolarDecoderInt8BlockSize1536_Decode);
