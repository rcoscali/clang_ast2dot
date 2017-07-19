#!/bin/bash
if ! test -z "$LLVM_ENV_DEFINED"; then
    echo "Undefining env for using new built LLVM" 
    PATH=`echo $PATH | sed -e "s,$LLVM_ROOT/bin:,,"`
    LD_LIBRARY_PATH=`echo $LD_LIBRARY_PATH | sed -e "s,:$LLVM_ROOT/lib,,"`
    LD_LIBRARY_PATH=`echo $LD_LIBRARY_PATH | sed -e "s,$LLVM_ROOT/lib,,"`
    if test "x$LD_LIBRARY_PATH" = "x"; then
	unset LD_LIBRARY_PATH
    fi
    unset LLVM_ROOT
    unset LLVM_ENV_DEFINED
else
    echo "No env defined for new built LLVM" 
fi
