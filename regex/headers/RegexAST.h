#pragma once

#include "RegexToken.h"
#include <memory>

class RegexASTNode {
public:
  virtual ~RegexASTNode() = default;
};

class CharacterNode : public RegexASTNode {
public:
  char value;
  CharacterNode(char v) : value(v) {}
};

class UnaryNode : public RegexASTNode {
public:
  Pointer<RegexASTNode> operand;
  UnaryNode(Pointer<RegexASTNode> op) : operand(std::move(op)) {}
};

class BinaryNode : public RegexASTNode {
public:
  Pointer<RegexASTNode> left;
  Pointer<RegexASTNode> right;
  BinaryNode(Pointer<RegexASTNode> l, Pointer<RegexASTNode> r)
      : left(std::move(l)), right(std::move(r)) {}
};
