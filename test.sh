#!/usr/bin/env sh

total=0
for f in `ls test/*.in`
do
    val=`./tsm < $f | head -n1`
    echo "test $f result: $val"
    total=$(($total + $val))
done
echo "Total price: $total";
