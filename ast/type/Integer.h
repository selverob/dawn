//
// Created by selvek on 13.5.2018.
//

#ifndef DAWN_INTEGER_H
#define DAWN_INTEGER_H

#include <llvm/Support/SMLoc.h>
#include <llvm/IR/Type.h>
#include "Type.h"

namespace ast {
    class Integer : public Type {
    private:
        static Integer *I;
    public:
        Integer(): Type(Type::Kind::Integer) {}

        bool operator==(const Type &Other) override {
            return llvm::isa<Integer>(Other);
        }

        static Integer *get();

        static bool classof(const Type *T);

        bool isLLVMType(llvm::Type *T) override;
    };
}


#endif //DAWN_INTEGER_H
