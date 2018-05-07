//
// Created by selvek on 5.5.2018.
//

#ifndef DAWN_PARSER_H
#define DAWN_PARSER_H

#include "../lex/Lexer.h"
#include "expr/Expr.h"
#include "stmt/Stmt.h"
#include <iostream>
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/raw_ostream.h"

namespace ast {
    class Parser {
        Lexer L;
        Lexeme NextLexeme;

        llvm::SourceMgr &Sources;

        llvm::DenseMap<int, unsigned> Precedences;
        unsigned getPrecedence(Lexeme::Kind k);

        std::unique_ptr<Stmt> parseIfStmt();
        std::unique_ptr<Stmt> parseWhileStmt();
        std::unique_ptr<Stmt> parseForStmt();
        std::unique_ptr<Stmt> parseCompoundStmt();
        std::unique_ptr<Stmt> parseIdentifierStmt();
        std::unique_ptr<Expr> parseBinExprRHS(unsigned Precedence, std::unique_ptr<Expr> LHS);

        std::unique_ptr<Expr> parsePrimaryExpr();
        std::unique_ptr<Expr> parseIdentifierExpr();
        std::unique_ptr<Expr> parseFunctionCall(const Lexeme &Ident);
        std::unique_ptr<Expr> parseNumberExpr();
        std::unique_ptr<Expr> parseUnaryOpExpr();
        std::unique_ptr<Expr> parseParenExpr();
        Lexeme getLexeme();

        nullptr_t LogError(llvm::SMLoc Loc, llvm::StringRef Err) const {
            Sources.PrintMessage(Loc, llvm::SourceMgr::DiagKind::DK_Error, Err);
            return nullptr;
        }
    public:

        Parser(llvm::SourceMgr &Sources, Lexer L);
        std::unique_ptr<Expr> parseExpr();
        std::unique_ptr<Stmt> parseStmt();
    };
}


#endif //DAWN_PARSER_H
