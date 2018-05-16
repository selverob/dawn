//
// Created by selvek on 15.5.2018.
//

#ifndef DAWN_LVALUE_H
#define DAWN_LVALUE_H

#include "Expr.h"

namespace ast {
    class Lvalue : public Expr {
    public:
        enum class Kind {
            Var,
            ArrayIdx
        };

        Kind K;
        Lvalue(llvm::SMLoc Loc, Kind K): Expr(Loc), K(K) {}
        virtual const std::string &Ident() const = 0;
    };
}


#endif //DAWN_LVALUE_H
