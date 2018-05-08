//
// Created by selvek on 7.5.2018.
//

#ifndef DAWN_FUNCTION_H
#define DAWN_FUNCTION_H

#include <string>
#include <vector>
#include <memory>
#include "../Node.h"
#include "../Visitor.h"
#include "../stmt/Stmt.h"
#include "Vars.h"
#include "Prototype.h"

namespace ast {
    class Function : public Node {
    public:
        std::unique_ptr<Prototype> Proto;
        std::unique_ptr<Vars> Variables;
        std::unique_ptr<Stmt> Body;

        Function(llvm::SMLoc Loc,std::unique_ptr<Prototype> Proto, std::unique_ptr<Vars> Variables, std::unique_ptr<Stmt> Body) :
                Node(Loc), Proto(std::move(Proto)), Variables(std::move(Variables)), Body(std::move(Body)) {}


        void accept(Visitor &V) override {
            V.visit(*this);
        }
    };
}


#endif //DAWN_FUNCTION_H
