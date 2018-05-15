//
// Created by selvek on 13.5.2018.
//

#ifndef DAWN_ARRAY_H
#define DAWN_ARRAY_H

#include <cstddef>
#include <llvm/Support/SMLoc.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include "Type.h"
#include "ArrayBound.h"

namespace ast {
    class Array : public Type {
    public:
        ArrayBound *From, *To;
        Type *ElemType;

        Array(Type *ElemType, ArrayBound *From, ArrayBound *To):
                Type(Type::Kind::Array), From(From), To(To), ElemType(ElemType) {}

        size_t size() const {
            return To->value() - From->value();
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

        static Array *get(Type *ElemType, ArrayBound *From, ArrayBound *To) {
            //TODO: Maybe manage the memory better here (or, like, at all) (shared_ptr?)
            return new Array(ElemType, From, To);
        }

        bool isLLVMType(llvm::Type *T) override {
            auto *Arr = llvm::dyn_cast<llvm::ArrayType>(T);
            if (!Arr)
                return false;
            return Arr->getNumElements() == size() && ElemType->isLLVMType(Arr->getElementType());
        }
    };
}

#endif //DAWN_ARRAY_H
