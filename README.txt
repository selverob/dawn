DAWN
====

Dawn is an implementation of a simple Pascal compiler (based on LLVM)
for CTU FIT BI-PJP subject.

It can run code that's functionally almost identical to the examples located
in the original repository with language specification. The language that we
have implemented differs slightly from the original examples so please use the
ones in local examples directory.

Major differences include:
- Algebraic syntax (x := x - 1, x := x + 1) instead of inc(x), dec(x)
- Stricter syntax around loop block termination
- write() function not implemented (not required by spec, only used in one
  example)

Building and running
====================

Since the project is an LLVM frontend, LLVM development libraries must
be installed on the machine used for development. Since the basic support
library (writeln and readln functions) is distributed in textual form and
compiled with the program, it's also necessary to have clang installed.
Additionally, CMake is required to build the compiler.

To build, change into the source directory (the one with CMakeLists.txt in
it) and run `cmake .`. That should generate necessary build cache and files
(by default, it generates a Makefile) and then run `make`.

This process will build an executable called `dawn` in the source directory.
It may be useful when debugging, but it has an impractical interface and
only generates .o files. It's better to use the `dawnc` script located
in the source directory. The syntax that it can be called with is:

./dawnc <source file path> <executable destination path>

It also prints the generated IR, to ease debugging (and because who doesn't
like looking at LLVM IR?).


