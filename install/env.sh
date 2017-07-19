#!/bin/bash
if test -z "$LLVM_ENV_DEFINED"; then
    echo "Defining env for using new built LLVM" 
    export LLVM_ENV_DEFINED=yes
    export LLVM_ROOT=$HOME/Sources/llvm/install
    PATH=$LLVM_ROOT/bin:$PATH
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH${LD_LIBRARY_PATH:+:}$LLVM_ROOT/lib
else
    echo "Env already defined for new built LLVM" 
fi
