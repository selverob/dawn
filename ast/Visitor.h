//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_ABSTRACTVISITOR_H
#define DAWN_ABSTRACTVISITOR_H

class Expr;

class AbstractVisitor {
    virtual void visit(Expr &e) = 0;
};


#endif //DAWN_ABSTRACTVISITOR_H
