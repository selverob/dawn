//
// Created by selvek on 15.5.2018.
//

#ifndef DAWN_ARRAYIDXEXPR_H
#define DAWN_ARRAYIDXEXPR_H

#include <string>
#include <memory>
#include "../Visitor.h"
#include "Lvalue.h"
#include "Expr.h"

namespace ast {
    class ArrayIdxExpr : public Lvalue {
    public:
        std::string VarName;
        std::unique_ptr<Expr> Idx;

        ArrayIdxExpr(llvm::SMLoc Loc, std::string Name, std::unique_ptr<Expr> Idx):
                Lvalue(Loc, Lvalue::Kind::ArrayIdx), VarName(std::move(Name)), Idx(std::move(Idx)) {}

        void accept(Visitor &V) override {
            V.visit(*this);
        }

        const std::string &Ident() const override {
            return VarName;
        }

        static bool classof(const Lvalue *V) {
            return V->K == Lvalue::Kind::ArrayIdx;
        }
    };
}


#endif //DAWN_ARRAYIDXEXPR_H
