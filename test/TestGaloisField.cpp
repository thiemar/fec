#include <gtest/gtest.h>
#include <cstdlib>
#include "FEC/GaloisField.h"
#include "FEC/ReedSolomon.h"

TEST(GaloisFieldTest, Exponentiation) {
    using TestGaloisField = Thiemar::GaloisField<8u, Thiemar::ReedSolomon::Polynomials::m_8_285>;

    uint8_t gf256_p285[] = {   1,   2,   4,   8,  16,  32,  64, 128,  29,  58, 116, 232, 205, 135,  19,  38,
                              76, 152,  45,  90, 180, 117, 234, 201, 143,   3,   6,  12,  24,  48,  96, 192,
                             157,  39,  78, 156,  37,  74, 148,  53, 106, 212, 181, 119, 238, 193, 159,  35,
                              70, 140,   5,  10,  20,  40,  80, 160,  93, 186, 105, 210, 185, 111, 222, 161,
                              95, 190,  97, 194, 153,  47,  94, 188, 101, 202, 137,  15,  30,  60, 120, 240,
                             253, 231, 211, 187, 107, 214, 177, 127, 254, 225, 223, 163,  91, 182, 113, 226,
                             217, 175,  67, 134,  17,  34,  68, 136,  13,  26,  52, 104, 208, 189, 103, 206,
                             129,  31,  62, 124, 248, 237, 199, 147,  59, 118, 236, 197, 151,  51, 102, 204,
                             133,  23,  46,  92, 184, 109, 218, 169,  79, 158,  33,  66, 132,  21,  42,  84,
                             168,  77, 154,  41,  82, 164,  85, 170,  73, 146,  57, 114, 228, 213, 183, 115,
                             230, 209, 191,  99, 198, 145,  63, 126, 252, 229, 215, 179, 123, 246, 241, 255,
                             227, 219, 171,  75, 150,  49,  98, 196, 149,  55, 110, 220, 165,  87, 174,  65,
                             130,  25,  50, 100, 200, 141,   7,  14,  28,  56, 112, 224, 221, 167,  83, 166,
                              81, 162,  89, 178, 121, 242, 249, 239, 195, 155,  43,  86, 172,  69, 138,   9,
                              18,  36,  72, 144,  61, 122, 244, 245, 247, 243, 251, 235, 203, 139,  11,  22,
                              44,  88, 176, 125, 250, 233, 207, 131,  27,  54, 108, 216, 173,  71, 142,   1 };

    for (std::size_t i = 0u; i < sizeof(gf256_p285); i++) {
        EXPECT_EQ((int)gf256_p285[i], (int)TestGaloisField::antilog(i)) << "Buffers differ at index " << i;
    }
}

TEST(GaloisFieldTest, Logarithm) {
    using TestGaloisField = Thiemar::GaloisField<8u, Thiemar::ReedSolomon::Polynomials::m_8_285>;

    uint8_t gf256_p285[] = {   1,   2,   4,   8,  16,  32,  64, 128,  29,  58, 116, 232, 205, 135,  19,  38,
                              76, 152,  45,  90, 180, 117, 234, 201, 143,   3,   6,  12,  24,  48,  96, 192,
                             157,  39,  78, 156,  37,  74, 148,  53, 106, 212, 181, 119, 238, 193, 159,  35,
                              70, 140,   5,  10,  20,  40,  80, 160,  93, 186, 105, 210, 185, 111, 222, 161,
                              95, 190,  97, 194, 153,  47,  94, 188, 101, 202, 137,  15,  30,  60, 120, 240,
                             253, 231, 211, 187, 107, 214, 177, 127, 254, 225, 223, 163,  91, 182, 113, 226,
                             217, 175,  67, 134,  17,  34,  68, 136,  13,  26,  52, 104, 208, 189, 103, 206,
                             129,  31,  62, 124, 248, 237, 199, 147,  59, 118, 236, 197, 151,  51, 102, 204,
                             133,  23,  46,  92, 184, 109, 218, 169,  79, 158,  33,  66, 132,  21,  42,  84,
                             168,  77, 154,  41,  82, 164,  85, 170,  73, 146,  57, 114, 228, 213, 183, 115,
                             230, 209, 191,  99, 198, 145,  63, 126, 252, 229, 215, 179, 123, 246, 241, 255,
                             227, 219, 171,  75, 150,  49,  98, 196, 149,  55, 110, 220, 165,  87, 174,  65,
                             130,  25,  50, 100, 200, 141,   7,  14,  28,  56, 112, 224, 221, 167,  83, 166,
                              81, 162,  89, 178, 121, 242, 249, 239, 195, 155,  43,  86, 172,  69, 138,   9,
                              18,  36,  72, 144,  61, 122, 244, 245, 247, 243, 251, 235, 203, 139,  11,  22,
                              44,  88, 176, 125, 250, 233, 207, 131,  27,  54, 108, 216, 173,  71, 142,   1 };

    for (std::size_t i = 0u; i < sizeof(gf256_p285) - 1u; i++) {
        EXPECT_EQ((int)i, (int)TestGaloisField::log(gf256_p285[i])) << "Buffers differ at index " << i;
    }
}

/*
Expected outputs from:
http://www.ee.unb.ca/cgi-bin/tervo/calc2.pl?num=1&den=255&f=m&p=36&d=1&y=1&m=1.
*/
TEST(GaloisFieldTest, Multiplication) {
    using TestGaloisField = Thiemar::GaloisField<8u, Thiemar::ReedSolomon::Polynomials::m_8_285>;

    /* One or both arguments zero. */
    EXPECT_EQ(0u, TestGaloisField::multiply(124u, 0u));
    EXPECT_EQ(0u, TestGaloisField::multiply(0u, 23u));
    EXPECT_EQ(0u, TestGaloisField::multiply(0u, 0u));

    /* Max/min. */
    EXPECT_EQ(1u, TestGaloisField::multiply(1u, 1u));
    EXPECT_EQ(255u, TestGaloisField::multiply(255u, 1u));
    EXPECT_EQ(255u, TestGaloisField::multiply(1u, 255u));
    EXPECT_EQ(226u, TestGaloisField::multiply(255u, 255u));

    /* A few random cases. */
    EXPECT_EQ(1u, TestGaloisField::multiply(12u, 61u));
    EXPECT_EQ(251u, TestGaloisField::multiply(29u, 72u));
    EXPECT_EQ(22u, TestGaloisField::multiply(193u, 50u));
    EXPECT_EQ(76u, TestGaloisField::multiply(3u, 207u));
    EXPECT_EQ(122u, TestGaloisField::multiply(142u, 244u));
}

/*
Expected outputs from:
http://www.ee.unb.ca/cgi-bin/tervo/calc2.pl?num=1&den=255&f=m&p=36&d=1&y=1&m=1.
*/
TEST(GaloisFieldTest, Division) {
    using TestGaloisField = Thiemar::GaloisField<8u, Thiemar::ReedSolomon::Polynomials::m_8_285>;

    /* One or both arguments zero. */
    EXPECT_EQ(0u, TestGaloisField::divide(124u, 0u));
    EXPECT_EQ(0u, TestGaloisField::divide(0u, 23u));
    EXPECT_EQ(0u, TestGaloisField::divide(0u, 0u));

    /* Max/min. */
    EXPECT_EQ(1u, TestGaloisField::divide(1u, 1u));
    EXPECT_EQ(255u, TestGaloisField::divide(255u, 1u));
    EXPECT_EQ(253u, TestGaloisField::divide(1u, 255u));
    EXPECT_EQ(1u, TestGaloisField::divide(255u, 255u));

    /* A few random cases. */
    EXPECT_EQ(80u, TestGaloisField::divide(12u, 61u));
    EXPECT_EQ(74u, TestGaloisField::divide(29u, 72u));
    EXPECT_EQ(52u, TestGaloisField::divide(193u, 50u));
    EXPECT_EQ(78u, TestGaloisField::divide(3u, 207u));
    EXPECT_EQ(143u, TestGaloisField::divide(142u, 244u));
}

TEST(GaloisFieldPolynomialTest, Addition) {
    using TestGaloisField = Thiemar::GaloisField<8u, Thiemar::ReedSolomon::Polynomials::m_8_285>;
    std::array<TestGaloisField::gf_t, 80u> p1;
    std::array<TestGaloisField::gf_t, 8u> p2;

    std::array<TestGaloisField::gf_t, 80u> p3 = TestGaloisField::add(p1, p2);
    (void)p3;
}

TEST(GaloisFieldPolynomialTest, Multiplication) {
    using TestGaloisField = Thiemar::GaloisField<8u, Thiemar::ReedSolomon::Polynomials::m_8_285>;
    std::array<TestGaloisField::gf_t, 80u> p1;
    std::array<TestGaloisField::gf_t, 8u> p2;

    std::array<TestGaloisField::gf_t, 87u> p3 = TestGaloisField::multiply(p1, p2);
    (void)p3;
}

TEST(GaloisFieldPolynomialTest, Remainder) {
    using TestGaloisField = Thiemar::GaloisField<8u, Thiemar::ReedSolomon::Polynomials::m_8_285>;
    std::array<TestGaloisField::gf_t, 80u> p1;
    std::array<TestGaloisField::gf_t, 8u> p2;

    std::array<TestGaloisField::gf_t, 7u> p3 = TestGaloisField::remainder(p1, p2);
    (void)p3;
}
