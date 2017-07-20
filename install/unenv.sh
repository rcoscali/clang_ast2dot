#!/bin/bash
if ! test -z "$AST2DOT_ENV_DEFINED"; then
    echo "Undefining env for using new built AST2DOT" 
    PATH=`echo $PATH | sed -e "s,$AST2DOT_ROOT/bin:,,"`
    LD_LIBRARY_PATH=`echo $LD_LIBRARY_PATH | sed -e "s,:$AST2DOT_ROOT/lib,,"`
    LD_LIBRARY_PATH=`echo $LD_LIBRARY_PATH | sed -e "s,$AST2DOT_ROOT/lib,,"`
    if test "x$LD_LIBRARY_PATH" = "x"; then
	unset LD_LIBRARY_PATH
    fi
    unset AST2DOT_ROOT
    unset AST2DOT_ENV_DEFINED
else
    echo "No env defined for new built AST2DOT" 
fi
