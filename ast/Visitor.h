//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_ABSTRACTVISITOR_H
#define DAWN_ABSTRACTVISITOR_H


namespace ast {
    class ArrayIdxExpr;
    class BinaryOpExpr;
    class CallExpr;
    class NumExpr;
    class UnaryOpExpr;
    class VarExpr;
    class AssignmentStmt;
    class CallStmt;
    class CompoundStmt;
    class ExitStmt;
    class ForStmt;
    class IfStmt;
    class WhileStmt;
    class Consts;
    class Prototype;
    class Function;
    class Program;
    class Vars;

    class Visitor {
    public:
        virtual void visit(ArrayIdxExpr &E) = 0;
        virtual void visit(BinaryOpExpr &E) = 0;
        virtual void visit(CallExpr &E) = 0;
        virtual void visit(NumExpr &E) = 0;
        virtual void visit(UnaryOpExpr &E) = 0;
        virtual void visit(VarExpr &E) = 0;
        virtual void visit(AssignmentStmt &E) = 0;
        virtual void visit(CallStmt &E) = 0;
        virtual void visit(CompoundStmt &E) = 0;
        virtual void visit(ExitStmt &E) = 0;
        virtual void visit(ForStmt &E) = 0;
        virtual void visit(IfStmt &E) = 0;
        virtual void visit(WhileStmt &E) = 0;
        virtual void visit(Consts &E) = 0;
        virtual void visit(Prototype &E) = 0;
        virtual void visit(Function &E) = 0;
        virtual void visit(Program &E) = 0;
        virtual void visit(Vars &E) = 0;
    };
}


#endif //DAWN_ABSTRACTVISITOR_H
