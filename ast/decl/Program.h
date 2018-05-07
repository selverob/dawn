//
// Created by selvek on 7.5.2018.
//

#ifndef DAWN_PROGRAM_H
#define DAWN_PROGRAM_H


#include <string>
#include <vector>
#include <memory>
#include "../Node.h"
#include "../stmt/Stmt.h"
#include "Vars.h"
#include "Consts.h"
#include "Function.h"

namespace ast {
    class Program : public Node {
    public:
        std::string Name;
        std::unique_ptr<Vars> Variables;
        std::unique_ptr<Consts> Constants;
        std::vector<std::unique_ptr<Function>> Functions;
        std::unique_ptr<Stmt> Body;

        Program(llvm::SMLoc Loc, std::string Name) :
                Node(Loc), Name(std::move(Name)) {}

        void addVariables(std::unique_ptr<Vars> V) {
            Variables = std::move(V);
        }

        void addConsts(std::unique_ptr<Consts> C) {
            Constants = std::move(C);
        }

        void addFunction(std::unique_ptr<Function> F) {
            Functions.push_back(std::move(F));
        }

        void addBody(std::unique_ptr<Stmt> B) {
            Body = std::move(B);
        }

        void accept(Visitor &V) override {
            V.visit(*this);
        }
    };
}

#endif //DAWN_PROGRAM_H
