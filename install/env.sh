#!/bin/bash
if test -z "$AST2DOT_ENV_DEFINED"; then
    echo "Defining env for using new built AST2DOT" 
    export AST2DOT_ENV_DEFINED=yes
    export AST2DOT_ROOT=$HOME/Sources/llvm/install
    PATH=$AST2DOT_ROOT/bin:$PATH
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH${LD_LIBRARY_PATH:+:}$AST2DOT_ROOT/lib
else
    echo "Env already defined for new built AST2DOT" 
fi
