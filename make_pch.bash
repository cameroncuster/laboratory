#!/bin/bash
# Precompile <bits/stdc++.h> for the nvim keymap flag sets (F4/F5/F6).
# Because the keymaps pass -I ~/.cache/cp-pch, the bits/stdc++.h (+ .gch)
# there shadows the system header and GCC picks the PCH variant matching the
# compile flags — from any directory, not just ~/laboratory.
# Re-run after upgrading GCC or changing the flags in ~/cfg (lua/cp.lua).
set -euo pipefail
pch_dir="$HOME/.cache/cp-pch"

# locate the real stdc++.h for the current g++ and copy it into the cache
# (grep -m1 would SIGPIPE g++ under pipefail, so buffer the output first)
preproc=$(echo '#include <bits/stdc++.h>' | g++ -std=c++26 -x c++ -E -)
real_hdr=$(grep -m1 'bits/stdc++\.h' <<<"$preproc" | sed 's/.*"\(.*stdc++\.h\)".*/\1/')
echo "source header: $real_hdr"
mkdir -p "$pch_dir/bits"
cp "$real_hdr" "$pch_dir/bits/stdc++.h"
cd "$pch_dir"

# one PCH per flag set, all inside the .gch directory
rm -rf bits/stdc++.h.gch
mkdir bits/stdc++.h.gch

# NOTE: keep these in sync with M.cpp_flags in ~/cfg/.config/nvim/lua/cp.lua
f4_flags="-I. -I$pch_dir -std=c++26 -Wall -O2"
f5_flags="-I. -I$pch_dir -std=c++26 -Wall"
f6_flags="-std=c++26 -I. -I$pch_dir -g -gdwarf-4 -fno-omit-frame-pointer
  -Wall -Wextra -Wshadow -Wpedantic -Wformat=2 -Wlogical-op -Wfloat-equal
  -Wcast-qual -Wcast-align -Wshift-overflow=2 -Wduplicated-cond
  -Wnull-dereference -fsanitize=address,undefined -fno-sanitize-recover=all
  -fstack-protector -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
  -D_GLIBCXX_SANITIZE_VECTOR -D_GLIBCXX_ASSERTIONS"

for name in f4 f5 f6; do
  flags_var="${name}_flags"
  echo "building $name pch..."
  # shellcheck disable=SC2086
  g++ ${!flags_var} -x c++-header bits/stdc++.h -o "bits/stdc++.h.gch/$name.gch"
done

echo "done:"
du -sh bits/stdc++.h.gch
