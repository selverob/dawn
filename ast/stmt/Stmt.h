//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_STATEMENT_H
#define DAWN_STATEMENT_H

namespace ast {
    class Stmt : public Node {
    public:
        Stmt(llvm::SMLoc Loc) : Node(Loc) {}
    };
}



#endif //DAWN_STATEMENT_H
