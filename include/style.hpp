#ifndef STYLE_HPP
#define STYLE_HPP

#include "css.hpp"
#include "dom.hpp"

#include <map>
#include <string>

namespace Style {

// forward declaration
class StyledNode;
struct ruleOrder;

typedef std::vector<StyledNode>                          StyledNodeVector;
typedef std::map<std::string, CSS::ValuePtr>             PropertyMap;
typedef std::pair<CSS::DeclarationSet, CSS::Specificity> ScoredRule;
typedef std::multiset<ScoredRule, ruleOrder>             PriorityRuleSet;

struct ruleOrder {
    bool operator()(const ScoredRule & a, const ScoredRule & b) const {
        return a.second < b.second;
    }
};

/**
 * A DOM Node with CSS styles applied
 */
class StyledNode {
   public:
    /**
     * Creates a Styled Node
     * @param node reference to DOM Node
     * @param props CSS properties to apply
     * @param children styled DOM children
     */
    explicit StyledNode(DOM::NodePtr     node,
                        PropertyMap      props    = PropertyMap(),
                        StyledNodeVector children = StyledNodeVector());

    /**
     * Copy ctor
     * @param rhs StyledNode to copy
     */
    StyledNode(const StyledNode & rhs);

    /**
     * Returns the value of a style on the node, or nullptr if the style is not
     * applied.
     * @param style style to get
     * @return value of style, or nullptr if DNE
     */
    CSS::ValuePtr value(const std::string & style) const;

    /**
     * Returns the value of a style on the node, or a passed default value if
     * the style is not applied.
     * @param style style to get
     * @param deflt fallback default
     * @return value of style, or default if DNE
     */
    CSS::ValuePtr value_or(const std::string &   style,
                           const CSS::ValuePtr & deflt) const;

    /**
     * Returns children
     * @return children
     */
    StyledNodeVector getChildren() const;

    /**
     * Creates a StyledNode tree from a DOM tree and CSS style sheet
     * @param domRoot DOM root node
     * @param css style sheet
     * @return root to StyledNode tree
     */
    static StyledNode from(const DOM::NodePtr &    domRoot,
                           const CSS::StyleSheet & css);

   private:
    /**
     * Builds the styles for a single DOM node
     * @param node DOM node
     * @param css style sheet to apply
     * @return map of styles
     */
    static PropertyMap mapStyles(const DOM::ElementNode * node,
                                 const CSS::StyleSheet &  css);

    /**
     * Matches css rules to a DOM node
     * @param node DOM node
     * @param css style sheet to apply
     * @return set of rules, ordered by increasing specificity
     */
    static PriorityRuleSet matchRules(const DOM::ElementNode * node,
                                      const CSS::StyleSheet &  css);

    /**
     * Determines if a selector matches a node
     * @param selector selector to match
     * @param node DOM node to match
     * @return whether selector matches node
     */
    static bool selectorMatches(const CSS::Selector &    selector,
                                const DOM::ElementNode * node);

    DOM::NodePtr     node;
    PropertyMap      props;
    StyledNodeVector children;
};
}  // namespace Style

#endif
