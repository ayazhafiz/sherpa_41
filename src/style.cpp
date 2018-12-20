#ifndef STYLE_CPP
#define STYLE_CPP

#include "style.hpp"

/**
 * Creates a Styled Node
 * @param node reference to DOM Node
 * @param props CSS properties to apply
 * @param children styled DOM children
 */
Style::StyledNode::StyledNode(DOM::NodePtr            node,
                              Style::PropertyMap      props,
                              Style::StyledNodeVector children)
    : node(std::move(node)),
      props(std::move(props)),
      children(std::move(children)) {
}

/**
 * Copy ctor
 * @param rhs StyledNode to copy
 */
Style::StyledNode::StyledNode(const Style::StyledNode & rhs)
    : node(rhs.node->clone()), props(), children(rhs.children) {
    std::for_each(rhs.props.begin(),
                  rhs.props.end(),
                  [this](const auto & prop) {
                      props[prop.first] = prop.second->clone();
                  });
}

/**
 * Returns the value of a style on the node, or nullptr if the style is not
 * applied.
 * @param style style to get
 * @return value of style, or nullptr if DNE
 */
CSS::ValuePtr Style::StyledNode::value(const std::string & style) const {
    auto cand = props.find(style);
    if (cand != props.end()) {
        return cand->second->clone();
    } else {
        return nullptr;
    }
}

/**
 * Returns the value of a style on the node, or a passed default value if
 * the style is not applied.
 * @param style style to get
 * @param deflt fallback default
 * @return value of style, or default if DNE
 */
CSS::ValuePtr Style::StyledNode::value_or(const std::string &   style,
                                          const CSS::ValuePtr & deflt) const {
    if (auto cand = value(style)) {
        return cand;
    }
    return deflt->clone();
}

/**
 * Returns children
 * @return children
 */
Style::StyledNodeVector Style::StyledNode::getChildren() const {
    return children;
}

/**
 * Creates a StyledNode tree from a DOM tree and CSS style sheet
 * @param domRoot DOM root node
 * @param css style sheet
 * @return root to StyledNode tree
 */
Style::StyledNode Style::StyledNode::from(const DOM::NodePtr &    domRoot,
                                          const CSS::StyleSheet & css) {
    if (auto elem = dynamic_cast<DOM::ElementNode *>(domRoot.get())) {
        auto             children = elem->getChildren();
        StyledNodeVector styledChildren;
        std::transform(children.begin(),
                       children.end(),
                       std::back_inserter(styledChildren),
                       [&css](const auto & child) {
                           return StyledNode::from(child->clone(), css);
                       });

        return StyledNode(domRoot->clone(),
                          StyledNode::mapStyles(elem, css),
                          std::move(styledChildren));
    } else {
        return StyledNode(domRoot->clone());
    }
}

/**
 * Builds the styles for a single DOM node
 * @param node DOM node
 * @param css style sheet to apply
 * @return map of styles
 */
Style::PropertyMap Style::StyledNode::mapStyles(
    const DOM::ElementNode * const node,
    const CSS::StyleSheet &        css) {
    PropertyMap props;
    auto        rules = matchRules(node, css);
    std::for_each(rules.begin(), rules.end(), [&props](const auto & rule) {
        const auto & decls = rule.first;

        for (const auto & decl : decls) {
            props[decl.name] = decl.value->clone();
        }
    });
    return props;
}

/**
 * Matches css rules to a DOM node
 * @param node DOM node
 * @param css style sheet to apply
 * @return set of rules, ordered by increasing specificity
 */
Style::PriorityRuleSet Style::StyledNode::matchRules(
    const DOM::ElementNode * const node,
    const CSS::StyleSheet &        css) {
    PriorityRuleSet rules;
    std::for_each(css.begin(), css.end(), [&node, &rules](const auto & rule) {
        const auto & sels = rule.selectors;

        // find first selector that matches the node
        auto selector = std::
            find_if(sels.begin(), sels.end(), [&node](const auto & sel) {
                return StyledNode::selectorMatches(sel, node);
            });

        // add rule into set if it matches node
        if (selector != sels.end()) {
            auto s = selector->specificity();
            rules.insert(
                std::make_pair(rule.declarations, selector->specificity()));
        }
    });
    return rules;
}

/**
 * Determines if a selector matches a node
 * @param selector selector to match
 * @param node DOM node to match
 * @return whether selector matches node
 */
bool Style::StyledNode::selectorMatches(const CSS::Selector &          selector,
                                        const DOM::ElementNode * const node) {
    auto   tag      = node->tagName();
    auto   id       = node->getId();
    auto   cls      = node->getClasses();
    auto & checkCls = selector.klass;

    if (!selector.tag.empty() && selector.tag != tag) {
        return false;
    }

    if (!selector.id.empty() && selector.id != id) {
        return false;
    }

    if (!checkCls.empty()) {
        for (const auto & cl : checkCls) {
            if (std::find(cls.begin(), cls.end(), cl) == cls.end()) {
                return false;
            }
        }
    }

    return true;  // all selectors accounted for
}

#endif