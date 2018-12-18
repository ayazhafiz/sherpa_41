#ifndef HTML_HPP
#define HTML_HPP

#include "dom.hpp"

#include <string>

namespace HTML {
class Parser {
   public:
    /**
     * Constructs a Parser
     * @param html HTML to parse
     */
    explicit Parser(const std::string & html);

    /**
     * Parses the HTML into a DOM tree
     * @return DOM tree
     */
    DOM::NodePtr evaluate();

    ~Parser() = default;

    Parser() = delete;

    Parser(const Parser &) = delete;

    Parser & operator=(const Parser &) = delete;

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

    /**
     * Builds a string from the program until predicate satisfied
     * @param predicate when to stop building
     * @return built string
     */
    std::string build_until(std::function<bool(char)> const & predicate);

    /**
     * Ensures that the next characters are as expected, then pushes
     * the program pointer
     * @param next characters to ensure
     */
    void consume(const std::string & next);

    /**
     * Consumes whitespace, then optionally ensures next characters are as
     * expected
     * @param next characters to ensure
     */
    void consume_whitespace(const std::string & next = "");

    /**
     * Pushes the program pointer some units ahead
     * @param dist distance to push pointer
     */
    void pushPtr(uint64_t dist = 1);

    /**
     * Determines the next characters of the program
     * @param prefix characters to match program to
     * @return whether program contains `prefix` next
     */
    bool peek(std::string prefix) const;

    /**
     * Determines if entire program read
     * @return program read?
     */
    bool eof() const;

    /**
     * Trim whitespace from right end of string
     * @param str string to trim
     * @return right-trimmed string
     */
    static inline std::string rtrim(const std::string & str);

    std::string program;
    uint64_t    ptr;
};
}  // namespace HTML

#endif
