#ifndef HTML_CPP
#define HTML_CPP

#include "html.hpp"

/**
 * Constructs a Parser
 * @param html HTML to parse
 */
HTML::Parser::Parser(const std::string & html) : program(html), ptr(0) {
}

/**
 * Parses the HTML into a DOM tree
 * @return DOM tree
 */
DOM::NodePtr HTML::Parser::evaluate() {
    auto roots = parseChildren();

    if (roots.size() == 1 && roots.front()->is("html")) {
        return std::move(roots.front());
    } else {
        return DOM::NodePtr(
            new DOM::ElementNode("html", DOM::AttributeMap(), roots));
    }
}

/**
 * Parses children of a DOM Node
 * @return Node children
 */
DOM::NodeVector HTML::Parser::parseChildren() {
    DOM::NodeVector roots;
    while (true) {
        consume_whitespace();
        if (eof() || peek("</")) {
            break;
        }
        roots.push_back(DOM::NodePtr(parseNode()));
    }
    return roots;
}

/**
 * Parses a single DOM Node
 * @return parsed Node
 */
DOM::NodePtr HTML::Parser::parseNode() {
    if (peek("<!--")) {
        return parseCommentNode();
    } else if (peek("<")) {
        return parseElementNode();
    } else {
        return parseTextNode();
    }
}

/**
 * Parses text in the DOM
 * @return Text node
 */
DOM::NodePtr HTML::Parser::parseTextNode() {
    auto text = build_until([this](char) { return peek("<"); });
    return DOM::NodePtr(new DOM::TextNode(rtrim(text)));
}

/**
 * Parses a comment in the DOM
 * @return Comment node
 */
DOM::NodePtr HTML::Parser::parseCommentNode() {
    consume("<!--");
    auto comment = build_until([this](char) { return peek("-->"); });
    consume("-->");

    return DOM::NodePtr(new DOM::CommentNode(rtrim(comment)));
}

/**
 * Parses a DOM Element
 * @return Element node
 */
DOM::NodePtr HTML::Parser::parseElementNode() {
    consume("<");
    auto tagName    = build_until([](char c) { return !isalnum(c); });
    auto attributes = parseAttributes();
    consume_whitespace(">");

    auto children = parseChildren();

    consume("</");
    consume_whitespace(tagName);
    consume_whitespace(">");

    return DOM::NodePtr(new DOM::ElementNode(tagName, attributes, children));
}

/**
 * Parses Element attributes
 * @return attributes
 */
DOM::AttributeMap HTML::Parser::parseAttributes() {
    DOM::AttributeMap attr;
    while (true) {
        consume_whitespace();
        if (eof() || peek(">")) {
            break;
        }
        auto attrName = build_until([](char c) { return !isalnum(c); });
        consume("=\"");
        auto attrValue = build_until([](char c) { return c == '"'; });
        consume("\"");
        attr.insert(attrName, attrValue);
    }
    return attr;
}

/**
 * Builds a string from the program until predicate satisfied
 * @param predicate when to stop building
 * @return built string
 */
std::string HTML::Parser::build_until(std::function<bool(char)> const & predicate) {
    consume_whitespace();
    std::string res;
    while (!eof() && !predicate(program[ptr])) {
        res += program[ptr];
        pushPtr();
    }
    return res;
}

/**
 * Ensures that the next characters are as expected, then pushes
 * the program pointer
 * @param next characters to ensure
 */
void HTML::Parser::consume(const std::string & next) {
    assert(peek(next));
    pushPtr(next.length());
}

/**
 * Consumes whitespace, then optionally ensures next characters are as
 * expected
 * @param next characters to ensure
 */
void HTML::Parser::consume_whitespace(const std::string & next) {
    while (isspace(program[ptr])) {
        pushPtr();
    }
    consume(next);
}

/**
 * Pushes the program pointer some units ahead
 * @param dist distance to push pointer
 */
void HTML::Parser::pushPtr(uint64_t dist) {
    ptr += dist;
}

/**
 * Determines the next characters of the program
 * @param prefix characters to match program to
 * @return whether program contains `prefix` next
 */
bool HTML::Parser::peek(std::string prefix) const {
    return program.rfind(prefix, ptr) == ptr;
}

/**
 * Determines if entire program read
 * @return program read?
 */
bool HTML::Parser::eof() const {
    return ptr >= program.length();
}

/**
 * Trim whitespace from right end of string
 * @param str string to trim
 * @return right-trimmed string
 */
std::string HTML::Parser::rtrim(const std::string & str) {
    auto res(str);
    res.erase(std::find_if(res.rbegin(),
                           res.rend(),
                           [](auto ch) { return !isspace(ch); })
                  .base(),
              res.end());
    return res;
}

#endif