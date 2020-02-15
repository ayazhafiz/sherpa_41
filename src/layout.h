// sherpa_41's Layout module, licensed under MIT. (c) hafiz, 2018

#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include <vector>

#include "style.h"

/**
 * The Layout module performs computations on a styled node to figure out its
 * location on the browser page, producing a Layout Tree not unlike the Style
 * Tree or DOM Tree that holds positional information on all the nodes in the
 * browser page.
 *
 * The following Layouts are currently supported:
 *  - AnonymousBox: a non-rendered box to hold any number of children
 *  - StyledBox: a box with arbitrary styling of any display type
 */
namespace Layout {

// forward declaration
class Box;
struct Edges;

using BoxPtr = std::unique_ptr<Box>;
using BoxVector = std::vector<BoxPtr>;

/**
 * Block display types
 */
enum DisplayType { Block, Inline, None };
auto stodisplay(const std::string& s) -> DisplayType;
auto snodetodisplay(const Style::StyledNode& node, const std::string& deflt = "inline")
    -> DisplayType;

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
  Rectangle() = delete;

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
  [[nodiscard]] auto expand(const Edges& edge) const -> Rectangle;

  Coordinates origin;
  double width, height;
};

/**
 * Describes width of edges in a block's dimensions
 */
struct Edges {
 public:
  Edges() = delete;

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
  BoxDimensions() = delete;

  /**
   * Creates box dimensions
   * @param location box location and size
   * @param margin box margins
   * @param padding box padding
   * @param border box borders
   */
  explicit BoxDimensions(Rectangle location,
                         Edges margin = Edges(0, 0, 0, 0),
                         Edges padding = Edges(0, 0, 0, 0),
                         Edges border = Edges(0, 0, 0, 0));

  /**
   * Area covered by box and its padding
   * @return area covered
   */
  [[nodiscard]] auto paddingArea() const -> Rectangle;

  /**
   * Area covered by box, padding, and borders
   * @return area covered
   */
  [[nodiscard]] auto borderArea() const -> Rectangle;

  /**
   * Area covered by box, padding, borders, and margins
   * @return area covered
   */
  [[nodiscard]] auto marginArea() const -> Rectangle;

 public:
  Coordinates origin;
  double width, height;
  Edges margin, padding, border;
};

/**
 * An abstract base Box that describes any other layout box in the Layout Tree
 */
class Box {
 public:
  /**
   * Creates an abstract Box
   * @param dimensions box dimensions
   * @param children box children
   */
  Box(BoxDimensions dimensions, const BoxVector& children);

  virtual ~Box() = default;

  /**
   * Returns box dimensions
   * @return dimensions
   */
  [[nodiscard]] auto getDimensions() const -> BoxDimensions;

  /**
   * Returns box children
   * @return children
   */
  [[nodiscard]] auto getChildren() const -> BoxVector;

  /**
   * Clones a box into a unique_ptr
   * @return cloned box
   */
  [[nodiscard]] virtual auto clone() const -> BoxPtr = 0;

  /**
   * Creates a tree of boxes from a styled node root and a browser window
   * @param root styled node root
   * @param window browser window size
   * @return pointer to root of box tree
   */
  static auto from(const Style::StyledNode& root, BoxDimensions window) -> BoxPtr;

  /**
   * Creates a tree of boxes from a styled node root
   * @param root styled node root
   * @return pointer to root of box tree
   */
  static auto from(const Style::StyledNode& root) -> BoxPtr;

 protected:
  BoxDimensions dimensions;
  BoxVector children;
};

/**
 * An anonymous box that is not itself rendered, but serves to contain its
 * children. The primary use case for this is differing display types; for
 * instance, multiple `inline` boxes after a `block` box will go in an anonymous
 * box.
 */
class AnonymousBox : public Box {
 public:
  /**
   * Creates an anonymous box
   * @param children box children
   * @note anonymous box is not rendered, and so has zero dimension
   */
  explicit AnonymousBox(const BoxVector& children = BoxVector());

  ~AnonymousBox() override = default;

  AnonymousBox(const AnonymousBox& rhs) = delete;

  /**
   * Clones an anonymous box
   * @return cloned box
   */
  [[nodiscard]] auto clone() const -> BoxPtr override;
};

/**
 * A box with arbitrary styling defined by a StyledNode, that can have any
 * number of children and any display type.
 */
class StyledBox : public Box {
 public:
  StyledBox() = delete;

  /**
   * Creates a styled box with content
   * @param dimensions box dimensions
   * @param content box content
   * @param display display type
   * @param children box children
   */
  StyledBox(BoxDimensions dimensions,
            const Style::StyledNode& content,
            DisplayType display = Inline,
            const BoxVector& children = BoxVector());

  ~StyledBox() override = default;

  StyledBox(const StyledBox& rhs) = delete;

  /**
   * Clones a styled box
   * @return styled box
   */
  [[nodiscard]] auto clone() const -> BoxPtr override;

  /**
   * Returns content
   * @return content of styled node
   */
  [[nodiscard]] auto getContent() const -> Style::StyledNode;

 private:
  /**
   * Lays out a box and its children
   * @param container parent container dimensions
   */
  void layout(const BoxDimensions& container);

  /**
   * Lays out *this box's children, updating *this box's height
   */
  void layoutChildren();

  /**
   * Lays out a box with block display type and its children
   * @param container parent container dimensions
   */
  void setBlockLayout(const BoxDimensions& container);

  /**
   * Calculates and sets box width based off parent
   * @param container parent container dimensions
   */
  void setWidth(const BoxDimensions& container);

  /**
   * Positions the box within its parent container using widths and parent
   * dimensions
   * @param container parent container dimensions
   */
  void setPosition(const BoxDimensions& container);

  /**
   * Determines explicit height, or calculates height from children if no
   * explicit height is given
   */
  void setHeight();

  /**
   * Get the box an inline node should go into, or a create a new one
   * @return box to put inline node in
   */
  auto getInlineContainer() -> Box*;

  Style::StyledNode content;
  DisplayType display;

  friend Box;
};
}  // namespace Layout

#endif
