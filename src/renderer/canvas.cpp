// sherpa_41's Canvas renderer, licensed under MIT. (c) hafiz, 2018

#ifndef RENDERER_CANVAS_CPP
#define RENDERER_CANVAS_CPP

#include "renderer/canvas.hpp"

/**
 * Creates blank canvas of a width and height
 * @param width canvas width
 * @param height canvas height
 */
Canvas::Canvas(uint64_t width, uint64_t height)
    : width(width),
      height(height),
      pixels(PxVector(width * height, RGBA(1, 1, 1, 0))) {
}

/**
 * Creates a canvas from a root box and a specified frame width/height
 * @param root box to start drawing from
 * @param frame width and height to draw in
 */
Canvas::Canvas(const Layout::BoxPtr & root, const Layout::Rectangle & frame)
    : width(static_cast<uint64_t>(frame.width)),
      height(static_cast<uint64_t>(frame.height)),
      pixels(PxVector(width * height, RGBA(1, 1, 1, 0))) {
    auto cmds = Display::Command::createQueue(root);
    while (!cmds.empty()) {
        cmds.front()->acceptRenderer(*this);
        cmds.pop();
    }
}

/**
 * Renders a Rectangle Command
 * @param cmd command to render
 */
void Canvas::render(const Display::RectangleCmd & cmd) {
    const auto rect  = cmd.getRectangle();
    const auto color = RGBA(cmd.getColor());

    // set rectangle edges, bounded to canvas
    const auto x0 = toPx(rect.origin.x, 0, width);
    const auto y0 = toPx(rect.origin.y, 0, height);
    const auto x1 = toPx(rect.origin.x + rect.width, 0, width);
    const auto y1 = toPx(rect.origin.y + rect.height, 0, height);

    // color rectangle pixels accordingly
    for (uint64_t y = y0; y < y1; ++y) {
        for (uint64_t x = x0; x < x1; ++x) {
            setPixel(x + y * width, color);
        }
    }
}

/**
 * Returns vector of RGBA pixels representing the Canvas
 * @return pixels
 */
std::vector<uint8_t> Canvas::getPixels() const {
    std::vector<uint8_t> rawPixels;
    rawPixels.reserve(width * height * 4);
    std::for_each(pixels.begin(),
                  pixels.end(),
                  [&rawPixels](const RGBA & pixel) {
                      for (auto channel : pixel.channels()) {
                          rawPixels.push_back(
                              static_cast<uint8_t>(channel * 255));
                      }
                  });
    return rawPixels;
}

Canvas::RGBA::RGBA(double r, double g, double b, double a)
    : r(r), g(g), b(b), a(a) {
}

/**
 * Creates an RGBA from a ColorValue
 * @param color ColorValue to convert
 */
Canvas::RGBA::RGBA(const CSS::ColorValue & color)
    : r(color.r / 255.), g(color.g / 255.), b(color.b / 255.), a(color.a) {
}

/**
 * Returns an array of RGBA channels
 * @return RGBA color channels
 */
std::array<double, 4> Canvas::RGBA::channels() const {
    return {{r, g, b, a}};
}

/**
 * Sets a pixel by blending a color with the background
 * @param location pixel to color
 * @param fg foreground color to apply to pixel
 */
void Canvas::setPixel(uint64_t location, const RGBA & fg) {
    RGBA stacked(0, 0, 0, 0);
    RGBA bg(pixels[location]);

    // alpha channel blending
    stacked.a = 1 - (1 - bg.a) * (1 - fg.a);
    if (stacked.a != 0) {
        stacked.r = (bg.r * bg.a * (1 - fg.a) / stacked.a) +
                    (fg.r * fg.a / stacked.a);
        stacked.g = (bg.g * bg.a * (1 - fg.a) / stacked.a) +
                    (fg.g * fg.a / stacked.a);
        stacked.b = (bg.b * bg.a * (1 - fg.a) / stacked.a) +
                    (fg.b * fg.a / stacked.a);
    }

    pixels[location] = stacked;
}

/**
 * Converts a start location to a pixel position on the canvas
 * @param x location to convert
 * @param min min bound
 * @param max max bound
 * @return converted location, bounded by canvas size
 */
uint64_t Canvas::toPx(double x, uint64_t min, uint64_t max) {
    return std::min(max, std::max(min, static_cast<uint64_t>(x)));
}

#endif