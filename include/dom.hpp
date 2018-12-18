#ifndef DOM_HPP
#define DOM_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

class Visitor;

namespace DOM {

// forward declaration
class Node;

typedef std::unique_ptr<Node> NodePtr;
typedef std::vector<NodePtr>  NodeVector;

class AttributeMap : public std::map<std::string, std::string> {
   public:
    /**
     * Creates an Attribute Map
     */
    AttributeMap() = default;

    /**
     * Inserts an attribute
     * @param attribute attribute to add
     * @param value value of attribute
     */
    void insert(const std::string & attribute, const std::string & value);

    /**
     * Pretty-prints attributes
     * @return printed attributes
     */
    std::string print() const;

   private:
    std::vector<std::string> order;
};

class Node {
   public:
    /**
     * Creates a DOM Node
     * @param tag node tag name
     */
    explicit Node(const std::string & tag);

    /**
     * Pure virtual destructor prevents unanticipated instantiation
     */
    virtual ~Node() = 0;

    /**
     * Determines whether the Node is of specified type
     * @param tag tag to match
     * @return whether Node is of `tag` type
     */
    bool is(std::string tag) const;

    /**
     * Returns the tag name of the Node
     * @return Node tag
     */
    std::string tagName() const;

    /**
     * Accepts a visitor to the node
     * @param visitor accepted visitor
     */
    virtual void acceptVisitor(Visitor & visitor) const = 0;

    /**
     * Clone the Node to a unique pointer
     * @return cloned Node
     */
    virtual NodePtr clone() = 0;

   private:
    std::string tag;
};

class TextNode : public Node {
   public:
    /**
     * Creates a Text Node
     * @param text node content
     */
    explicit TextNode(const std::string & text);

    /**
     * Default dtor
     */
    ~TextNode() override = default;

    /**
     * Returns text
     * @return text
     */
    std::string getText() const;

    /**
     * Accepts a visitor to the node
     * @param visitor accepted visitor
     */
    void acceptVisitor(Visitor & visitor) const override;

   private:
    /**
     * Clone the Text Node to a unique pointer
     * @return cloned Node
     */
    NodePtr clone() override;

    std::string text;
};

class CommentNode : public Node {
   public:
    /**
     * Creates a Comment Node
     * @param comment node content
     */
    explicit CommentNode(const std::string & comment);

    /**
     * Default dtor
     */
    ~CommentNode() override = default;

    /**
     * Returns comment
     * @return comment
     */
    std::string getComment() const;

    /**
     * Accepts a visitor to the node
     * @param visitor accepted visitor
     */
    void acceptVisitor(Visitor & visitor) const override;

   private:
    /**
     * Clone the Comment Node to a unique pointer
     * @return cloned Node
     */
    NodePtr clone() override;

    std::string comment;
};

class ElementNode : public Node {
   public:
    /**
     * Creates an Element Node
     * @param tag node tag name
     * @param attributes node attributes
     * @param children children nodes
     */
    explicit ElementNode(const std::string &  tag,
                         const AttributeMap & attributes = AttributeMap(),
                         const NodeVector &   children   = NodeVector());

    /**
     * Default dtor
     */
    ~ElementNode() override = default;

    /**
     * Returns pointers to children nodes
     * @return children nodes
     */
    std::vector<Node *> getChildren() const;

    /**
     * Returns pretty-printed attributes
     * @return attributes
     */
    std::string getAttributes() const;

    /**
     * Accepts a visitor to the node
     * @param visitor accepted visitor
     */
    void acceptVisitor(Visitor & visitor) const override;

   private:
    /**
     * Clone the Element Node to a unique pointer
     * @return cloned Node
     */
    NodePtr clone() override;

    AttributeMap attributes;
    NodeVector   children;
};
}  // namespace DOM

#endif  // DOM_HPP
