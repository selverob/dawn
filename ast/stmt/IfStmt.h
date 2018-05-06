//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_IFSTMT_H
#define DAWN_IFSTMT_H

#include <memory>
#include "Stmt.h"
#include "../Visitor.h"
#include "../expr/Expr.h"

namespace ast {
    class IfStmt : public Stmt {
    public:
        std::unique_ptr<Expr> Condition;
        std::unique_ptr<Stmt> IfBody;
        std::unique_ptr<Stmt> ElseBody;

        IfStmt(std::unique_ptr<Expr> Condition, std::unique_ptr<Stmt> IfBody, std::unique_ptr<Stmt> ElseBody) :
                Condition(std::move(Condition)), IfBody(std::move(IfBody)), ElseBody(std::move(ElseBody)) {}

        void accept(Visitor &V) override {
            V.visit(*this);
        }
    };
}


#endif //DAWN_IFSTMT_H
