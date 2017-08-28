#include <benchmark/benchmark.h>
#include <cstdlib>

/* Benchmark against ezpwd implementation. */
#include <ezpwd/rs>

/* Benchmark against mersinvald implementation. */
#include <rs.hpp>

#define MESSAGE_PARITY_LENGTH 8u
#define MESSAGE_DATA_LENGTH 100u

void EZPWD_ReedSolomonEncoder_Encode(benchmark::State& state) {
    ezpwd::RS<255u, 255u-MESSAGE_PARITY_LENGTH> rs;
    std::array<uint8_t, MESSAGE_DATA_LENGTH+MESSAGE_PARITY_LENGTH> rs_container;
    
    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < MESSAGE_DATA_LENGTH; i++) {
        rs_container[i] = std::rand() & 0xffu;
    }

    while(state.KeepRunning()) {
        rs.encode(rs_container);
    }
}

BENCHMARK(EZPWD_ReedSolomonEncoder_Encode);

void MersinvaldReedSolomonEncoder_Encode(benchmark::State& state) {
    RS::ReedSolomon<MESSAGE_DATA_LENGTH, MESSAGE_PARITY_LENGTH> rs;
    uint8_t message[MESSAGE_DATA_LENGTH] = {0u};
    uint8_t encoded[MESSAGE_DATA_LENGTH+MESSAGE_PARITY_LENGTH] = {0u};
    
    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < MESSAGE_DATA_LENGTH; i++) {
        message[i] = std::rand() & 0xffu;
    }

    while(state.KeepRunning()) {
        rs.Encode(message, encoded);
    }
}

BENCHMARK(MersinvaldReedSolomonEncoder_Encode);
