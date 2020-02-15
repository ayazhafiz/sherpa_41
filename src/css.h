// sherpa_41's CSS module, licensed under MIT. (c) hafiz, 2018

#ifndef CSS_HPP
#define CSS_HPP

#include <array>
#include <memory>
#include <numeric>
#include <set>
#include <vector>

#include "parser/parser.h"

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

using ValuePtr = std::unique_ptr<Value>;
using Specificity = std::vector<uint64_t>;
using PrioritySelectorSet = std::multiset<Selector, specificityOrder>;
using DeclarationSet = std::vector<Declaration>;

/**
 * Normalizes a printed floating point value
 * 1.00000 -> 1.0
 * @param value value to normalize
 * @return normalized string
 */
auto normalizeFp(double value) -> std::string;

/**
 * Creates a ValuePtr from a concrete value
 * @param val value to create a pointer of
 * @return ValuePtr
 */
auto make_value(const Value& val) -> ValuePtr;

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
  [[nodiscard]] virtual auto clone() const -> ValuePtr = 0;

  /**
   * Prints a declaration value
   * @return printed value
   */
  [[nodiscard]] virtual auto print() const -> std::string = 0;

  /**
   * Returns if *this is of specified value
   * @param value value to compare
   * @return whether *this is `value`
   */
  [[nodiscard]] virtual auto is(const std::string& value) const -> bool;

  /**
   * Returns the unit value, or 0 if not a UnitValue
   * @return unit value, or 0 if not UnitValue
   */
  [[nodiscard]] virtual auto unitValue() const -> double;
};

/**
 * A text declaration value
 */
struct TextValue : public Value {
 public:
  TextValue() = delete;
  ~TextValue() override = default;

  /**
   * Creates a text value
   * @param value text value
   */
  explicit TextValue(std::string value);

  TextValue(const TextValue& rhs) = default;

  /**
   * Clones a TextValue
   * @return cloned value
   */
  [[nodiscard]] auto clone() const -> ValuePtr override;

  /**
   * Prints a text declaration value
   * @return printed value
   */
  [[nodiscard]] auto print() const -> std::string override;

  std::string value;
};

enum Unit { px, em, rem, vw, vh, percent };
auto UnitRaw() -> std::vector<std::string>;
/**
 * A unit declaration value
 */
struct UnitValue : public Value {
 public:
  UnitValue() = delete;
  ~UnitValue() override = default;

  /**
   * Creates a unit value
   * @param value magnitude
   * @param unit unit used
   */
  UnitValue(double value, Unit unit);

  UnitValue(const UnitValue& rhs) = default;

  /**
   * Clones a UnitValue
   * @return cloned value
   */
  [[nodiscard]] auto clone() const -> ValuePtr override;

  /**
   * Prints a unit declaration value
   * @return printed value
   */
  [[nodiscard]] auto print() const -> std::string override;

  double value;
  Unit unit;
};

/**
 * A color declaration value
 */
struct ColorValue : public Value {
 public:
  ColorValue() = delete;
  ~ColorValue() override = default;

  /**
   * Creates a color value
   * @param r red channel
   * @param g green channel
   * @param b blue channel
   * @param a alpha channel
   */
  ColorValue(uint8_t r, uint8_t g, uint8_t b, double a);

  ColorValue(const ColorValue& rhs) = default;

  /**
   * Clones a ColorValue
   * @return cloned value
   */
  [[nodiscard]] auto clone() const -> ValuePtr override;

  /**
   * Prints a color declaration value
   * @return printed value
   */
  [[nodiscard]] auto print() const -> std::string override;

  /**
   * Returns an array of RGB color channels
   * @return color channels
   */
  [[nodiscard]] auto channels() const -> std::array<uint8_t, 3>;

  uint8_t r, g, b;
  double a;
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
  explicit Selector(std::string tag = "",
                    std::string id = "",
                    std::vector<std::string> klass = {});

  /**
   * Determines the specificity of the selector, prioritized by
   * (id, class, tag). High specificity is more important.
   * @return specificity vector
   */
  [[nodiscard]] auto specificity() const -> Specificity;

  /**
   * Prints a selector in the form `tag#id.class1.class2`
   * @return pretty-printed selector
   */
  [[nodiscard]] auto print() const -> std::string;

  std::string tag;
  std::string id;
  std::vector<std::string> klass;
};

/**
 * Orders Selectors by decreasing specificity
 */
struct specificityOrder {
  auto operator()(const Selector& a, const Selector& b) const -> bool {
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
  Declaration(std::string name, ValuePtr&& value);

  /**
   * Copy ctor
   * @param rhs declaration to copy
   */
  Declaration(const Declaration& rhs);

  /**
   * Prints a declaration in the form `name: value;`
   * @return pretty-printed declaration
   */
  [[nodiscard]] auto print() const -> std::string;

  std::string name;
  ValuePtr value;
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
  DeclarationSet declarations;
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
  void acceptVisitor(Visitor& visitor) const;
};
}  // namespace CSS

#endif
