// sherpa_41's HTML Parser, licensed under MIT. (c) hafiz, 2018

#ifndef PARSER_HTML_HPP
#define PARSER_HTML_HPP

#include "dom.h"
#include "parser/parser.h"

/**
 * HTML Parser, parsing text into a DOM tree represented using the DOM module.
 *
 * So far, the following features are supported:
 *  - arbitrary HTML elements, text, comments
 *  - arbitrary element attributes
 *  - <html> parent error correction
 */
class HTMLParser : public Parser<DOM::NodePtr> {
 public:
  /**
   * Creates an HTML Parser
   * @param html HTML to parse
   */
  explicit HTMLParser(std::string html);

  /**
   * Default dtor
   */
  ~HTMLParser() override = default;

  /**
   * Parses the HTML into a DOM tree
   * @return DOM tree
   */
  DOM::NodePtr evaluate() override;

 private:
  /**
   * Parses children of a DOM Node
   * @return Node children
   */
  DOM::NodeVector parseChildren();

  /**
   * Parses a single DOM Node
   * @return parsed Node
   */
  DOM::NodePtr parseNode();

  /**
   * Parses text in the DOM
   * @return Text node
   */
  DOM::NodePtr parseTextNode();

  /**
   * Parses a comment in the DOM
   * @return Comment node
   */
  DOM::NodePtr parseCommentNode();

  /**
   * Parses a DOM Element
   * @return Element node
   */
  DOM::NodePtr parseElementNode();

  /**
   * Parses Element attributes
   * @return attributes
   */
  DOM::AttributeMap parseAttributes();
};

#endif
