//
// Created by selvek on 6.5.2018.
//

#include "Printer.h"
#include "NumExpr.h"
#include "VarExpr.h"
#include "UnaryOpExpr.h"
#include "CallExpr.h"
#include "BinaryOpExpr.h"

ast::Printer::Printer(llvm::raw_ostream &Out): Out(Out) {}

void ast::Printer::visit(ast::BinaryOpExpr &E) {
    Out << "("; E.L->accept(*this); Out << ")";
    printOperator(E.Op);
    Out << "("; E.R->accept(*this); Out << ")";
}

void ast::Printer::visit(ast::CallExpr &E) {
    Out << E.FunctionName;
    Out << "(";
    for (size_t i = 0; i < E.Args.size(); i++) {
        E.Args[i]->accept(*this);
        if (i != E.Args.size() - 1) {
            Out << ", ";
        }
    }
    Out << ")";
}

void ast::Printer::visit(ast::NumExpr &E) {
    Out << E.Value;
}

void ast::Printer::visit(ast::UnaryOpExpr &E) {
    switch (E.Op) {
        case Lexeme::Kind::MINUS:
            Out << "-(";
            break;
        case Lexeme::Kind::NOT:
            Out << "not (";
            break;
        default:
            llvm_unreachable("Invalid unary operation encountered");
    }
    E.Expression->accept(*this);
    Out << ")";
}

void ast::Printer::visit(ast::VarExpr &E) {
    Out << E.VarName;
}

void ast::Printer::printOperator(Lexeme::Kind K) {
    switch (K) {
        case Lexeme::Kind::AND: Out << " and "; break;
        case Lexeme::Kind::OR: Out << " or "; break;
        case Lexeme::Kind::LT: Out << " < "; break;
        case Lexeme::Kind::LTE: Out << " <= "; break;
        case Lexeme::Kind::EQ: Out << " = "; break;
        case Lexeme::Kind::GTE: Out << " != "; break;
        case Lexeme::Kind::GT: Out << " > "; break;
        case Lexeme::Kind::NEQ: Out << " >= "; break;
        case Lexeme::Kind::PLUS: Out << " + "; break;
        case Lexeme::Kind::MINUS: Out << " - "; break;
        case Lexeme::Kind::MOD: Out << " mod "; break;
        case Lexeme::Kind::MUL: Out << " * "; break;
        case Lexeme::Kind::DIV: Out << " / "; break;
        default: llvm_unreachable("Invalid binary expression encountered");
    }
}