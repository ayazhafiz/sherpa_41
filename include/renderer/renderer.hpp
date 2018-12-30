// sherpa_41's Base Renderer, licensed under MIT. (c) hafiz, 2018

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "display.hpp"

/**
 * A renderer interface for specific rendering applications to extend, currently
 * for the Canvas.
 *
 * Allows for rendering various display commands through visitation.
 */
class Renderer {
   public:
    virtual ~Renderer() = default;

    virtual void render(const Display::RectangleCmd &) = 0;
};

#endif  // VISITOR_HPP