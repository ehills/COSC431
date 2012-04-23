#!/bin/bash
# $1 = file to search

./search $1 >| output
cat output | grep "-" >| all_words
cat all_words | awk '{print $2}' >| words
rm -f all_words
sort words >| sorted_words
rm -f words
uniq sorted_words >| uniq_words
echo "Done"
