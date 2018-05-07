//
// Created by selvek on 5.5.2018.
//

#ifndef DAWN_EXPR_H
#define DAWN_EXPR_H

#include "../Node.h"

namespace ast {
    class Expr : public Node {
    public:
        Expr(llvm::SMLoc Loc): Node(Loc) {}
    };
}

#endif //DAWN_EXPR_H
