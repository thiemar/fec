#include <benchmark/benchmark.h>
#include <cstdlib>
#include "FEC/ReedSolomon.h"

/* Benchmark against ezpwd implementation. */
#include <ezpwd/rs>

/* Benchmark against mersinvald implementation. */
#include <rs.hpp>

#define MESSAGE_PARITY_LENGTH 32u
#define MESSAGE_DATA_LENGTH 223u

void ReedSolomonEncoder_Encode(benchmark::State& state) {
    using TestEncoder = Thiemar::ReedSolomon::ReedSolomonEncoder<8u, Thiemar::ReedSolomon::Polynomials::m_8_285, 8u>;
    uint8_t buf[MESSAGE_DATA_LENGTH+MESSAGE_PARITY_LENGTH] = {};

    /* Seed RNG for repeatibility. */
    std::srand(123u);
    for (std::size_t i = 0u; i < MESSAGE_DATA_LENGTH; i++) {
        buf[i] = std::rand() & 0xffu;
    }

    while(state.KeepRunning()) {
        TestEncoder::encode<MESSAGE_DATA_LENGTH>(buf);
    }
}

BENCHMARK(ReedSolomonEncoder_Encode);

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
    uint8_t message[MESSAGE_DATA_LENGTH] = {};
    uint8_t encoded[MESSAGE_DATA_LENGTH+MESSAGE_PARITY_LENGTH] = {};
    
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
