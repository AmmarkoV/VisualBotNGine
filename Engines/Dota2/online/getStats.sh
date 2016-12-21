#!/bin/bash

rm winning
wget https://www.dotabuff.com/heroes/winning  
sed -e 's/<[^>]*>//g' winning > winclean
cat winclean | grep -o "HeroWin RatePick RateKDA Ratio.*" > winclean2
sed -i 's/"HeroWin RatePick RateKDA Ratio"/ /g' winclean2
sed -i 's/%/ /g' winclean2
sed -i 's/^[a-z][a-z]*/& /;s/[0-9][0-9]*/&  /' winclean2 

exit 0
