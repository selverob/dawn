//
// Created by selvek on 8.5.2018.
//

#ifndef DAWN_PROTOTYPE_H
#define DAWN_PROTOTYPE_H

#include <string>
#include <vector>
#include <memory>
#include "../Node.h"
#include "../Visitor.h"
#include "../stmt/Stmt.h"
#include "Vars.h"

namespace ast {
    class Prototype : public Node {
    public:
        std::string Name;
        Type* ReturnType;
        std::vector<std::pair<std::string, Type*>> Parameters;

        Prototype(llvm::SMLoc Loc, std::string Name, Type* ReturnType) :
                Node(Loc), Name(std::move(Name)), ReturnType(ReturnType) {}

        void addParameter(std::string Name, Type* ParamType) {
            Parameters.emplace_back(Name, ParamType);
        }

        void accept(Visitor &V) override {
            V.visit(*this);
        }

        bool operator== (const Prototype &Other) const {
            if (!(Name == Other.Name &&
                  ReturnType == Other.ReturnType))
                return false;
            if (Parameters.size() != Other.Parameters.size())
                return false;
            for (size_t i = 0; i < Parameters.size(); i++) {
                if (Parameters[i].second != Other.Parameters[i].second)
                    return false;
            }
            return true;
        }
    };
}

#endif //DAWN_PROTOTYPE_H
