// sherpa_41's Printer, licensed under MIT. (c) hafiz, 2018

#ifndef VISITOR_PRINTER_HPP
#define VISITOR_PRINTER_HPP

#include <sstream>

#include "css.h"
#include "visitor.h"

/**
 * Prints various classes in the browser to stdout. This is meant more for
 * pretty-printing and testing internal representations than for client viewing
 * of the engine outputs.
 */
class Printer : public Visitor {
 public:
  ~Printer() override = default;

  /**
   * Prints text
   * @param node Text node
   */
  void visit(const DOM::TextNode& node) override;

  /**
   * Prints a comment
   * @param node Comment node
   */
  void visit(const DOM::CommentNode& node) override;

  /**
   * Prints an element
   * @param node Element node
   */
  void visit(const DOM::ElementNode& node) override;

  /**
   * Prints a style sheet
   * @param ss style sheet
   */
  void visit(const CSS::StyleSheet& ss) override;

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
  std::stringstream& tabs();

  /**
   * Print opening tag
   * @return stream for chaining
   */
  std::stringstream& openTag();

  /**
   * Print closing tag
   * @return stream for chaining
   */
  std::string closeTag();

  std::stringstream tree;
  uint64_t tabIndent = 0;
};

#endif
