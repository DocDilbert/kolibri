#ifndef KOLIBRI_SRC_PASCAL_INTERPRETER_H_
#define KOLIBRI_SRC_PASCAL_INTERPRETER_H_

#include <map>
#include <sstream>
#include <string>

#include "languages/ast.h"
#include "languages/i_ast_visitor.h"

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
  using nonterm_type = typename TMakeType<Ast<TMakeType, TTerm>>::type;
  using term_type = TTerm;

  struct RuleResult {
    std::string expr;
    bool error;
    const char* error_msg;
  };

  class Visitor : public IAstVisitor<TMakeType, term_type> {
   public:
    Visitor(PascalState& state) : state_(state), return_double_(0) {}

    void Visit(AstProgram<TMakeType, term_type>& ast) override {
      ast.GetProgram()->Accept(*this);
    }

    void Visit(AstBlock<TMakeType, term_type>& ast) override {
      ast.GetCompoundStatement()->Accept(*this);
    }

    void Visit(AstVariableDeclaration<TMakeType, term_type>& ast) override {}

    void Visit(AstConst<TMakeType, term_type>& ast) override {
      auto const_type = ast.GetConstType();
      auto value = ast.GetValue();

      switch (const_type) {
        case ConstType::kInteger: {
          auto value_i = std::stoi(std::string(value.GetValue()));
          return_double_ = static_cast<double>(value_i);
          break;
        }
        case ConstType::kReal: {
          auto value_d = std::stod(std::string(value.GetValue()));
          return_double_ = value_d;
          break;
        }
        default: {
          break;
        }
      }
    }

    void Visit(AstNop<TMakeType, term_type>& ast) override {}


    void Visit(AstId<TMakeType, term_type>& ast) override {
      std::string var_name = std::string(ast.GetName().GetValue());
      std::for_each(var_name.begin(), var_name.end(), [](char& c) { c = ::tolower(c); });
      return_double_ = state_.Get(std::string(var_name));
    }

    void Visit(AstCompoundStatement<TMakeType, term_type>& ast) override {
      auto statements = ast.GetStatements();
      for (int i = 0; i < statements.size(); i++) {
        auto& statement = statements[i];
        statement->Accept(*this);
      }
    }

    void Visit(AstUnaryOp<TMakeType, term_type>& ast) override {
      auto operand = ast.GetOperand();
      operand->Accept(*this);

      auto type = ast.GetOperator().GetId();
      switch (type) {
        case term_type::id_type::kPlus: {
          return_double_ = +return_double_; 
          break;
        }
        case term_type::id_type::kMinus: {
          return_double_ = -return_double_;
          break;
        }

        default:
          break;
      }
    }
    void Visit(AstBinaryOp<TMakeType, term_type>& ast) override {
      auto operand_lhs = ast.GetOperandLhs();
      auto operand_rhs = ast.GetOperandRhs();
      switch (ast.GetOperator().GetId()) {
        case term_type::id_type::kPlus: {
          operand_lhs->Accept(*this);
          auto lhs = return_double_;
          operand_rhs->Accept(*this);
          auto rhs = return_double_;
          return_double_ = lhs + rhs;
          break;
        }
        case term_type::id_type::kMinus: {
          operand_lhs->Accept(*this);
          auto lhs = return_double_;
          operand_rhs->Accept(*this);
          auto rhs = return_double_;
          return_double_ = lhs - rhs;
          break;
        }
        case term_type::id_type::kMultiply: {
          operand_lhs->Accept(*this);
          auto lhs = return_double_;
          operand_rhs->Accept(*this);
          auto rhs = return_double_;
          return_double_ = lhs * rhs;
          break;
        }
        case term_type::id_type::kIntegerDiv: {
          operand_lhs->Accept(*this);
          auto lhs = return_double_;
          operand_rhs->Accept(*this);
          auto rhs = return_double_;
          return_double_ = lhs / rhs;
          break;
        }
        case term_type::id_type::kFloatDiv: {
          operand_lhs->Accept(*this);
          auto lhs = return_double_;
          operand_rhs->Accept(*this);
          auto rhs = return_double_;
          return_double_ = lhs / rhs;
          break;
        }
        case term_type::id_type::kAssign: {
          assert(operand_lhs->GetTypeId() == AstTypeId::kAstId);
          auto& id = dynamic_cast<AstId<MakeShared, term_type>&>(*operand_lhs);
          
          std::string var_name = std::string(id.GetName().GetValue());
          std::for_each(var_name.begin(), var_name.end(), [](char& c) { c = ::tolower(c); });

          operand_rhs->Accept(*this);

          state_.Assign(var_name, return_double_);
          break;
        }
        default:
          break;
      }
    }

    double GetReturnDouble() { return return_double_; }


   private:
    double return_double_;
    PascalState& state_;
  };

  PascalState Interpret(nonterm_type node) {
    PascalState state;
    Visitor visitor(state);
    node->Accept(visitor);
    
    std::string expr = std::to_string(visitor.GetReturnDouble());

    return state;
  }
};

}  // namespace pascal
}  // namespace languages
#endif