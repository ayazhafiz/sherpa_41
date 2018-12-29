#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include "style.hpp"

#include <vector>

namespace Layout {

// forward declaration
class Box;
class Edges;

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
     * @param width rectangle width
     * @param height rectangle height
     */
    Rectangle(double startX, double startY, double width, double height);

    /**
     * Expands a rectangle by some edges
     * @param edge edges to expand by
     * @return expanded rectangle
     */
    Rectangle expand(const Edges & edge) const;

    Coordinates origin;
    double      width, height;
};

/**
 * Describes width of edges in a block's dimensions
 */
struct Edges {
   public:
    /**
     * Creates edge dimensions
     * @param top top edge width
     * @param left left edge width
     * @param bottom bottom edge width
     * @param right right edge width
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
     * Area covered by box and its padding
     * @return area covered
     */
    Rectangle paddingArea();

    /**
     * Area covered by box, padding, and borders
     * @return area covered
     */
    Rectangle borderArea();

    /**
     * Area covered by box, padding, borders, and margins
     * @return area covered
     */
    Rectangle marginArea();

   public:
    Coordinates origin;
    double      width, height;
    Edges       margin, padding, border;
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

   protected:
    BoxDimensions dimensions;
    BoxVector     children;
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
     * Lays out a box and its children
     * @param container parent container dimensions
     */
    void layout(const BoxDimensions & container);

    /**
     * Lays out *this box's children, updating *this box's height
     */
    void layoutChildren();

    /**
     * Lays out a box with block display type and its children
     * @param container parent container dimensions
     */
    void setBlockLayout(const BoxDimensions & container);

    /**
     * Calculates and sets box width based off parent
     * @param container parent container dimensions
     */
    void setWidth(const BoxDimensions & container);

    /**
     * Positions the box within its parent container using widths and parent
     * dimensions
     * @param container parent container dimensions
     */
    void setPosition(const BoxDimensions & container);

    /**
     * Determines explicit height, or calculates height from children if no
     * explicit height is given
     */
    void setHeight();

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