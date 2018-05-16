//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_PRINTER_H
#define DAWN_PRINTER_H


#include "Visitor.h"
#include "llvm/Support/raw_ostream.h"
#include "../lex/Lexeme.h"
#include "type/Type.h"

namespace ast {
    class Printer : public Visitor {
        llvm::raw_ostream &Out;
        int Indendation;

        void printOperator(Lexeme::Kind K);
        void printPrototype(Prototype &E);
        void printType(Type *T);
        void printIndentation();
    public:
        Printer(llvm::raw_ostream &Out);

        void visit(BinaryOpExpr &E) override;

        void visit(CallExpr &E) override;

        void visit(NumExpr &E) override;

        void visit(UnaryOpExpr &E) override;

        void visit(VarExpr &E) override;

        void visit(AssignmentStmt &E) override;

        void visit(CallStmt &E) override;

        void visit(CompoundStmt &E) override;

        void visit(ExitStmt &E) override;

        void visit(ForStmt &E) override;

        void visit(IfStmt &E) override;

        void visit(WhileStmt &E) override;

        void visit(Consts &E) override;

        void visit(Prototype &E) override;

        void visit(Function &E) override;

        void visit(Program &E) override;

        void visit(Vars &E) override;

        void visit(ArrayIdxExpr &E) override;
    };
}


#endif //DAWN_PRINTER_H
