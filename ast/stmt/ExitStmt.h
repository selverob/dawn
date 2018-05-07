//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_EXITSTMT_H
#define DAWN_EXITSTMT_H

#include "Stmt.h"
#include "../Visitor.h"

namespace ast {
    class ExitStmt : public Stmt {
    public:
        ExitStmt(llvm::SMLoc Loc) : Stmt(Loc) {}

        virtual void accept(Visitor &V) {
            V.visit(*this);
        }
    };
}


#endif //DAWN_EXITSTMT_H
