//
// Created by selvek on 13.5.2018.
//

#ifndef DAWN_ARRAY_H
#define DAWN_ARRAY_H

#include <cstddef>
#include <llvm/Support/SMLoc.h>
#include "Type.h"

namespace ast {
    class Array : public Type {
    public:
        size_t From, To;
        Type *ElemType;

        Array(Type *ElemType, size_t From, size_t To):
                Type(Type::Kind::Array), From(From), To(To), ElemType(ElemType) {}

        size_t size() const {
            return To - From;
        }

        static bool classof(const Type *T) {
            return T->K == Type::Kind::Array;
        }

        bool operator==(const Type &Other) override {
            if (llvm::isa<Array>(Other)) {
                auto &ArrOther = llvm::cast<Array>(Other);
                return ElemType == ArrOther.ElemType && From == ArrOther.From && To == ArrOther.To;
            }
            return false;
        }

        static Array *get(Type *ElemType, size_t From, size_t To) {
            //TODO: Maybe manage the memory better here (or, like, at all)
            return new Array(ElemType, From, To);
        }
    };
}

#endif //DAWN_ARRAY_H
