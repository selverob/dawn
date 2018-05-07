//
// Created by selvek on 3.5.2018.
//

#include <cassert>
#include "Lexeme.h"

Lexeme::Lexeme(Lexeme::Kind K, llvm::SMLoc Position) : K(K), Loc(Position), Char(0), NumericValue(0) {}

Lexeme::Lexeme(Lexeme::Kind K, char Char, llvm::SMLoc Position) : K(K), Char(Char), Loc(Position), NumericValue(0) {
    assert(K == Kind::UNKNOWN);
}

Lexeme::Lexeme(Lexeme::Kind K, const std::string &IdentifierStr, llvm::SMLoc Position) :
        K(K), IdentifierStr(IdentifierStr), Loc(Position), Char(0), NumericValue(0) {
    assert(K == Kind::IDENT);
}

Lexeme::Lexeme(Lexeme::Kind K, long NumericValue, llvm::SMLoc Position) :
        K(K), NumericValue(NumericValue), Loc(Position), Char(0) {
    assert(K == Kind::NUMBER);
}

std::ostream &operator<<(std::ostream &Stream, const Lexeme::Kind &K) {
    switch (K) {
        case Lexeme::Kind::UNKNOWN: Stream << "UNKNOWN"; break;
        case Lexeme::Kind::ENDOFFILE: Stream << "ENDOFFILE"; break;
        case Lexeme::Kind::PROGRAM: Stream << "PROGRAM"; break;
        case Lexeme::Kind::CONST: Stream << "CONST"; break;
        case Lexeme::Kind::VAR: Stream << "VAR"; break;
        case Lexeme::Kind::FUNCTION: Stream << "FUNCTION"; break;
        case Lexeme::Kind::IF: Stream << "IF"; break;
        case Lexeme::Kind::THEN: Stream << "THEN"; break;
        case Lexeme::Kind::ELSE: Stream << "ELSE"; break;
        case Lexeme::Kind::WHILE: Stream << "WHILE"; break;
        case Lexeme::Kind::DO: Stream << "DO"; break;
        case Lexeme::Kind::FOR: Stream << "FOR"; break;
        case Lexeme::Kind::TO: Stream << "TO"; break;
        case Lexeme::Kind::DOWNTO: Stream << "DOWNTO"; break;
        case Lexeme::Kind::BEGIN: Stream << "BEGIN"; break;
        case Lexeme::Kind::END: Stream << "END"; break;
        case Lexeme::Kind::EXIT: Stream << "EXIT"; break;
        case Lexeme::Kind::AND: Stream << "AND"; break;
        case Lexeme::Kind::OR: Stream << "OR"; break;
        case Lexeme::Kind::LT: Stream << "LT"; break;
        case Lexeme::Kind::LTE: Stream << "LTE"; break;
        case Lexeme::Kind::EQ: Stream << "EQ"; break;
        case Lexeme::Kind::GTE: Stream << "GTE"; break;
        case Lexeme::Kind::GT: Stream << "GT"; break;
        case Lexeme::Kind::NEQ: Stream << "NEQ"; break;
        case Lexeme::Kind::PLUS: Stream << "PLUS"; break;
        case Lexeme::Kind::MINUS: Stream << "MINUS"; break;
        case Lexeme::Kind::MOD: Stream << "MOD"; break;
        case Lexeme::Kind::MUL: Stream << "MUL"; break;
        case Lexeme::Kind::DIV: Stream << "DIV"; break;
        case Lexeme::Kind::LPAR: Stream << "LPAR"; break;
        case Lexeme::Kind::RPAR: Stream << "RPAR"; break;
        case Lexeme::Kind::ASSIGN: Stream << "ASSIGN"; break;
        case Lexeme::Kind::NUMBER: Stream << "NUMBER"; break;
        case Lexeme::Kind::IDENT: Stream << "IDENT"; break;
    }
    return Stream;
}

std::ostream &operator<<(std::ostream &Stream, const Lexeme &L) {
    Stream << L.K;
    switch (L.K) {
        case Lexeme::Kind::UNKNOWN:
            Stream << ' ' << L.Char;
            break;
        case Lexeme::Kind::IDENT:
            Stream << ' ' << L.IdentifierStr;
            break;
        case Lexeme::Kind ::NUMBER:
            Stream << ' ' << L.NumericValue;
            break;
    }
    return Stream;
}
