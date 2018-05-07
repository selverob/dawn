//
// Created by selvek on 7.5.2018.
//

#ifndef DAWN_VARS_H
#define DAWN_VARS_H

#include <string>
#include <memory>
#include "../Node.h"
#include "../Visitor.h"
#include "../stmt/Stmt.h"

namespace ast {
    class Vars : public Node {
    public:
        std::vector<std::pair<std::string, std::string>> Variables;

        Vars(llvm::SMLoc Loc): Node(Loc) {}

        void addVar(std::string Name, std::string Type) {
            Variables.emplace_back(std::move(Name), std::move(Type));
        }

        void accept(Visitor &V) override {
            V.visit(*this);
        }
    };
}


#endif //DAWN_VARS_H
