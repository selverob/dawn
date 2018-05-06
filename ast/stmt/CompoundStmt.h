//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_COMPOUNDSTATEMENT_H
#define DAWN_COMPOUNDSTATEMENT_H

#include <memory>
#include <vector>
#include "Stmt.h"
#include "../Visitor.h"
#include "../expr/Expr.h"

namespace ast {
    class CompoundStmt : public Stmt {
    public:
        std::vector<std::unique_ptr<Stmt>> Body;

        CompoundStmt() = default;

        void addStmt(std::unique_ptr<Stmt> S) {
            Body.push_back(std::move(S));
        }

        void accept(Visitor &V) override {
            V.visit(*this);
        }
    };
}


#endif //DAWN_COMPOUNDSTATEMENT_H
