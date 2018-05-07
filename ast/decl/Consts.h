//
// Created by selvek on 7.5.2018.
//

#ifndef DAWN_CONSTS_H
#define DAWN_CONSTS_H

#include <string>
#include <vector>
#include <memory>
#include <llvm/ADT/StringMap.h>
#include "../Node.h"
#include "../Visitor.h"
#include "../stmt/Stmt.h"

namespace ast {
    class Consts : public Node {
    public:
        std::vector<std::pair<std::string, long>> Constants;

        explicit Consts(llvm::SMLoc Loc): Node(Loc) {}

        void addConst(std::string Name, long Value) {
            Constants.emplace_back(std::move(Name), Value);
        }

        void accept(Visitor &V) override {
            V.visit(*this);
        }
    };
}


#endif //DAWN_CONSTS_H
