// sherpa_41's HTML Parser, licensed under MIT. (c) hafiz, 2018

#ifndef PARSER_HTML_CPP
#define PARSER_HTML_CPP

#include "parser/html.h"

#include <cctype>

#include "parser.cpp"

/**
 * Creates an HTML Parser
 * @param html HTML to parse
 */
HTMLParser::HTMLParser(std::string html) : Parser<DOM::NodePtr>(std::move(html)) {}

/**
 * Parses the HTML into a DOM tree
 * @return DOM tree
 */
auto HTMLParser::evaluate() -> DOM::NodePtr {
  auto roots = parseChildren();

  if (roots.size() == 1 && roots.front()->is("html")) {
    return std::move(roots.front());
  } else {
    return DOM::NodePtr(new DOM::ElementNode("html", DOM::AttributeMap(), roots));
  }
}

/**
 * Parses children of a DOM Node
 * @return Node children
 */
auto HTMLParser::parseChildren() -> DOM::NodeVector {
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
auto HTMLParser::parseNode() -> DOM::NodePtr {
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
auto HTMLParser::parseTextNode() -> DOM::NodePtr {
  auto text = build_until([this](char) { return peek("<"); });
  return DOM::NodePtr(new DOM::TextNode(rtrim(text)));
}

/**
 * Parses a comment in the DOM
 * @return Comment node
 */
auto HTMLParser::parseCommentNode() -> DOM::NodePtr {
  consume("<!--");
  auto comment = build_until([this](char) { return peek("-->"); });
  consume("-->");

  return DOM::NodePtr(new DOM::CommentNode(rtrim(comment)));
}

/**
 * Parses a DOM Element
 * @return Element node
 */
auto HTMLParser::parseElementNode() -> DOM::NodePtr {
  consume("<");
  auto tagName = build_until([](char c) { return !std::isalnum(c); });
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
auto HTMLParser::parseAttributes() -> DOM::AttributeMap {
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
