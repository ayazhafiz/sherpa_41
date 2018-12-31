// sherpa_41's CSS Parser test fixture, licensed under MIT. (c) hafiz, 2018

#include "parser/css.hpp"

#include "../util.hpp"
#include "visitor/printer.hpp"

#include <gtest/gtest.h>

class CSSParserTest : public ::testing::Test {};

TEST_F(CSSParserTest, TagSelector) {
    CSSParser parser("body {}");
    auto      eval = parser.evaluate();
    ASSERT_PRINT(&eval, R"(
body {
}

)");
}

TEST_F(CSSParserTest, IdSelector) {
    CSSParser parser("#id {}");
    auto      eval = parser.evaluate();
    ASSERT_PRINT(&eval, R"(
#id {
}

)");
}

TEST_F(CSSParserTest, ClassSelector) {
    CSSParser parser(".id {}");
    auto      eval = parser.evaluate();
    ASSERT_PRINT(&eval, R"(
.id {
}

)");
}

TEST_F(CSSParserTest, UniversalSelector) {
    CSSParser parser("* {}");
    auto      eval = parser.evaluate();
    ASSERT_PRINT(&eval, R"(
* {
}

)");
}

TEST_F(CSSParserTest, CombinedSelectors) {
    CSSParser parser("span#solo.class1.class2 {}");
    auto      eval = parser.evaluate();
    ASSERT_PRINT(&eval, R"(
span#solo.class1.class2 {
}

)");

    parser = CSSParser("   span.class3#solo.class4#aSolo#bSolo   {}");
    eval   = parser.evaluate();
    ASSERT_PRINT(&eval, R"(
span#bSolo.class3.class4 {
}

)");
}

TEST_F(CSSParserTest, MultipleSelectors) {
    CSSParser parser("span  , #id.class , \t\n.c.k.b, div#a.c.e {} body {}");
    auto      eval = parser.evaluate();
    ASSERT_PRINT(&eval, R"(
div#a.c.e, #id.class, .c.k.b, span {
}

body {
}

)");
}

TEST_F(CSSParserTest, TextDeclaration) {
    CSSParser parser("body { text-decoration: none; }");
    auto      eval = parser.evaluate();
    ASSERT_PRINT(&eval, R"(
body {
	text-decoration: none;
}

)");
}

TEST_F(CSSParserTest, UnitDeclaration) {
    CSSParser parser(
        "body { font-size:15px;font-size:1.0em;font-size:5.5vh; }");
    auto eval = parser.evaluate();
    ASSERT_PRINT(&eval, R"(
body {
	font-size: 15px;
	font-size: 1em;
	font-size: 5.5vh;
}

)");
}

TEST_F(CSSParserTest, ColorDeclaration) {
    CSSParser parser(
        "body { color:rgba(100,202,97,0.2);color:rgb(55,44,33);} "
        "div.first{ color:#A45D10; }");
    auto eval = parser.evaluate();
    ASSERT_PRINT(&eval, R"(
body {
	color: rgba(100, 202, 97, 0.2);
	color: rgba(55, 44, 33, 1);
}

div.first {
	color: rgba(164, 93, 16, 1);
}

)");
}

TEST_F(CSSParserTest, WhitespaceAndComments) {
    CSSParser parser(R"(
body     , // this is a body tag
	.span#id 	{
// this is a comment
		font-size // another comment
:				15px // comments everywhere!
; // nothing after here parsed: color: red;
color:     rgba(  0, 10 , // a useless comment, even
20  , .55);
    }
)");
    auto      eval = parser.evaluate();
    ASSERT_PRINT(&eval, R"(
#id.span, body {
	font-size: 15px;
	color: rgba(0, 10, 20, 0.55);
}

)");
}