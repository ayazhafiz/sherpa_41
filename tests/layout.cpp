// sherpa_41's Layout module test fixture, licensed under MIT. (c) hafiz, 2018

#include "layout.hpp"

#include <gtest/gtest.h>

class LayoutTest : public ::testing::Test {};

using namespace Layout;

TEST_F(LayoutTest, BoxCtorDtor) {
    AnonymousBox anonymousBox;
    StyledBox    styledBox(BoxDimensions(Rectangle(0, 0, 0, 0)),
                        Style::StyledNode(DOM::NodePtr(new DOM::TextNode(""))));
    BoxPtr       boxPtr1(anonymousBox.clone());
    BoxPtr       boxPtr2(styledBox.clone());
    BoxPtr       boxPtr3(boxPtr1->clone());
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

TEST_F(LayoutTest, FromDisplayNone) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap;
    propertyMap["display"] = CSS::make_value(CSS::TextValue("none"));
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap));

    ASSERT_EQ(Box::from(styledNode, boxDimensions), nullptr);
}

TEST_F(LayoutTest, FromChildrenDisplayBlock) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap1;
    propertyMap1["display"] = CSS::make_value(CSS::TextValue("block"));
    Style::PropertyMap propertyMap2;
    propertyMap2["display"] = CSS::make_value(CSS::TextValue("block"));
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap1),
                                 {Style::StyledNode(DOM::NodePtr(
                                                        new DOM::TextNode("")),
                                                    std::move(propertyMap2))});
    auto              box = Box::from(styledNode, boxDimensions);

    ASSERT_EQ(box->getChildren().size(), 1);
}

TEST_F(LayoutTest, FromChildrenDisplayInline_One) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap1;
    propertyMap1["display"] = CSS::make_value(CSS::TextValue("block"));
    Style::PropertyMap propertyMap2;
    propertyMap2["display"] = CSS::make_value(CSS::TextValue("inline"));
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap1),
                                 {Style::StyledNode(DOM::NodePtr(
                                                        new DOM::TextNode("")),
                                                    std::move(propertyMap2))});
    auto              box      = Box::from(styledNode, boxDimensions);
    auto              children = box->getChildren();

    ASSERT_EQ(children.size(), 1);
    ASSERT_TRUE(dynamic_cast<AnonymousBox *>(children[0].get()));
    ASSERT_EQ(children[0]->getChildren().size(), 1);
}

TEST_F(LayoutTest, FromChildrenDisplayInline_Multiple) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap1;
    propertyMap1["display"] = CSS::make_value(CSS::TextValue("block"));
    Style::PropertyMap propertyMap2;
    propertyMap2["display"] = CSS::make_value(CSS::TextValue("inline"));
    Style::PropertyMap propertyMap3;
    propertyMap3["display"] = CSS::make_value(CSS::TextValue("inline"));
    Style::PropertyMap propertyMap4;
    propertyMap4["display"] = CSS::make_value(CSS::TextValue("inline"));
    Style::StyledNode
         styledNode(DOM::NodePtr(new DOM::TextNode("")),
                   std::move(propertyMap1),
                   {Style::StyledNode(DOM::NodePtr(new DOM::TextNode("")),
                                      std::move(propertyMap2)),
                    Style::StyledNode(DOM::NodePtr(new DOM::TextNode("")),
                                      std::move(propertyMap3)),
                    Style::StyledNode(DOM::NodePtr(new DOM::TextNode("")),
                                      std::move(propertyMap4))});
    auto box      = Box::from(styledNode, boxDimensions);
    auto children = box->getChildren();

    ASSERT_EQ(children.size(), 1);
    ASSERT_TRUE(dynamic_cast<AnonymousBox *>(children[0].get()));
    ASSERT_EQ(children[0]->getChildren().size(), 3);

    for (const auto & child : children[0]->getChildren()) {
        ASSERT_TRUE(dynamic_cast<StyledBox *>(child.get()));
    }
}

TEST_F(LayoutTest, FromChildrenDisplayInline_Within_Inline) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap1;
    propertyMap1["display"] = CSS::make_value(CSS::TextValue("block"));
    Style::PropertyMap propertyMap2;
    propertyMap2["display"] = CSS::make_value(CSS::TextValue("inline"));
    Style::PropertyMap propertyMap3;
    propertyMap3["display"] = CSS::make_value(CSS::TextValue("inline"));

    auto iISN = Style::StyledNode(DOM::NodePtr(new DOM::TextNode("")),
                                  std::move(propertyMap3));
    auto oISN = Style::StyledNode(DOM::NodePtr(new DOM::TextNode("")),
                                  std::move(propertyMap2),
                                  {iISN});
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap1),
                                 {oISN});
    auto              box      = Box::from(styledNode, boxDimensions);
    auto              children = box->getChildren();

    ASSERT_EQ(children.size(), 1);
    ASSERT_TRUE(dynamic_cast<AnonymousBox *>(children[0].get()));

    auto anonBox = children[0].get();

    ASSERT_EQ(anonBox->getChildren().size(), 1);

    auto outerNode = dynamic_cast<StyledBox *>(anonBox->getChildren()[0].get())
                         ->getContent();

    ASSERT_EQ(outerNode.value("display")->print(), "inline");
    ASSERT_EQ(anonBox->getChildren()[0]->getChildren().size(), 1);

    auto innerNode = dynamic_cast<StyledBox *>(
                         anonBox->getChildren()[0]->getChildren()[0].get())
                         ->getContent();

    ASSERT_EQ(innerNode.value("display")->print(), "inline");
}

TEST_F(LayoutTest, FromChildrenDisplayNone) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap1;
    propertyMap1["display"] = CSS::make_value(CSS::TextValue("block"));
    Style::PropertyMap propertyMap2;
    propertyMap2["display"] = CSS::make_value(CSS::TextValue("none"));
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap1),
                                 {Style::StyledNode(DOM::NodePtr(
                                                        new DOM::TextNode("")),
                                                    std::move(propertyMap2))});
    auto              box = Box::from(styledNode, boxDimensions);

    ASSERT_EQ(box->getChildren().size(), 0);
}

TEST_F(LayoutTest, LayoutDisplayBlock) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap;
    propertyMap["display"] = CSS::make_value(CSS::TextValue("block"));
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap));

    ASSERT_TRUE(
        dynamic_cast<StyledBox *>(Box::from(styledNode, boxDimensions).get()));
}

TEST_F(LayoutTest, LayoutDisplayBlock_WidthGTContainer_NotAutoWidth) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap;
    propertyMap["display"] = CSS::make_value(CSS::TextValue("block"));
    propertyMap["width"]   = CSS::make_value(CSS::UnitValue(0, CSS::px));
    propertyMap["margin"]  = CSS::make_value(CSS::TextValue("auto"));
    propertyMap["padding"] = CSS::make_value(CSS::UnitValue(10, CSS::px));
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap));
    auto              box  = Box::from(styledNode, boxDimensions);
    auto              dims = box->getDimensions();

    ASSERT_EQ(dims.width, 0);
    ASSERT_EQ(dims.margin.left, 0);
    ASSERT_EQ(dims.margin.right, -19);
}

TEST_F(LayoutTest, LayoutDisplayBlock_AllConstrained) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap;
    propertyMap["display"] = CSS::make_value(CSS::TextValue("block"));
    propertyMap["width"]   = CSS::make_value(CSS::UnitValue(0, CSS::px));
    propertyMap["margin"]  = CSS::make_value(CSS::UnitValue(0, CSS::px));
    propertyMap["padding"] = CSS::make_value(CSS::UnitValue(0, CSS::px));
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap));
    auto              box  = Box::from(styledNode, boxDimensions);
    auto              dims = box->getDimensions();

    ASSERT_EQ(dims.width, 0);
    ASSERT_EQ(dims.margin.left, 0);
    ASSERT_EQ(dims.margin.right, 1);
}

TEST_F(LayoutTest, LayoutDisplayBlock_AutoMarginRight) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap;
    propertyMap["display"]      = CSS::make_value(CSS::TextValue("block"));
    propertyMap["width"]        = CSS::make_value(CSS::UnitValue(0, CSS::px));
    propertyMap["margin"]       = CSS::make_value(CSS::UnitValue(0, CSS::px));
    propertyMap["margin-right"] = CSS::make_value(CSS::TextValue("auto"));
    propertyMap["padding"]      = CSS::make_value(CSS::UnitValue(0, CSS::px));
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap));
    auto              box  = Box::from(styledNode, boxDimensions);
    auto              dims = box->getDimensions();

    ASSERT_EQ(dims.width, 0);
    ASSERT_EQ(dims.margin.left, 0);
    ASSERT_EQ(dims.margin.right, 1);
}

TEST_F(LayoutTest, LayoutDisplayBlock_AutoMarginLeft) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap;
    propertyMap["display"]     = CSS::make_value(CSS::TextValue("block"));
    propertyMap["width"]       = CSS::make_value(CSS::UnitValue(0, CSS::px));
    propertyMap["margin"]      = CSS::make_value(CSS::UnitValue(0, CSS::px));
    propertyMap["margin-left"] = CSS::make_value(CSS::TextValue("auto"));
    propertyMap["padding"]     = CSS::make_value(CSS::UnitValue(0, CSS::px));
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap));
    auto              box  = Box::from(styledNode, boxDimensions);
    auto              dims = box->getDimensions();

    ASSERT_EQ(dims.width, 0);
    ASSERT_EQ(dims.margin.left, 1);
    ASSERT_EQ(dims.margin.right, 0);
}

TEST_F(LayoutTest, LayoutDisplayBlock_AutoWidth_FitUnderflow) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap;
    propertyMap["display"] = CSS::make_value(CSS::TextValue("block"));
    propertyMap["width"]   = CSS::make_value(CSS::TextValue("auto"));
    propertyMap["margin"]  = CSS::make_value(CSS::TextValue("auto"));
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap));
    auto              box  = Box::from(styledNode, boxDimensions);
    auto              dims = box->getDimensions();

    ASSERT_EQ(dims.width, 1);
    ASSERT_EQ(dims.margin.left, 0);
    ASSERT_EQ(dims.margin.right, 0);
}

TEST_F(LayoutTest, LayoutDisplayBlock_AutoWidth_MarginRightFitUnderflow) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap;
    propertyMap["display"] = CSS::make_value(CSS::TextValue("block"));
    propertyMap["width"]   = CSS::make_value(CSS::TextValue("auto"));
    propertyMap["margin"]  = CSS::make_value(CSS::TextValue("auto"));
    propertyMap["padding"] = CSS::make_value(CSS::UnitValue(10, CSS::px));
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap));
    auto              box  = Box::from(styledNode, boxDimensions);
    auto              dims = box->getDimensions();

    ASSERT_EQ(dims.width, 0);
    ASSERT_EQ(dims.margin.left, 0);
    ASSERT_EQ(dims.margin.right, -19);
}

TEST_F(LayoutTest, LayoutDisplayBlock_AutoLeftMargin_AutoRightMargin) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap;
    propertyMap["display"] = CSS::make_value(CSS::TextValue("block"));
    propertyMap["width"]   = CSS::make_value(CSS::UnitValue(0, CSS::px));
    propertyMap["margin"]  = CSS::make_value(CSS::TextValue("auto"));
    propertyMap["padding"] = CSS::make_value(CSS::UnitValue(0, CSS::px));
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap));
    auto              box  = Box::from(styledNode, boxDimensions);
    auto              dims = box->getDimensions();

    ASSERT_EQ(dims.width, 0);
    ASSERT_EQ(dims.margin.left, 0.5);
    ASSERT_EQ(dims.margin.right, 0.5);
}

TEST_F(LayoutTest, LayoutDisplayInline) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap;
    propertyMap["display"] = CSS::make_value(CSS::TextValue("inline"));
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap));

    ASSERT_TRUE(
        dynamic_cast<StyledBox *>(Box::from(styledNode, boxDimensions).get()));
}

TEST_F(LayoutTest, SetHeightExplicitly) {
    BoxDimensions      boxDimensions(Rectangle(0, 0, 1, 1));
    Style::PropertyMap propertyMap;
    propertyMap["display"] = CSS::make_value(CSS::TextValue("block"));
    propertyMap["height"]  = CSS::make_value(CSS::UnitValue(50, CSS::px));
    Style::StyledNode styledNode(DOM::NodePtr(new DOM::TextNode("")),
                                 std::move(propertyMap));
    auto              box  = Box::from(styledNode, boxDimensions);
    auto              dims = box->getDimensions();

    ASSERT_EQ(dims.height, 50);
}