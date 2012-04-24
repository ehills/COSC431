#!/bin/bash
# $1 = file to search

./search $1 >| words
sort words >| sorted_words
rm -f words
uniq sorted_words >| uniq_words
rm -f sorted_words
echo "Total of " `wc -l uniq_words`
echo "Done"
