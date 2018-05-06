//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_STATEMENT_H
#define DAWN_STATEMENT_H

namespace ast {
    class Visitor;

    class Stmt {
    public:
        virtual void accept(Visitor &V) = 0;
    };
}



#endif //DAWN_STATEMENT_H
