// sherpa_41's CSS module, licensed under MIT. (c) hafiz, 2018

#ifndef CSS_CPP
#define CSS_CPP

#include "css.hpp"

#include "visitor/visitor.hpp"

/**
 * Normalizes a printed floating point value
 * 1.00000 -> 1.0
 * @param value value to normalize
 * @return normalized string
 */
std::string CSS::normalizeFp(double value) {
    auto     val    = std::to_string(value);
    uint64_t offset = 1;
    if (val.find_last_not_of('0') == val.find('.')) {
        offset = 0;
    }
    val.erase(val.find_last_not_of('0') + offset, std::string::npos);
    return val;
}

/**
 * Creates a ValuePtr from a concrete value
 * @param val value to create a pointer of
 * @return ValuePtr
 */
CSS::ValuePtr CSS::make_value(const CSS::Value & val) {
    return val.clone();
}

/**
 * Returns if *this is of specified value
 * @param value value to compare
 * @return whether *this is `value`
 */
bool CSS::Value::is(std::string value) const {
    return print() == value;
}

/**
 * Returns the unit value, or 0 if not a UnitValue
 * @return unit value, or 0 if not UnitValue
 */
double CSS::Value::unitValue() const {
    if (auto unit = dynamic_cast<const UnitValue *>(this)) {
        return unit->value;
    }
    return 0;
}

/**
 * Creates a text value
 * @param value text value
 */
CSS::TextValue::TextValue(std::string value) : value(std::move(value)) {
}

/**
 * Clones a TextValue
 * @return cloned value
 */
CSS::ValuePtr CSS::TextValue::clone() const {
    return CSS::ValuePtr(new CSS::TextValue(*this));
}

/**
 * Prints a text declaration value
 * @return printed value
 */
std::string CSS::TextValue::print() const {
    return value;
}

/**
 * String forms of units
 * @return units as strings
 */
std::vector<std::string> CSS::UnitRaw() {
    return {"px", "em", "rem", "vw", "vh", "%"};
}

/**
 * Creates a unit value
 * @param value magnitude
 * @param unit unit used
 */
CSS::UnitValue::UnitValue(double value, CSS::Unit unit)
    : value(value), unit(unit) {
}

/**
 * Clones a UnitValue
 * @return cloned value
 */
CSS::ValuePtr CSS::UnitValue::clone() const {
    return CSS::ValuePtr(new CSS::UnitValue(*this));
}

/**
 * Prints a unit declaration value
 * @return printed value
 */
std::string CSS::UnitValue::print() const {
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
    : r(r), g(g), b(b), a(a) {
}

/**
 * Clones a ColorValue
 * @return cloned value
 */
CSS::ValuePtr CSS::ColorValue::clone() const {
    return CSS::ValuePtr(new CSS::ColorValue(*this));
}

/**
 * Prints a color declaration value
 * @return printed value
 */
std::string CSS::ColorValue::print() const {
    return "rgba(" + std::to_string(r) + ", " + std::to_string(g) + ", " +
           std::to_string(b) + ", " + normalizeFp(a) + ")";
}

/**
 * Returns a vector of RGB color channels
 * @return color channels
 */
std::vector<uint8_t> CSS::ColorValue::channels() const {
    return {r, g, b};
}

/**
 * Creates a CSS Selector
 * @param tag selector tag
 * @param id selector id
 * @param klass selector classes
 */
CSS::Selector::Selector(std::string              tag,
                        std::string              id,
                        std::vector<std::string> klass)
    : tag(std::move(tag)), id(std::move(id)), klass(std::move(klass)) {
}

/**
 * Determines the specificity of the selector, prioritized by
 * (id, class, tag). High specificity is more important.
 * @return specificity vector
 */
CSS::Specificity CSS::Selector::specificity() const {
    return {id.empty() ? 0UL : 1, klass.size(), tag.empty() ? 0UL : 1};
}

/**
 * Prints a selector in the form `tag#id.class1.class2`
 * @return pretty-printed selector
 */
std::string CSS::Selector::print() const {
    std::string res(tag);
    if (!id.empty()) {
        res += "#" + id;
    }
    res += std::accumulate(klass.begin(),
                           klass.end(),
                           std::string(),
                           [](auto acc, auto cl) { return acc + "." + cl; });
    return res.empty() ? "*" : res;
}

/**
 * Creates a CSS declaration
 * @param name declaration name
 * @param value declaration value
 */
CSS::Declaration::Declaration(std::string name, CSS::ValuePtr && value)
    : name(std::move(name)), value(std::move(value)) {
}

/**
 * Copy ctor
 * @param rhs declaration to copy
 */
CSS::Declaration::Declaration(const CSS::Declaration & rhs)
    : name(rhs.name), value(rhs.value->clone()) {
}

/**
 * Prints a declaration in the form `name: value;`
 * @return pretty-printed declaration
 */
std::string CSS::Declaration::print() const {
    return name + ": " + value->print() + ";";
}

/**
 * Creates a CSS rule
 * @param selectors selectors to apply rule for
 * @param declarations declarations of rule
 */
CSS::Rule::Rule(PrioritySelectorSet selectors, DeclarationSet declarations)
    : selectors(std::move(selectors)), declarations(std::move(declarations)) {
}

/**
 * Accepts a visitor to the style sheet
 * @param visitor accepted visitor
 */
void CSS::StyleSheet::acceptVisitor(Visitor & visitor) const {
    visitor.visit(*this);
}

#endif