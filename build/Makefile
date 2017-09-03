
MAKEFLAGS+= --no-builtin-rules
export MAKE:=${MAKE} -${MAKEFLAGS}
.SILENT:

.DEFAULT: all
all::

.PHONY: help
help:
	ninja -t targets

# NoOp rules for this Makefile
Makefile: ;

# Anything we don't know how to build will use this rule.
#
% :: ;
	ninja -v $@
