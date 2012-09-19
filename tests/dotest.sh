#!/bin/bash

# Set the executable path and dictionary path
exepath=$1
export MXFLIB_DATA_DIR=$2
echo exepath is $exepath
echo datadir is $2

function runtest ()
{
	echo Testing $1 with $2 in $3

	# Run the test as specified
	$3/$1 $2 > out.temp

	# Compare results and display if different
	diff out.temp $1.out > $1.err
	if [ $? -eq 0 ]
	then
	    echo Test Passed
	    echo "    $1 Passed" >> dotest.txt
	    rm -f $1.err
	else
	    echo *Test FAILED*
	    echo "    $1 *FAILED*" >> dotest.txt
	    cat $1.err >&2
	fi
	rm -f out.temp
	rm -f $1.out
}

# Clear the summary
rm -f dotest.txt

for f in *.out
do
    name=`basename $f`
    base=${name%%.*}
    if [ -e $base.args ]
    then
	args=`cat $base.args`
	runtest $base "$args" $exepath
    fi
done

# Print a summary report
if [ -e dotest.txt ]
then
    echo ""
    echo "Test Summary:"
    cat dotest.txt
    echo ""
    rm -f dotest.txt
fi



