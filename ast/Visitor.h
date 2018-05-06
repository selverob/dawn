//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_ABSTRACTVISITOR_H
#define DAWN_ABSTRACTVISITOR_H


namespace ast {
    class BinaryOpExpr;
    class CallExpr;
    class NumExpr;
    class UnaryOpExpr;
    class VarExpr;

    class Visitor {
    public:
        virtual void visit(BinaryOpExpr &E) = 0;
        virtual void visit(CallExpr &E) = 0;
        virtual void visit(NumExpr &E) = 0;
        virtual void visit(UnaryOpExpr &E) = 0;
        virtual void visit(VarExpr &E) = 0;
    };
}


#endif //DAWN_ABSTRACTVISITOR_H
