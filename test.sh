for f in `ls test/*.in` ; do echo "testing: $f"; ./tsm < $f | head -n1 ; done
