// sherpa_41's Base Parser, licensed under MIT. (c) hafiz, 2018

#ifndef PARSER_HPP
#define PARSER_HPP

#include <algorithm>
#include <functional>
#include <string>

/**
 * A basic parser, not meant to evaluate anything on its own.
 * The parser serves as a base for others, currently the HTML and CSS parser.
 *
 * @tparam EvalType type of program to evaluate to
 */
template <typename EvalType>
class Parser {
   public:
    /**
     * Constructs a Parser
     * @param program program to parse
     */
    explicit Parser(std::string program);

    /**
     * Default dtor
     */
    virtual ~Parser() = default;

    /**
     * Generic evaluation of the parser
     * @return result of parsing program
     */
    virtual EvalType evaluate() = 0;

   protected:
    typedef std::function<bool(char)> const PrefixComparator;

    /**
     * Builds a string of some length
     * @param len length of string to build
     * @return built string
     */
    std::string build(uint64_t len);

    /**
     * Builds a string from the program until predicate satisfied
     * @param predicate when to stop building
     * @return built string
     */
    std::string build_until(PrefixComparator & predicate);

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
    bool peek(const std::string & prefix) const;

    /**
     * Determines the next character of the program
     * @param predicate lambda to match prefix to
     * @return whether program prefix satisfies lambda
     */
    bool peek(PrefixComparator predicate) const;

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
    static inline std::string rtrim(const std::string & str) {
        auto res(str);
        res.erase(std::find_if(res.rbegin(),
                               res.rend(),
                               [](auto ch) { return !std::isspace(ch); })
                      .base(),
                  res.end());
        return res;
    }

   private:
    std::string program;
    uint64_t    ptr;
};

#endif