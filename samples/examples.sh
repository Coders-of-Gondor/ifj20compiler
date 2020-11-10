#/bin/bash

# IFJ20 - Priklady pro studenty pro dokresleni zadani - TODO otestovat!!!

COMPILER="go build"  # go build neni treba (je zahrnut v go run); zde muze byt vas prekladac IFJ20 (napr. ./ifj20)
INTERPRETER="go run"  # zde muze byt interpret IFJcode20 (./ic20int)
HELPER="ifj20.go"

for i in *.go; do
	if [[ $i == $HELPER ]]; then
		continue
	fi

	IN=`echo $i | sed 's/\(.*\)\.go/\1.in/g'`
	OUT=`echo $i | sed 's/\(.*\)\.go/\1.out/g'`
	PRG=$i

	# $COMPILER $i $HELPER > $PRG   # nekontroluji se chyby prekladu (pro chybove testy nutno rozsirit)
	# RETURNED_COMPILER=$?

	echo -n "DIFF: $INTERPRETER $PRG and $OUT: "
	cat $IN | $INTERPRETER $PRG $HELPER | diff - $OUT > /dev/null 2> /dev/null
	DIFFOK=$?
	if [ "x$DIFFOK" == "x0" ]; then
		echo "OK"
	else
		echo "DIFFERENCE to $OUT"
		cat $IN | $INTERPRETER $PRG $HELPER | diff - $OUT
	fi
        echo
done;

# rm *.tmp

exit 0
