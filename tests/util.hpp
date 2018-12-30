#ifndef TEST_UTIL_HPP
#define TEST_UTIL_HPP

#include "css.hpp"
#include "dom.hpp"

#include "visitor/printer.hpp"

#include <gtest/gtest.h>

template <typename Host>
inline void ASSERT_PRINT(const Host & node, const std::string & res) {
    Printer printer;
    node->acceptVisitor(printer);
    ASSERT_EQ("\n" + printer.result(), res);
}

#endif