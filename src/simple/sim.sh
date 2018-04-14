#!/usr/bin/env bash

if [ "$1" == "debug" ]; then
	PREFIX="libtool --mode=execute gdb --args"
else
	PREFIX=
fi

$PREFIX ./example-simple \
	--config-file simple.conf \
	--config-variables instruments.conf \
	--simulation-file ./USTP_Multi_20180223.csv $@
