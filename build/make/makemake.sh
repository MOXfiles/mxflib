#!/bin/bash

# Update the list of object files in makefile $2 in directory $1 
function makemake ()
{
	echo Updating $1/$2

	# Build a list of object filenames
	rm -f sed.temp
	for f in {$1/*.c,$1/*.cpp}
	do
	    if [ -e $f ]
	    then
		b=`basename $f`
		if [ $b != esp_template.cpp ]		# Ignore known dummy file
		then
		    echo $'\t'\$\(OBJSDIR\)/${b%%.*}.o >> sed.temp \\
		fi
	    fi
	done
	echo >> sed.temp

	# Move the current file to be a backup
	rm -f $1/$2.bak
	mv $1/$2 $1/$2.bak

	# Replace the object files list
	sed '/OBJS :=/,/^[ \t\r]*$/ {
/OBJS :=/ a\
OBJS := \\
/OBJS :=/ r sed.temp
d }' < $1/$2.bak > $1/$2

	# Remove the list of object filenames
	rm -f sed.temp
}

for dir in *
do
    if [ -d $dir ]
    then
        if [ -e $dir/makefile ] ; then makemake $dir "makefile" ; fi
        if [ -e $dir/Makefile ] ; then makemake $dir "Makefile" ; fi
    fi
done


