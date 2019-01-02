// sherpa_41's Style module test fixture, licensed under MIT. (c) hafiz, 2018

#include "style.hpp"

#include "parser/css.hpp"
#include "parser/html.hpp"

#include <gtest/gtest.h>

class StyleTest : public ::testing::Test {};

using namespace Style;

TEST_F(StyleTest, StyledNodeCtorDtor) {
    StyledNode                  styledNode(nullptr);
    std::unique_ptr<StyledNode> snuptr(new StyledNode(nullptr));
    StyledNode                  styledNode2(DOM::NodePtr(new DOM::TextNode("")),
                           PropertyMap(),
                           StyledNodeVector());
    StyledNode *                styledNode3 = new StyledNode(DOM::NodePtr(
                                                  new DOM::TextNode("")),
                                              PropertyMap(),
                                              StyledNodeVector());

    ASSERT_NO_THROW(delete styledNode3);
}

TEST_F(StyleTest, value) {
    CSSParser  css("html {font-size:15px;}");
    HTMLParser html("<html></html>");
    auto       root = StyledNode::from(html.evaluate(), css.evaluate());
    ASSERT_EQ(root.value("font-size")->print(), "15px");
    ASSERT_EQ(root.value("font-size", "other-size", "rah")->print(), "15px");
    ASSERT_EQ(root.value("font-size", "font-size")->print(), "15px");
    ASSERT_EQ(root.value("other-size", "font-size", "rah")->print(), "15px");
    ASSERT_EQ(root.value("other-size", "another-size"), nullptr);
    ASSERT_EQ(root.value_or("font-size", CSS::TextValue("NO VALUE"))->print(),
              "15px");
    ASSERT_EQ(root.value_or<std::string>("other-size",
                                         "font-size",
                                         CSS::TextValue("NO VALUE"))
                  ->print(),
              "15px");
    ASSERT_EQ(root.value_or_zero("other-size", "another-size")->print(), "0px");
}

TEST_F(StyleTest, OneSelector) {
    CSSParser  css("html {font-size:15px;color:red;color:#e5e5e5;}");
    HTMLParser html("<html></html>");
    auto       root = StyledNode::from(html.evaluate(), css.evaluate());
    ASSERT_EQ(root.value("font-size")->print(), "15px");
    ASSERT_EQ(root.value("color")->print(), "rgba(229, 229, 229, 1)");
}

TEST_F(StyleTest, WorksWithSelectors) {
    CSSParser  css(R"(
html#id.class1.class2{font-size:15px;}
.class1{color:red;}
.class2{background:green;}
#id{text-decoration:none;}
html{display:block;}
*{font-style:normal;}
)");
    HTMLParser html(R"(
<html id="id" class="class1 class2"></html>
)");

    auto root = StyledNode::from(html.evaluate(), css.evaluate());

    ASSERT_EQ(root.value("font-size")->print(), "15px");
    ASSERT_EQ(root.value("color")->print(), "red");
    ASSERT_EQ(root.value("background")->print(), "green");
    ASSERT_EQ(root.value("text-decoration")->print(), "none");
    ASSERT_EQ(root.value("display")->print(), "block");
    ASSERT_EQ(root.value("font-style")->print(), "normal");
}

TEST_F(StyleTest, SpecificityOverload) {
    CSSParser  css(R"(
.c1.c2.c3{color:black;}
#id.c1.c2{color:red;}
#id.c1{color:green;font-size:1px;}
html#id{font-size:10px;display:block;}
html.c1{display:inline;text-decoration:none;}
html{text-decoration:reset;}
)");
    HTMLParser html(R"(
<html id="id" class="c1 c2"></html>
)");

    auto root = StyledNode::from(html.evaluate(), css.evaluate());

    ASSERT_EQ(root.value("color")->print(), "red");
    ASSERT_EQ(root.value("font-size")->print(), "1px");
    ASSERT_EQ(root.value("display")->print(), "block");
    ASSERT_EQ(root.value("text-decoration")->print(), "none");
}

TEST_F(StyleTest, UselessRules) {
    CSSParser  css("html#id.c1.c2{color:red;}");
    HTMLParser html("<html></html>");

    auto root = StyledNode::from(html.evaluate(), css.evaluate());

    ASSERT_EQ(root.value_or("color", CSS::TextValue("NO VALUE"))->print(),
              "NO VALUE");
}

TEST_F(StyleTest, NestedNodes) {
    CSSParser  css("html{color:red;}span{color:green;}div{color:blue;}");
    HTMLParser html("<html><span></span><div></div></html>");

    auto root     = StyledNode::from(html.evaluate(), css.evaluate());
    auto children = root.getChildren();

    ASSERT_EQ(root.value("color")->print(), "red");
    ASSERT_EQ(children[0].value("color")->print(), "green");
    ASSERT_EQ(children[1].value("color")->print(), "blue");
}

TEST_F(StyleTest, NonElementNodes) {
    CSSParser  css("*{color:red;}");
    HTMLParser html(R"(
<html>
	<!-- comment! -->
    text!
</html>
)");

    auto root     = StyledNode::from(html.evaluate(), css.evaluate());
    auto children = root.getChildren();

    ASSERT_EQ(root.value("color")->print(), "red");
    ASSERT_EQ(children[0].value("color"), nullptr);
    ASSERT_EQ(children[1].value("color"), nullptr);
}
