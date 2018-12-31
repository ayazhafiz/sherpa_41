#include "css.hpp"

#include <gtest/gtest.h>

class CSSTest : public ::testing::Test {};

using namespace CSS;

TEST_F(CSSTest, ValueCtorDtor) {
    TextValue  text("txt");
    UnitValue  unit(1.0, px);
    ColorValue color(0, 0, 0, 0);
    TextValue  text2(text);
    UnitValue  unit2(unit);
    ColorValue color2(color);
}

TEST_F(CSSTest, makeValue) {
    TextValue val("hello");

    ASSERT_EQ(make_value(val)->print(), "hello");
    ASSERT_FALSE(make_value(val) == make_value(val));
}

TEST_F(CSSTest, valueIs) {
    TextValue  text("txt");
    UnitValue  unit(1.0, px);
    ColorValue color(0, 0, 0, 0);

    ASSERT_TRUE(text.is("txt"));
    ASSERT_TRUE(unit.is("1px"));
    ASSERT_TRUE(color.is("rgba(0, 0, 0, 0)"));
}

TEST_F(CSSTest, unitValue) {
    TextValue text("txt");
    UnitValue unit(1.0, px);

    ASSERT_EQ(text.unitValue(), 0);
    ASSERT_EQ(unit.unitValue(), 1);
}

TEST_F(CSSTest, printing) {
    TextValue  text("txt");
    UnitValue  unit(1.0, px);
    ColorValue color(0, 0, 0, 0);

    ASSERT_EQ(text.print(), "txt");
    ASSERT_EQ(unit.print(), "1px");
    ASSERT_EQ(color.print(), "rgba(0, 0, 0, 0)");
    ASSERT_EQ(color.channels(), std::vector<uint8_t>({0, 0, 0}));
}

TEST_F(CSSTest, SelectorCtorDtor) {
    Selector selector("tag", "id", {"class1", "class2"});
}

TEST_F(CSSTest, DeclarationCtorDtor) {
    Declaration declaration("key", make_value(TextValue("value")));
}

TEST_F(CSSTest, RuleCtorDtor) {
    Rule rule(PrioritySelectorSet, DeclarationSet);
}

TEST_F(CSSTest, StyleSheetCtorDtor) {
    StyleSheet styleSheet;
}