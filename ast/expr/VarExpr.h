//
// Created by selvek on 5.5.2018.
//

#ifndef DAWN_VAREXPR_H
#define DAWN_VAREXPR_H

#include <string>

#include "Expr.h"
#include "../Visitor.h"

namespace ast {
    class VarExpr : public Expr {
    public:
        VarExpr(llvm::SMLoc Loc, std::string VarName): Expr(Loc), VarName(std::move(VarName)) {}

        void accept(Visitor &V) override {
            V.visit(*this);
        }

        std::string VarName;
    };
}


#endif //DAWN_VAREXPR_H
