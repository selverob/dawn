//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_NODE_H
#define DAWN_NODE_H

#include "llvm/Support/SMLoc.h"

namespace ast {
    class Visitor;

    class Node {
    public:
        virtual void accept(Visitor &V) = 0;
        Node(llvm::SMLoc Loc): Loc(Loc) {}
        llvm::SMLoc Loc;
    };
}


#endif //DAWN_NODE_H
