//
// Created by selvek on 13.5.2018.
//

#ifndef DAWN_TYPE_H
#define DAWN_TYPE_H

#include "llvm/Support/Casting.h"
#include "../Node.h"

namespace ast {
    class Type {
    public:
        enum class Kind {
            Array,
            Integer,
            Void,
        };
        Kind K;
        explicit Type(Kind K): K(K) {}

        virtual bool operator== (const Type &Other) = 0;
        virtual bool operator!= (const Type &Other) {
            return !(*this == Other);
        }
    };
}


#endif //DAWN_TYPE_H
