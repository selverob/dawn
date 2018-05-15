//
// Created by selvek on 15.5.2018.
//

#ifndef DAWN_NUMERICARRAYBOUND_H
#define DAWN_NUMERICARRAYBOUND_H

#include "ArrayBound.h"

namespace ast {
    class NumericArrayBound : public ArrayBound {
    public:
        int Value;
        NumericArrayBound(int Value): ArrayBound(ArrayBound::Kind::Numeric), Value(Value) {}

        int value() override {
            return Value;
        }

        static bool classof(const ArrayBound *B) {
            return B->K == ArrayBound::Kind::Numeric;
        }
    };
}


#endif //DAWN_NUMERICARRAYBOUND_H
