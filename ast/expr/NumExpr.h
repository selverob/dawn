//
// Created by selvek on 5.5.2018.
//

#ifndef DAWN_NUMEXPR_H
#define DAWN_NUMEXPR_H

#include "Expr.h"
#include "../Visitor.h"

namespace ast {
    class NumExpr : public Expr {
    public:
        NumExpr(long Value) : Value(Value) {}

        void accept(Visitor &V) override {
            V.visit(*this);
        }

        long Value;
    };
}

#endif //DAWN_NUMEXPR_H
