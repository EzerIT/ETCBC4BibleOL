#!/bin/bash

# Copyright © 2017 Ezer IT Consulting.
# Released under an MIT License.
 
# This script changes the lower dots of the first word of Ps 27,13 from U0323 to U05c5

db="$1"

set $(echo 'select all objects where [verse book=Psalmi and chapter=27 and verse=13 [word get g_word_utf8]]' | mql -d "$db")

# First line of expected result is something like:
# //  <  < [ verse 1049682 { 314554-314563 } false  //  <  < [ word 1049684 { 314554 } false (g_word_utf8="\xd7\x9c\xd7\x84\xcc\xa3\xd7\x95\xd6\xbc\xd7\x9c\xd6\xb5\xd6\x97\xd7\x84\xcc\xa3\xd7\x90\xd7\x84\xcc\xa3") //  <  >
#
# This means that
# ${18} is word monad ('314554')
# ${21} is feature list ('(g_word_utf8="....")')

monad=${18}
src_word=${21#(g_word_utf8=}  # Remove initial (g_word_utf8=
src_word=${src_word%)}        # Remove final )
dst_word=${src_word//\\xcc\\xa3/\\xd7\\x85}  # Replace all occurrences of U0323 with U05c5

mql -d "$db" <<EOF
UPDATE OBJECTS BY MONADS = { $monad }
[word
  g_word_utf8:=$dst_word;
]
GO

EOF


