#ifndef KOLIBRI_SRC_INTERPRETER_H_
#define KOLIBRI_SRC_INTERPRETER_H_

#include <iostream>
#include <string>

#include "languages/ast_types.h"
#include "languages/i_ast_visitor.h"

namespace languages {

template <template <class> class TMakeType, typename TTerm>
class PrintAst {
 public:
  using nonterm_type = typename TMakeType<Ast<TMakeType, TTerm>>::type;
  using term_type = TTerm;

  class Visitor : public IAstVisitor<TMakeType, term_type> {
   public:
    Visitor(std::ostream& stream, unsigned ncount) : stream_(stream), ncount_(ncount) {}

    void Visit(AstProgram<TMakeType, term_type>& ast) override {
      auto program_id = ast.GetProgramId();
      assert(program_id->GetTypeId() == AstTypeId::kAstId);

      auto& pname = dynamic_cast<AstId<MakeShared, term_type>&>(*program_id);

      stream_ << "  node" << ncount_ << " [label=\"Program\n" << pname.GetName().GetValue() << "\"]" << std::endl;
      auto orig_count = ncount_;

      ncount_++;
      stream_ << "  node" << orig_count << " -> node" << ncount_ << std::endl;

      auto program = ast.GetProgram();
      program->Accept(*this);
    }

    void Visit(AstBlock<TMakeType, term_type>& ast) override {
      auto orig_cnt = ncount_;
      stream_ << "  node" << orig_cnt << " [label=\"Block\"]" << std::endl;
      auto var_decls = ast.GetVarDeclarations();
      for (int i = 0; i < var_decls.size(); i++) {
        ncount_++;
        stream_ << "  node" << orig_cnt << " -> node" << ncount_ << std::endl;
        var_decls[i]->Accept(*this);
      }

      ncount_++;
      stream_ << "  node" << orig_cnt << " -> node" << ncount_ << std::endl;
      auto compound_statement = ast.GetCompoundStatement();
      compound_statement->Accept(*this);
    }

    void Visit(AstVariableDeclaration<TMakeType, term_type>& ast) override {
      // auto& var_decls_cast = dynamic_cast<AstVariableDeclaration<pointer_type, TTerm>&>(*var_decls[i]);
      stream_ << "  node" << ncount_ << " [label=\"VarDecl\"]" << std::endl;
      auto orig_cnt = ncount_;
      ncount_++;
      auto id = ast.GetId();
      stream_ << "  node" << ncount_ << " [label=\"" << id.GetValue() << "\"]" << std::endl;
      stream_ << "  node" << orig_cnt << " -> node" << ncount_ << std::endl;

      ncount_++;
      auto type = ast.GetType();
      stream_ << "  node" << ncount_ << " [label=\"" << type.GetValue() << "\"]" << std::endl;
      stream_ << "  node" << orig_cnt << " -> node" << ncount_ << std::endl;
    }

    void Visit(AstConst<TMakeType, term_type>& ast) override {
      auto const_type = ast.GetConstType();
      auto value = ast.GetValue();

      switch (const_type) {
        case ConstType::kInteger:
          stream_ << "  node" << ncount_ << " [label=\"(int)\n" << value.GetValue() << "\"]" << std::endl;
          break;
        case ConstType::kReal:
          stream_ << "  node" << ncount_ << " [label=\"(float)\n" << value.GetValue() << "\"]" << std::endl;
          break;
        default:
          assert(true);
          break;
      }
    }

    void Visit(AstNop<TMakeType, term_type>& ast) override { stream_ << "  node" << ncount_ << " [label=\"Nop\"]" << std::endl; }

    void Visit(AstId<TMakeType, term_type>& ast) override {
      stream_ << "  node" << ncount_ << " [label=\"Var:\\n" << ast.GetName().GetValue() << "\"]" << std::endl;
    }

    void Visit(AstUnaryOp<TMakeType, term_type>& ast) override {
      auto op_val = ast.GetOperator().GetValue();

      auto operand = ast.GetOperand();

      stream_ << "  node" << ncount_ << " [label=\"(unary)\n" << op_val << "\"]" << std::endl;
      auto orig_cnt = ncount_;
      ncount_++;

      stream_ << "  node" << orig_cnt << " -> node" << ncount_ << std::endl;

      operand->Accept(*this);
    }

    void Visit(AstCompoundStatement<TMakeType, term_type>& ast) override {
      auto orig_cnt_ = ncount_;
      std::string label = "CompoundStatement";
      stream_ << "  node" << orig_cnt_ << " [label=\"" << label << "\"]" << std::endl;
      auto statements = ast.GetStatements();
      for (int i = 0; i < statements.size(); i++) {
        auto& statement = statements[i];
        ncount_++;

        stream_ << "  node" << orig_cnt_ << " -> node" << ncount_ << std::endl;
        statement->Accept(*this);
      }
    }

    void Visit(AstBinaryOp<TMakeType, term_type>& ast) override {
      auto op_val = ast.GetOperator().GetValue();

      unsigned orig_cnt = ncount_;
      stream_ << "  node" << orig_cnt << " [label=\"" << op_val << "\"]" << std::endl;

      auto operand_lhs = ast.GetOperandLhs();
      ncount_++;
      stream_ << "  node" << orig_cnt << " -> node" << ncount_ << std::endl;
      operand_lhs->Accept(*this);

      auto operand_rhs = ast.GetOperandRhs();
      ncount_++;
      stream_ << "  node" << orig_cnt << " -> node" << ncount_ << std::endl;
      operand_rhs->Accept(*this);
    }

   private:
    std::ostream& stream_;
    unsigned ncount_;
  };

  void Print(std::ostream& stream, nonterm_type node) {
    stream << "digraph astgraph {" << std::endl;
    stream << "node [shape=circle, fontsize=12, fontname=\"Courier\", height=.1];" << std::endl;
    stream << "ranksep=.3;" << std::endl;
    stream << "edge [arrowsize=.5]" << std::endl;
    stream << std::endl;
    Visitor visitor(stream, 0);
    node->Accept(visitor);
    stream << "}" << std::endl;
  }
};

}  // namespace languages
#endif