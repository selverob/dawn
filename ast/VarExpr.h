//
// Created by selvek on 5.5.2018.
//

#ifndef DAWN_VAREXPR_H
#define DAWN_VAREXPR_H

#include <string>
#include "Expr.h"

namespace ast {
    class VarExpr : public Expr {
        std::string VarName;
    public:
        VarExpr(const std::string &VarName): VarName(VarName) {}
    };
}


#endif //DAWN_VAREXPR_H
