#!/usr/bin/env sh

total=0
for f in `ls test/*.in`
do
    ./tsm < $f > $f.out
    val=`head -n1 $f.out`
    echo "test $f result: $val"
    total=$(($total + $val))
done
echo "Total price: $total";
