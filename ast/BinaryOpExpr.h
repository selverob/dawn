//
// Created by selvek on 5.5.2018.
//

#ifndef DAWN_BINARYOPEXPR_H
#define DAWN_BINARYOPEXPR_H

#include <memory>
#include "../lex/Lexeme.h"
#include "Expr.h"
#include "Visitor.h"


namespace ast {
    class BinaryOpExpr : public Expr {
    public:
        BinaryOpExpr(Lexeme::Kind Op, std::unique_ptr<Expr> L, std::unique_ptr<Expr> R) :
                Op(Op), L(std::move(L)), R(std::move(R)) {}

        void accept(Visitor &V) override {
            V.visit(*this);
        }

        Lexeme::Kind Op;
        std::unique_ptr<Expr> L, R;
    };
}

#endif //DAWN_BINARYOPEXPR_H
