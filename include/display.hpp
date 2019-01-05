// sherpa_41's Display module, licensed under MIT. (c) hafiz, 2018

#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "css.hpp"
#include "layout.hpp"

#include <memory>
#include <queue>

class Renderer;

/**
 * The Display module issues commands for rendering various boxes with styled
 * elements in the browser. This separates the concern of _what_ to render with
 * _how_ to render it, with each command having the ability to accept any kind
 * of renderer.
 *
 * The following rendering commands are supported:
 *  - RectangleCmd: a rectangle of a solid color
 */
namespace Display {
// forward declaration
class Command;

using CommandPtr   = std::unique_ptr<Command>;
using CommandQueue = std::queue<CommandPtr>;

/**
 * An abstract class describing a display command
 */
class Command {
   public:
    /**
     * Pure virtual dtor
     */
    virtual ~Command() = 0;

    /**
     * Accepts a renderer to the command
     * @param renderer accepted renderer
     */
    virtual void acceptRenderer(Renderer & renderer) const = 0;

    /**
     * Creates a queue of display commands to execute
     * @param root root layout node
     * @return queue of commands
     */
    static CommandQueue createQueue(const Layout::BoxPtr & root);

   private:
    /**
     * Creates the commands to render a box
     * @param box box to render
     * @param queue queue to add commands to
     */
    static void renderBox(const Layout::BoxPtr & box, CommandQueue & queue);

    /**
     * Creates the commands to render the background of a box
     * @param box box to render background of
     * @param queue queue to add commands to
     */
    static void renderBackground(const Layout::BoxPtr & box,
                                 CommandQueue &         queue);

    /**
     * Creates the commands to render the borders of a box
     * @param box box to render borders of
     * @param queue queue to add commands to
     */
    static void renderBorders(const Layout::BoxPtr & box, CommandQueue & queue);

    /**
     * Gets the color value of a style from a box, or nullptr if no color is
     * specified for that style
     * @tparam Args variadic arguments, should be strings
     * @param box box to get style from
     * @param style style to look up
     * @param backup backup styles to look up
     * @return color value, or nullptr if it does not exist
     */
    template <typename... Args>
    static CSS::ValuePtr getColor(const Layout::BoxPtr & box,
                                  const std::string &    style,
                                  const Args &... backup);
};

/**
 * A command to render a rectangle
 */
class RectangleCmd : public Command {
   public:
    /**
     * Command to create a rectangle of a color
     * @param rectangle rectangle to create
     * @param color color to color rectangle
     */
    RectangleCmd(const Layout::Rectangle & rectangle,
                 const CSS::ColorValue &   color);

    /**
     * Accepts a renderer to the command
     * @param renderer accepted renderer
     */
    void acceptRenderer(Renderer & renderer) const override;

    /**
     * Returns encompassing rectangle
     * @return rectangle
     */
    Layout::Rectangle getRectangle() const;

    /**
     * Returns color
     * @return color
     */
    CSS::ColorValue getColor() const;

   private:
    Layout::Rectangle rectangle;
    CSS::ColorValue   color;
};
}  // namespace Display

#endif