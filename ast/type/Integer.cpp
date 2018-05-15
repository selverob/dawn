//
// Created by selvek on 13.5.2018.
//

#include <llvm/IR/Type.h>
#include "Integer.h"

ast::Integer *ast::Integer::I = nullptr;

bool ast::Integer::classof(const ast::Type *T) {
    return T->K == Type::Kind::Integer;
}

ast::Integer *ast::Integer::get() {
    if (I == nullptr)
        I = new Integer();
    return I;
}

bool ast::Integer::isLLVMType(llvm::Type *T) {
    return T->isIntegerTy(64);
}
