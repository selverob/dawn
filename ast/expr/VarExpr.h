//
// Created by selvek on 5.5.2018.
//

#ifndef DAWN_VAREXPR_H
#define DAWN_VAREXPR_H

#include <string>

#include "Expr.h"
#include "../Visitor.h"
#include "Lvalue.h"

namespace ast {
    class VarExpr : public Lvalue {
    public:
        std::string VarName;

        VarExpr(llvm::SMLoc Loc, std::string VarName): Lvalue(Loc, Lvalue::Kind::Var), VarName(std::move(VarName)) {}

        void accept(Visitor &V) override {
            V.visit(*this);
        }

        const std::string &Ident() const override {
            return VarName;
        }

        static bool classof(const Lvalue *V) {
            return V->K == Lvalue::Kind::Var;
        }
    };
}


#endif //DAWN_VAREXPR_H
