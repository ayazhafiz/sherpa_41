#ifndef DOM_DATA_CPP
#define DOM_DATA_CPP

#include "dom.hpp"

#include <numeric>

/**
 * Inserts an attribute
 * @param attribute attribute to add
 * @param value value of attribute
 */
void DOM::AttributeMap::insert(const std::string & attribute,
                               const std::string & value) {
    if (this->find(attribute) == end()) {
        (*this)[attribute] = value;
        order.push_back(attribute);
    }
}

std::string DOM::AttributeMap::print() const {
    if (size() == 0) {
        return "";
    }
    auto        start = order.begin();
    std::string first = order.front();
    return std::accumulate(std::next(start),
                           order.end(),
                           first + "=\"" +
                               const_cast<AttributeMap &>(*this)[first] + "\"",
                           [this](auto acc, auto attr) {
                               return acc + " " + attr + "=\"" +
                                      const_cast<AttributeMap &>(*this)[attr] +
                                      "\"";
                           });
}

#endif