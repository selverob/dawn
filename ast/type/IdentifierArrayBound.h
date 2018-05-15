//
// Created by selvek on 15.5.2018.
//

#ifndef DAWN_IDENTIFIERARRAYBOUND_H
#define DAWN_IDENTIFIERARRAYBOUND_H

#include "ArrayBound.h"

namespace ast {
    class IdentifierArrayBound : public ArrayBound {
    private:
        int Value;
        bool ValueSet;
    public:
        std::string Ident;

        IdentifierArrayBound(std::string Ident):
                ArrayBound(ArrayBound::Kind::Identifier), Value(0), ValueSet(false), Ident(std::move(Ident)) {}

        void setValue(int Val) {
            ValueSet = true;
            Value = Val;
        }

        int value() override {
            if (!ValueSet)
                llvm::report_fatal_error("Trying to form an array type with unspecified constant bounds");
            return Value;
        }

        static bool classof(const ArrayBound *B) {
            return B->K == ArrayBound::Kind::Identifier;
        }
    };
}

#endif //DAWN_IDENTIFIERARRAYBOUND_H
