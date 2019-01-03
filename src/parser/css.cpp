// sherpa_41's CSS Parser, licensed under MIT. (c) hafiz, 2018

#ifndef PARSER_CSS_CPP
#define PARSER_CSS_CPP

#include "parser/css.hpp"

#include "parser.cpp"

#include <algorithm>
#include <functional>

/**
 * Creates a CSS Parser
 * @param css
 */
CSSParser::CSSParser(std::string css)
    : Parser<CSS::StyleSheet>(std::move(css)) {
}

/**
 * Parses CSS into engine-operable format
 * @return vector of parsed CSS rules
 */
CSS::StyleSheet CSSParser::evaluate() {
    CSS::StyleSheet styles;
    while (true) {
        consume_whitespace();
        if (eof()) {
            break;
        }
        styles.push_back(parseRule());
    }
    return styles;
}

/**
 * Parses a single rule in the style sheet
 * @return parsed Rule
 */
CSS::Rule CSSParser::parseRule() {
    // must explicity define order of evaluation
    auto sels  = parseSelectors();
    auto decls = parseDeclarations();
    return CSS::Rule(sels, decls);
}

/**
 * Parses rule selectors of form `tag#id.class`
 * @return vector of Selectors
 */
CSS::PrioritySelectorSet CSSParser::parseSelectors() {
    CSS::PrioritySelectorSet res;
    CSS::Selector            selector;
    auto                     invalid = [this](char c) {
        return !std::isalnum(c) && !peek("_") && !peek("-");
    };
    bool ranOnce = false;
    while (!eof()) {
        consume_whitespace();
        if (peek("{")) {  // end of selectors
            if (ranOnce) {
                res.insert(selector);
            }
            return res;
        } else if (peek(",")) {
            consume(",");
            break;
        } else if (peek("#")) {  // id
            consume("#");
            selector.id = build_until(invalid);
        } else if (peek(".")) {  // class
            consume(".");
            selector.klass.push_back(build_until(invalid));
        } else if (peek("*")) {  // universal selector
            consume("*");
        } else {  // tag
            selector.tag = build_until(invalid);
        }
        ranOnce = true;
    }

    auto rest = parseSelectors();
    res.insert(selector);
    res.insert(rest.begin(), rest.end());
    return res;
}

/**
 * Parses rule declarations of form `{ rule: value; }`
 * @return vector of Declarations
 */
CSS::DeclarationSet CSSParser::parseDeclarations() {
    CSS::DeclarationSet declarations;
    consume("{");
    while (!eof()) {
        consume_whitespace();
        if (peek("}")) {
            break;
        }
        auto name = build_until(
            [this](char c) { return !std::isalpha(c) && !peek("-"); });
        consume_whitespace(":");
        consume_whitespace();
        declarations.emplace_back(CSS::Declaration(name, parseValue()));
        consume_whitespace(";");
    }
    consume("}");
    return declarations;
}

/**
 * Parses a value, for example `15px` or `rgba(0,0,0,0)`
 * @return pointer to Value
 */
CSS::ValuePtr CSSParser::parseValue() {
    auto invalid = [this](char c) {
        return !std::isalnum(c) && !peek("_") && !peek("-");
    };
    auto notFloat = std::not_fn(cisfloat);

    if (peek(cisfloat)) {
        return CSS::ValuePtr(
            new CSS::UnitValue(std::stod(build_until(notFloat)), parseUnit()));
    } else if (peek("rgb")) {
        return parseRGB();
    } else if (peek("#")) {
        return parseHex();
    } else {
        return CSS::ValuePtr(new CSS::TextValue(build_until(invalid)));
    }
}

/**
 * Parses RGB color
 * @return RGB ColorValue
 */
CSS::ValuePtr CSSParser::parseRGB() {
    auto notDigit = std::not_fn(cisdigit);
    bool hasAlpha = peek("rgba");
    hasAlpha ? consume("rgba") : consume("rgb");
    consume("(");
    std::vector<uint8_t> vals;
    while (!peek(")") && vals.size() < 3) {
        consume_whitespace();
        vals.push_back((uint8_t)std::stoul(build_until(notDigit)));
        if (peek(")")) {
            break;
        }
        consume_whitespace(",");
    }

    double alpha = hasAlpha ? std::stod(build_until(std::not_fn(cisfloat))) : 1;

    consume(")");

    return CSS::ValuePtr(new CSS::ColorValue(vals[0], vals[1], vals[2], alpha));
}

/**
 * Parses Hex color
 * @return Hex to RGB ColorValue
 */
CSS::ValuePtr CSSParser::parseHex() {
    consume("#");
    auto hexStr = build_until(std::not_fn(cisalnum));
    auto hex    = std::stoul(hexStr, nullptr, 16);
    if (hexStr.size() == 3) {  // duplicate each character
        // 0x000RGB => 0x0R0G0B
        auto hhex = ((hex & 0xF00) << 8) | ((hex & 0x0F0) << 4) | (hex & 0x00F);
        // 0x0R0G0B | 0xR0G0B0 => 0xRRGGBB
        hex = hhex | hhex << 4;
    }
    return CSS::ValuePtr(
        new CSS::ColorValue(static_cast<uint8_t>((hex >> 16) & 255),
                            static_cast<uint8_t>((hex >> 8) & 255),
                            static_cast<uint8_t>(hex & 255),
                            1));
}

/**
 * Parses Unit
 * @return Unit
 */
CSS::Unit CSSParser::parseUnit() {
    auto raw    = build_until(std::not_fn(cisalpha));
    auto rawArr = CSS::UnitRaw();
    return static_cast<CSS::Unit>(std::find(rawArr.begin(), rawArr.end(), raw) -
                                  rawArr.begin());
}

void CSSParser::consume_whitespace(const std::string & next) {
    while (!eof() && (peek(cisspace) || peek("/*"))) {
        if (peek("/*")) {
            build_until([this](char) { return peek("*/"); });
            consume("*/");
        }
        Parser<CSS::StyleSheet>::consume_whitespace();
    }
    consume(next);
}

#endif