#!/bin/bash

rm winning
wget https://www.dotabuff.com/heroes/winning  
sed -e 's/<[^>]*>/ /g' winning > winclean
cat winclean | grep -o "Hero  Win Rate  Pick Rate  KDA Ratio.*" > winclean2
sed -i 's/"Hero  Win Rate  Pick Rate  KDA Ratio"/ /g' winclean2
sed -i 's/%/ /g' winclean2 

exit 0
