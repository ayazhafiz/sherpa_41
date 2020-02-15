// sherpa_41's DOM module, licensed under MIT. (c) hafiz, 2018

#ifndef DOM_HPP
#define DOM_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

class Visitor;

/**
 * The DOM module represents a tree of DOM nodes from a parsed HTML file. A base
 * Node class is used to represent a multitude of derived nodes that may be in
 * the DOM tree - this is not unlike a composite pattern.
 *
 * The following nodes are supported:
 *  - TextNode: a block of text in an element
 *  - CommentNode: a comment
 *  - ElementNode: an HTML element
 */
namespace DOM {

// forward declaration
class Node;

using NodePtr = std::unique_ptr<Node>;
using NodeVector = std::vector<NodePtr>;

/**
 * A map of DOM attributes, adapted to allow visitors
 */
class AttributeMap : public std::map<std::string, std::string> {
 public:
  /**
   * Inserts an attribute
   * @param attribute attribute to add
   * @param value value of attribute
   */
  void insert(const std::string& attribute, const std::string& value);

  /**
   * Pretty-prints attributes
   * @return printed attributes
   */
  [[nodiscard]] auto print() const -> std::string;

 private:
  std::vector<std::string> order;
};

/**
 * An abstract DOM node
 */
class Node {
 public:
  /**
   * Creates a DOM Node
   * @param tag node tag name
   */
  explicit Node(std::string tag);

  /**
   * Pure virtual destructor prevents unanticipated instantiation
   */
  virtual ~Node() = 0;

  /**
   * Determines whether the Node is of specified type
   * @param cand tag to match
   * @return whether Node is of `cand` type
   */
  [[nodiscard]] auto is(const std::string& cand) const -> bool;

  /**
   * Returns the tag name of the Node
   * @return Node tag
   */
  [[nodiscard]] auto tagName() const -> std::string;

  /**
   * Accepts a visitor to the node
   * @param visitor accepted visitor
   */
  virtual void acceptVisitor(Visitor& visitor) const = 0;

  /**
   * Clone the Node to a unique pointer
   * @return cloned Node
   */
  virtual auto clone() -> NodePtr = 0;

 private:
  std::string tag;
};

/**
 * A node of text in the DOM
 */
class TextNode : public Node {
 public:
  /**
   * Creates a Text Node
   * @param text node content
   */
  explicit TextNode(std::string text);

  TextNode(const TextNode& rhs) = delete;

  /**
   * Default dtor
   */
  ~TextNode() override = default;

  /**
   * Returns text
   * @return text
   */
  [[nodiscard]] auto getText() const -> std::string;

  /**
   * Accepts a visitor to the node
   * @param visitor accepted visitor
   */
  void acceptVisitor(Visitor& visitor) const override;

 private:
  /**
   * Clone the Text Node to a unique pointer
   * @return cloned Node
   */
  auto clone() -> NodePtr override;

  std::string text;
};

/**
 * A comment in the DOM
 */
class CommentNode : public Node {
 public:
  /**
   * Creates a Comment Node
   * @param comment node content
   */
  explicit CommentNode(std::string comment);

  CommentNode(const CommentNode& rhs) = delete;

  /**
   * Default dtor
   */
  ~CommentNode() override = default;

  /**
   * Returns comment
   * @return comment
   */
  [[nodiscard]] auto getComment() const -> std::string;

  /**
   * Accepts a visitor to the node
   * @param visitor accepted visitor
   */
  void acceptVisitor(Visitor& visitor) const override;

 private:
  /**
   * Clone the Comment Node to a unique pointer
   * @return cloned Node
   */
  auto clone() -> NodePtr override;

  std::string comment;
};

/**
 * An element in the DOM
 */
class ElementNode : public Node {
 public:
  /**
   * Creates an Element Node
   * @param tag node tag name
   * @param attributes node attributes
   * @param children children nodes
   */
  explicit ElementNode(std::string tag,
                       AttributeMap attributes = AttributeMap(),
                       const NodeVector& children = NodeVector());

  ElementNode(const ElementNode& rhs) = delete;

  /**
   * Default dtor
   */
  ~ElementNode() override = default;

  /**
   * Returns pointers to children nodes
   * @return children nodes
   */
  [[nodiscard]] auto getChildren() const -> NodeVector;

  /**
   * Returns pretty-printed attributes
   * @return attributes
   */
  [[nodiscard]] auto getAttributes() const -> std::string;

  /**
   * Returns id of element
   * @return id
   */
  [[nodiscard]] auto getId() const -> std::string;

  /**
   * Returns classes of element
   * @return classes
   */
  [[nodiscard]] auto getClasses() const -> std::vector<std::string>;

  /**
   * Accepts a visitor to the node
   * @param visitor accepted visitor
   */
  void acceptVisitor(Visitor& visitor) const override;

 private:
  /**
   * Clone the Element Node to a unique pointer
   * @return cloned Node
   */
  auto clone() -> NodePtr override;

  AttributeMap attributes;
  NodeVector children;
};
}  // namespace DOM

#endif  // DOM_HPP
