#!/bin/sh

# Fail if any given ELF file or archive was built for the hard-float ABI.
# Needed because the 6.11 soft-float build links with --no-warn-mismatch,
# which would otherwise hide such mixtures…

status=0
for f in "$@"; do
  if readelf -A "$f" 2>/dev/null | grep -q 'Tag_ABI_VFP_args: VFP registers'; then
    echo "error: $f uses the hard-float ABI" >&2
    status=1
  fi
done
exit $status
