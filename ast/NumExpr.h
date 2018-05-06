//
// Created by selvek on 5.5.2018.
//

#ifndef DAWN_NUMEXPR_H
#define DAWN_NUMEXPR_H

namespace ast {
    class NumExpr : public Expr {
        long Value;
    public:
        NumExpr(long Value) : Value(Value) {}
    };
}

#endif //DAWN_NUMEXPR_H
