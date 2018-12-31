// sherpa_41's Printer, licensed under MIT. (c) hafiz, 2018

#ifndef VISITOR_PRINTER_CPP
#define VISITOR_PRINTER_CPP

#include "visitor/printer.hpp"

/**
 * Prints text
 * @param node Text node
 */
void Printer::visit(const DOM::TextNode & node) {
    tabs() << node.getText() << std::endl;
}

/**
 * Prints a comment
 * @param node Comment node
 */
void Printer::visit(const DOM::CommentNode & node) {
    openTag() << "!-- " << node.getComment() << " --" << closeTag();
}

/**
 * Prints an element
 * @param node Element node
 */
void Printer::visit(const DOM::ElementNode & node) {
    openTag() << node.tagName();
    if (!node.getAttributes().empty()) {
        tree << " " << node.getAttributes();
    }
    tree << closeTag();

    ++tabIndent;
    auto children = node.getChildren();
    std::for_each(children.begin(), children.end(), [this](const auto & child) {
        child->acceptVisitor(*this);
    });
    --tabIndent;

    openTag() << "/" << node.tagName() << closeTag();
}

/**
 * Prints a style sheet
 * @param ss style sheet
 */
void Printer::visit(const CSS::StyleSheet & ss) {
    std::for_each(ss.begin(), ss.end(), [this](const auto & rule) {
        const auto & sels     = rule.selectors;
        const auto & decls    = rule.declarations;
        const auto & firstSel = sels.begin();
        tabs() << std::accumulate(std::next(firstSel),
                                  sels.end(),
                                  firstSel->print(),
                                  [](auto acc, const auto & sel) {
                                      return acc + ", " + sel.print();
                                  });

        tree << " {" << std::endl;
        ++tabIndent;
        std::for_each(decls.begin(), decls.end(), [this](const auto & decl) {
            tabs() << decl.print() << std::endl;
        });
        --tabIndent;
        tree << "}\n" << std::endl;
    });
}

/**
 * Returns pretty-printed DOM tree
 * @return DOM tree
 */
std::string Printer::result() {
    return tree.str();
}

/**
 * Print tab indent
 * @return stream for chaining
 */
std::stringstream & Printer::tabs() {
    tree << std::string(tabIndent, '\t');
    return tree;
}

/**
 * Print opening tag
 * @return stream for chaining
 */
std::stringstream & Printer::openTag() {
    tabs() << "<";
    return tree;
}

/**
 * Print closing tag
 * @return stream for chaining
 */
std::string Printer::closeTag() {
    return ">\n";
}

#endif