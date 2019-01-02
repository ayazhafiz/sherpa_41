// sherpa_41's CSS module test fixture, licensed under MIT. (c) hafiz, 2018

#include "css.hpp"

#include <gtest/gtest.h>

class CSSTest : public ::testing::Test {};

using namespace CSS;

TEST_F(CSSTest, ValueCtorDtor) {
    TextValue    text("txt");
    UnitValue    unit(1.0, px);
    ColorValue   color(0, 0, 0, 0);
    TextValue    text2(text);
    UnitValue    unit2(unit);
    ColorValue   color2(color);
    ValuePtr     text3  = ValuePtr(new TextValue("txt"));
    ValuePtr     unit3  = ValuePtr(new UnitValue(1.0, px));
    ValuePtr     color3 = ValuePtr(new ColorValue(0, 0, 0, 0));
    Value *      text4  = new TextValue("txt");
    Value *      unit4  = new UnitValue(1.0, px);
    Value *      color4 = new ColorValue(0, 0, 0, 0);
    TextValue *  text5  = new TextValue("txt");
    UnitValue *  unit5  = new UnitValue(1.0, px);
    ColorValue * color5 = new ColorValue(0, 0, 0, 0);

    ASSERT_NO_THROW(delete text4);
    ASSERT_NO_THROW(delete unit4);
    ASSERT_NO_THROW(delete color4);
    ASSERT_NO_THROW(delete text5);
    ASSERT_NO_THROW(delete unit5);
    ASSERT_NO_THROW(delete color5);
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
    ASSERT_EQ(color.channels(), (std::array<uint8_t, 3>{{0, 0, 0}}));
}

TEST_F(CSSTest, SelectorCtorDtor) {
    Selector selector("tag", "id", {"class1", "class2"});
}

TEST_F(CSSTest, DeclarationCtorDtor) {
    Declaration declaration("key", make_value(TextValue("value")));
    Declaration declaration2(declaration);
}

TEST_F(CSSTest, RuleCtorDtor) {
    Rule rule(PrioritySelectorSet, DeclarationSet);
}

TEST_F(CSSTest, StyleSheetCtorDtor) {
    StyleSheet styleSheet;
}