#!/bin/bash

# Copyright © 2024 Claus Tøndering
# Released under an MIT License.

echo Copying database...
cp ../ETCBC4 .

echo Finding jussives that must be changed to cohortatives
jus2co=$(mql -d ETCBC4 juss2coho.mql | ./extract_monads.sh)

echo Finding cohortatives...
coho=$(mql -d ETCBC4 cohortative.mql | ./extract_monads.sh; echo "$jus2co")

echo Finding emphatic imperatives...
emim=$(mql -d ETCBC4 emphatic_imperative.mql | ./extract_monads.sh)

echo Finding jussives...
juss=$(mql -d ETCBC4 jussive.mql | ./extract_monads.sh | uniq | grep -F -w -v "$jus2co")

echo Generating update script...
cat <<EOF > update.mql
UPDATE ENUMERATION verbal_tense_t = {
ADD juss = 101,
ADD coho = 102,
ADD emim = 103
}
GO

EOF


cat <<EOF >> update.mql
UPDATE OBJECTS BY MONADS = { $(echo $coho | sed -e 's/ /,/g') }
[word
  vt:=coho;
]
GO

EOF

cat <<EOF >> update.mql
UPDATE OBJECTS BY MONADS = { $(echo $emim | sed -e 's/ /,/g') }
[word
  vt:=emim;
]
GO

EOF


cat <<EOF >> update.mql
UPDATE OBJECTS BY MONADS = { $(echo $juss | sed -e 's/ /,/g') }
[word
  vt:=juss;
]
GO

EOF

echo Applying updates...
mql -d ETCBC4 update.mql

rm update.mql

echo Done
