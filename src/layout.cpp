#ifndef LAYOUT_CPP
#define LAYOUT_CPP

#include "layout.hpp"

#include "css.hpp"

Layout::DisplayType Layout::stodisplay(const std::string & s) {
    if (s == "block") {
        return Block;
    }
    if (s == "inline") {
        return Inline;
    } else {
        return None;
    }
}

Layout::DisplayType Layout::snodetodisplay(const Style::StyledNode & node,
                                           const std::string &       deflt) {
    return Layout::stodisplay(
        node.value_or("display", CSS::ValuePtr(new CSS::TextValue(deflt)))
            ->print());
}

/**
 * Creates a rectangle
 * @param startX start x coordinate
 * @param startY start y coordinate
 * @param length rectangle length
 * @param height rectangle height
 */
Layout::Rectangle::Rectangle(double startX,
                             double startY,
                             double length,
                             double height)
    : origin(Coordinates{startX, startY}), length(length), height(height) {
}

/**
 * Creates edge dimensions
 * @param top top edge length
 * @param left left edge length
 * @param bottom bottom edge length
 * @param right right edge length
 */
Layout::Edges::Edges(double top, double left, double bottom, double right)
    : top(top), left(left), bottom(bottom), right(right) {
}

/**
 * Creates box dimensions
 * @param location box location and size
 * @param margin box margins
 * @param padding box padding
 * @param border box borders
 */
Layout::BoxDimensions::BoxDimensions(Layout::Rectangle location,
                                     Layout::Edges     margin,
                                     Layout::Edges     padding,
                                     Layout::Edges     border)
    : location(location), margin(margin), padding(padding), border(border) {
}

/**
 * Returns start coordinates of the box
 * @return origin coordinates of the box
 */
Layout::Coordinates Layout::BoxDimensions::origin() const {
    return location.origin;
}

/**
 * Return length of box
 * @return length
 */
double Layout::BoxDimensions::length() const {
    return location.length;
}

/**
 * Return height of box
 * @return height
 */
double Layout::BoxDimensions::height() const {
    return location.height;
}

/**
 * Creates an abstract Box
 * @param dimensions box dimensions
 * @param children box children
 */
Layout::Box::Box(Layout::BoxDimensions     dimensions,
                 const Layout::BoxVector & children)
    : dimensions(dimensions), children() {
    this->children.reserve(children.size());
    std::for_each(children.begin(), children.end(), [this](const auto & child) {
        this->children.push_back(child->clone());
    });
}

/**
 * Returns box dimensions
 * @return dimensions
 */
Layout::BoxDimensions Layout::Box::getDimensions() const {
    return dimensions;
}

/**
 * Returns box children
 * @return children
 */
Layout::BoxVector Layout::Box::getChildren() const {
    BoxVector boxes;
    std::transform(children.begin(),
                   children.end(),
                   std::back_inserter(boxes),
                   [](const auto & child) { return child->clone(); });
    return boxes;
}

/**
 * Creates a tree of boxes from a styled node root
 * @param root styled node root
 * @return pointer to root of box tree
 */
Layout::BoxPtr Layout::Box::from(const Style::StyledNode & styledRoot) {
    auto display = snodetodisplay(styledRoot);
    if (display == None) {  // display: none; is not included
        return nullptr;
    }

    StyledBox root(BoxDimensions(Rectangle(0, 0, 0, 0)), styledRoot, display);
    auto      children = styledRoot.getChildren();

    for (const auto & child : children) {
        auto cDisp = snodetodisplay(child);
        switch (cDisp) {
            case Block:
                root.children.push_back(Box::from(child));
                break;
            case Inline:
                root.getInlineContainer()->children.push_back(Box::from(child));
                break;
            case None:
            default:
                break;
        }
    }

    return root.clone();
}

/**
 * Creates an anonymous box
 * @param children box children
 * @note anonymous box is not rendered, and so has zero dimension
 */
Layout::AnonymousBox::AnonymousBox(const BoxVector & children)
    : Box(BoxDimensions(Rectangle(0, 0, 0, 0)), children) {
}

/**
 * Clones an anonymous box
 * @return cloned box
 */
Layout::BoxPtr Layout::AnonymousBox::clone() const {
    return BoxPtr(new AnonymousBox(getChildren()));
}

/**
 * Creates a styled box with content
 * @param dimensions box dimensions
 * @param content box content
 * @param display display type
 * @param children box children
 */
Layout::StyledBox::StyledBox(Layout::BoxDimensions dimensions,
                             Style::StyledNode     content,
                             Layout::DisplayType   display,
                             const BoxVector &     children)
    : Box(dimensions, children), content(content), display(display) {
}

/**
 * Clones a styled box
 * @return styled box
 */
Layout::BoxPtr Layout::StyledBox::clone() const {
    return BoxPtr(
        new StyledBox(getDimensions(), content, display, getChildren()));
}

/**
 * Get the box an inline node should go into, or a create a new one
 * @return box to put inline node in
 */
Layout::Box * Layout::StyledBox::getInlineContainer() {
    if (display == Inline) {
        return this;
    } else {  // *this is a block display
        // if there is already an anonymous node to hold inline content, use it
        // otherwise, create a new anonymous node
        Box * last = children.back().get();
        if (dynamic_cast<AnonymousBox *>(last) == nullptr) {
            children.push_back(BoxPtr(new AnonymousBox()));
        }
        return children.back().get();
    }
}

#endif