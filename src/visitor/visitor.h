// sherpa_41's Base Visitor, licensed under MIT. (c) hafiz, 2018

#ifndef VISITOR_HPP
#define VISITOR_HPP

#include "css.h"
#include "dom.h"

/**
 * A visitor interface for other iterators and applications to extend, currently
 * for the Printer
 *
 * Allows for visiting various classes in the browser through visitation
 */
class Visitor {
 public:
  virtual ~Visitor() = default;

  virtual void visit(const DOM::TextNode&) = 0;
  virtual void visit(const DOM::CommentNode&) = 0;
  virtual void visit(const DOM::ElementNode&) = 0;
  virtual void visit(const CSS::StyleSheet&) = 0;
};

#endif  // VISITOR_HPP
