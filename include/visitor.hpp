#ifndef VISITOR_HPP
#define VISITOR_HPP

#include "css.hpp"
#include "dom.hpp"

class Visitor {
   public:
    Visitor()          = default;
    virtual ~Visitor() = default;

    virtual void visit(const DOM::TextNode &)    = 0;
    virtual void visit(const DOM::CommentNode &) = 0;
    virtual void visit(const DOM::ElementNode &) = 0;
    virtual void visit(const CSS::StyleSheet &)  = 0;
};

#endif  // VISITOR_HPP