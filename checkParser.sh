#!/bin/bash

./search data/wsj.xml >| output
cat output | grep "-" >| all_words
cat all_words | awk '{print $2}' >| words
rm -f all_words
sort words >| sorted_words
rm -f words
uniq words >| uniq_words
"Done"
