#!/bin/sh

trap_ctrlc ()
{
    echo 0 > gpio14/value; sleep 1; echo 0 > gpio3/value; sleep 1; echo 0 > gpio4/value
    exit 2
}


trap "trap_ctrlc" 2

cd /sys/class/gpio

echo 1 > gpio4/value; sleep 1; echo 1 > gpio3/value; echo 1 > gpio14/value

while [ true ]; do
    sleep 1
done
