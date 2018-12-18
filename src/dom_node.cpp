#ifndef DOM_NODE_CPP
#define DOM_NODE_CPP

#include "dom.hpp"
#include "visitor.hpp"

/**
 * Creates a DOM Node
 * @param tag node tag name
 */
DOM::Node::Node(const std::string & tag) : tag(tag) {
}

/**
 * Pure virtual destructor prevents unanticipated instantiation
 */
DOM::Node::~Node() = default;

/**
 * Determines whether the Node is of specified type
 * @param tag tag to match
 * @return whether Node is of `tag` type
 */
bool DOM::Node::is(std::string tag) const {
    return tagName() == tag;
}

/**
 * Returns the tag name of the Node
 * @return Node tag
 */
std::string DOM::Node::tagName() const {
    return tag;
}

/**
 * Creates a Text Node
 * @param tag node tag name
 * @param text node content
 */
DOM::TextNode::TextNode(const std::string & text)
    : Node("TEXT NODE"), text(text) {
}

/**
 * Returns text
 * @return text
 */
std::string DOM::TextNode::getText() const {
    return text;
}

/**
 * Accepts a visitor to the node
 * @param visitor accepted visitor
 */
void DOM::TextNode::acceptVisitor(Visitor & visitor) const {
    visitor.visit(*this);
}

/**
 * Clone the Text Node to a unique pointer
 * @return cloned Node
 */
DOM::NodePtr DOM::TextNode::clone() {
    return NodePtr(new TextNode(text));
}

/**
 * Creates a Comment Node
 * @param comment node content
 */
DOM::CommentNode::CommentNode(const std::string & comment)
    : Node("COMMENT NODE"), comment(comment) {
}

/**
 * Returns comment
 * @return comment
 */
std::string DOM::CommentNode::getComment() const {
    return comment;
}

/**
 * Accepts a visitor to the node
 * @param visitor accepted visitor
 */
void DOM::CommentNode::acceptVisitor(Visitor & visitor) const {
    visitor.visit(*this);
}

/**
 * Clone the Comment Node to a unique pointer
 * @return cloned Node
 */
DOM::NodePtr DOM::CommentNode::clone() {
    return NodePtr(new CommentNode(comment));
}

/**
 * Creates an Element Node
 * @param tag node tag name
 * @param attributes node attributes
 * @param children children nodes
 */
DOM::ElementNode::ElementNode(const std::string &  tag,
                              const AttributeMap & attributes,
                              const NodeVector &   children)
    : Node(tag), attributes(attributes), children() {
    this->children.reserve(children.size());
    std::for_each(children.begin(), children.end(), [this](const auto & child) {
        this->children.push_back(child->clone());
    });
}

/**
 * Returns pointers to children nodes
 * @return children nodes
 */
std::vector<DOM::Node *> DOM::ElementNode::getChildren() const {
    std::vector<DOM::Node *> nodes;
    std::transform(children.begin(),
                   children.end(),
                   std::back_inserter(nodes),
                   [](auto & child) { return child.get(); });
    return nodes;
}

/**
 * Returns pretty-printed attributes
 * @return attributes
 */
std::string DOM::ElementNode::getAttributes() const {
    return attributes.print();
}

/**
 * Accepts a visitor to the node
 * @param visitor accepted visitor
 */
void DOM::ElementNode::acceptVisitor(Visitor & visitor) const {
    visitor.visit(*this);
}

/**
 * Clone the Element Node to a unique pointer
 * @return cloned Node
 */
DOM::NodePtr DOM::ElementNode::clone() {
    return NodePtr(new ElementNode(tagName(), attributes, children));
}

#endif  // DOM_NODE_CPP
