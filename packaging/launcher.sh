#!/bin/sh

# Work around PocketBook firmware 6.11 rejecting executable-stack
# system libraries.
export GLIBC_TUNABLES="${GLIBC_TUNABLES}${GLIBC_TUNABLES:+:}glibc.rtld.execstack=2"

exec "$(dirname "$0")/taranis/taranis.bin" "$@"
