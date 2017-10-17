#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/ReedSolomon.h"

/* Test against ezpwd implementation. */
#include <ezpwd/rs>

template <typename T, T... Is>
static std::array<T, sizeof...(Is)> poly_compare_helper(std::integer_sequence<T, Is...>) {
    return std::array<T, sizeof...(Is)>{ Is... };
}

#define MESSAGE_PARITY_LENGTH 32u
#define MESSAGE_DATA_LENGTH 223u

TEST(ReedSolomonDecoderTest, CheckNoErrors) {
    ezpwd::RS<255u, 255u-MESSAGE_PARITY_LENGTH> rs;
    std::array<uint8_t, MESSAGE_DATA_LENGTH+MESSAGE_PARITY_LENGTH> rs_container;

    using TestEncoder = Thiemar::ReedSolomon::ReedSolomonEncoder<
        8u, Thiemar::ReedSolomon::Polynomials::m_8_285, MESSAGE_PARITY_LENGTH>;
    uint8_t buf[MESSAGE_DATA_LENGTH+MESSAGE_PARITY_LENGTH] = {};
    
    std::srand(123u);
    for (std::size_t i = 0u; i < MESSAGE_DATA_LENGTH; i++) {
        rs_container[i] = std::rand() & 0xffu;
        buf[i] = rs_container[i];
    }

    rs.encode(rs_container);
    TestEncoder::encode<MESSAGE_DATA_LENGTH>(buf);

    EXPECT_EQ((int)0u, (int)TestEncoder::check<MESSAGE_DATA_LENGTH>(buf));
}

TEST(ReedSolomonDecoderTest, CheckErrors) {
    ezpwd::RS<255u, 255u-MESSAGE_PARITY_LENGTH> rs;
    std::array<uint8_t, MESSAGE_DATA_LENGTH+MESSAGE_PARITY_LENGTH> rs_container;

    using TestEncoder = Thiemar::ReedSolomon::ReedSolomonEncoder<
        8u, Thiemar::ReedSolomon::Polynomials::m_8_285, MESSAGE_PARITY_LENGTH>;
    uint8_t buf[MESSAGE_DATA_LENGTH+MESSAGE_PARITY_LENGTH] = {};
    
    std::srand(123u);
    for (std::size_t i = 0u; i < MESSAGE_DATA_LENGTH; i++) {
        rs_container[i] = std::rand() & 0xffu;
        buf[i] = rs_container[i];
    }

    rs.encode(rs_container);
    TestEncoder::encode<MESSAGE_DATA_LENGTH>(buf);
    buf[0] = 0u;

    EXPECT_NE((int)0u, (int)TestEncoder::check<MESSAGE_DATA_LENGTH>(buf));
}
