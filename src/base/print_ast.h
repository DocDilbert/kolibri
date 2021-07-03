#ifndef KOLIBRI_SRC_INTERPRETER_H_
#define KOLIBRI_SRC_INTERPRETER_H_

#include <iostream>
#include <string>

#include "base/i_ast_visitor.h"

namespace base {

template <template <class> class TMakeType, typename TTerm>
class PrintAst {
 public:
  using nonterm_type = typename TMakeType<Ast<TMakeType, TTerm>>::type;
  using term_type = TTerm;

  class Visitor : public base::IAstVisitor<TMakeType, term_type> {
   public:
    Visitor(std::ostream& stream, unsigned ncount) : stream_(stream), ncount_(ncount) {}

    void Visit(base::AstProgram<TMakeType, term_type>& ast) override {
      auto program_id = ast.GetProgramId();
      assert(program_id->GetTypeId() == AstTypeId::kAstId);

      auto& pname = dynamic_cast<AstId<MakeShared, term_type>&>(*program_id);

      stream_ << "  node" << ncount_ << " [label=\"Program\n" << pname.GetName() << "\"]" << std::endl;
      stream_ << "  node" << ncount_ << " -> node" << ncount_ + 1 << std::endl;
      Visitor visitor(stream_, ncount_ + 1);

      auto program = ast.GetProgram();
      program->Accept(visitor);
      ncount_ = visitor.ncount_;
    }

    void Visit(base::AstBlock<TMakeType, term_type>& ast) override {
      auto root_cnt = ncount_;
      stream_ << "  node" << root_cnt << " [label=\"Block\"]" << std::endl;
      auto var_decls = ast.GetVarDeclarations();
      for (int i = 0; i < var_decls.size(); i++) {
        ncount_++;
        Visitor visitor(stream_, ncount_);
        var_decls[i]->Accept(visitor);
        stream_ << "  node" << root_cnt << " -> node" << ncount_ << std::endl;
        ncount_ = visitor.ncount_;
      }

      ncount_++;
      stream_ << "  node" << root_cnt << " -> node" << ncount_ << std::endl;
      auto compound_statement = ast.GetCompoundStatement();
      Visitor visitor(stream_, ncount_);
      compound_statement->Accept(visitor);
      ncount_ = visitor.ncount_;
    }

    void Visit(base::AstVariableDeclaration<TMakeType, term_type>& ast) override {
      // auto& var_decls_cast = dynamic_cast<AstVariableDeclaration<pointer_type, TTerm>&>(*var_decls[i]);
      stream_ << "  node" << ncount_ << " [label=\"VarDecl\"]" << std::endl;
      auto root_cnt = ncount_;
      ncount_++;
      auto id = ast.GetId();
      stream_ << "  node" << ncount_ << " [label=\"" << id.GetValue() << "\"]" << std::endl;
      stream_ << "  node" << root_cnt << " -> node" << ncount_ << std::endl;

      ncount_++;
      auto type = ast.GetType();
      stream_ << "  node" << ncount_ << " [label=\"" << type.GetValue() << "\"]" << std::endl;
      stream_ << "  node" << root_cnt << " -> node" << ncount_ << std::endl;
    }
    void Visit(base::AstConst<TMakeType, term_type>& ast) override {
      auto const_type = ast.GetConstType();
      auto value = ast.GetValue();

      switch (const_type) {
        case ConstType::kInteger:
          stream_ << "  node" << ncount_ << " [label=\"(int)\n" << value << "\"]" << std::endl;
          break;
        case ConstType::kReal:
          stream_ << "  node" << ncount_ << " [label=\"(float)\n" << value << "\"]" << std::endl;
          break;
        default:
          assert(true);
          break;
      }
    }

    void Visit(base::AstNop<TMakeType, term_type>& ast) override { stream_ << "  node" << ncount_ << " [label=\"Nop\"]" << std::endl; }

    void Visit(base::AstId<TMakeType, term_type>& ast) override {
      stream_ << "  node" << ncount_ << " [label=\"Var:\\n" << ast.GetName() << "\"]" << std::endl;
    }

    void Visit(base::AstUnaryOp<TMakeType, term_type>& ast) override {
      auto type = ast.GetOpType();
      std::string label;
      switch (type) {
        case base::UnaryOpType::kPositiveOp:
          label = "unary +";
          break;
        case base::UnaryOpType::kNegativeOp:
          label = "unary -";
          break;

        default:
          assert(true);
          break;
      }
      auto operand = ast.GetOperand();

      stream_ << "  node" << ncount_ << " [label=\"" << label << "\"]" << std::endl;
      Visitor visitor(stream_, ncount_ + 1);
      operand->Accept(visitor);

      stream_ << "  node" << ncount_ << " -> node" << ncount_ + 1 << std::endl;
      ncount_ = visitor.ncount_;
    }

    void Visit(base::AstCompoundStatement<TMakeType, term_type>& ast) override {
      root_cnt_ = ncount_;
      std::string label = "CompoundStatement";
      stream_ << "  node" << root_cnt_ << " [label=\"" << label << "\"]" << std::endl;
      auto statements = ast.GetStatements();
      for (int i = 0; i < statements.size(); i++) {
        auto& statement = statements[i];
        Visitor visitor(stream_, ncount_ + 1);

        statement->Accept(visitor);
        stream_ << "  node" << root_cnt_ << " -> node" << ncount_ + 1 << std::endl;
        ncount_ = visitor.ncount_;
      }
    }

    void Visit(base::AstBinaryOp<TMakeType, term_type>& ast) override {
      auto op = ast.GetOpType();
      unsigned root_cnt = ncount_;
      std::string label;
      switch (op) {
        case base::BinaryOpType::kAdd:
          label = "+";
          break;
        case base::BinaryOpType::kSub:
          label = "-";
          break;
        case base::BinaryOpType::kMul:
          label = "*";
          break;
        case base::BinaryOpType::kIntegerDiv:
          label = "div";
          break;
        case base::BinaryOpType::kFloatDiv:
          label = "/";
          break;
        case base::BinaryOpType::kAssign:
          label = ":=";
          break;
        default:
          assert(true);
          break;
      }
      stream_ << "  node" << root_cnt << " [label=\"" << label << "\"]" << std::endl;

      Visitor visitor_lhs(stream_, ncount_ + 1);
      auto operand_lhs = ast.GetOperandLhs();
      operand_lhs->Accept(visitor_lhs);
      stream_ << "  node" << root_cnt << " -> node" << ncount_ + 1 << std::endl;
      ncount_ = visitor_lhs.ncount_;

      Visitor visitor_rhs(stream_, ncount_ + 1);
      auto operand_rhs = ast.GetOperandLhs();
      operand_rhs->Accept(visitor_rhs);
      stream_ << "  node" << root_cnt << " -> node" << ncount_ + 1 << std::endl;
      ncount_ = visitor_rhs.ncount_;
    }

   private:
    std::ostream& stream_;
    unsigned ncount_;
    unsigned root_cnt_;
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

}  // namespace base
#endif