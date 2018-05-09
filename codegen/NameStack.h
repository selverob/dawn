//
// Created by selvek on 9.5.2018.
//

#ifndef DAWN_VARIABLESTACK_H
#define DAWN_VARIABLESTACK_H

#include <llvm/ADT/StringMap.h>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ADT/StringMap.h"

namespace codegen {
    class NameStack {
        llvm::StringMap<llvm::AllocaInst*> Allocas;
        std::vector<llvm::SmallVector<llvm::StringRef, 4>> Stack;

    public:
        NameStack(const NameStack &other) = delete;

        void PushFrame();
        void PopFrame();


    };
}


#endif //DAWN_VARIABLESTACK_H
