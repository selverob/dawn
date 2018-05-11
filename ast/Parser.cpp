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

ast::Parser::Parser(llvm::SourceMgr &Sources, Lexer Lexer) : Sources(Sources), L(std::move(Lexer)),
                                                             NextLexeme(L.getLexeme()), Precedences(11) {
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
    return std::make_unique<ast::NumExpr>(N.Loc, N.NumericValue);
}

std::unique_ptr<ast::Expr> ast::Parser::parseIdentifierExpr() {
    auto Ident = getLexeme();
    assert(Ident.K == Lexeme::Kind::IDENT);
    if (NextLexeme.K != Lexeme::Kind::LPAR) {
        return std::make_unique<ast::VarExpr>(Ident.Loc, Ident.IdentifierStr);
    }
    return parseFunctionCall(Ident);
}

std::unique_ptr<ast::Expr> ast::Parser::parseUnaryOpExpr() {
    auto Op = getLexeme();
    assert(Op.K == Lexeme::Kind::MINUS || Op.K == Lexeme::Kind::NOT);
    auto E = parseExpr();
    return std::make_unique<ast::UnaryOpExpr>(Op.Loc, Op.K, std::move(E));
}

std::unique_ptr<ast::Expr> ast::Parser::parseParenExpr() {
    auto L = getLexeme();
    assert(L.K == Lexeme::Kind::LPAR);
    auto E = parseExpr();
    if (NextLexeme.K != Lexeme::Kind::RPAR)
        return LogError(NextLexeme.Loc, "Parenthesised expression was not closed");
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
            return LogError(NextLexeme.Loc, "Primary expression beginning with unexpected token");
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

        LHS = std::make_unique<ast::BinaryOpExpr>(LHS->Loc, Op, std::move(LHS), std::move(RHS));
    }
}

unsigned ast::Parser::getPrecedence(Lexeme::Kind K) {
    int Key = static_cast<int>(K);
    if (Precedences.count(Key) == 0)
        return 0;
    return Precedences[Key];
}

std::unique_ptr<ast::Expr> ast::Parser::parseFunctionCall(const Lexeme &Ident) {
    auto Call = std::make_unique<ast::CallExpr>(Ident.Loc, Ident.IdentifierStr);
    getLexeme();
    if (NextLexeme.K != Lexeme::Kind::RPAR) {
        while (true) {
            auto Arg = parseExpr();
            if (Arg == nullptr)
                return nullptr;
            Call->addArgument(std::move(Arg));
            if (!(NextLexeme.Char == ',' || NextLexeme.K == Lexeme::Kind::RPAR)) {
                return LogError(NextLexeme.Loc, "Expected function argument separator (,) or list end ())");
            }
            if (getLexeme().K == Lexeme::Kind::RPAR) {
                break;
            }
        }
    } else {
        getLexeme();
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
            return std::make_unique<ExitStmt>(NextLexeme.Loc);
        default:
            return LogError(NextLexeme.Loc, "Statement beginning with an invalid keyword");
    }
}

std::unique_ptr<ast::Stmt> ast::Parser::parseIfStmt() {
    auto KW = getLexeme();
    auto StartingLoc = KW.Loc;
    assert(KW.K == Lexeme::Kind::IF);
    auto Condition = parseExpr();
    if (Condition == nullptr)
        return nullptr;
    KW = getLexeme();
    if (KW.K != Lexeme::Kind::THEN) {
        return LogError(KW.Loc, "Expected 'then'");
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
            StartingLoc, std::move(Condition), std::move(Body), std::move(Else));
}

std::unique_ptr<ast::Stmt> ast::Parser::parseWhileStmt() {
    auto KW = getLexeme();
    auto StartingLoc = KW.Loc;
    assert(KW.K == Lexeme::Kind::WHILE);
    auto Condition = parseExpr();
    if (Condition == nullptr)
        return nullptr;
    KW = getLexeme();
    if (KW.K != Lexeme::Kind::DO)
        return LogError(KW.Loc, "Expected a 'do'");
    auto Body = parseStmt();
    if (Body == nullptr)
        return nullptr;
    return std::make_unique<WhileStmt>(StartingLoc, std::move(Condition), std::move(Body));
}

std::unique_ptr<ast::Stmt> ast::Parser::parseForStmt() {
    auto KW = getLexeme();
    auto StartingLoc = KW.Loc;
    assert(KW.K == Lexeme::Kind::FOR);
    auto VarName = getLexeme();
    if (VarName.K != Lexeme::Kind::IDENT)
        return LogError(VarName.Loc, "Expected variable name in for");
    KW = getLexeme();
    if (KW.K != Lexeme::Kind::ASSIGN)
        return LogError(KW.Loc, "Expected assignment operator in for");
    auto Lower = parseExpr();
    if (Lower == nullptr)
        return nullptr;
    auto Operator = getLexeme();
    if (Operator.K != Lexeme::Kind::TO && Operator.K != Lexeme::Kind::DOWNTO)
        return LogError(Operator.Loc, "Unknown operator in for, expecting 'downto' or 'to'");
    auto Upper = parseExpr();
    if (Upper == nullptr)
        return nullptr;
    KW = getLexeme();
    if (KW.K != Lexeme::Kind::DO)
        return LogError(KW.Loc, "Expecting a 'do' after for statement conditions");
    auto Body = parseStmt();
    if (Body == nullptr)
        return nullptr;
    return std::make_unique<ForStmt>(StartingLoc, VarName.IdentifierStr,
                                     Operator.K, std::move(Lower), std::move(Upper), std::move(Body));
}

std::unique_ptr<ast::Stmt> ast::Parser::parseCompoundStmt() {
    assert(NextLexeme.K == Lexeme::Kind::BEGIN);
    auto C = std::make_unique<CompoundStmt>(getLexeme().Loc);
    while (NextLexeme.K != Lexeme::Kind::END) {
        C->addStmt(parseStmt());
        if (!C->Body.back()) {
            return nullptr;
        }
        if (NextLexeme.Char != ';') {
            return LogError(NextLexeme.Loc, "Missing semicolon in compound statement");
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
        return std::make_unique<AssignmentStmt>(Id.Loc, Id.IdentifierStr, std::move(Val));
    }
    if (NextLexeme.K == Lexeme::Kind::LPAR) {
        return std::make_unique<CallStmt>(Id.Loc, parseFunctionCall(Id));
    }
    return LogError(Id.Loc, "Naked identifier encountered instead of a statement");
}

std::unique_ptr<ast::Consts> ast::Parser::parseConsts() {
    assert(NextLexeme.K == Lexeme::Kind::CONST);
    auto C = std::make_unique<Consts>(getLexeme().Loc);
    if (NextLexeme.K != Lexeme::Kind::IDENT)
        return LogError(NextLexeme.Loc, "Expecting a constant name in constant declaration");
    while (NextLexeme.K == Lexeme::Kind::IDENT) {
        auto Name = getLexeme().IdentifierStr;
        if (NextLexeme.K != Lexeme::Kind::EQ)
            return LogError(NextLexeme.Loc, "Expecting a '=' in constant declaration");
        getLexeme();
        if (NextLexeme.K != Lexeme::Kind::NUMBER)
            return LogError(NextLexeme.Loc, "Expecting a number literal in constant declaration");
        C->addConst(Name, NextLexeme.NumericValue);
        getLexeme();
        if (NextLexeme.Char != ';')
            return LogError(NextLexeme.Loc, "Expecting a ';' in constant declaration");
        getLexeme();
    }
    return std::move(C);
}


std::unique_ptr<ast::Prototype> ast::Parser::parsePrototype() {
    assert(NextLexeme.K == Lexeme::Kind::FUNCTION);
    auto StartingLoc = getLexeme().Loc;
    if (NextLexeme.K != Lexeme::Kind::IDENT)
        return LogError(NextLexeme.Loc, "Expecting a function name in function declaration");
    auto Name = getLexeme().IdentifierStr;
    if (NextLexeme.K != Lexeme::Kind::LPAR)
        return LogError(NextLexeme.Loc, "Expecting a '(' in function declaration");
    getLexeme();
    auto Proto = std::make_unique<ast::Prototype>(StartingLoc, Name, "void");
    while (NextLexeme.K != Lexeme::Kind::RPAR) {
        if (NextLexeme.K != Lexeme::Kind::IDENT)
            return LogError(NextLexeme.Loc, "Expecting parameter name in function declaration");
        auto Name = getLexeme().IdentifierStr;
        if (NextLexeme.Char != ':')
            return LogError(NextLexeme.Loc, "Expecting ':' in function declaration");
        getLexeme();
        if (NextLexeme.K != Lexeme::Kind::IDENT)
            return LogError(NextLexeme.Loc, "Expecting type name in function declaration");
        auto Type = getLexeme().IdentifierStr;
        Proto->addParameter(Name, Type);
        if (NextLexeme.Char == ';')
            getLexeme();
        else if (NextLexeme.K != Lexeme::Kind::RPAR)
            return LogError(NextLexeme.Loc, "Expecting parameter separator in function declaration");
    }
    getLexeme();
    if (NextLexeme.Char == ':') {
        getLexeme();
        if (NextLexeme.K != Lexeme::Kind::IDENT)
            return LogError(NextLexeme.Loc, "Function declaration missing a return type");
        Proto->ReturnType = getLexeme().IdentifierStr;
    }
    if (NextLexeme.Char != ';')
        return LogError(NextLexeme.Loc, "Function declarations must end with a ';'");
    getLexeme();
    return Proto;
}


std::unique_ptr<ast::Function> ast::Parser::parseFunction(std::unique_ptr<ast::Prototype> Proto) {
    auto Variables = std::make_unique<Vars>(NextLexeme.Loc);
    if (NextLexeme.K == Lexeme::Kind::VAR) {
        Variables = parseVars();
    }
    if (!Variables)
        return nullptr;
    auto Body = parseStmt();
    if (!Body)
        return nullptr;
    if (NextLexeme.Char != ';')
        return LogError(NextLexeme.Loc, "Function declarations should end with semicolons");
    getLexeme();
    return std::make_unique<ast::Function>(Proto->Loc, std::move(Proto), std::move(Variables), std::move(Body));
}



std::unique_ptr<ast::Vars> ast::Parser::parseVars() {
    assert(NextLexeme.K == Lexeme::Kind::VAR);
    auto V = std::make_unique<Vars>(getLexeme().Loc);
    if (NextLexeme.K != Lexeme::Kind::IDENT)
        return LogError(NextLexeme.Loc, "Expecting a variable name in variable declaration");
    while (NextLexeme.K == Lexeme::Kind::IDENT) {
        auto Names = std::vector<std::string> { getLexeme().IdentifierStr };
        while (NextLexeme.K == Lexeme::Kind::IDENT)
            Names.push_back(getLexeme().IdentifierStr);
        if (NextLexeme.Char != ':')
            return LogError(NextLexeme.Loc, "Expecting a ':' in variable declaration");
        getLexeme();
        if (NextLexeme.K != Lexeme::Kind::IDENT)
            return LogError(NextLexeme.Loc, "Expecting a type name in variable declaration");
        for (const auto &Name : Names)
            V->addVar(Name, NextLexeme.IdentifierStr);
        getLexeme();
        if (NextLexeme.Char != ';')
            return LogError(NextLexeme.Loc, "Expecting a ';' in variable declaration");
        getLexeme();
    }
    return std::move(V);
}

std::unique_ptr<ast::Program> ast::Parser::parse() {
    if (NextLexeme.K != Lexeme::Kind::PROGRAM)
        return LogError(NextLexeme.Loc, "Programs must begin with a 'program' statement");
    auto StartingLoc = getLexeme().Loc;
    if (NextLexeme.K != Lexeme::Kind::IDENT)
        return LogError(NextLexeme.Loc, "Programs must begin with the program's name");
    auto Program = std::make_unique<ast::Program>(StartingLoc, getLexeme().IdentifierStr);

    if (NextLexeme.Char != ';')
        return LogError(NextLexeme.Loc, "Program identification should be terminated with a semicolon");
    getLexeme();

    if (NextLexeme.K == Lexeme::Kind::CONST) {
        auto C = parseConsts();
        if (!C)
            return nullptr;
        Program->addConsts(std::move(C));
    } else {
        Program->addConsts(std::make_unique<Consts>(NextLexeme.Loc));
    }

    if (NextLexeme.K == Lexeme::Kind::VAR) {
        auto V = parseVars();
        if (!V)
            return nullptr;
        Program->addVariables(std::move(V));
    } else {
        Program->addVariables(std::make_unique<Vars>(NextLexeme.Loc));
    }

    while (NextLexeme.K == Lexeme::Kind::FUNCTION) {
        auto P = parsePrototype();
        if (!P)
            return nullptr;
        if (NextLexeme.K == Lexeme::Kind::FORWARD) {
            Program->addPrototype(std::move(P));
            getLexeme();
            if (NextLexeme.Char != ';')
                return LogError(NextLexeme.Loc, "forward statements should be terminated with a ';'");
            getLexeme();
        } else {
            auto F = parseFunction(std::move(P));
            if (!F)
                return nullptr;
            Program->addFunction(std::move(F));
        }
    }

    auto Body = parseStmt();
    if (!Body)
        return nullptr;
    Program->addBody(std::move(Body));

    if (NextLexeme.Char != '.')
        return LogError(NextLexeme.Loc, "Program must be terminated with a '.'");
    getLexeme();
    return Program;
}
