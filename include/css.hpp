// sherpa_41's CSS module, licensed under MIT. (c) hafiz, 2018

#ifndef CSS_HPP
#define CSS_HPP

#include "parser/parser.hpp"

#include <memory>
#include <numeric>
#include <set>
#include <vector>

class Visitor;

/**
 * The CSS module is designed to represent trivial stylesheets with a series
 * of rules. Rules consist of selectors and declarations.
 *
 * So far, the following is supported:
 *  __selectors__:
 *      - ids
 *      - classes
 *      - tags
 *      - wildcards (\*)
 *  __declarations__:
 *      - text values
 *      - color values (RGB/A, #HEX)
 *      - unit values (px, em, rem, etc... but only px is normalized)
 */
namespace CSS {

// forward declaration
struct Value;
struct Selector;
struct Declaration;
struct specificityOrder;

typedef std::unique_ptr<Value>                    ValuePtr;
typedef std::vector<uint64_t>                     Specificity;
typedef std::multiset<Selector, specificityOrder> PrioritySelectorSet;
typedef std::vector<Declaration>                  DeclarationSet;

/**
 * Normalizes a printed floating point value
 * 1.00000 -> 1.0
 * @param value value to normalize
 * @return normalized string
 */
std::string normalizeFp(double value);

/**
 * Creates a ValuePtr from a concrete value
 * @param val value to create a pointer of
 * @return ValuePtr
 */
ValuePtr make_value(const Value & val);

/**
 * Abstract base struct representing a CSS declaration value
 */
struct Value {
   public:
    virtual ~Value() = default;

    /**
     * Dynamically clones a Value into a unique_ptr
     * @return cloned Value as a pointer
     */
    virtual ValuePtr clone() const = 0;

    /**
     * Prints a declaration value
     * @return printed value
     */
    virtual std::string print() const = 0;

    /**
     * Returns if *this is of specified value
     * @param value value to compare
     * @return whether *this is `value`
     */
    virtual bool is(std::string value) const;

    /**
     * Returns the unit value, or 0 if not a UnitValue
     * @return unit value, or 0 if not UnitValue
     */
    virtual double unitValue() const;
};

/**
 * A text declaration value
 */
struct TextValue : public Value {
   public:
    TextValue()           = delete;
    ~TextValue() override = default;

    /**
     * Creates a text value
     * @param value text value
     */
    explicit TextValue(std::string value);

    TextValue(const TextValue & rhs) = default;

    /**
     * Clones a TextValue
     * @return cloned value
     */
    ValuePtr clone() const override;

    /**
     * Prints a text declaration value
     * @return printed value
     */
    std::string print() const override;

    std::string value;
};

enum Unit { px, em, rem, vw, vh, percent };
std::vector<std::string> UnitRaw();
/**
 * A unit declaration value
 */
struct UnitValue : public Value {
   public:
    UnitValue()           = delete;
    ~UnitValue() override = default;

    /**
     * Creates a unit value
     * @param value magnitude
     * @param unit unit used
     */
    UnitValue(double value, Unit unit);

    UnitValue(const UnitValue & rhs) = default;

    /**
     * Clones a UnitValue
     * @return cloned value
     */
    ValuePtr clone() const override;

    /**
     * Prints a unit declaration value
     * @return printed value
     */
    std::string print() const override;

    double value;
    Unit   unit;
};

/**
 * A color declaration value
 */
struct ColorValue : public Value {
   public:
    ColorValue()           = delete;
    ~ColorValue() override = default;

    /**
     * Creates a color value
     * @param r red channel
     * @param g green channel
     * @param b blue channel
     * @param a alpha channel
     */
    ColorValue(uint8_t r, uint8_t g, uint8_t b, double a);

    ColorValue(const ColorValue & rhs) = default;

    /**
     * Clones a ColorValue
     * @return cloned value
     */
    ValuePtr clone() const override;

    /**
     * Prints a color declaration value
     * @return printed value
     */
    std::string print() const override;

    /**
     * Returns a vector of RGB color channels
     * @return color channels
     */
    std::vector<uint8_t> channels() const;

    uint8_t r, g, b;
    double  a;
};

/**
 * Represents a CSS selector. Can be a
 * - tag (body, a, p, span)
 * - id (#intro, #user-selection)
 * - class (.full-width, .click-toggle)
 * or any combination of those, but has at most one tag and one id.
 */
struct Selector {
   public:
    /**
     * Creates a CSS Selector
     * @param tag selector tag
     * @param id selector id
     * @param klass selector classes
     */
    explicit Selector(std::string              tag   = "",
                      std::string              id    = "",
                      std::vector<std::string> klass = {});

    /**
     * Determines the specificity of the selector, prioritized by
     * (id, class, tag). High specificity is more important.
     * @return specificity vector
     */
    Specificity specificity() const;

    /**
     * Prints a selector in the form `tag#id.class1.class2`
     * @return pretty-printed selector
     */
    std::string print() const;

    std::string              tag;
    std::string              id;
    std::vector<std::string> klass;
};

/**
 * Orders Selectors by decreasing specificity
 */
struct specificityOrder {
    bool operator()(const Selector & a, const Selector & b) const {
        return a.specificity() > b.specificity();
    }
};

/**
 * Represents a CSS declaration, of form `name: value;`
 */
struct Declaration {
   public:
    /**
     * Creates a CSS declaration
     * @param name declaration name
     * @param value declaration value
     */
    Declaration(std::string name, ValuePtr && value);

    /**
     * Copy ctor
     * @param rhs declaration to copy
     */
    Declaration(const Declaration & rhs);

    /**
     * Prints a declaration in the form `name: value;`
     * @return pretty-printed declaration
     */
    std::string print() const;

    std::string name;
    ValuePtr    value;
};

/**
 * Represents a CSS rule, of form
 * ```
 * .selector {
 *     some-name: arbitraryValue;
 * }
 * ```
 */
struct Rule {
   public:
    /**
     * Creates a CSS rule
     * @param selectors selectors to apply rule for
     * @param declarations declarations of rule
     */
    Rule(PrioritySelectorSet selectors, DeclarationSet declarations);

    PrioritySelectorSet selectors;
    DeclarationSet      declarations;
};

/**
 * A style sheet, consisting of CSS rules. Adapts a vector of rules to allow
 * for visitors.
 */
class StyleSheet : public std::vector<Rule> {
   public:
    /**
     * Accepts a visitor to the style sheet
     * @param visitor accepted visitor
     */
    void acceptVisitor(Visitor & visitor) const;
};
}  // namespace CSS

#endif