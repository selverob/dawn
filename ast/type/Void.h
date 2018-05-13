//
// Created by selvek on 13.5.2018.
//

#ifndef DAWN_VOID_H
#define DAWN_VOID_H

#include <llvm/Support/SMLoc.h>
#include "Type.h"

namespace ast {
    class Void : public Type {
    private:
        static Void *V;
    public:
        Void(): Type(Type::Kind::Void) {}

        bool operator==(const Type &Other) override {
            return llvm::isa<Void>(Other);
        }

        static Void *get();

        static bool classof(const Type *T);
    };
}


#endif //DAWN_VOID_H
