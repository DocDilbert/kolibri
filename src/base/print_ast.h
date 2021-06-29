#ifndef INTERTEST_SRC_INTERPRETER_H_
#define INTERTEST_SRC_INTERPRETER_H_

#include <iostream>
#include <string>

#include "base/i_ast_visitor.h"

namespace base {

template <typename TNonTerm, typename TTerm>
class PrintAst {
 public:
  using nonterm_type = TNonTerm;
  using term_type = TTerm;

  class Visitor : public base::IAstVisitor<nonterm_type, term_type> {
   public:
    Visitor(std::ostream& stream, unsigned ncount) : stream_(stream), ncount_(ncount) {}

    void VisitProgram(nonterm_type program_id, nonterm_type program) override {
      assert(program_id->GetTypeId() == AstTypeId::kAstId);

      auto& pname = dynamic_cast<AstId<MakeShared, term_type>&>(*program_id);

      stream_ << "  node" << ncount_ << " [label=\"Program\n" << pname.GetName() << "\"]" << std::endl;
      stream_ << "  node" << ncount_ << " -> node" << ncount_ + 1 << std::endl;
      Visitor visitor(stream_, ncount_ + 1);
      program->Visit(visitor);
      ncount_ = visitor.ncount_;
    }

    void VisitBlock(std::vector<nonterm_type> const& var_decls, nonterm_type compound_statement) override {
      auto root_cnt = ncount_;
      stream_ << "  node" << root_cnt << " [label=\"Block\"]" << std::endl;

      for (int i = 0; i < var_decls.size(); i++) {
        ncount_++;
        Visitor visitor(stream_, ncount_);
        var_decls[i]->Visit(visitor);
        stream_ << "  node" << root_cnt << " -> node" << ncount_ << std::endl;
        ncount_ = visitor.ncount_;
      }

      ncount_++;
      stream_ << "  node" << root_cnt << " -> node" << ncount_ << std::endl;

      Visitor visitor(stream_, ncount_);
      compound_statement->Visit(visitor);
      ncount_ = visitor.ncount_;
    }

    void VisitVariableDeclaration(term_type id, term_type type) override {
      // auto& var_decls_cast = dynamic_cast<AstVariableDeclaration<pointer_type, TTerm>&>(*var_decls[i]);
      stream_ << "  node" << ncount_ << " [label=\"VarDecl\"]" << std::endl;
      auto root_cnt = ncount_;
      ncount_++;
      stream_ << "  node" << ncount_ << " [label=\"" << id.GetValue() << "\"]" << std::endl;
      stream_ << "  node" << root_cnt << " -> node" << ncount_ << std::endl;

      ncount_++;
      stream_ << "  node" << ncount_ << " [label=\"" << type.GetValue() << "\"]" << std::endl;
      stream_ << "  node" << root_cnt << " -> node" << ncount_ << std::endl;
    }
    void VisitIntegerConst(ConstType const_type, std::string value) override {
      switch (const_type) {
        case ConstType::INTEGER:
          stream_ << "  node" << ncount_ << " [label=\"(int)\n" << value << "\"]" << std::endl;
          break;
        case ConstType::REAL:
          stream_ << "  node" << ncount_ << " [label=\"(float)\n" << value << "\"]" << std::endl;
          break;
        default:
          assert(true);
          break;
      }
    }

    void VisitNop() override { stream_ << "  node" << ncount_ << " [label=\"Nop\"]" << std::endl; }

    void VisitId(std::string name) override { stream_ << "  node" << ncount_ << " [label=\"Var:\\n" << name << "\"]" << std::endl; }

    void VisitUnaryOp(base::UnaryOpType type, nonterm_type operand) override {
      std::string label;
      switch (type) {
        case base::UnaryOpType::POSITIVE_OP:
          label = "unary +";
          break;
        case base::UnaryOpType::NEGATIVE_OP:
          label = "unary -";
          break;

        default:
          assert(true);
          break;
      }

      stream_ << "  node" << ncount_ << " [label=\"" << label << "\"]" << std::endl;
      Visitor visitor(stream_, ncount_ + 1);
      operand->Visit(visitor);

      stream_ << "  node" << ncount_ << " -> node" << ncount_ + 1 << std::endl;
      ncount_ = visitor.ncount_;
    }

    void VisitCompoundStatement(std::vector<nonterm_type> const& statements) override {
      root_cnt_ = ncount_;
      std::string label = "CompoundStatement";
      stream_ << "  node" << root_cnt_ << " [label=\"" << label << "\"]" << std::endl;

      for (int i = 0; i < statements.size(); i++) {
        auto& statement = statements[i];
        Visitor visitor(stream_, ncount_ + 1);

        statement->Visit(visitor);
        stream_ << "  node" << root_cnt_ << " -> node" << ncount_ + 1 << std::endl;
        ncount_ = visitor.ncount_;
      }
    }

    void VisitBinaryOp(base::BinaryOpType op, nonterm_type operand_lhs, nonterm_type operand_rhs) override {
      unsigned root_cnt = ncount_;
      std::string label;
      switch (op) {
        case base::BinaryOpType::ADD:
          label = "+";
          break;
        case base::BinaryOpType::SUB:
          label = "-";
          break;
        case base::BinaryOpType::MUL:
          label = "*";
          break;
        case base::BinaryOpType::INTEGER_DIV:
          label = "div";
          break;
        case base::BinaryOpType::FLOAT_DIV:
          label = "/";
          break;
        case base::BinaryOpType::ASSSIGN:
          label = ":=";
          break;
        default:
          assert(true);
          break;
      }
      stream_ << "  node" << root_cnt << " [label=\"" << label << "\"]" << std::endl;

      Visitor visitor_lhs(stream_, ncount_ + 1);
      operand_lhs->Visit(visitor_lhs);
      stream_ << "  node" << root_cnt << " -> node" << ncount_ + 1 << std::endl;
      ncount_ = visitor_lhs.ncount_;

      Visitor visitor_rhs(stream_, ncount_ + 1);
      operand_rhs->Visit(visitor_rhs);
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
    node->Visit(visitor);
    stream << "}" << std::endl;
  }
};

}  // namespace base
#endif