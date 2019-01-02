// sherpa_41's Layout module, licensed under MIT. (c) hafiz, 2018

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
        node.value_or("display", CSS::TextValue(deflt))->print());
}

/**
 * Creates a rectangle
 * @param startX start x coordinate
 * @param startY start y coordinate
 * @param width rectangle width
 * @param height rectangle height
 */
Layout::Rectangle::Rectangle(double startX,
                             double startY,
                             double width,
                             double height)
    : origin(Coordinates{startX, startY}), width(width), height(height) {
}

/**
 * Expands a rectangle by some edges
 * @param edge edges to expand by
 * @return expanded rectangle
 */
Layout::Rectangle Layout::Rectangle::expand(const Layout::Edges & edge) const {
    return Rectangle(origin.x - edge.left,
                     origin.y - edge.top,
                     width + edge.left + edge.right,
                     height + edge.top + edge.bottom);
}

/**
 * Creates edge dimensions
 * @param top top edge width
 * @param left left edge width
 * @param bottom bottom edge width
 * @param right right edge width
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
    : origin(location.origin),
      width(location.width),
      height(location.height),
      margin(margin),
      padding(padding),
      border(border) {
}

/**
 * Area covered by box and its padding
 * @return area covered
 */
Layout::Rectangle Layout::BoxDimensions::paddingArea() const {
    return Rectangle(origin.x, origin.y, width, height).expand(padding);
}

/**
 * Area covered by box, padding, and borders
 * @return area covered
 */
Layout::Rectangle Layout::BoxDimensions::borderArea() const {
    return paddingArea().expand(border);
}

/**
 * Area covered by box, padding, borders, and margins
 * @return area covered
 */
Layout::Rectangle Layout::BoxDimensions::marginArea() const {
    return borderArea().expand(margin);
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
 * Creates a tree of boxes from a styled node root and a browser window
 * @param root styled node root
 * @param window browser window size
 * @return pointer to root of box tree
 */
Layout::BoxPtr Layout::Box::from(const Style::StyledNode & root,
                                 Layout::BoxDimensions     window) {
    // layout algorithm assumes height is initially zero
    // TODO: Store window height for vmin/vmax/% values
    window.height = 0;

    auto rootBox = from(root);
    if (auto sRoot = dynamic_cast<StyledBox *>(rootBox.get())) {
        sRoot->layout(window);
    }
    return rootBox;
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
 * Returns content
 * @return content of styled node
 */
Style::StyledNode Layout::StyledBox::getContent() const {
    return content;
}

/**
 * Lays out a block and its children
 * @param container parent container dimensions
 */
void Layout::StyledBox::layout(const Layout::BoxDimensions & container) {
    switch (display) {
        case Block:
            setBlockLayout(container);
            break;
        case Inline:
        case None:
        default:
            break;
    }
}

void Layout::StyledBox::layoutChildren() {
    std::for_each(children.begin(), children.end(), [this](BoxPtr & child) {
        if (auto styledChild = dynamic_cast<StyledBox *>(child.get())) {
            styledChild->layout(dimensions);

            // parent height must be updated after each child is laid out so
            // block children are stacked below each other
            dimensions.height += child->getDimensions().marginArea().height;
        }
    });
}

/**
 * Lays out a box with block display type and its children
 * @param container parent container dimensions
 */
void Layout::StyledBox::setBlockLayout(
    const Layout::BoxDimensions & container) {
    setWidth(container);     // determine width from parent
    setPosition(container);  // determine position inside parent
    layoutChildren();
    setHeight();  // height depends on children
}

/**
 * Calculates and sets box width based off parent
 * @param container parent container dimensions
 */
void Layout::StyledBox::setWidth(const Layout::BoxDimensions & container) {
    auto width = content.value_or("width", CSS::TextValue("auto"));

    auto marginLeft   = content.value_or_zero("margin-left", "margin");
    auto marginRight  = content.value_or_zero("margin-right", "margin");
    auto paddingLeft  = content.value_or_zero("padding-left", "padding");
    auto paddingRight = content.value_or_zero("padding-right", "padding");
    auto borderLeft   = content.value_or_zero("border-left-width",
                                            "border-width");
    auto borderRight  = content.value_or_zero("border-right-width",
                                             "border-width");

    double totalWidth(0);
    for (const auto & dim : {&width,
                             &marginLeft,
                             &marginRight,
                             &paddingLeft,
                             &paddingRight,
                             &borderLeft,
                             &borderRight}) {
        totalWidth += (*dim)->unitValue();
    }

    auto zero = CSS::make_value(CSS::UnitValue(0, CSS::px));

    // if box is too big and width is not auto, zero the margins
    if (totalWidth > container.width && !width->is("auto")) {
        if (marginLeft->is("auto")) {
            marginLeft = zero->clone();
        }
        if (marginRight->is("auto")) {
            marginRight = zero->clone();
        }
    }

    // calculate box underflow
    double underflow = container.width - totalWidth;
    bool   autoW = width->is("auto"), autoML = marginLeft->is("auto"),
         autoMR = marginRight->is("auto");

    // Eliminate under/overflow by adjusting expandable (auto) dimensions
    if (!autoW && !autoML && !autoMR) {  // all dimensions constrained, update
                                         // right margin
        marginRight = CSS::make_value(
            CSS::UnitValue(marginRight->unitValue() + underflow, CSS::px));
    } else if (!autoW && !autoML) {  // only right margin adjustable
        marginRight = CSS::make_value(CSS::UnitValue(underflow, CSS::px));
    } else if (!autoW && !autoMR) {  // only left margin adjustable
        marginLeft = CSS::make_value(CSS::UnitValue(underflow, CSS::px));
    } else if (autoW) {  // width is auto, zero other auto dimensions
        if (autoML) {
            marginLeft = zero->clone();
        }
        if (autoMR) {
            marginRight = zero->clone();
        }

        if (underflow >= 0) {  // set width to fit underflow
            width = CSS::make_value(CSS::UnitValue(underflow, CSS::px));
        } else {  // with overflow, adjust right margin
            width       = zero->clone();
            marginRight = CSS::make_value(
                CSS::UnitValue(marginRight->unitValue() + underflow, CSS::px));
        }
    } else {  // only margins are adjustable, make them evenly split underflow
        marginLeft  = CSS::make_value(CSS::UnitValue(underflow / 2, CSS::px));
        marginRight = CSS::make_value(CSS::UnitValue(underflow / 2, CSS::px));
    }

    // store computed values
    dimensions.width         = width->unitValue();
    dimensions.margin.left   = marginLeft->unitValue();
    dimensions.margin.right  = marginRight->unitValue();
    dimensions.padding.left  = paddingLeft->unitValue();
    dimensions.padding.right = paddingRight->unitValue();
    dimensions.border.left   = borderLeft->unitValue();
    dimensions.border.right  = borderRight->unitValue();
}

/**
 * Positions the box within its parent container using widths and parent
 * dimensions
 * @param container parent container dimensions
 */
void Layout::StyledBox::setPosition(const Layout::BoxDimensions & container) {
    const auto & c = content;
    auto &       d = dimensions;

    // transfer styles
    d.margin.top     = c.value_or_zero("margin-top", "margin")->unitValue();
    d.margin.bottom  = c.value_or_zero("margin-bottom", "margin")->unitValue();
    d.padding.top    = c.value_or_zero("padding-top", "padding")->unitValue();
    d.padding.bottom = c.value_or_zero("padding-bottom", "padding")
                           ->unitValue();
    d.border.top = c.value_or_zero("border-top-width", "border-width")
                       ->unitValue();
    d.border.bottom = c.value_or_zero("border-bottom-width", "border-width")
                          ->unitValue();

    // set x-start coordinate
    d.origin.x = container.origin.x + d.margin.left + d.padding.left +
                 d.border.left;

    // set y-start coordinate by placing it below all existing blocks in the
    // container (block positioning)
    d.origin.y = container.height + container.origin.y + d.margin.top +
                 d.padding.top + d.border.top;
}

/**
 * Determines explicit height, or calculates height from children if no
 * explicit height is given
 */
void Layout::StyledBox::setHeight() {
    if (auto height = dynamic_cast<CSS::UnitValue *>(
            content.value("height").get())) {
        dimensions.height = height->value;
    }
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
        Box * last = children.empty() ? nullptr : children.back().get();
        if (dynamic_cast<AnonymousBox *>(last) == nullptr) {
            children.push_back(BoxPtr(new AnonymousBox()));
        }
        return children.back().get();
    }
}

#endif