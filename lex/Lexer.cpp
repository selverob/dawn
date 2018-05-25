//
// Created by selvek on 3.5.2018.
//

#include <set>
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/SMLoc.h"
#include "Lexer.h"

const std::set<char> OctalChars {'0', '1', '2', '3', '4', '5', '6', '7'};
const std::set<char> DecimalChars {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
const std::set<char> HexadecimalChars {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                       'A', 'a', 'B', 'b', 'C', 'c', 'D', 'd', 'E', 'e', 'F', 'f'};

Lexer::Lexer(llvm::SourceMgr &SrcMgr, unsigned FileIdx): Sources(SrcMgr) {
    auto Buf = Sources.getMemoryBuffer(FileIdx);
    Current = (char *) Buf->getBufferStart();
    End = (char *) Buf->getBufferEnd();
    LastChar = ' ';
}

Lexer::Lexer(Lexer &&Other):
        Sources(Other.Sources), Current(Other.Current), End(Other.End), LastChar(Other.LastChar) {
    Other.LastChar = ' ';
    Other.Current = nullptr;
    Other.End = nullptr;
}


void Lexer::readChar() {
    if (Current == End) {
        LastChar = EOF;
    } else {
        LastChar = *(Current++);
    }
}

Lexeme Lexer::getLexeme() {
    assert(Current != nullptr);
    while (isspace(LastChar) || LastChar == '#') {
        while (isspace(LastChar))
            readChar();
        if (LastChar == '#') {
            while (LastChar != '\n')
                readChar();
        }
    }

    if (isdigit(LastChar)) {
        return readNumber(10);
    }
    if (LastChar == '$') {
        readChar();
        return readNumber(16);
    }
    if (LastChar == '&') {
        readChar();
        return readNumber(8);
    }

    if (isalnum(LastChar) || LastChar == '_') {
        return readIdentifier();
    }

    if (LastChar == EOF) {
        return Lexeme(Lexeme::Kind::ENDOFFILE, llvm::SMLoc::getFromPointer(End));
    }
    return readOperator();
}

Lexeme Lexer::readIdentifier() {
    auto Loc = getLoc();
    std::string Identifier;
    Identifier = LastChar;
    readChar();
    while (isalnum(LastChar) || LastChar == '_') {
        Identifier += LastChar;
        readChar();
    }
    Lexeme::Kind Kind = llvm::StringSwitch<Lexeme::Kind>(Identifier)
            .Case("program", Lexeme::Kind::PROGRAM)
            .Case("const", Lexeme::Kind::CONST)
            .Case("var", Lexeme::Kind::VAR)
            .Case("function", Lexeme::Kind::FUNCTION)
            .Case("procedure", Lexeme::Kind::FUNCTION)
            .Case("if", Lexeme::Kind::IF)
            .Case("then", Lexeme::Kind::THEN)
            .Case("else", Lexeme::Kind::ELSE)
            .Case("while", Lexeme::Kind::WHILE)
            .Case("do", Lexeme::Kind::DO)
            .Case("for", Lexeme::Kind::FOR)
            .Case("to", Lexeme::Kind::TO)
            .Case("downto", Lexeme::Kind::DOWNTO)
            .Case("begin", Lexeme::Kind::BEGIN)
            .Case("end", Lexeme::Kind::END)
            .Case("exit", Lexeme::Kind::EXIT)
            .Case("and", Lexeme::Kind::AND)
            .Case("or", Lexeme::Kind::OR)
            .Case("not", Lexeme::Kind::NOT)
            .Case("mod", Lexeme::Kind::MOD)
            .Case("forward", Lexeme::Kind::FORWARD)
            .Case("of", Lexeme::Kind::OF)
            .Case("array", Lexeme::Kind::ARRAY)
            .Default(Lexeme::Kind::IDENT);
    if (Kind == Lexeme::Kind::IDENT) {
        return Lexeme(Kind, Identifier, Loc);
    } else {
        return Lexeme(Kind, Loc);
    }
}

Lexeme Lexer::readNumber(int base) {
    auto Loc = getLoc();
    const std::set<char> *AcceptableChars;
    switch (base) {
        case 8:
            AcceptableChars = &OctalChars;
            break;
        case 10:
            AcceptableChars = &DecimalChars;
            break;
        case 16:
            AcceptableChars = &HexadecimalChars;
            break;
        default:
            llvm_unreachable("Only numbers with base 8, 10 and 16 are supported");
    }
    std::string Num;
    while (AcceptableChars->find(LastChar) != AcceptableChars->end()) {
        Num += LastChar;
        readChar();
    }
    return Lexeme(Lexeme::Kind::NUMBER, std::stol(Num, nullptr, base), Loc);
}

Lexeme Lexer::readOperator() {
    auto Loc = getLoc();
    Lexeme L(Lexeme::Kind::UNKNOWN, LastChar, Loc);
    switch (LastChar) {
        case '=':
            L = Lexeme(Lexeme::Kind::EQ, Loc);
            break;
        case '+':
            L = Lexeme(Lexeme::Kind::PLUS, Loc);
            break;
        case '-':
            L = Lexeme(Lexeme::Kind::MINUS, Loc);
            break;
        case '*':
            L = Lexeme(Lexeme::Kind::MUL, Loc);
            break;
        case '/':
            L = Lexeme(Lexeme::Kind::DIV, Loc);
            break;
        case '(':
            L = Lexeme(Lexeme::Kind::LPAR, Loc);
            break;
        case ')':
            L = Lexeme(Lexeme::Kind::RPAR, Loc);
            break;
        case ':':
            readChar();
            if (LastChar == '=')
                L = Lexeme(Lexeme::Kind::ASSIGN, getLoc());
            else
                L = Lexeme(Lexeme::Kind::UNKNOWN, ':', getLoc());
            break;
        case '<':
            readChar();
            if (LastChar == '=')
                L = Lexeme(Lexeme::Kind::LTE, getLoc());
            if (LastChar == '>')
                L = Lexeme(Lexeme::Kind::NEQ, getLoc());
            else
                L = Lexeme(Lexeme::Kind::LT, getLoc());
            break;
        case '>':
            readChar();
            if (LastChar == '=')
                L = Lexeme(Lexeme::Kind::GTE, getLoc());
            else
                L = Lexeme(Lexeme::Kind::GT, getLoc());
            break;
    }
    readChar();
    return L;
}

llvm::SMLoc Lexer::getLoc() {
    return llvm::SMLoc::getFromPointer(Current);
}

