//
// Created by selvek on 5.5.2018.
//

#ifndef DAWN_CALLEXPR_H
#define DAWN_CALLEXPR_H

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include "Expr.h"
#include "../Visitor.h"

namespace ast {
    class CallExpr : public Expr {
    public:
        CallExpr(llvm::SMLoc Loc, std::string FunctionName) :
                Expr(Loc), FunctionName(std::move(FunctionName)) {}

        void addArgument(std::unique_ptr<Expr> Arg) {
            Args.push_back(std::move(Arg));
        }

        void accept(Visitor &V) override {
            V.visit(*this);
        }

        std::string FunctionName;
        std::vector<std::unique_ptr<Expr>> Args;
    };
}


#endif //DAWN_CALLEXPR_H
