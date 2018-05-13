//
// Created by selvek on 13.5.2018.
//

#include "Void.h"

ast::Void *ast::Void::V = nullptr;

bool ast::Void::classof(const Type *T) {
    return T->K == Type::Kind::Void;
}

ast::Void *ast::Void::get() {
    if (V == nullptr)
        V = new Void();
    return V;
}
