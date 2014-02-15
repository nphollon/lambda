#ifndef H_NODE
#define H_NODE

#include <string>
#include <vector>

using namespace std;

class Node {
public:
  static const Node* byName(const string&);
  static const Node* byChildren(const Node&, const Node&);
  static const Node* parse(const string&);

  bool isTerminal() const;
  const string getName() const;
  const Node* getApplicator() const;
  const Node* getInput() const;

  bool operator==(const Node&) const;
  bool operator!=(const Node&) const;

  ~Node();  

private:
  const string* name;
  const Node* applicator;
  const Node* input;

  Node(const string&, const Node*, const Node*);

  static const Node* copy(const Node& n);

  static const string WHITESPACE;
  static const string NON_NAME_CHARS;

  static const Node* constructParseTree(const string&);
  static bool validateParens(const string&);
  static vector<string> splitAtLastToken(const string&);
  static string trim(const string&);
  static bool isWrapped(const string&);
  static int findLastOpenParen(const string&);
};

std::ostream& operator<<(std::ostream& os, const Node& obj);

#endif
