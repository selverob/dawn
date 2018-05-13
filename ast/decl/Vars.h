//
// Created by selvek on 7.5.2018.
//

#ifndef DAWN_VARS_H
#define DAWN_VARS_H

#include <string>
#include <memory>
#include <vector>
#include "../Node.h"
#include "../Visitor.h"
#include "../stmt/Stmt.h"
#include "../type/Type.h"

namespace ast {
    class Vars : public Node {
    public:
        std::vector<std::pair<std::string, Type*>> Variables;

        Vars(llvm::SMLoc Loc): Node(Loc) {}

        void addVar(std::string Name, Type *VarType) {
            Variables.emplace_back(std::move(Name), VarType);
        }

        void accept(Visitor &V) override {
            V.visit(*this);
        }
    };
}


#endif //DAWN_VARS_H
