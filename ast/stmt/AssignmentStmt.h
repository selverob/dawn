//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_ASSIGNMENTSTMT_H
#define DAWN_ASSIGNMENTSTMT_H

#include <string>
#include <memory>
#include "Stmt.h"
#include "../expr/Expr.h"
#include "../Visitor.h"

namespace ast {
    class AssignmentStmt : public Stmt {
    public:
        std::string Var;
        std::unique_ptr<Expr> Value;

        AssignmentStmt(llvm::SMLoc Loc, std::string Var, std::unique_ptr<Expr> Value) : Stmt(Loc), Var(std::move(Var)),
                                                                                        Value(std::move(Value)) {}

        virtual void accept(Visitor &V) {
            V.visit(*this);
        }
    };
}


#endif //DAWN_ASSIGNMENTSTMT_H
