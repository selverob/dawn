//
// Created by selvek on 5.5.2018.
//

#ifndef DAWN_EXPR_H
#define DAWN_EXPR_H

namespace ast {
    class Visitor;

    class Expr {
    public:
        virtual void accept(Visitor &V) = 0;
    };
}

#endif //DAWN_EXPR_H
