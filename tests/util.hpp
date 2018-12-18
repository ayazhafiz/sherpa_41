#ifndef TEST_UTIL_HPP
#define TEST_UTIL_HPP

#include "dom.hpp"
#include "printer.hpp"

#include <gtest/gtest.h>

inline void ASSERT_PRINT(const DOM::NodePtr & node, const std::string & res) {
    Printer printer;
    node->acceptVisitor(printer);
    ASSERT_EQ("\n" + printer.result(), res);
}

#endif