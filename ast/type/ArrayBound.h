//
// Created by selvek on 15.5.2018.
//

#ifndef DAWN_ARRAYBOUND_H
#define DAWN_ARRAYBOUND_H

#include "llvm/Support/Casting.h"

namespace ast {
    class ArrayBound{
    public:
        enum class Kind {
            Numeric,
            Identifier
        };
        Kind K;
        explicit ArrayBound(Kind K): K(K) {}

        virtual int value() = 0;
    };
}


#endif //DAWN_ARRAYBOUND_H
