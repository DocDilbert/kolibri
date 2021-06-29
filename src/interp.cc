#include <stdint.h>

#include <fstream>
#include <iostream>

#include "base/ast_factory.h"
#include "base/ast_types.h"
#include "base/print_ast.h"
#include "languages/calc/calc_lexer.h"
#include "languages/calc/calc_parser.h"
#include "languages/pascal/pascal_interpreter.h"
#include "languages/pascal/pascal_lexer.h"
#include "languages/pascal/pascal_parser.h"
#include "lexer/lexer.h"
#include "token_out.h"

using namespace base;
using namespace std;
using namespace lexer;
using namespace languages::pascal;
using namespace languages::calc;
using namespace parser;

template <class T>
struct Tag1 {};

template <class T>
struct Tag2 {};

template <template <class> class Tag>
struct IntTag {
  typedef Tag<int> type;
};

// TODO: Klasse zu CalcParser zusammenführen 
using CGrammar = CalculatorGrammar<std::shared_ptr<Ast<MakeShared, CalcToken>>, CalcLexer::iterator_type>;

void doPascal(string content) {
  IntTag<Tag1>::type d;
  PascalLexer lexer(content.c_str(), strlen(content.c_str()));
  AstFactory<std::shared_ptr<Ast<MakeShared, PascalToken>>, PascalToken> ast_factory;
  PascalParserFactory parser_factory(ast_factory);
  PascalParser pparser(parser_factory);

  // CalcLexer lexer(content.c_str(), strlen(content.c_str()));
  // AstFactory<CalcAstTraits> ast_factory;
  // Parser<CGrammar> pparser(ast_factory);

  auto it = lexer.begin();
  auto end = lexer.end();

  for (; it != end; ++it) {
    cout << (*it) << endl;
  }

  auto res = pparser.Expr(lexer.begin(), lexer.end());
  cout << "------------------" << endl;
  cout << "Parsing:" << endl;
  if (res.is_error) {
    cout << "ERROR" << endl;
  }
  cout << res.error_msg << endl;
  PrintAst<std::shared_ptr<Ast<MakeShared, PascalToken>>, PascalToken> show_ast;
  cout << "Writing Ast to output.dot" << endl;
  ofstream dot_file("output.dot");
  show_ast.Print(dot_file, res.node);

  cout << "------------------" << endl;

  PascalInterpreter<std::shared_ptr<Ast<MakeShared, PascalToken>>, PascalToken> pascal_interp;
  auto result = pascal_interp.Interpret(res.node);
  auto var_list = result.ListVariables();
  cout << var_list << endl;
}

int main(int argc, char* argv[]) {
  if (argc == 2) {
    string filename = argv[1];
    ifstream myfile(filename);

    if (myfile.is_open()) {
      std::string str((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>());
      doPascal(str);
    } else {
      cout << "ERROR: Unable to open file \"" << filename << "\"." << endl;
      return -1;
    }
  } else {
    cout << "usage: lexer <filename>" << endl;
  }
  return -1;
}
