#ifndef PRINTER_CPP
#define PRINTER_CPP

#include "printer.hpp"

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
    std::for_each(children.begin(), children.end(), [this](auto child) {
        child->acceptVisitor(*this);
    });
    --tabIndent;

    openTag() << "/" << node.tagName() << closeTag();
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