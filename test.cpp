#include "pch.h"
#include "../Variant/variant.hpp"
#include <string>

TEST(VariantTest, DefaultConstructorIntTest) {
    Variant<int, double, std::string> v(5);

    EXPECT_TRUE(v.holds_alternative<int>());
    EXPECT_EQ(v.get<int>(), 5);
}

TEST(VariantTest, DefaultConstructorStringTest) {
    Variant<int, double, std::string> v(std::string{ "abc" });

    EXPECT_TRUE(v.holds_alternative<std::string>());
    EXPECT_EQ(v.get<std::string>(), std::string{ "abc" });
}

TEST(VariantTest, CopyConstructorTest) {
    Variant<int, double, std::string> v1(5.0);
    Variant<int, double, std::string> v2(v1);

    EXPECT_TRUE(v2.holds_alternative<double>());
    EXPECT_EQ(v2.get<double>(), 5.0);
}

TEST(VariantTest, MoveConstructorTest) {
    Variant<int, std::string> v1(std::string{ "abc" });
    Variant<int, std::string> v2(std::move(v1));

    EXPECT_TRUE(v2.holds_alternative<std::string>());
    EXPECT_EQ(v2.get<std::string>(), std::string{ "abc" });
}

TEST(VariantTest, GetByTypeTest) {
    Variant<int, double, std::string> v(5);

    EXPECT_EQ(v.get<int>(), 5);
}

TEST(VariantTest, GetByIndexTest) {
    Variant<int, double, std::string> v(5.0);

    EXPECT_EQ(v.get<1>(), 5.0);
}

TEST(VariantTest, HoldsAlternativeTest) {
    Variant<int, double, std::string> v(std::string{ "abc" });

    EXPECT_TRUE(v.holds_alternative<std::string>());
    EXPECT_FALSE(v.holds_alternative<int>());
    EXPECT_FALSE(v.holds_alternative<double>());
}

TEST(VariantTest, IndexTest) {
    Variant<int, double, std::string> v1(5);
    Variant<int, double, std::string> v2(5.0);
    Variant<int, double, std::string> v3(std::string{ "abc" });

    EXPECT_EQ(v1.index(), 0);
    EXPECT_EQ(v2.index(), 1);
    EXPECT_EQ(v3.index(), 2);
}

TEST(VariantTest, CopyAssignmentTest) {
    Variant<int, double, std::string> v1(5);
    Variant<int, double, std::string> v2(10.0);

    v2 = v1;
    EXPECT_TRUE(v2.holds_alternative<int>());
    EXPECT_EQ(v2.get<int>(), 5);
}

TEST(VariantTest, MoveAssignmentTest) {
    Variant<int, std::string> v1(std::string{ "abc" });
    Variant<int, std::string> v2(5);

    v2 = std::move(v1);
    EXPECT_TRUE(v2.holds_alternative<std::string>());
    EXPECT_EQ(v2.get<std::string>(), std::string{ "abc" });
}

TEST(VariantTest, SelfAssignmentTest) {
    Variant<int, double> v(5);
    v = v;

    EXPECT_TRUE(v.holds_alternative<int>());
    EXPECT_EQ(v.get<int>(), 5);
}

TEST(VariantTest, GetByTypeThrowTest) {
    Variant<int, double> v(5);
    EXPECT_THROW(v.get<double>(), std::runtime_error);
}

TEST(VariantTest, GetByIndexThrowTest) {
    Variant<int, double> v(5);
    EXPECT_THROW(v.get<1>(), std::runtime_error);
}
