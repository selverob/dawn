//
// Created by selvek on 5.5.2018.
//

#ifndef DAWN_UNARYOPEXPR_H
#define DAWN_UNARYOPEXPR_H

#include <bits/unique_ptr.h>
#include "../lex/Lexeme.h"

#include "Expr.h"
#include "Visitor.h"

namespace ast {
    class UnaryOpExpr : public Expr {
    public:
        UnaryOpExpr(Lexeme::Kind Op, std::unique_ptr<Expr> Expression) :
                Op(Op), Expression(std::move(Expression)) {}

        void accept(Visitor &V) override {
            V.visit(*this);
        }

        Lexeme::Kind Op;
        std::unique_ptr<Expr> Expression;
    };
}


#endif //DAWN_UNARYOPEXPR_H
