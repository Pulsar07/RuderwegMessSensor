#!/bin/bash -x 

cd doc
doxygen Doxyfile 
TEMP_FILE=$( mktemp )

pandoc --write=gfm -i html/index.html -o $TEMP_FILE
BEGIN=$( grep -n 'class="PageDoc"' $TEMP_FILE )
BEGIN=${BEGIN%%:*}
BEGIN=$(( BEGIN - 1)) 

tail --lines=+${BEGIN} $TEMP_FILE  > ../README.md
rm $TEMP_FILE

