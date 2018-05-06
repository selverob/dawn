//
// Created by selvek on 5.5.2018.
//

#ifndef DAWN_PARSER_H
#define DAWN_PARSER_H

#include "../lex/Lexer.h"
#include "Expr.h"
#include <iostream>
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/raw_ostream.h"

namespace ast {
    class Parser {
        Lexer L;
        Lexeme NextLexeme;

        llvm::DenseMap<int, unsigned> Precedences;
        unsigned getPrecedence(Lexeme::Kind k);


        std::unique_ptr<Expr> parseBinExprRHS(unsigned Precedence, std::unique_ptr<Expr> LHS);
        std::unique_ptr<Expr> parsePrimaryExpr();
        std::unique_ptr<Expr> parseIdentifierExpr();
        std::unique_ptr<Expr> parseNumberExpr();
        std::unique_ptr<Expr> parseUnaryOpExpr();
        std::unique_ptr<Expr> parseParenExpr();

        Lexeme getLexeme();
        nullptr_t LogError(llvm::StringRef problem) const {
            llvm::errs() << problem;
            return nullptr;
        }

    public:
        explicit Parser(Lexer L);
        std::unique_ptr<Expr> parseExpr();
    };
}


#endif //DAWN_PARSER_H
