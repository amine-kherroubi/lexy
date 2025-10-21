#include "../../include/visualization/regex_ast_visualizer.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

int RegexASTVisualizer::node_counter_ = 0;

String RegexASTVisualizer::escapeLabel(const String &label) {
  String result;
  for (char c : label) {
    if (c == '"')
      result += "\\\"";
    else if (c == '\\')
      result += "\\\\";
    else if (c == '\n')
      result += "\\n";
    else if (c == '\t')
      result += "\\t";
    else if (c == '\r')
      result += "\\r";
    else
      result += c;
  }
  return result;
}

void RegexASTVisualizer::generateDotFile(const String &dot_content,
                                         const String &filename) {
  ofstream file(filename);
  if (!file.is_open()) {
    cerr << "Error: Could not create file " << filename << endl;
    return;
  }
  file << dot_content;
  file.close();
  cout << "Generated DOT file: " << filename << endl;
}

void RegexASTVisualizer::renderDotFile(const String &dot_filename,
                                       const String &image_filename,
                                       const String &output_format) {
  String output_file = image_filename + "." + output_format;
  String command =
      "dot -T" + output_format + " " + dot_filename + " -o " + output_file;

  int result = system(command.c_str());
  if (result == 0) {
    cout << "Rendered visualization: " << output_file << endl;
  } else {
    cerr << "Error: Failed to render with Graphviz. Make sure 'dot' is "
            "installed."
         << endl;
  }
}

void RegexASTVisualizer::visualizeAST(const RegexASTNode *root,
                                      const String &dot_path,
                                      const String &image_path) {
  if (!root) {
    cerr << "Error: Cannot visualize null AST" << endl;
    return;
  }

  node_counter_ = 0;
  ostringstream dot;

  dot << "digraph AST {\n";
  dot << "  node [shape=box, style=rounded];\n";
  dot << "  rankdir=TB;\n\n";

  visitNode(root, dot);

  dot << "}\n";

  String dot_filename = dot_path + ".dot";
  generateDotFile(dot.str(), dot_filename);
  renderDotFile(dot_filename, image_path);
}

int RegexASTVisualizer::visitNode(const RegexASTNode *node,
                                  std::ostringstream &dot) {
  if (!node)
    return -1;

  // Try each node type using dynamic_cast
  if (auto charNode = dynamic_cast<const CharNode *>(node)) {
    return visitChar(charNode, dot);
  } else if (auto dotNode = dynamic_cast<const DotNode *>(node)) {
    return visitDot(dotNode, dot);
  } else if (auto charSetNode = dynamic_cast<const CharSetNode *>(node)) {
    return visitCharSet(charSetNode, dot);
  } else if (auto concatNode = dynamic_cast<const ConcatNode *>(node)) {
    return visitConcat(concatNode, dot);
  } else if (auto altNode = dynamic_cast<const AltNode *>(node)) {
    return visitAlt(altNode, dot);
  } else if (auto starNode = dynamic_cast<const StarNode *>(node)) {
    return visitStar(starNode, dot);
  } else if (auto plusNode = dynamic_cast<const PlusNode *>(node)) {
    return visitPlus(plusNode, dot);
  } else if (auto questionNode = dynamic_cast<const QuestionNode *>(node)) {
    return visitQuestion(questionNode, dot);
  } else if (auto rangeNode = dynamic_cast<const RangeNode *>(node)) {
    return visitRange(rangeNode, dot);
  }

  return -1;
}

int RegexASTVisualizer::visitChar(const CharNode *node,
                                  std::ostringstream &dot) {
  int id = node_counter_++;
  String label = "Char\\n'";
  label += escapeLabel(String(1, node->value_));
  label += "'";
  dot << "  node" << id << " [label=\"" << label << "\"];\n";
  return id;
}

int RegexASTVisualizer::visitDot(const DotNode *node, std::ostringstream &dot) {
  int id = node_counter_++;
  dot << "  node" << id << " [label=\"Dot\\n(.)\"];\n";
  return id;
}

int RegexASTVisualizer::visitCharSet(const CharSetNode *node,
                                     std::ostringstream &dot) {
  int id = node_counter_++;
  String label = "CharSet";
  if (node->negated_) {
    label += "\\n[^";
  } else {
    label += "\\n[";
  }

  // Add chars
  for (char c : node->chars_) {
    label += escapeLabel(String(1, c));
  }

  // Add ranges
  for (const auto &range : node->ranges_) {
    label += escapeLabel(String(1, range.first));
    label += "-";
    label += escapeLabel(String(1, range.second));
  }

  label += "]";
  dot << "  node" << id << " [label=\"" << label << "\"];\n";
  return id;
}

int RegexASTVisualizer::visitConcat(const ConcatNode *node,
                                    std::ostringstream &dot) {
  int id = node_counter_++;
  dot << "  node" << id << " [label=\"Concat\"];\n";

  int leftId = visitNode(node->left_.get(), dot);
  int rightId = visitNode(node->right_.get(), dot);

  if (leftId != -1) {
    dot << "  node" << id << " -> node" << leftId << ";\n";
  }
  if (rightId != -1) {
    dot << "  node" << id << " -> node" << rightId << ";\n";
  }

  return id;
}

int RegexASTVisualizer::visitAlt(const AltNode *node, std::ostringstream &dot) {
  int id = node_counter_++;
  dot << "  node" << id << " [label=\"Alt\\n(|)\"];\n";

  int leftId = visitNode(node->left_.get(), dot);
  int rightId = visitNode(node->right_.get(), dot);

  if (leftId != -1) {
    dot << "  node" << id << " -> node" << leftId << ";\n";
  }
  if (rightId != -1) {
    dot << "  node" << id << " -> node" << rightId << ";\n";
  }

  return id;
}

int RegexASTVisualizer::visitStar(const StarNode *node,
                                  std::ostringstream &dot) {
  int id = node_counter_++;
  dot << "  node" << id << " [label=\"Star\\n(*)\"];\n";

  int childId = visitNode(node->child_.get(), dot);
  if (childId != -1) {
    dot << "  node" << id << " -> node" << childId << ";\n";
  }

  return id;
}

int RegexASTVisualizer::visitPlus(const PlusNode *node,
                                  std::ostringstream &dot) {
  int id = node_counter_++;
  dot << "  node" << id << " [label=\"Plus\\n(+)\"];\n";

  int childId = visitNode(node->child_.get(), dot);
  if (childId != -1) {
    dot << "  node" << id << " -> node" << childId << ";\n";
  }

  return id;
}

int RegexASTVisualizer::visitQuestion(const QuestionNode *node,
                                      std::ostringstream &dot) {
  int id = node_counter_++;
  dot << "  node" << id << " [label=\"Question\\n(?)\"];\n";

  int childId = visitNode(node->child_.get(), dot);
  if (childId != -1) {
    dot << "  node" << id << " -> node" << childId << ";\n";
  }

  return id;
}

int RegexASTVisualizer::visitRange(const RangeNode *node,
                                   std::ostringstream &dot) {
  int id = node_counter_++;
  String label = "Range\\n{" + std::to_string(node->min_) + ",";
  if (node->max_ == -1) {
    label += "}";
  } else {
    label += std::to_string(node->max_) + "}";
  }
  dot << "  node" << id << " [label=\"" << label << "\"];\n";

  int childId = visitNode(node->child_.get(), dot);
  if (childId != -1) {
    dot << "  node" << id << " -> node" << childId << ";\n";
  }

  return id;
}