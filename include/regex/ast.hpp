#pragma once

#include "../core/types.hpp"

class RegexASTNode {
public:
  virtual ~RegexASTNode() = default;
};

class CharNode : public RegexASTNode {
public:
  char value;
  CharNode(char v) : value(v) {}
};

class DotNode : public RegexASTNode {};

class CharSetNode : public RegexASTNode {
public:
  bool negated;
  Vector<char> chars;
  Vector<std::pair<char, char>> ranges;

  CharSetNode(bool neg = false) : negated(neg) {}
};

class ConcatNode : public RegexASTNode {
public:
  Pointer<RegexASTNode> left;
  Pointer<RegexASTNode> right;

  ConcatNode(Pointer<RegexASTNode> l, Pointer<RegexASTNode> r)
      : left(std::move(l)), right(std::move(r)) {}
};

class AltNode : public RegexASTNode {
public:
  Pointer<RegexASTNode> left;
  Pointer<RegexASTNode> right;

  AltNode(Pointer<RegexASTNode> l, Pointer<RegexASTNode> r)
      : left(std::move(l)), right(std::move(r)) {}
};

class StarNode : public RegexASTNode {
public:
  Pointer<RegexASTNode> child;

  StarNode(Pointer<RegexASTNode> c) : child(std::move(c)) {}
};

class PlusNode : public RegexASTNode {
public:
  Pointer<RegexASTNode> child;

  PlusNode(Pointer<RegexASTNode> c) : child(std::move(c)) {}
};

class QuestionNode : public RegexASTNode {
public:
  Pointer<RegexASTNode> child;

  QuestionNode(Pointer<RegexASTNode> c) : child(std::move(c)) {}
};

class RangeNode : public RegexASTNode {
public:
  Pointer<RegexASTNode> child;
  int min;
  int max; // -1 means unbounded

  RangeNode(Pointer<RegexASTNode> c, int min_val, int max_val)
      : child(std::move(c)), min(min_val), max(max_val) {}
};