// sherpa_41's Canvas renderer, licensed under MIT. (c) hafiz, 2018

#ifndef RENDERER_CANVAS_HPP
#define RENDERER_CANVAS_HPP

#include "display.hpp"

#include "renderer/renderer.hpp"

/**
 * The Canvas is a rendering scheme designed for image rasterization,
 * particularly into PNG or JPG formats. It renders a Layout tree hierarchically
 * into an array of pixels, each pixel composed of an RGBA color value.
 */
class Canvas : public Renderer {
   public:
    typedef std::vector<CSS::ColorValue> PxVector;

    /**
     * Creates blank canvas of a width and height
     * @param width canvas width
     * @param height canvas height
     */
    Canvas(uint64_t width, uint64_t height);

    /**
     * Creates a canvas from a root box and a specified frame width/height
     * @param root box to start drawing from
     * @param frame width and height to draw in
     */
    Canvas(const Layout::BoxPtr & root, const Layout::Rectangle & frame);

    /**
     * Default dtor
     */
    ~Canvas() override = default;

    /**
     * Renders a Rectangle Command
     * @param cmd command to paint
     */
    void render(const Display::RectangleCmd & cmd) override;

    /**
     * Returns vector of RGBA pixels representing the Canvas
     * @return pixels
     */
    std::vector<uint8_t> getPixels() const;

   private:
    /**
     * A ColorValue-like struct that only deals with channels in [0, 1] for
     * blending purposes.
     */
    struct RGBA {
        RGBA() = default;

        /**
         * Creates an RGBA from a ColorValue
         * @param color ColorValue to convert
         */
        explicit RGBA(const CSS::ColorValue & color);

        /**
         * Converts an RGBA into a ColorValue
         * @return ColorValue
         */
        CSS::ColorValue toColorValue() const;

        double r, g, b, a;
    };

    /**
     * Sets a pixel by blending a color with the background
     * @param location pixel to color
     * @param fg foreground color to apply to pixel
     */
    void setPixel(uint64_t location, RGBA fg);

    /**
     * Converts a start location to a pixel position on the canvas
     * @param x location to convert
     * @param min min bound
     * @param max max bound
     * @return converted location, bounded by canvas size
     */
    uint64_t toPx(double x, uint64_t min, uint64_t max);

    uint64_t width, height;
    PxVector pixels;
};

#endif