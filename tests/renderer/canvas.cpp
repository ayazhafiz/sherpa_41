// sherpa_41's Canvas renderer test fixture, licensed under MIT. (c) hafiz, 2018

#include "renderer/canvas.hpp"

#include "parser/css.hpp"
#include "parser/html.hpp"

#include <gtest/gtest.h>

class CanvasTest : public ::testing::Test {};

TEST_F(CanvasTest, CtorDtor) {
    Canvas canvas(800, 600);
    Canvas canvas2(Layout::BoxPtr(new Layout::AnonymousBox()),
                   Layout::Rectangle(0, 0, 0, 0));
}

TEST_F(CanvasTest, getPixels) {
    Canvas canvas(2, 1);
    ASSERT_EQ(canvas.getPixels(),
              std::vector<uint8_t>({255, 255, 255, 0, 255, 255, 255, 0}));
}

TEST_F(CanvasTest, renderRectangle) {
    Display::RectangleCmd rectangleCmd(Layout::Rectangle(0, 0, 1, 1),
                                       CSS::ColorValue(111, 111, 111, 0.2));
    Canvas                canvas(1, 1);
    canvas.render(rectangleCmd);
    ASSERT_EQ(canvas.getPixels(), std::vector<uint8_t>({111, 111, 111, 51}));
}

TEST_F(CanvasTest, renderFromSource) {
    HTMLParser html("<html></html>");
    CSSParser  css("* { background: #000000; display: block; padding: 1px; }");
    auto       style = Style::StyledNode::from(html.evaluate(), css.evaluate());
    auto       layout = Layout::Box::from(style,
                                    Layout::BoxDimensions(
                                        Layout::Rectangle(0, 0, 1, 1)));
    Canvas     canvas(layout, Layout::Rectangle(0, 0, 1, 1));
    ASSERT_EQ(canvas.getPixels(), std::vector<uint8_t>({0, 0, 0, 255}));
}