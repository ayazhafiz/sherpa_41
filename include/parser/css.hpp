// sherpa_41's CSS Parser, licensed under MIT. (c) hafiz, 2018

#ifndef PARSER_CSS_HPP
#define PARSER_CSS_HPP

#include "../css.hpp"
#include "parser/parser.hpp"

#include <cctype>

/**
 * CSS Parser, parsing text into a style sheet represented using the CSS module.
 *
 * So far, the following features are supported:
 *  - tag, id, class, and wildcard selectors
 *  - text, numerical unit, and color (RGB/A, #HEX) declarations
 *  - comments (simply ignored)
 *  - mandatory semicolons at the end of declarations
 */
class CSSParser : public Parser<CSS::StyleSheet> {
   public:
    /**
     * Creates a CSS Parser
     * @param css
     */
    explicit CSSParser(std::string css);

    /**
     * Default dtor
     */
    ~CSSParser() override = default;

    /**
     * Parses CSS into engine-operable format
     * @return vector of parsed CSS rules
     */
    CSS::StyleSheet evaluate() override;

   private:
    /**
     * Parses a single rule in the style sheet
     * @return parsed Rule
     */
    CSS::Rule parseRule();

    /**
     * Parses rule selectors of form `tag#id.class`
     * @return vector of Selectors
     */
    CSS::PrioritySelectorSet parseSelectors();

    /**
     * Parses rule declarations of form `rule: value;`
     * @return vector of Declarations
     */
    CSS::DeclarationSet parseDeclarations();

    /**
     * Parses a value, for example `15px` or `rgba(0,0,0,0)`
     * @return pointer to Value
     */
    CSS::ValuePtr parseValue();

    /**
     * Parses RGB color
     * @return RGB ColorValue
     */
    CSS::ValuePtr parseRGB();

    /**
     * Parses Hex color
     * @return Hex to RGB ColorValue
     */
    CSS::ValuePtr parseHex();

    /**
     * Parses Unit
     * @return Unit
     */
    CSS::Unit parseUnit();

    /**
     * Consumes whitespace and comments, then optionally ensures next
     * characters are as expected
     * @param next characters to ensure
     */
    void consume_whitespace(const std::string & next = "");

    // casted std::is_ methods for parameter use
    static constexpr auto cisdigit = static_cast<int (*)(int)>(std::isdigit);
    static constexpr auto cisfloat = [](char c) {
        return std::isdigit(c) || c == '.' || c == '-';
    };
    static constexpr auto cisalpha = static_cast<int (*)(int)>(std::isalpha);
    static constexpr auto cisalnum = static_cast<int (*)(int)>(std::isalnum);
    static constexpr auto cisspace = static_cast<int (*)(int)>(std::isspace);
};

#endif