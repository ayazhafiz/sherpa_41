// sherpa_41's Base Parser, licensed under MIT. (c) hafiz, 2018

#ifndef PARSER_CPP
#define PARSER_CPP

#include "parser/parser.h"

#include <cassert>

/**
 * Constructs a Parser
 * @param program program to parse
 */
template <typename EvalType>
Parser<EvalType>::Parser(std::string program) : program(std::move(program)), ptr(0ULL) {}

/**
 * Builds a string of some length
 * @param len length of string to build
 * @return built string
 */
template <typename EvalType>
auto Parser<EvalType>::build(uint64_t len) -> std::string {
  return build_until([&len](char) { return len-- == 0; });
}

/**
 * Builds a string from the program until predicate satisfied
 * @param predicate when to stop building
 * @return built string
 */
template <typename EvalType>
auto Parser<EvalType>::build_until(Parser<EvalType>::PrefixComparator& predicate)
    -> std::string {
  consume_whitespace();
  std::string res;
  while (!eof() && !peek(predicate)) {
    res += program[ptr];
    pushPtr();
  }
  return res;
}

/**
 * Ensures that the next characters are as expected, then pushes
 * the program pointer
 * @param next characters to ensure
 */
template <typename EvalType>
void Parser<EvalType>::consume(const std::string& next) {
  assert(peek(next));
  pushPtr(next.length());
}

/**
 * Consumes whitespace, then optionally ensures next characters are as
 * expected
 * @param next characters to ensure
 */
template <typename EvalType>
void Parser<EvalType>::consume_whitespace(const std::string& next) {
  while (std::isspace(program[ptr])) {
    pushPtr();
  }
  consume(next);
}

/**
 * Pushes the program pointer some units ahead
 * @param dist distance to push pointer
 */
template <typename EvalType>
void Parser<EvalType>::pushPtr(uint64_t dist) {
  ptr += dist;
}

/**
 * Determines the next characters of the program
 * @param prefix characters to match program to
 * @return whether program contains `prefix` next
 */
template <typename EvalType>
auto Parser<EvalType>::peek(const std::string& prefix) const -> bool {
  return program.rfind(prefix, ptr) == ptr;
}

/**
 * Determines the next character of the program
 * @param predicate lambda to match prefix to
 * @return whether program prefix satisfies lambda
 */
template <typename EvalType>
auto Parser<EvalType>::peek(const Parser<EvalType>::PrefixComparator& predicate) const
    -> bool {
  return predicate(program[ptr]);
}

/**
 * Determines if entire program read
 * @return program read?
 */
template <typename EvalType>
auto Parser<EvalType>::eof() const -> bool {
  return ptr >= program.length();
}

#endif
