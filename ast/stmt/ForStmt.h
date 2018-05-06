//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_FORSTMT_H
#define DAWN_FORSTMT_H

#include <memory>
#include "Stmt.h"
#include "../Visitor.h"
#include "../expr/Expr.h"
#include "../../lex/Lexeme.h"

namespace ast {
    class ForStmt : public Stmt {
    public:
        std::string Var;
        Lexeme::Kind Operator;
        std::unique_ptr<Expr> Begin, End;
        std::unique_ptr<Stmt> Body;

        ForStmt(std::string Var, Lexeme::Kind Operator, std::unique_ptr<Expr> Begin,
                std::unique_ptr<Expr> End, std::unique_ptr<Stmt> Body) :
                Var(std::move(Var)), Operator(Operator), Begin(std::move(Begin)), End(std::move(End)), Body(std::move(Body)) {}

        void accept(Visitor &V) override {
            V.visit(*this);
        }
    };
}



#endif //DAWN_FORSTMT_H
