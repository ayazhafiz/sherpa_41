// sherpa_41's Printer test fixture, licensed under MIT. (c) hafiz, 2018

#include "visitor/printer.hpp"

#include "css.hpp"
#include "dom.hpp"

#include "../util.hpp"

#include <gtest/gtest.h>

class PrinterTest : public ::testing::Test {};

TEST_F(PrinterTest, CtorDtor) {
    Printer printer;
}

TEST_F(PrinterTest, RootTag) {
    using namespace DOM;
    auto dom = NodePtr(new ElementNode("html"));

    ASSERT_PRINT(dom, R"(
<html>
</html>
)");
}

TEST_F(PrinterTest, Attributes) {
    using namespace DOM;
    AttributeMap attr;
    attr.insert("style", "font-size: 15px;");
    attr.insert("lang", "en");
    auto dom = NodePtr(new ElementNode("html", attr));

    ASSERT_PRINT(dom, R"(
<html style="font-size: 15px;" lang="en">
</html>
)");
}

TEST_F(PrinterTest, Text) {
    using namespace DOM;
    auto text = NodePtr(new TextNode("Hello World"));

    ASSERT_PRINT(text, R"(
Hello World
)");
}

TEST_F(PrinterTest, Comment) {
    using namespace DOM;
    auto text = NodePtr(new CommentNode("Some comment"));

    ASSERT_PRINT(text, R"(
<!-- Some comment -->
)");
}

TEST_F(PrinterTest, NestedElements) {
    using namespace DOM;
    AttributeMap bodyAttr;
    bodyAttr.insert("style", "font-size: 15px;");
    bodyAttr.insert("lang", "en");
    AttributeMap pAttr;
    pAttr.insert("id", "intro");
    pAttr.insert("class", "full-width color-primary");

    NodeVector pChildren;
    pChildren.push_back(NodePtr(new TextNode("This is an intro paragraph.")));

    NodeVector bodyChildren;
    bodyChildren.push_back(NodePtr(new TextNode("Welcome to my website!")));
    bodyChildren.push_back(NodePtr(new ElementNode("p", pAttr, pChildren)));

    auto body = NodePtr(new ElementNode("body", bodyAttr, bodyChildren));

    ASSERT_PRINT(body, R"(
<body style="font-size: 15px;" lang="en">
	Welcome to my website!
	<p id="intro" class="full-width color-primary">
		This is an intro paragraph.
	</p>
</body>
)");
}

TEST_F(PrinterTest, CSSRule) {
    using namespace CSS;
    std::vector<Declaration> decls;
    decls.emplace_back(
        Declaration("font-size", ValuePtr(new UnitValue(15.4, px))));
    decls.emplace_back(
        Declaration("text-decoration", ValuePtr(new TextValue("none"))));
    decls.emplace_back(
        Declaration("color", ValuePtr(new ColorValue(155, 202, 187, 92))));
    Rule       rule({Selector("span", "myId", {"class1", "class2"}),
               Selector("a"),
               Selector("", "id"),
               Selector("", "", {"klass"})},
              std::move(decls));
    StyleSheet ss;
    ss.emplace_back(std::move(rule));

    ASSERT_PRINT(&ss, R"(
span#myId.class1.class2, #id, .klass, a {
	font-size: 15.4px;
	text-decoration: none;
	color: rgba(155, 202, 187, 92);
}

)");
}

TEST_F(PrinterTest, CSSRules) {
    using namespace CSS;
    std::vector<Declaration> decls1;
    decls1.emplace_back(
        Declaration("font-size", ValuePtr(new UnitValue(15.4, px))));
    decls1.emplace_back(
        Declaration("text-decoration", ValuePtr(new TextValue("none"))));
    decls1.emplace_back(
        Declaration("color", ValuePtr(new ColorValue(155, 202, 187, 92))));
    Rule rule1({Selector("span", "myId", {"class1", "class2"}),
                Selector("a"),
                Selector("", "id"),
                Selector("", "", {"klass"})},
               std::move(decls1));
    std::vector<Declaration> decls2;
    decls2.emplace_back(
        Declaration("font-size", ValuePtr(new UnitValue(15.4, px))));
    decls2.emplace_back(
        Declaration("text-decoration", ValuePtr(new TextValue("none"))));
    decls2.emplace_back(
        Declaration("color", ValuePtr(new ColorValue(155, 202, 187, 92))));
    Rule       rule2({Selector("span", "myId", {"class1", "class2"}),
                Selector("a"),
                Selector("", "id"),
                Selector("", "", {"klass"})},
               std::move(decls2));
    StyleSheet ss;
    ss.emplace_back(std::move(rule1));
    ss.emplace_back(std::move(rule2));

    ASSERT_PRINT(&ss, R"(
span#myId.class1.class2, #id, .klass, a {
	font-size: 15.4px;
	text-decoration: none;
	color: rgba(155, 202, 187, 92);
}

span#myId.class1.class2, #id, .klass, a {
	font-size: 15.4px;
	text-decoration: none;
	color: rgba(155, 202, 187, 92);
}

)");
}
