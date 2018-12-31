// sherpa_41's Layout module test fixture, licensed under MIT. (c) hafiz, 2018

#include "layout.hpp"

#include <gtest/gtest.h>

class LayoutTest : public ::testing::Test {};

using namespace Layout;

TEST_F(LayoutTest, BoxCtorDtor) {
    AnonymousBox anonymousBox;
    StyledBox    styledBox(BoxDimensions(Rectangle(0, 0, 0, 0)),
                        Style::StyledNode(DOM::NodePtr(new DOM::TextNode(""))));
}

TEST_F(LayoutTest, stringToDisplayType) {
    ASSERT_EQ(stodisplay("block"), Block);
    ASSERT_EQ(stodisplay("inline"), Inline);
    ASSERT_EQ(stodisplay("none"), None);
    ASSERT_EQ(stodisplay("literally not even a display type"), None);
}

TEST_F(LayoutTest, styledNodeToDisplayType) {
    Style::PropertyMap propertyMap;
    propertyMap["display"] = CSS::make_value(CSS::TextValue("block"));
    auto snode = Style::StyledNode(DOM::NodePtr(new DOM::TextNode("")),
                                   std::move(propertyMap));
    ASSERT_EQ(snodetodisplay(snode, ""), Block);

    auto snode2 = Style::StyledNode(DOM::NodePtr(new DOM::TextNode("")));
    ASSERT_EQ(snodetodisplay(snode2, "inline"), Inline);
}

TEST_F(LayoutTest, Rectangle) {
    Rectangle rectangle(0, 0, 1, 1);
    auto      rect2 = rectangle.expand(Edges(1, 1, 1, 1));

    ASSERT_EQ(rect2.origin.x, -1);
    ASSERT_EQ(rect2.origin.y, -1);
    ASSERT_EQ(rect2.height, 3);
    ASSERT_EQ(rect2.width, 3);
}

TEST_F(LayoutTest, Edges) {
    Edges edges(0, 0, 0, 0);
}

TEST_F(LayoutTest, BoxDimensions) {
    BoxDimensions boxDimensions(Rectangle(0, 0, 1, 1),
                                Edges(1, 1, 1, 1),   // margin
                                Edges(1, 1, 1, 1),   // padding
                                Edges(2, 3, 4, 5));  // border
    auto          pad = boxDimensions.paddingArea();
    auto          bor = boxDimensions.borderArea();
    auto          mar = boxDimensions.marginArea();

    ASSERT_EQ(pad.width, 3);
    ASSERT_EQ(pad.height, 3);
    ASSERT_EQ(bor.width, 11);
    ASSERT_EQ(bor.height, 9);
    ASSERT_EQ(mar.width, 13);
    ASSERT_EQ(mar.height, 11);
}