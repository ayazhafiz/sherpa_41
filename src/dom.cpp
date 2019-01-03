// sherpa_41's DOM module, licensed under MIT. (c) hafiz, 2018

#ifndef DOM_CPP
#define DOM_CPP

#include "dom.hpp"

#include "visitor/visitor.hpp"

#include <iterator>
#include <sstream>

/**
 * Inserts an attribute
 * @param attribute attribute to add
 * @param value value of attribute
 */
void DOM::AttributeMap::insert(const std::string & attribute,
                               const std::string & value) {
    if (find(attribute) == end()) {
        (*this)[attribute] = value;
        order.push_back(attribute);
    }
}

std::string DOM::AttributeMap::print() const {
    if (empty()) {
        return "";
    }

    auto assign = [this](const auto & attr) {
        return attr + "=\"" + const_cast<AttributeMap &>(*this)[attr] + "\"";
    };
    auto        start = order.begin();
    std::string first = order.front();
    return std::accumulate(std::next(start),
                           order.end(),
                           assign(first),
                           [&assign](auto acc, auto attr) {
                               return acc + " " + assign(attr);
                           });
}

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
 * @param cand tag to match
 * @return whether Node is of `cand` type
 */
bool DOM::Node::is(const std::string & cand) const {
    return tagName() == cand;
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
DOM::ElementNode::ElementNode(std::string        tag,
                              AttributeMap       attributes,
                              const NodeVector & children)
    : Node(std::move(tag)), attributes(std::move(attributes)), children() {
    this->children.reserve(children.size());
    std::for_each(children.begin(), children.end(), [this](const auto & child) {
        this->children.push_back(child->clone());
    });
}

/**
 * Returns pointers to children nodes
 * @return children nodes
 */
DOM::NodeVector DOM::ElementNode::getChildren() const {
    NodeVector nodes;
    std::transform(children.begin(),
                   children.end(),
                   std::back_inserter(nodes),
                   [](const auto & child) { return child->clone(); });
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
 * Returns id of element
 * @return id
 */
std::string DOM::ElementNode::getId() const {
    auto id = attributes.find("id");
    return id != attributes.end() ? id->second : "";
}

/**
 * Returns classes of element
 * @return classes
 */
std::vector<std::string> DOM::ElementNode::getClasses() const {
    auto classes = attributes.find("class");
    if (classes == attributes.end()) {
        return {};
    }
    std::istringstream iss(classes->second);

    return {std::istream_iterator<std::string>{iss},
            std::istream_iterator<std::string>{}};  // split classes by space
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

#endif
