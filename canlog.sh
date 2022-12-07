#!/bin/sh

TEMPFILE=''

trap_ctrlc ()
{
    # perform cleanup here
    echo Time Stamp,ID,Extended,Bus,LEN,D1,D2,D3,D4,D5,D6,D7,D8
    cat $TEMPFILE | sed -e 's/ (\(...\)\.\(......\))  can\(.\)  \(...\)   \[\(.\)\]  \(.*\)/\1\2,00000\4,false,\3,\5,\6,/' -ne 's/ /,/gp'

    rm $TEMPFILE

    # exit shell script with error code 2
    # if omitted, shell script will continue execution
    exit 2
}
 

if [ -f /proc/$$/fd/1 ]
then
    # initialise trap to call trap_ctrlc function
    # when signal 2 (SIGINT) is received
    trap "trap_ctrlc" 2
    TEMPFILE=`mktemp`
    candump -t z $1 > $TEMPFILE
else
    candump -t z $1 | sed -e 's/ (\(...\)\.\(......\))  can\(.\)  \(...\)   \[\(.\)\]  \(.*\)/\1\2,00000\4,false,\3,\5,\6,/' -ne 's/ /,/gp'
fi
