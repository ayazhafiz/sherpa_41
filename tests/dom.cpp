// sherpa_41's DOM module test fixture, licensed under MIT. (c) hafiz, 2018

#include "dom.hpp"

#include <gtest/gtest.h>

class DOMTest : public ::testing::Test {};

using namespace DOM;

TEST_F(DOMTest, AttributeMap) {
    AttributeMap attributeMap;
    attributeMap.insert("a", "b");
    ASSERT_EQ(attributeMap.print(), R"(a="b")");
}

TEST_F(DOMTest, NodeCtorDtor) {
    CommentNode commentNode("hello comment!");
    TextNode    textNode("hello text!");
    ElementNode elementNode("div");
}