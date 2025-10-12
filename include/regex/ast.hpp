#pragma once

#include "../core/types.hpp"

class RegexASTNode {
public:
  virtual ~RegexASTNode() = default;
};

class CharNode : public RegexASTNode {
public:
  char value_;
  CharNode(char v) : value_(v) {}
};

class DotNode : public RegexASTNode {};

class CharSetNode : public RegexASTNode {
public:
  bool negated_;
  Vector<char> chars_;
  Vector<std::pair<char, char>> ranges_;

  CharSetNode(bool neg = false) : negated_(neg) {}
};

class ConcatNode : public RegexASTNode {
public:
  Pointer<RegexASTNode> left_;
  Pointer<RegexASTNode> right_;

  ConcatNode(Pointer<RegexASTNode> l, Pointer<RegexASTNode> r)
      : left_(std::move(l)), right_(std::move(r)) {}
};

class AltNode : public RegexASTNode {
public:
  Pointer<RegexASTNode> left_;
  Pointer<RegexASTNode> right_;

  AltNode(Pointer<RegexASTNode> l, Pointer<RegexASTNode> r)
      : left_(std::move(l)), right_(std::move(r)) {}
};

class StarNode : public RegexASTNode {
public:
  Pointer<RegexASTNode> child_;

  StarNode(Pointer<RegexASTNode> c) : child_(std::move(c)) {}
};

class PlusNode : public RegexASTNode {
public:
  Pointer<RegexASTNode> child_;

  PlusNode(Pointer<RegexASTNode> c) : child_(std::move(c)) {}
};

class QuestionNode : public RegexASTNode {
public:
  Pointer<RegexASTNode> child_;

  QuestionNode(Pointer<RegexASTNode> c) : child_(std::move(c)) {}
};

class RangeNode : public RegexASTNode {
public:
  Pointer<RegexASTNode> child_;
  int min_;
  int max_; // -1 means unbounded

  RangeNode(Pointer<RegexASTNode> c, int min_val, int max_val)
      : child_(std::move(c)), min_(min_val), max_(max_val) {}
};