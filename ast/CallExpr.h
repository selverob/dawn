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

namespace ast {
    class CallExpr : public Expr {
        std::string FunctionName;
        std::vector<std::unique_ptr<Expr>> Args;
    public:
        CallExpr(std::string FunctionName) :
                FunctionName(std::move(FunctionName)) {}

        void addArgument(std::unique_ptr<Expr> Arg) {
            Args.push_back(std::move(Arg));
        }
    };
}


#endif //DAWN_CALLEXPR_H
