//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_PRINTER_H
#define DAWN_PRINTER_H


#include "Visitor.h"
#include "llvm/Support/raw_ostream.h"
#include "../lex/Lexeme.h"

namespace ast {
    class Printer : public Visitor {
        llvm::raw_ostream &Out;

        void printOperator(Lexeme::Kind K);
    public:
        Printer(llvm::raw_ostream &Out);

        void visit(BinaryOpExpr &E) override;

        void visit(CallExpr &E) override;

        void visit(NumExpr &E) override;

        void visit(UnaryOpExpr &E) override;

        void visit(VarExpr &E) override;
    };
}


#endif //DAWN_PRINTER_H
