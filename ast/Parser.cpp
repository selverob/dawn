//
// Created by selvek on 5.5.2018.
//

#include "Parser.h"
#include "expr/NumExpr.h"
#include "expr/VarExpr.h"
#include "expr/CallExpr.h"
#include "expr/UnaryOpExpr.h"
#include "expr/BinaryOpExpr.h"
#include "stmt/AssignmentStmt.h"
#include "stmt/CallStmt.h"
#include "stmt/CompoundStmt.h"
#include "stmt/ExitStmt.h"
#include "stmt/ForStmt.h"
#include "stmt/IfStmt.h"
#include "stmt/WhileStmt.h"

ast::Parser::Parser(Lexer Lexer): L(std::move(Lexer)), NextLexeme(L.getLexeme()), Precedences(11) {
    Precedences[static_cast<int>(Lexeme::Kind::AND)] = 10;
    Precedences[static_cast<int>(Lexeme::Kind::OR)] = 10;
    Precedences[static_cast<int>(Lexeme::Kind::EQ)] = 20;
    Precedences[static_cast<int>(Lexeme::Kind::NEQ)] = 20;
    Precedences[static_cast<int>(Lexeme::Kind::LT)] = 30;
    Precedences[static_cast<int>(Lexeme::Kind::LTE)] = 30;
    Precedences[static_cast<int>(Lexeme::Kind::GT)] = 30;
    Precedences[static_cast<int>(Lexeme::Kind::GTE)] = 30;
    Precedences[static_cast<int>(Lexeme::Kind::PLUS)] = 40;
    Precedences[static_cast<int>(Lexeme::Kind::MINUS)] = 40;
    Precedences[static_cast<int>(Lexeme::Kind::MUL)] = 50;
    Precedences[static_cast<int>(Lexeme::Kind::DIV)] = 50;
    Precedences[static_cast<int>(Lexeme::Kind::MOD)] = 60;
}

Lexeme ast::Parser::getLexeme() {
    Lexeme OldLexeme = NextLexeme;
    NextLexeme = L.getLexeme();
    return OldLexeme;
}

std::unique_ptr<ast::Expr> ast::Parser::parseNumberExpr() {
    auto N = getLexeme();
    assert(N.K == Lexeme::Kind::NUMBER);
    return std::make_unique<ast::NumExpr>(N.NumericValue);
}

std::unique_ptr<ast::Expr> ast::Parser::parseIdentifierExpr() {
    auto Ident = getLexeme();
    assert(Ident.K == Lexeme::Kind::IDENT);
    if (NextLexeme.K != Lexeme::Kind::LPAR) {
        return std::make_unique<ast::VarExpr>(Ident.IdentifierStr);
    }
    return parseFunctionCall(Ident);
}

std::unique_ptr<ast::Expr> ast::Parser::parseUnaryOpExpr() {
    auto Op = getLexeme();
    assert(Op.K == Lexeme::Kind::MINUS || Op.K == Lexeme::Kind::NOT);
    auto E = parseExpr();
    return std::make_unique<ast::UnaryOpExpr>(Op.K, std::move(E));
}

std::unique_ptr<ast::Expr> ast::Parser::parseParenExpr() {
    auto L = getLexeme();
    assert(L.K == Lexeme::Kind::LPAR);
    auto E = parseExpr();
    if (NextLexeme.K != Lexeme::Kind::RPAR) {
        return LogError("Unclosed parenthesised expression");
    }
    getLexeme();
    return E;
}

std::unique_ptr<ast::Expr> ast::Parser::parsePrimaryExpr() {
    switch (NextLexeme.K) {
        case Lexeme::Kind::IDENT:
            return parseIdentifierExpr();
        case Lexeme::Kind::NUMBER:
            return parseNumberExpr();
        case Lexeme::Kind::MINUS:
        case Lexeme::Kind::NOT:
            return parseUnaryOpExpr();
        case Lexeme::Kind::LPAR:
            return parseParenExpr();
        default:
            return LogError("Primary expression beginning with unexpected token");
    }
}

std::unique_ptr<ast::Expr> ast::Parser::parseExpr() {
    auto LHS = parsePrimaryExpr();
    if (!LHS)
        return nullptr;

    return parseBinExprRHS(1, std::move(LHS));
}

std::unique_ptr<ast::Expr> ast::Parser::parseBinExprRHS(unsigned Precedence, std::unique_ptr<ast::Expr> LHS) {
    while (true) {
        unsigned CurrentPrecedence = getPrecedence(NextLexeme.K);
        if (CurrentPrecedence < Precedence) {
            return LHS;
        }

        auto Op = getLexeme().K;
        auto RHS = parsePrimaryExpr();
        if (!RHS)
            return nullptr;

        unsigned NextPrecedence = getPrecedence(NextLexeme.K);
        if (CurrentPrecedence < NextPrecedence) {
            RHS = parseBinExprRHS(CurrentPrecedence + 1, std::move(RHS));
            if (!RHS)
                return nullptr;
        }

        LHS = std::make_unique<ast::BinaryOpExpr>(Op, std::move(LHS), std::move(RHS));
    }
}

unsigned ast::Parser::getPrecedence(Lexeme::Kind K) {
    int Key = static_cast<int>(K);
    if (Precedences.count(Key) == 0)
        return 0;
    return Precedences[Key];
}

std::unique_ptr<ast::Expr> ast::Parser::parseFunctionCall(const Lexeme &Ident) {
    auto Call = std::make_unique<ast::CallExpr>(Ident.IdentifierStr);
    getLexeme();
    while (true) {
        auto Arg = parseExpr();
        if (Arg == nullptr)
            return nullptr;
        Call->addArgument(std::move(Arg));
        if (!(NextLexeme.Char == ',' || NextLexeme.K == Lexeme::Kind::RPAR)) {
            return LogError("Expected function argument separator (,) or list end ())");
        }
        if (getLexeme().K == Lexeme::Kind::RPAR) {
            break;
        }
    }
    return std::unique_ptr<Expr>(std::move(Call));
}

std::unique_ptr<ast::Stmt> ast::Parser::parseStmt() {
    switch (NextLexeme.K) {
        case Lexeme::Kind::IDENT:
            return parseIdentifierStmt();
        case Lexeme::Kind::IF:
            return parseIfStmt();
        case Lexeme::Kind::WHILE:
            return parseWhileStmt();
        case Lexeme::Kind::FOR:
            return parseForStmt();
        case Lexeme::Kind::BEGIN:
            return parseCompoundStmt();
        case Lexeme::Kind::EXIT:
            getLexeme();
            return std::make_unique<ExitStmt>();
    }
}

std::unique_ptr<ast::Stmt> ast::Parser::parseIfStmt() {
    auto KW = getLexeme();
    assert(KW.K == Lexeme::Kind::IF);
    auto Condition = parseExpr();
    if (Condition == nullptr)
        return nullptr;
    KW = getLexeme();
    if (KW.K != Lexeme::Kind::THEN) {
        return LogError("Expected 'then'");
    }
    auto Body = parseStmt();
    if (Body == nullptr)
        return nullptr;
    auto Else = std::unique_ptr<Stmt>(nullptr);
    if (NextLexeme.K == Lexeme::Kind::ELSE) {
        getLexeme();
        Else = parseStmt();
        if (Else == nullptr)
            return nullptr;
    }
    return std::make_unique<ast::IfStmt>(
            std::move(Condition), std::move(Body), std::move(Else));
}

std::unique_ptr<ast::Stmt> ast::Parser::parseWhileStmt() {
    auto KW = getLexeme();
    assert(KW.K == Lexeme::Kind::WHILE);
    auto Condition = parseExpr();
    if (Condition == nullptr)
        return nullptr;
    KW = getLexeme();
    if (KW.K != Lexeme::Kind::DO)
        return LogError("Expected a 'do'");
    auto Body = parseStmt();
    if (Body == nullptr)
        return nullptr;
    return std::make_unique<WhileStmt>(std::move(Condition), std::move(Body));
}

std::unique_ptr<ast::Stmt> ast::Parser::parseForStmt() {
    auto KW = getLexeme();
    assert(KW.K == Lexeme::Kind::FOR);
    auto VarName = getLexeme();
    if (VarName.K != Lexeme::Kind::IDENT)
        return LogError("Expected variable name in for");
    KW = getLexeme();
    if (KW.K != Lexeme::Kind::ASSIGN)
        return LogError("Expected assignment operator in for");
    auto Lower = parseExpr();
    if (Lower == nullptr)
        return nullptr;
    auto Operator = getLexeme();
    if (Operator.K != Lexeme::Kind::TO && Operator.K != Lexeme::Kind::DOWNTO)
        return LogError("Unknown operator in for, expecting 'downto' or 'to'");
    auto Upper = parseExpr();
    if (Upper == nullptr)
        return nullptr;
    KW = getLexeme();
    if (KW.K != Lexeme::Kind::DO)
        return LogError("Expecting a 'do' after for statement conditions");
    auto Body = parseStmt();
    if (Body == nullptr)
        return nullptr;
    return std::make_unique<ForStmt>(
            VarName.IdentifierStr, Operator.K, std::move(Lower), std::move(Upper), std::move(Body));
}

std::unique_ptr<ast::Stmt> ast::Parser::parseCompoundStmt() {
    assert(NextLexeme.K == Lexeme::Kind::BEGIN);
    getLexeme();
    auto C = std::make_unique<CompoundStmt>();
    while (NextLexeme.K != Lexeme::Kind::END) {
        C->addStmt(parseStmt());
        if (!C->Body.back()) {
            return nullptr;
        }
        if (NextLexeme.Char != ';') {
            std::cout << NextLexeme << std::endl;
            return LogError("Missing semicolon in compound statement");
        }
        getLexeme();
    }
    getLexeme();
    return std::unique_ptr(std::move(C));
}

std::unique_ptr<ast::Stmt> ast::Parser::parseIdentifierStmt() {
    auto Id = getLexeme();
    assert(Id.K == Lexeme::Kind::IDENT);
    if (NextLexeme.K == Lexeme::Kind::ASSIGN) {
        getLexeme();
        auto Val = parseExpr();
        if (Val == nullptr)
            return nullptr;
        return std::make_unique<AssignmentStmt>(Id.IdentifierStr, std::move(Val));
    }
    if (NextLexeme.K == Lexeme::Kind::LPAR) {
        return std::make_unique<CallStmt>(parseFunctionCall(Id));
    }
    return LogError("Naked identifier encountered instead of a statement");
}

