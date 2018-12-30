// sherpa_41's Style module, licensed under MIT. (c) hafiz, 2018

#ifndef STYLE_HPP
#define STYLE_HPP

#include "css.hpp"
#include "dom.hpp"

#include <map>
#include <string>

/**
 * The Style module is designed to build and represent styled nodes - DOM
 * elements with CSS styles directly attached to them. This provides a layer of
 * separation between the DOM tree/Stylesheet parsing and the positional Layout
 * module, and arbitrarily styles ___any___ node.
 */
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
     * Returns the value of a style, or any number of backup styles on the node,
     * or nullptr if the style is not applied.
     * @tparam Args variadic arguments, should be strings
     * @param style style to get
     * @param backup any number of backup styles to check
     * @return value of style, or nullptr if DNE
     */
    template <typename... Args>
    CSS::ValuePtr value(const std::string & style,
                        const Args &... backup) const {
        auto cand = props.find(style);
        if (cand != props.end()) {
            return cand->second->clone();
        } else {
            return value(backup...);
        }
    }

    /**
     * Returns the value of a style, or any number of backup styles on the node,
     * or a passed default value if none of the styles are applied.
     * @tparam Args variadic arguments, should be strings
     * @param style style to get
     * @param backup any number of backup styles to check
     * @param deflt fallback default
     * @return value of style, or `deflt` if DNE
     */
    template <typename... Args>
    CSS::ValuePtr value_or(const std::string & style,
                           const Args &... backup,
                           const CSS::Value & deflt) const {
        if (auto cand = value(style, backup...)) {
            return cand;
        }
        return deflt.clone();
    }

    /**
     * Returns the value of a style, or any number of backup styles on the node,
     * or zero if none of the styles are applied.
     * @tparam Args variadic arguments, should be strings
     * @param style style to get
     * @param backup any number of backup styles to check
     * @return value of style, or zero if DNE
     */
    template <typename... Args>
    CSS::ValuePtr value_or_zero(const std::string & style,
                                const Args &... backup) const {
        return value_or<std::string>(style,
                                     backup...,
                                     CSS::UnitValue(0, CSS::px));
    }

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
     * `value` base case - no style found, nullptr returned
     * @return nullptr
     */
    CSS::ValuePtr value() const;

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
