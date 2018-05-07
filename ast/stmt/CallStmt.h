//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_CALLSTMT_H
#define DAWN_CALLSTMT_H

#include <string>
#include <memory>
#include "Stmt.h"
#include "../expr/Expr.h"
#include "../Visitor.h"

namespace ast {
    class CallStmt : public Stmt {
    public:
        std::unique_ptr<Expr> Callee;

        CallStmt(llvm::SMLoc Loc, std::unique_ptr<Expr> Callee) : Stmt(Loc), Callee(std::move(Callee)) {}

        virtual void accept(Visitor &V) {
            V.visit(*this);
        }
    };
}

#endif //DAWN_CALLSTMT_H
