#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/Polar.h"

template <typename T, T... I>
constexpr std::array<T, sizeof...(I)> expand_sequence(std::integer_sequence<T, I...>) {
    return std::array<T, sizeof...(I)>{ I ... };
}

#define N 2048u
#define K 1024u

TEST(PolarCodeConstructionTest, CalculateBhattacharyyaBounds) {
    std::array<std::size_t, K> data_indices = expand_sequence(
        Thiemar::Polar::PolarCodeConstructor<N, K, -2>::data_index_sequence{});

    for (std::size_t i = 0u; i < data_indices.size(); i++) {
        // printf("%lu\n", data_indices[i]);
        // EXPECT_EQ((int)test_in[i], (int)test_decoded_fecmagic[i]) << "Buffers differ at index " << i;
    }
}
