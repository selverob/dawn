//
// Created by selvek on 5.5.2018.
//

#ifndef DAWN_BINARYOPEXPR_H
#define DAWN_BINARYOPEXPR_H

#include <memory>
#include "../lex/Lexeme.h"
#include "Expr.h"

namespace ast {
    class BinaryOpExpr : public Expr {
        Lexeme::Kind Op;
        std::unique_ptr<Expr> L, R;
    public:
        BinaryOpExpr(Lexeme::Kind Op, std::unique_ptr<Expr> L, std::unique_ptr<Expr> R) :
                Op(Op), L(std::move(L)), R(std::move(R)) {}
    };
}

#endif //DAWN_BINARYOPEXPR_H
