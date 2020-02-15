// sherpa_41's test fixture utilities, licensed under MIT. (c) hafiz, 2018

#ifndef TEST_UTIL_HPP
#define TEST_UTIL_HPP

#include <gtest/gtest.h>

#include "css.h"
#include "dom.h"
#include "visitor/printer.h"

template <typename Host>
inline void ASSERT_PRINT(const Host& node, const std::string& res) {
  Printer printer;
  node->acceptVisitor(printer);
  ASSERT_EQ("\n" + printer.result(), res);
}

#endif
