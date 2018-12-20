#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include "style.hpp"

#include <vector>

namespace Layout {

// forward declaration
class Box;

typedef std::unique_ptr<Box> BoxPtr;
typedef std::vector<BoxPtr>  BoxVector;

/**
 * Block display types
 */
enum DisplayType { Block, Inline, None };
DisplayType stodisplay(const std::string & s);
DisplayType snodetodisplay(const Style::StyledNode & node,
                           const std::string &       deflt = "inline");

/**
 * x, y coordinates on a 2D plane
 */
struct Coordinates {
    double x, y;
};

/**
 * Describes a rectangle in a block's dimensions
 */
struct Rectangle {
   public:
    /**
     * Creates a rectangle
     * @param startX start x coordinate
     * @param startY start y coordinate
     * @param length rectangle length
     * @param height rectangle height
     */
    Rectangle(double startX, double startY, double length, double height);

    Coordinates origin;
    double      length, height;
};

/**
 * Describes lengths of edges in a block's dimensions
 */
struct Edges {
   public:
    /**
     * Creates edge dimensions
     * @param top top edge length
     * @param left left edge length
     * @param bottom bottom edge length
     * @param right right edge length
     */
    Edges(double top, double left, double bottom, double right);

    double top, left, bottom, right;
};

/**
 * Coordinates and dimensions of a box and its edges
 */
struct BoxDimensions {
   public:
    /**
     * Creates box dimensions
     * @param location box location and size
     * @param margin box margins
     * @param padding box padding
     * @param border box borders
     */
    explicit BoxDimensions(Rectangle location,
                           Edges     margin  = Edges(0, 0, 0, 0),
                           Edges     padding = Edges(0, 0, 0, 0),
                           Edges     border  = Edges(0, 0, 0, 0));

    /**
     * Returns start coordinates of the box
     * @return origin coordinates of the box
     */
    Coordinates origin() const;

    /**
     * Return length of box
     * @return length
     */
    double length() const;

    /**
     * Return height of box
     * @return height
     */
    double height() const;

   private:
    Rectangle location;

   public:
    Edges margin, padding, border;
};

class Box {
   public:
    /**
     * Creates an abstract Box
     * @param dimensions box dimensions
     * @param children box children
     */
    Box(BoxDimensions dimensions, const BoxVector & children);

    virtual ~Box() = default;

    /**
     * Returns box dimensions
     * @return dimensions
     */
    BoxDimensions getDimensions() const;

    /**
     * Returns box children
     * @return children
     */
    BoxVector getChildren() const;

    /**
     * Clones a box into a unique_ptr
     * @return cloned box
     */
    virtual BoxPtr clone() const = 0;

    /**
     * Creates a tree of boxes from a styled node root
     * @param root styled node root
     * @return pointer to root of box tree
     */
    static BoxPtr from(const Style::StyledNode & root);

   private:
    BoxDimensions dimensions;

   protected:
    BoxVector children;
};

class AnonymousBox : public Box {
   public:
    /**
     * Creates an anonymous box
     * @param children box children
     * @note anonymous box is not rendered, and so has zero dimension
     */
    explicit AnonymousBox(const BoxVector & children = BoxVector());

    ~AnonymousBox() override = default;

    AnonymousBox(const AnonymousBox & rhs) = delete;

    /**
     * Clones an anonymous box
     * @return cloned box
     */
    BoxPtr clone() const override;
};

class StyledBox : public Box {
   public:
    /**
     * Creates a styled box with content
     * @param dimensions box dimensions
     * @param content box content
     * @param display display type
     * @param children box children
     */
    StyledBox(BoxDimensions     dimensions,
              Style::StyledNode content,
              DisplayType       display  = Inline,
              const BoxVector & children = BoxVector());

    ~StyledBox() override = default;

    StyledBox(const StyledBox & rhs) = delete;

    /**
     * Clones a styled box
     * @return styled box
     */
    BoxPtr clone() const override;

   private:
    /**
     * Get the box an inline node should go into, or a create a new one
     * @return box to put inline node in
     */
    Box * getInlineContainer();

    Style::StyledNode content;
    DisplayType       display;

    friend Box;
};
}  // namespace Layout

#endif