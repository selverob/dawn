//
// Created by selvek on 3.5.2018.
//

#include <set>
#include "llvm/ADT/StringSwitch.h"
#include "Lexer.h"

const std::set<char> OctalChars {'0', '1', '2', '3', '4', '5', '6', '7'};
const std::set<char> DecimalChars {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
const std::set<char> HexadecimalChars {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                       'A', 'a', 'B', 'b', 'C', 'c', 'D', 'd', 'E', 'e', 'F', 'f'};

Lexer::Lexer(llvm::MemoryBufferRef Input) :
        Current((char *) Input.getBufferStart()),
        End((char *) Input.getBufferEnd()),
        LastChar(' ') {}

Lexer::Lexer(char *Start, char *End) :
        Current(Start),
        End(End),
        LastChar(' ') {}

Lexer::Lexer(Lexer &&Other) : Current(Other.Current), End(Other.End), LastChar(Other.LastChar) {
    Other.Current = nullptr;
    Other.End = nullptr;
}

Lexeme Lexer::getLexeme() {
    assert(Current != nullptr);
    while (isspace(LastChar))
        readChar();


    if (isalpha(LastChar)) {
        return readIdentifier();
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

    if (LastChar == EOF) {
        return Lexeme(Lexeme::Kind::ENDOFFILE);
    }
    return readOperator();
}



void Lexer::readChar() {
    if (Current == End) {
        LastChar = EOF;
    } else {
        LastChar = *(Current++);
    }
}

Lexeme Lexer::readIdentifier() {
    std::string Identifier;
    Identifier = LastChar;
    readChar();
    while (isalnum(LastChar)) {
        Identifier += LastChar;
        readChar();
    }
    Lexeme::Kind Kind = llvm::StringSwitch<Lexeme::Kind>(Identifier)
            .Case("program", Lexeme::Kind::PROGRAM)
            .Case("const", Lexeme::Kind::CONST)
            .Case("var", Lexeme::Kind::VAR)
            .Case("function", Lexeme::Kind::FUNCTION)
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
            .Default(Lexeme::Kind::IDENT);
    if (Kind == Lexeme::Kind::IDENT) {
        return Lexeme(Kind, Identifier);
    } else {
        return Lexeme(Kind);
    }
}

Lexeme Lexer::readNumber(int base) {
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
    return Lexeme(Lexeme::Kind::NUMBER, std::stol(Num, nullptr, base));
}

Lexeme Lexer::readOperator() {
    Lexeme L(Lexeme::Kind::UNKNOWN);
    switch (LastChar) {
        case '=':
            L = Lexeme(Lexeme::Kind::EQ);
            break;
        case '+':
            L = Lexeme(Lexeme::Kind::PLUS);
            break;
        case '-':
            readChar();
            L = Lexeme(Lexeme::Kind::MINUS);
            break;
        case '*':
            readChar();
            L = Lexeme(Lexeme::Kind::MUL);
            break;
        case '/':
            readChar();
            L = Lexeme(Lexeme::Kind::DIV);
            break;
        case '(':
            readChar();
            L = Lexeme(Lexeme::Kind::LPAR);
            break;
        case ')':
            readChar();
            L = Lexeme(Lexeme::Kind::RPAR);
            break;
        case ':':
            readChar();
            if (LastChar == '=')
                L = Lexeme(Lexeme::Kind::ASSIGN);
            else
                L = Lexeme(Lexeme::Kind::UNKNOWN, ':');
            break;
        case '!':
            readChar();
            if (LastChar == '=')
                L = Lexeme(Lexeme::Kind::NEQ);
            else
                L = Lexeme(Lexeme::Kind::UNKNOWN, '!');
            break;
        case '<':
            readChar();
            if (LastChar == '=')
                L = Lexeme(Lexeme::Kind::LTE);
            else
                L = Lexeme(Lexeme::Kind::LT);
            break;
        case '>':
            readChar();
            if (LastChar == '=')
                L = Lexeme(Lexeme::Kind::GTE);
            else
                L = Lexeme(Lexeme::Kind::GT);
            break;
    }
    readChar();
    return L;
}

