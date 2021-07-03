#ifndef KOLIBRI_SRC_PASCAL_INTERPRETER_H_
#define KOLIBRI_SRC_PASCAL_INTERPRETER_H_

#include <map>
#include <sstream>
#include <string>

#include "base/ast_types.h"
#include "base/i_ast_visitor.h"

namespace languages {
namespace pascal {

class PascalState {
 public:
  PascalState() : global_scope_() {}

  void Assign(std::string var_name, double value) { global_scope_[var_name] = value; }

  double Get(std::string var_name) { return global_scope_[var_name]; }

  std::string ListVariables() {
    std::stringstream ss;
    for (auto it = global_scope_.begin(); it != global_scope_.end(); ++it) {
      auto entry = *it;
      ss << entry.first << " := " << entry.second << std::endl;
    }
    return ss.str();
  }

 private:
  std::map<std::string, double> global_scope_;
};

template <template <class> class TMakeType, typename TTerm>
class PascalInterpreter {
 public:
  using nonterm_type = typename TMakeType<base::Ast<TMakeType, TTerm>>::type;
  using term_type = TTerm;

  struct RuleResult {
    std::string expr;
    bool error;
    const char* error_msg;
  };

  class Visitor : public base::IAstVisitor<TMakeType, term_type> {
   public:
    Visitor(PascalState& state) : state_(state), last_num_(0) {}
    Visitor(Visitor const& visitor) : state_(visitor.state_), last_num_(0) {}

    void Visit(base::AstProgram<TMakeType, term_type>& ast) override {
      Visitor visitor(*this);
      ast.GetProgram()->Accept(visitor);
    }

    void Visit(base::AstBlock<TMakeType, term_type>& ast) override {
      Visitor visitor(*this);
      ast.GetCompoundStatement()->Accept(visitor);
    }

    void Visit(base::AstVariableDeclaration<TMakeType, term_type>& ast) override {}

    void Visit(base::AstConst<TMakeType, term_type>& ast) override {
      auto const_type = ast.GetConstType();
      auto value = ast.GetValue();

      switch (const_type) {
        case base::ConstType::kInteger: {
          auto value_i = std::stoi(value);
          last_num_ = static_cast<double>(value_i);
          break;
        }
        case base::ConstType::kReal: {
          auto value_d = std::stod(value);
          last_num_ = value_d;
          break;
        }
        default: {
          break;
        }
      }
    }

    void Visit(base::AstNop<TMakeType, term_type>& ast) override {}
    void Visit(base::AstId<TMakeType, term_type>& ast) override { last_num_ = state_.Get(ast.GetName()); }

    void Visit(base::AstCompoundStatement<TMakeType, term_type>& ast) override {
      auto statements = ast.GetStatements();
      for (int i = 0; i < statements.size(); i++) {
        auto& statement = statements[i];
        Visitor visitor(*this);

        statement->Accept(visitor);
      }
    }

    void Visit(base::AstUnaryOp<TMakeType, term_type>& ast) override {
      Visitor visitor(*this);
      auto operand = ast.GetOperand();
      operand->Accept(visitor);

      auto type = ast.GetOpType();
      switch (type) {
        case base::UnaryOpType::kPositiveOp:
          last_num_ = visitor.GetLastNum();
          break;
        case base::UnaryOpType::kNegativeOp:
          last_num_ = -visitor.GetLastNum();
          break;

        default:
          break;
      }
    }
    void Visit(base::AstBinaryOp<TMakeType, term_type>& ast) override {
      auto operand_lhs = ast.GetOperandLhs();
      auto operand_rhs = ast.GetOperandRhs();
      switch (ast.GetOpType()) {
        case base::BinaryOpType::kAdd: {
          Visitor visitor_lhs(*this);
          Visitor visitor_rhs(*this);

          operand_lhs->Accept(visitor_lhs);
          operand_rhs->Accept(visitor_rhs);
          last_num_ = visitor_lhs.GetLastNum() + visitor_rhs.GetLastNum();
          break;
        }
        case base::BinaryOpType::kSub: {
          Visitor visitor_lhs(*this);
          Visitor visitor_rhs(*this);

          operand_lhs->Accept(visitor_lhs);
          operand_rhs->Accept(visitor_rhs);
          last_num_ = visitor_lhs.GetLastNum() - visitor_rhs.GetLastNum();
          break;
        }
        case base::BinaryOpType::kMul: {
          Visitor visitor_lhs(*this);
          Visitor visitor_rhs(*this);

          operand_lhs->Accept(visitor_lhs);
          operand_rhs->Accept(visitor_rhs);
          last_num_ = visitor_lhs.GetLastNum() * visitor_rhs.GetLastNum();
          break;
        }
        case base::BinaryOpType::kIntegerDiv: {
          Visitor visitor_lhs(*this);
          Visitor visitor_rhs(*this);

          operand_lhs->Accept(visitor_lhs);
          operand_rhs->Accept(visitor_rhs);
          last_num_ = visitor_lhs.GetLastNum() / visitor_rhs.GetLastNum();
          break;
        }
        case base::BinaryOpType::kFloatDiv: {
          Visitor visitor_lhs(*this);
          Visitor visitor_rhs(*this);

          operand_lhs->Accept(visitor_lhs);
          operand_rhs->Accept(visitor_rhs);
          last_num_ = visitor_lhs.GetLastNum() / visitor_rhs.GetLastNum();
          break;
        }
        case base::BinaryOpType::kAssign: {
          Visitor visitor_rhs(*this);

          assert(operand_lhs->GetTypeId() == base::AstTypeId::kAstId);
          auto& id = dynamic_cast<base::AstId<base::MakeShared, term_type>&>(*operand_lhs);
          operand_rhs->Accept(visitor_rhs);

          state_.Assign(id.GetName(), visitor_rhs.GetLastNum());
          break;
        }
        default:
          break;
      }
    }

    double GetLastNum() { return last_num_; }

   private:
    double last_num_;

    PascalState& state_;
  };

  PascalState Interpret(nonterm_type node) {
    PascalState state;
    Visitor visitor(state);

    node->Accept(visitor);
    std::string expr = std::to_string(visitor.GetLastNum());

    return state;
  }
};

}  // namespace pascal
}  // namespace languages
#endif