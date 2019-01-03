// sherpa_41's HTML Parser, licensed under MIT. (c) hafiz, 2018

#ifndef PARSER_HTML_CPP
#define PARSER_HTML_CPP

#include "parser/html.hpp"

#include "parser.cpp"

#include <cctype>

/**
 * Creates an HTML Parser
 * @param html HTML to parse
 */
HTMLParser::HTMLParser(std::string html)
    : Parser<DOM::NodePtr>(std::move(html)) {
}

/**
 * Parses the HTML into a DOM tree
 * @return DOM tree
 */
DOM::NodePtr HTMLParser::evaluate() {
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
DOM::NodeVector HTMLParser::parseChildren() {
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
DOM::NodePtr HTMLParser::parseNode() {
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
DOM::NodePtr HTMLParser::parseTextNode() {
    auto text = build_until([this](char) { return peek("<"); });
    return DOM::NodePtr(new DOM::TextNode(rtrim(text)));
}

/**
 * Parses a comment in the DOM
 * @return Comment node
 */
DOM::NodePtr HTMLParser::parseCommentNode() {
    consume("<!--");
    auto comment = build_until([this](char) { return peek("-->"); });
    consume("-->");

    return DOM::NodePtr(new DOM::CommentNode(rtrim(comment)));
}

/**
 * Parses a DOM Element
 * @return Element node
 */
DOM::NodePtr HTMLParser::parseElementNode() {
    consume("<");
    auto tagName    = build_until([](char c) { return !std::isalnum(c); });
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
DOM::AttributeMap HTMLParser::parseAttributes() {
    DOM::AttributeMap attr;
    while (true) {
        consume_whitespace();
        if (eof() || peek(">")) {
            break;
        }
        auto attrName = build_until([](char c) { return !std::isalnum(c); });
        consume("=\"");
        auto attrValue = build_until([](char c) { return c == '"'; });
        consume("\"");
        attr.insert(attrName, attrValue);
    }
    return attr;
}

#endif