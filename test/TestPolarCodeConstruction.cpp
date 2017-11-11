#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Polar.h"

template <int32_t... B>
constexpr std::array<int32_t, sizeof...(B)> expand_b_params(std::integer_sequence<int32_t, B...>) {
    return std::array<int32_t, sizeof...(B)>{ B ... };
}

#define BLOCK_SIZE 10u

TEST(PolarCodeConstructionTest, CalculateBhattacharyyaBounds) {
    std::array<int32_t, 1u << BLOCK_SIZE> b_params = expand_b_params(
        Thiemar::Detail::BhattacharyyaBoundSequence<BLOCK_SIZE, -2>::b_param_sequence{});

    for (std::size_t i = 0u; i < b_params.size(); i++) {
        printf("%d\n", b_params[i]);
        // EXPECT_EQ((int)test_in[i], (int)test_decoded_fecmagic[i]) << "Buffers differ at index " << i;
    }
}
