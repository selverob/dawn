cmake_minimum_required(VERSION 3.10)
project(dawn)

set(CMAKE_CXX_STANDARD 17)
find_package(LLVM REQUIRED CONFIG)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
llvm_map_components_to_libnames(llvm_libs core)

add_executable(dawn main.cpp lex/Lexeme.h lex/Lexer.cpp lex/Lexer.h lex/Lexeme.cpp lex/Lexeme.h ast/expr/Expr.h ast/expr/VarExpr.h ast/expr/NumExpr.h ast/expr/CallExpr.h ast/expr/UnaryOpExpr.h ast/expr/BinaryOpExpr.h ast/Parser.cpp ast/Parser.h ast/Visitor.h ast/Printer.cpp ast/Printer.h ast/stmt/Stmt.h ast/stmt/AssignmentStmt.h ast/stmt/IfStmt.h ast/stmt/WhileStmt.h ast/stmt/ForStmt.h ast/stmt/CompoundStmt.h ast/stmt/ExitStmt.h ast/stmt/CallStmt.h ast/Node.h ast/decl/Function.h ast/decl/Vars.h ast/decl/Consts.h ast/decl/Program.h)

target_link_libraries(dawn ${llvm_libs})

