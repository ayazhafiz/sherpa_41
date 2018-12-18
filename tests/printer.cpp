#include "printer.hpp"

#include "dom.hpp"

#include "./util.hpp"

#include <gtest/gtest.h>

// The fixture for testing the vector class
class PrinterTest : public ::testing::Test {};

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
