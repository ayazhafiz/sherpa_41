#ifndef PRINTER_HPP
#define PRINTER_HPP

#include "css.hpp"
#include "visitor.hpp"

#include <sstream>

/**
 * Allows for printing various classes in the browser
 */
class Printer : public Visitor {
   public:
    Printer()           = default;
    ~Printer() override = default;

    /**
     * Prints text
     * @param node Text node
     */
    void visit(const DOM::TextNode & node) override;

    /**
     * Prints a comment
     * @param node Comment node
     */
    void visit(const DOM::CommentNode & node) override;

    /**
     * Prints an element
     * @param node Element node
     */
    void visit(const DOM::ElementNode & node) override;

    /**
     * Prints a style sheet
     * @param ss style sheet
     */
    void visit(const CSS::StyleSheet & ss) override;

    /**
     * Returns pretty-printed DOM tree
     * @return DOM tree
     */
    std::string result();

   private:
    /**
     * Print tab indent
     * @return stream for chaining
     */
    std::stringstream & tabs();

    /**
     * Print opening tag
     * @return stream for chaining
     */
    std::stringstream & openTag();

    /**
     * Print closing tag
     * @return stream for chaining
     */
    std::string closeTag();

    std::stringstream tree;
    uint64_t          tabIndent = 0;
};

#endif