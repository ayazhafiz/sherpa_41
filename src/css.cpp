// sherpa_41's CSS module, licensed under MIT. (c) hafiz, 2018

#ifndef CSS_CPP
#define CSS_CPP

#include "css.h"

#include "visitor/visitor.h"

/**
 * Normalizes a printed floating point value, e.g.
 *    0.780 -> 0.78
 *    1.000 -> 1
 */
auto CSS::normalizeFp(double value) -> std::string {
  std::string sval{std::to_string(value)};
  auto erase_from = sval.find_last_not_of('0');
  if (sval[erase_from] == '.') {
    --erase_from;
  }
  return sval.substr(0, ++erase_from);
}

/**
 * Clones a ValuePtr from a concrete value.
 */
auto CSS::make_value(const CSS::Value& val) -> CSS::ValuePtr {
  return val.clone();
}

/**
 * Returns whether this CSS value matches a string.
 */
auto CSS::Value::is(const std::string& value) const -> bool {
  return print() == value;
}

/**
 * Returns the unit value, or 0 if not a UnitValue
 * @return unit value, or 0 if not UnitValue
 */
auto CSS::Value::unitValue() const -> double {
  if (auto unit = dynamic_cast<const UnitValue*>(this)) {
    return unit->value;
  }
  return 0;
}

/**
 * Creates a text value
 * @param value text value
 */
CSS::TextValue::TextValue(std::string value) : value(std::move(value)) {}

/**
 * Clones a TextValue
 * @return cloned value
 */
auto CSS::TextValue::clone() const -> CSS::ValuePtr {
  return CSS::ValuePtr(new CSS::TextValue(*this));
}

/**
 * Prints a text declaration value
 * @return printed value
 */
auto CSS::TextValue::print() const -> std::string {
  return value;
}

/**
 * String forms of units
 * @return units as strings
 */
auto CSS::UnitRaw() -> std::vector<std::string> {
  return {"px", "em", "rem", "vw", "vh", "%"};
}

/**
 * Creates a unit value
 * @param value magnitude
 * @param unit unit used
 */
CSS::UnitValue::UnitValue(double value, CSS::Unit unit) : value(value), unit(unit) {}

/**
 * Clones a UnitValue
 * @return cloned value
 */
auto CSS::UnitValue::clone() const -> CSS::ValuePtr {
  return CSS::ValuePtr(new CSS::UnitValue(*this));
}

/**
 * Prints a unit declaration value
 * @return printed value
 */
auto CSS::UnitValue::print() const -> std::string {
  return normalizeFp(value) + CSS::UnitRaw()[unit];
}

/**
 * Creates a color value
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
CSS::ColorValue::ColorValue(uint8_t r, uint8_t g, uint8_t b, double a)
    : r(r), g(g), b(b), a(a) {}

/**
 * Clones a ColorValue
 * @return cloned value
 */
auto CSS::ColorValue::clone() const -> CSS::ValuePtr {
  return CSS::ValuePtr(new CSS::ColorValue(*this));
}

/**
 * Prints a color declaration value
 * @return printed value
 */
auto CSS::ColorValue::print() const -> std::string {
  return "rgba(" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) +
         ", " + normalizeFp(a) + ")";
}

/**
 * Returns an array of RGB color channels
 * @return color channels
 */
auto CSS::ColorValue::channels() const -> std::array<uint8_t, 3> {
  return {{r, g, b}};
}

/**
 * Creates a CSS Selector
 * @param tag selector tag
 * @param id selector id
 * @param klass selector classes
 */
CSS::Selector::Selector(std::string tag, std::string id, std::vector<std::string> klass)
    : tag(std::move(tag)), id(std::move(id)), klass(std::move(klass)) {}

/**
 * Determines the specificity of the selector, prioritized by
 * (id, class, tag). High specificity is more important.
 * @return specificity vector
 */
auto CSS::Selector::specificity() const -> CSS::Specificity {
  return {id.empty() ? 0UL : 1, klass.size(), tag.empty() ? 0UL : 1};
}

/**
 * Prints a selector in the form `tag#id.class1.class2`
 * @return pretty-printed selector
 */
auto CSS::Selector::print() const -> std::string {
  std::string res(tag);
  if (!id.empty()) {
    res += "#" + id;
  }
  res += std::accumulate(klass.begin(), klass.end(), std::string(),
                         [](auto acc, auto cl) { return acc + "." + cl; });
  return res.empty() ? "*" : res;
}

/**
 * Creates a CSS declaration
 * @param name declaration name
 * @param value declaration value
 */
CSS::Declaration::Declaration(std::string name, CSS::ValuePtr&& value)
    : name(std::move(name)), value(std::move(value)) {}

/**
 * Copy ctor
 * @param rhs declaration to copy
 */
CSS::Declaration::Declaration(const CSS::Declaration& rhs)
    : name(rhs.name), value(rhs.value->clone()) {}

/**
 * Prints a declaration in the form `name: value;`
 * @return pretty-printed declaration
 */
auto CSS::Declaration::print() const -> std::string {
  return name + ": " + value->print() + ";";
}

/**
 * Creates a CSS rule
 * @param selectors selectors to apply rule for
 * @param declarations declarations of rule
 */
CSS::Rule::Rule(PrioritySelectorSet selectors, DeclarationSet declarations)
    : selectors(std::move(selectors)), declarations(std::move(declarations)) {}

/**
 * Accepts a visitor to the style sheet
 * @param visitor accepted visitor
 */
void CSS::StyleSheet::acceptVisitor(Visitor& visitor) const {
  visitor.visit(*this);
}

#endif
