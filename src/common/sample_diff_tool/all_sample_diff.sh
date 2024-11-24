#!/bin/sh

### download
cd ../../../prob/lib/
# scala download_pukiwiki.scala
cat <<\EOF >&2
ダウンロードは済んでいるとします。
EOF
# というか上のやり方だと引数がなくてこわれないか？
cd -

### diff
for c in {A..H}; do
    echo $c && scala sample_diff.scala ../../../prob/pukiwiki/$c/$c.pukiwiki ../../$c-*/rime-out/test/ ./tmp/$c/
done
