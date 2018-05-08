//
// Created by selvek on 6.5.2018.
//

#include "Printer.h"
#include "expr/NumExpr.h"
#include "expr/VarExpr.h"
#include "expr/UnaryOpExpr.h"
#include "expr/CallExpr.h"
#include "expr/BinaryOpExpr.h"
#include "stmt/AssignmentStmt.h"
#include "stmt/CallStmt.h"
#include "stmt/CompoundStmt.h"
#include "stmt/ForStmt.h"
#include "stmt/IfStmt.h"
#include "stmt/WhileStmt.h"
#include "decl/Consts.h"
#include "decl/Vars.h"
#include "decl/Function.h"
#include "decl/Program.h"

ast::Printer::Printer(llvm::raw_ostream &Out): Out(Out), Indendation(0) {}

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

void ast::Printer::visit(ast::AssignmentStmt &E) {
    Out << E.Var << " := ";
    E.Value->accept(*this);
}

void ast::Printer::visit(ast::CallStmt &E) {
    E.Callee->accept(*this);
}

void ast::Printer::visit(ast::CompoundStmt &E) {
    Out << "begin\n";
    Indendation++;
    for (auto &Stmt : E.Body) {
        printIndentation();
        Stmt->accept(*this);
        Out << ";\n";
    }
    Indendation--;
    printIndentation(); Out << "end";
}

void ast::Printer::visit(ast::ExitStmt &E) {
    Out << "exit";
}

void ast::Printer::visit(ast::ForStmt &E) {
    Out << "for " << E.Var << " := ";
    E.Begin->accept(*this);
    if (E.Operator == Lexeme::Kind::TO)
        Out << " to ";
    else
        Out << " downto ";
    E.End->accept(*this); Out << " do \n";
    Indendation++;
    printIndentation(); E.Body->accept(*this);
    Indendation--;

}

void ast::Printer::visit(ast::IfStmt &E) {
    Out << "if ";
    E.Condition->accept(*this);
    Out << " then\n";
    Indendation++;
    printIndentation(); E.IfBody->accept(*this);
    Indendation--;
    if (E.ElseBody) {
        Out << '\n';
        printIndentation();
        Out << "else\n";
        Indendation++;
        printIndentation(); E.ElseBody->accept(*this);
        Indendation--;
    }
}

void ast::Printer::visit(ast::WhileStmt &E) {
    Out << "while ";
    E.Condition->accept(*this);
    Out << " do\n";
    Indendation++;
    printIndentation(); E.Body->accept(*this);
    Indendation--;
}

void ast::Printer::visit(ast::Consts &E) {
    if (E.Constants.size() == 0)
        return;
    Out << "const\n";
    Indendation++;
    for (const auto &C : E.Constants) {
        printIndentation(); Out << C.first << " = " << C.second << ";\n";
    }
    Indendation--;
}

void ast::Printer::printPrototype(ast::Prototype &E) {
    Out << "function " << E.Name << "(";
    for (size_t i = 0; i < E.Parameters.size(); i++) {
        Out << E.Parameters[i].first
            << ": "
            << E.Parameters[i].second;
        if (i < E.Parameters.size() - 1)
            Out << "; ";
    }
    Out << ")";
    if (E.ReturnType != "void")
        Out << ": " << E.ReturnType;
    Out << ';';
}


void ast::Printer::visit(ast::Prototype &E) {
    printPrototype(E);
    Out << " forward; \n";
}

void ast::Printer::visit(ast::Function &E) {
    printPrototype(*E.Proto);
    Out << "\n";
    E.Variables->accept(*this);
    printIndentation();
    E.Body->accept(*this);
    printIndentation(); Out << ";\n";
}

void ast::Printer::visit(ast::Program &E) {
    Out << "program " << E.Name << ";\n";
    E.Constants->accept(*this);
    Out << '\n';
    E.Variables->accept(*this);
    Out << '\n';
    for (auto &P : E.Prototypes) {
        P->accept(*this);
        Out << '\n';
    }
    for (auto &F : E.Functions) {
        F->accept(*this);
        Out << '\n';
    }
    E.Body->accept(*this);
    Out << '.';
}

void ast::Printer::visit(ast::Vars &E) {
    if (E.Variables.size() == 0)
        return;
    Out << "var\n";
    Indendation++;
    for (const auto &V : E.Variables) {
        printIndentation(); Out << V.first << ": " << V.second << ";\n";
    }
    Indendation--;
}

void ast::Printer::printIndentation() {
    for (int i = 0; i < Indendation; i++) {
        Out << '\t';
    }
}
